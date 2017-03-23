/*
 *	Trabalho prático 2: SNMP-DCC
 *
 *	Autores:	Felipe Tadeu Costa Ribeiro 	  -	2013066028
 *				Tomás Henrique Batista Aguiar -	2013066346
 *
 *	Professores:	Daniel Fernandes Macedo
 *					Marcos Augusto Menezes Viera
 *					Lucas Augusto Maia da Silva
 *
 *	Implementação do programa do Agente
 *
 *	Forma de executar:	makefile								#Gera todos os executáveis (agente e monitor).
 *						./agente [ipv6_monitor] [porta_monitor]	#Executa o programa presente.
 *
 *	Nota-se que argc = 3
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>

#include "snmpdcc.h"

int main(int argc, char **argv)
{	
	/*	Verifica se a quantidade de argumentos está correta	*/
	if(argc != 3)																
	{
		fprintf(stderr, "Utilização:\n");												/*	Imprime mensagem de erro	*/
		fprintf(stderr, "\t./agente [end_ipv6_monitor] [porta_monitor]\n");				/*	Mostra forma correta de executar o programa	*/
		return EXIT_FAILURE;															/*	Encerra o programa com sinal de erro	*/
	}

	/*	Cria o socket pelo qual acontecerá a comunicação	*/
	int sockfd = socket(AF_INET6, SOCK_DGRAM, 0);										/* 	Socket criado como IPv6 e UDP (rede e transporte)	*/
	if(sockfd == -1)																	/*	Se houve erro na criação do socket	*/
	{
		perror("Criação de socket: ");													/*	Imprime esse erro e */
		return EXIT_FAILURE;															/*	sai do programa com sinal de erro	*/
	}

	uint16_t port = atoi(argv[2]);														/*	Variável que guarda a porta utilizada pelo monitor	*/

	/*	Coloca as informações do monitor em uma struct	*/
	struct sockaddr_in6 mon;															/*	Struct que guarda o formato do endereço do monitor	*/
	bzero(&mon, sizeof(mon));															/*	Coloca zero nos campos da struct	*/
	socklen_t mlen = sizeof(mon);														/*	Variável que recebe o tamanho da struct do monitor	*/
	mon.sin6_family = AF_INET6;															/*	Protocolo de rede usado: IPv6	*/
	inet_pton(AF_INET6, argv[1], &mon.sin6_addr);										/*	Variável "in6_addr" recebe endereço do monitor	*/
	mon.sin6_port = htons(port);														/*	Variável que recebe o número da porta do monitor	*/

	const char req[] = "request";														/* Variável constante que recebe a string "request"	*/

	fprintf(stdout, "Sending %s...\n", req);

	/*	Envia "request" para o monitor e verifica erros de envio	*/
	if(sendto(sockfd, req, strlen(req), 0, (struct sockaddr *) &mon, mlen) == -1)
	{
		perror("sendto failed: ");														/*	No caso de erro de envio, imprime o erro	*/
		return EXIT_FAILURE;															/*	e encerra o agente com sinal de erro	*/
	}


	fprintf(stdout, "Waiting answer...\n");

	char buffer[MAX_SIZE + 6];															/*	Cria buffer que receberá a resposta do monitor	*/
	memset(buffer, 0, sizeof(buffer));													/*	Coloca zero na memória do buffer	*/

	/*	Recebe a resposta do monitor e verifica erros de recebimento	*/
	int len = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &mon, &mlen);
	if(len <= 0)																			
	{
		if(len == 0)																	/*	Verifica se erro foi fechamento do monitor	*/
		{
			perror("Peer has been shuted down: ");										/*	Imprime que o monitor foi fechado	*/
			return EXIT_FAILURE;														/*	Encerra o agente com sinal de erro	*/
		}
		else																			/*	Erro foi de recebimento	*/
		{
			perror("sendto failed: ");													/*	Imprime tal erro	*/
			return EXIT_FAILURE;														/*	e fecha o agente com sinal de erro	*/
		}
	}
	buffer[len] = 0;																	/*	Coloca final de string no buffer posição len	*/

	/*	Imprime a resposta recebida na tela	*/
	printSNMP(buffer, argv[1]);

	fprintf(stdout, "See you later!\n");												/*	Imprime na tela uma saudação de fechamento	*/

	close(sockfd);																		/*	Fecha o socket	*/

	return EXIT_SUCCESS;																/*	Encerra o agente com sinal de sucesso	*/
}

