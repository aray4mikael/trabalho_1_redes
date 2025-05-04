# Sistema de Votação - Trabalho de Redes de Computadores

Este projeto implementa um sistema de votação distribuído usando sockets TCP/IP, desenvolvido como parte do trabalho de Redes de Computadores. O sistema consiste em um servidor que gerencia os votos e clientes que permitem aos usuários votarem nos candidatos.

## Funcionalidades

- Sistema cliente-servidor baseado em TCP/IP
- Gerenciamento de múltiplos candidatos
- Interface de votação para clientes
- Contagem e exibição de resultados
- Suporte a múltiplos clientes simultâneos

## Requisitos

- Sistema operacional Windows
- Compilador GCC
- Bibliotecas de rede do Windows (wsock32, ws2_32)

## Compilação

### Servidor
```bash
gcc main.c -lwsock32 -lws2_32 -o servidor
```

### Cliente
```bash
gcc main.c -lwsock32 -lws2_32 -o cliente
```

## Configuração

1. O servidor utiliza a porta 8888 por padrão
2. O cliente precisa do endereço IP do servidor para se conectar
3. O arquivo `candidatos.txt` deve ser configurado com os dados dos candidatos

## Uso

1. Inicie o servidor primeiro:
   ```bash
   ./servidor
   ```

2. Configure o número de candidatos e seus dados no servidor

3. Inicie o cliente:
   ```bash
   ./cliente
   ```

4. Digite o IP do servidor e a porta quando solicitado

5. Siga as instruções na interface para votar

## Estrutura do Projeto

- `servidor/`: Contém o código do servidor
  - `main.c`: Implementação do servidor
  - `candidatos.txt`: Arquivo de configuração dos candidatos

- `cliente/`: Contém o código do cliente
  - `main.c`: Implementação do cliente
  - `candidatos.txt`: Arquivo de configuração dos candidatos

## Observações

- O sistema foi desenvolvido para Windows
- Certifique-se de que o firewall permita a comunicação na porta 8888
- O servidor deve ser iniciado antes dos clientes
