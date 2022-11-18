//****************************************************************************
// Comunicacao de Dados
// Professor: Gustavo Kunzel
// Cliente TCP simples
// Implementa um cliente TCP que envia pacotes para servidores
// diferentes em uma porta
// Nota: a biblioteca -lws2_32 deve ser incluida no linker para funcionar
//     no projeto do DEV jÃ¡ foi incluido atraves das opcoes de projeto
//****************************************************************************

#define _WIN32_WINNT 0x0501

#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// **************************************************************************
// definicoes iniciais
#define TAMANHO_BUFFER 512          // tamanho do buffer de recepcao
#define DEFAULT_PORT "8888"        // porta de comunicacao
#define IP_SERVIDOR "192.168.18.4"     // ip do servidor
// **************************************************************************

//declarar variavel ip_servidor
char ip_servidor[16] = IP_SERVIDOR;

// read ip_servidor from scanf
void read_ip_servidor(char *ip_servidor) {
    printf("Digite o IP do servidor: ");
    scanf("%s", ip_servidor);
}

//define variavel da porta
char porta[6];

// read porta from scanf
void read_porta(char *porta) {
    printf("Digite a porta: ");
    scanf("%s", porta);
}

//struct candidato
typedef struct {
    int id;
    char nome[50];
    int numero;
    int votos;
} Candidato;


int main(int argc, char **argv) {
    read_ip_servidor(ip_servidor);
    read_porta(porta);
    printf("IP: %s Porta: %s", ip_servidor, porta);
    // variaveis
    WSADATA wsaData;                  // variavel para o winsock
    int iResult;                      // variavel de status
    // sockets para conexao com o servidor
    SOCKET ConnectSocket = INVALID_SOCKET;
    // estrutura de enderecos
    struct addrinfo *result = NULL,
            *ptr = NULL,
            hints;
    // buffers de recepcao e envio
    char sendbuf[TAMANHO_BUFFER];
    char recvbuf[TAMANHO_BUFFER];
    int recvbuflen = TAMANHO_BUFFER;  // comprimento do buffer de recepcao

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
    printf("*************\nCliente: Meu nome: %s\n", hostname);

    // pega o ip da maquina e exibe
    PHOSTENT phe = gethostbyname(hostname);
    int i;
    for (i = 0; phe->h_addr_list[i] != 0; ++i) {
        struct in_addr addr;
        memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
        printf("Cliente: Meu endereco IP (%d): %s\n", i, inet_ntoa(addr));

    }

    // inicializa o tipo de conexao
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;    // datagramas TCP
    hints.ai_protocol = IPPROTO_TCP;

    // resolve o endereco e porta do servidor
    iResult = getaddrinfo(ip_servidor, porta, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo falhou com o erro: %d\n", iResult);
        WSACleanup();
        system("pause");
        return 1;
    }

    // tenta conectar ao endereco do servidor
    printf("*************\nTentando se conectar ao servidor de endereco %s na porta %s...\n", ip_servidor, porta);
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // cria um socket para se conectar ao servidor
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("Socket falhou com o erro: %ld\n", WSAGetLastError());
            WSACleanup();
            system("pause");
            return 1;
        }

        // tenta conecta ao servidor
        iResult = connect(ConnectSocket, ptr->ai_addr, (int) ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Nao foi possivel se conectar ao servidor!\n");
        system("pause");
        WSACleanup();
        return 1;
    }

    // *********************************************************************
    // Estabeleceu conexao com o servidor e pode trocar dados


//ler os candidatos do arquivo
    FILE *arq2;
    arq2 = fopen("candidatos.txt", "r");
    if (arq2 == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return 1;
    }
    char linha[100];
    int num_candidatos = 0;
    while (fgets(linha, 100, arq2) != NULL) {
        num_candidatos++;
    }
    fclose(arq2);
    Candidato candidatos[num_candidatos];
    arq2 = fopen("candidatos.txt", "r");
    if (arq2 == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return 1;
    }
    i = 0;
    while (fgets(linha, 100, arq2) != NULL) {
        char *token;
        token = strtok(linha, ",");
        candidatos[i].numero = atoi(token);
        token = strtok(NULL, ",");
        strcpy(candidatos[i].nome, token);
        candidatos[i].id = i;
        candidatos[i].votos = 0;
        i++;
    }
    fclose(arq2);
    //imprime candidatos
    printf("Candidatos:\n");
    for (i = 0; i < num_candidatos; i++) {
        printf("Nome: %s Numero: %d\n", candidatos[i].nome, candidatos[i].numero);
    }





    i = 0;
    while (!kbhit()) {
        int numeroCandidato = 0;
        Candidato candidato;
        candidato.numero = 0;
        candidato.nome[0] = '\0';
        candidato.id = 0;
        printf("\n\nDigite o numero do candidato: ");
        scanf("%d", &numeroCandidato);
        // varifica se o candidato está no vetor de candidatos
        for (int i = 0; i < num_candidatos; i++) {
            if (candidatos[i].numero == numeroCandidato) {
                candidato = candidatos[i];
                break;
            }
        }
        // se o candidato não estiver no vetor de candidatos
        if (candidato.numero == 0) {
            printf("Candidato nao encontrado!\n deseja alterar? (s/n)");
            char opcao;
            scanf("%s", &opcao);
            if (opcao == 's') {
                continue;
            }

        }else{
            printf("Candidato: %s\n", candidato.nome);
        }

        printf("Pressione enter para enviar o voto...");
        scanf("%d", &i);
        // envia o voto
        char mensagem[10];
        //mensagem recebe o numero do candidato
        sprintf(mensagem, "%d", candidato.numero);



        printf("Enviando \"Sou o cliente %s e envio o recado %s - no. %04d\"...\n",hostname, mensagem,i);

        // monta msg com um numero e o nome da maquina em forma de string
        sprintf(sendbuf, mensagem, hostname, i);
        // envia a mensagem
        iResult = send(ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
        if (iResult == SOCKET_ERROR) {
            printf("Falhou o envio com o erro: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            system("pause");
            return 1;
        }

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        printf("recv failed with error: %d\n", WSAGetLastError());
        if (recvbuflen > 0) {
            printf("Voto recebido!");
            //aguarda 10 segundos
            Sleep(10000);
            //limpa a tela
            system("cls");
        } else if (iResult == 0) {
            printf("Connection closed\n");
        } else {
            printf("recv failed with error: %d\n", WSAGetLastError());
        }


        i++;
        Sleep(1000);


    }

    // *********************************************************************
    // Encerra a conexao
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("Falhou encerrar a conexao com o erro: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        system("pause");
        return 1;
    }



    // cleanup
    printf("Encerrando socket...");
    closesocket(ConnectSocket);
    WSACleanup();

    system("pause");
    return 0;
}


