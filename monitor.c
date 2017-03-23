/*
 *  Trabalho prático 2: SNMP-DCC
 *
 *  Autores:    Felipe Tadeu Costa Ribeiro    - 2013066028
 *              Tomás Henrique Batista Aguiar - 2013066346
 *
 *  Professores:    Daniel Fernandes Macedo
 *                  Marcos Augusto Menezes Viera
 *                  Lucas Augusto Maia da Silva
 *
 *  Implementação do programa do Monitor
 *
 *  Forma de executar:  makefile                                #Gera todos os executáveis (agente e monitor).
 *                      ./monitor [porta_monitor] 				#Executa o programa presente.
 *
 *  Nota-se que argc = 2
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <strings.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#include "snmpdcc.h"

int main(int argc, char **argv)
{
	/*	Verifica se a quantidade de argumentos está correta	*/
    if(argc != 2)
    {
        fprintf(stdout, "Utilização:\n");												/*	Imprime mensagem de erro	*/
        fprintf(stdout, "\t./monitor [porta]\n");										/*	e mostra forma correta de executar o programa	*/
        return EXIT_FAILURE;															/*	Encerra o programa com sinal de erro	*/
    }

	/*	Cria o socket (endpoint) para comunicar-se	*/
    int sockfd = socket(AF_INET6, SOCK_DGRAM, 0);										/*	Socket criado como IPv6 e UDP (rede e transporte)	*/
    if(sockfd == -1)																	/*	Testa erro na criação do socket	*/
    {
        perror("Falha ao criar socket:");												/*	Imprime esse erro e	*/
        return EXIT_FAILURE;															/*	encerra o programa com sinal de erro	*/
    }

    uint16_t port = atoi(argv[1]);														/*	Variável que guarda a porta do endpoint (socket)	*/

	/*	Coloca as informações do monitor em uma struct	*/
    struct sockaddr_in6 mon;															/*	Struct que guarda o formato do endereço do monitor	*/
    bzero(&mon, sizeof(mon));															/*	Inicia a struct com os campos zerados	*/

    mon.sin6_family = AF_INET6;															/*	Protocolo de rede utilizado: IPv6	*/
    mon.sin6_port = htons(port);														/*	Variável que recebe a porta do endpoint do monitor	*/
    mon.sin6_addr = in6addr_any;														/*	Variável que recebe o endereço local	*/

    socklen_t mlen = sizeof(struct sockaddr_in6);										/*	Variável que recebe o tamanho da struct	*/

	/*	Liga o socket (ref.: sockfd) ao endereço local da máquina	*/
    if(bind(sockfd, (struct sockaddr *) &mon, mlen) == -1)								/*	Verifica erro nessa ligação	*/				
    {
        perror("Falha em bind:");														/*	Se houve erro, imprime-o na tela	*/
		close(sockfd);																	/*	Fecha o socket do monitor	*/
        return EXIT_FAILURE;															/*	Encerra o programa com sinal de erro	*/
    }

	char req_buffer[7];																	/*	Variável que recebe "request" do agente	*/

	struct sockaddr_in6 agnt;															/*	Struct que receberá as informações do agente	*/

	char buffer[MAX_SIZE] = {0};														/*	Buffer que enviará a resposta para o agente	*/
	
	/*	Loop indefinidamente	*/
	while(1)																			/*	Loop até que um sinal de interrupção seja recebido	*/
	{
		fprintf(stdout, "Waiting incomming requests...\n");
		bzero(&agnt, sizeof(agnt));														/*	Zera o espaço alocado para a struct do agente	*/
		socklen_t alen = sizeof(agnt);													/*	Variável que recebe o tamanho da struct do agente	*/

		memset(req_buffer, 0, sizeof(req_buffer));										/*	Zera o buffer de recebimento de "request"	*/

		/*	Recebe a mensagem do socket do agente e verifica erro de recebimento. Mensagem = "request"	*/
		uint8_t req_len = recvfrom(sockfd, req_buffer, sizeof(req_buffer), 0, (struct sockaddr *) &agnt, &alen);
		if(req_len < 0)
		{
			perror("recvfrom-request: ");												/*	Imprime erro e	*/	
			close(sockfd);																/*	Fecha o socket do monitor	*/
			return EXIT_FAILURE;														/*	encerra o programa com sinal de erro	*/
		}
		req_buffer[req_len] = 0;														/*	Encerra o buffer na posição req_len	*/

		/*	Compara o buffer recebido com o esperado	*/
		if(!strcmp(req_buffer, "request"))												/*	Se são iguais	*/
		{
			char agnt_addr[40];															/*	Cria variável para imprimir o endereço do agente	*/
			inet_ntop(AF_INET6, &agnt.sin6_addr, agnt_addr, sizeof(agnt_addr));			/*	Coloca o endereço na variável	*/
			
			uint16_t agnt_port = ntohs(agnt.sin6_port);									/*	Variével que recebe a porta do agente	*/

			/*	Imprime o endereço do agente e sua respectiva porta	*/
			fprintf(stdout, "New request from ip: %s on port: %d.\n", agnt_addr, agnt_port);

			fprintf(stdout, "Sending information to it ^^.\n");
			
			memset(buffer, 0, sizeof(buffer));											/*	Zera o espaço alocado para o buffer de envio	*/

			/*	Recupera as informações da máquina e verifica erro no processo	*/
			if(getNodeInfo(buffer) == -1)												/*	Se houve erro	*/		
			{
				fprintf(stderr, "Erro ao recuperar informações desta máquina.\n");		/*	Imprime que houve erro	*/
				close(sockfd);															/*	Fecha o socket do monitor	*/
				return EXIT_FAILURE;													/*	Encerra o programa com sinal de erro	*/
			}

			/*	Envia a resposta (answer) ao agente e verifica erro de envio	*/
			if(sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &agnt, alen) == -1)
			{
				perror("sendto: ");														/*	Se houve erro, imprime este	*/
				close(sockfd);															/*	Fecha o socket do monitor	*/
				return EXIT_FAILURE;													/*	Encerra o programa com sinal de erro	*/
			}
		}
	}

	
	close(sockfd);																		/*	Nunca chegará a ser executado	*/

	return EXIT_SUCCESS;																/*	Nunca chegará a ser executado	*/
}

