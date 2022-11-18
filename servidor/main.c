//****************************************************************************
// Comunicacao de Dados
// Professor: Gustavo Kunzel
// Servidor TCP simples
// Implementa um servidor TCP que aguarda o recebimento de pacotes de
// diferentes clientes em uma porta
// Nota: a biblioteca -lws2_32 deve ser incluida no linker para funcionar
//     no projeto do DEV jÃ¡ foi incluido atraves das opcoes de projeto
//****************************************************************************
#define _WIN32_WINNT 0x0501

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// **************************************************************************
// definicoes iniciais
#define TAMANHO_BUFFER 512          // tamanho do buffer de recepcao e envio
#define DEFAULT_PORT "8888"        // porta de comunicacao que sera usada

#define Pedra 0
#define Papel 1
#define Tesoura 2

#define Vitoria 0
#define Derrota 1
#define Empate 2
// **************************************************************************

//struct candidato
typedef struct {
    int id;
    char nome[50];
    int numero;
    int votos;
} Candidato;

int __cdecl main(void) {


    printf("Configurando candidatos...\n");
    printf("Informe o numero de candidatos: ");
    int num_candidatos;
    int i = 0;

    scanf("%d", &num_candidatos);
    Candidato candidatos[num_candidatos];
    for (i = 0; i < num_candidatos; i++) {
        printf("Informe o nome do candidato %d: ", i + 1);
        scanf("%s", candidatos[i].nome);
        printf("Informe o numero do candidato %d: ", i + 1);
        scanf("%d", &candidatos[i].numero);
        candidatos[i].id = i;
        candidatos[i].votos = 0;
    }
    //imprime candidatos
    printf("Candidatos:\n");
    for (i = 0; i < num_candidatos; i++) {
        printf("Nome: %s Numero: %d\n", candidatos[i].nome, candidatos[i].numero);
    }
    printf("Candidatos configurados com sucesso!\n");
    //armazena em um arquivo
    FILE *arq;
    arq = fopen("candidatos.txt", "w");
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return 1;
    }
    for (i = 0; i < num_candidatos; i++) {
        fprintf(arq, "%d,%s\n", candidatos[i].numero, candidatos[i].nome);
    }
    fclose(arq);
    printf("Arquivo salvo com sucesso!\n");
    // **************************************************************************

    // variaveis
    WSADATA wsaData;   // variavel para o winsock                                                        
    int iResult;       // variavel de status
    // sockets para conexao com o servidor
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;
    // estrutura de enderecos
    struct addrinfo *result = NULL;
    struct addrinfo hints;
    // buffers de recepcao e envio
    int iSendResult;
    char recvbuf[TAMANHO_BUFFER];
    int recvbuflen = TAMANHO_BUFFER;     // comprimento do buffer de recepcao  


    // inicializa o winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup falhou com o erro: %d\n", iResult);
        system("pause");
        return 1;
    }

    // pega informacoes da maquina
    char hostname[NI_MAXHOST];        // guarda nome da maquina
    gethostname(hostname, NI_MAXHOST); // pega nome da maquina
    printf("*************\nServidor: Meu nome: %s\n", hostname);

    // pega o ip da maquina e exibe
    PHOSTENT phe = gethostbyname(hostname);

    for (i = 0; phe->h_addr_list[i] != 0; ++i) {
        struct in_addr addr;
        memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
        printf("Servidor: Meu endereco IP (%d): %s\n", i, inet_ntoa(addr));

    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve o endereco do servidor e a porta 
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo falhou com erro: %d\n", iResult);
        WSACleanup();
        system("pause");
        return 1;
    }

    // Cria um socket para conexao com o servidor
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket falhou com erro: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        system("pause");
        return 1;
    }

    // Configura o socket TCP do servidor
    iResult = bind(ListenSocket, result->ai_addr, (int) result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind falhou com erro: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        system("pause");
        return 1;
    }


    freeaddrinfo(result);

    // Escuta aguardando um cliente 
    printf("*************\nServidor: Estou aguardando um cliente na porta %s...\n", DEFAULT_PORT);
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen falhou com erro: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        system("pause");
        return 1;
    }

    // Aceita o socket com o cliente
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept falhou com erro: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        system("pause");
        return 1;
    }

    // printf("Cliente aceito: %s\n",inet_ntoa(addr));

    // No longer need server socket
    closesocket(ListenSocket);

    // *********************************************************************
    // Estabeleceu conexao com o cliente e pode trocar dados
    int votosBrancos = 0;
    do {

        int resultado[3];
        char mensagem[3];

        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Voto recebido: \"%s\"\n", recvbuf);

            // pega os dois primeiros caracteres do buffer
            char voto[3];
            voto[0] = recvbuf[0];
            voto[1] = recvbuf[1];
            voto[2] = '\0';
            //copia voto para recvbuf
            strcpy(recvbuf, voto);
            //tranforma o voto em inteiro
            int votoInt = atoi(recvbuf);

            // verifica se o voto eh valido
            int votoValido = 0;
            for (i = 0; i < num_candidatos; i++) {
                if (votoInt == candidatos[i].numero) {
                    votoValido = 1;
                    candidatos[i].votos++;
                    printf("Voto valido recebido!\n");
                    break;
                }
            }
            if (votoValido == 0) {
                printf("Voto invalido recebido!\n");
                votosBrancos++;
            }


            mensagem[0] = resultado[0];
            mensagem[1] = resultado[1];
            mensagem[2] = resultado[2];

            printf("mensagem: %d %d %d\n", mensagem[0], mensagem[1], mensagem[2]);
            iSendResult = send(ClientSocket, mensagem, iResult, 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                system("pause");
                return 1;
            }
            //exibe o resultado atual da votacao
            printf("*************\nResultado atual da votacao:\n");
            for (i = 0; i < num_candidatos; i++) {
                printf("%s: %d\n", candidatos[i].nome, candidatos[i].votos);
            }
            printf("Votos brancos e nulos: %d\n", votosBrancos);
            printf("*************\n");

        } else if (iResult == 0)
            printf("Fechando conexao...\n");
        else {
            printf("recv falhou com o erro: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            system("pause");
            return 1;
        }

    } while (iResult > 0);

    // *********************************************************************
    // Encerra a conexao
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        system("pause");
        return 1;
    }

    // cleanup
    printf("Encerrando socket...");
    closesocket(ClientSocket);
    WSACleanup();

    system("pause");
    return 0;
}

