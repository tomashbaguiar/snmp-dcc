#include <stdio.h>      //fprintf,
#include <stdlib.h>     //system,
#include <sys/types.h>  //open,
#include <sys/stat.h>   //open,
#include <fcntl.h>      //open,
#include <string.h>		//strcpy,
#include <unistd.h>		//read,

#include "snmpdcc.h"

int
getNodeInfo(char *buffer) 
{
	/*	Cria arquivo de recuperação de informações	*/
	system("touch nodeinfo.txt");

    /*	Recupera o HOSTNAME do nó	*/
	system("echo \"HOSTNAME: \" > nodeinfo.txt");
	system("hostname >> nodeinfo.txt");     							/*	Coloca o nome do nó em um arquivo	*/

	/*	Recupera a hora atual no nó	*/
	system("echo \"DATE AND TIME: \" >> nodeinfo.txt");
	system("date >> nodeinfo.txt");										/*	Coloca a data e horário em um arquivo	*/

	/*	Recupera o UpTime do nó	*/
	system("echo \"UPTIME: \" >> nodeinfo.txt");
    system("uptime -p >> nodeinfo.txt");  								/*	Coloca o tempo em que o nó está ativo em um arquivo	*/

	/*	Recupera o endereço IPv6 do nó	*/
	system("echo \"IPv6: \" >> nodeinfo.txt");
    system("ip -6 addr | grep \"scope link\" >> nodeinfo.txt");			/*	Coloca o endereço IPv6 do nó em um arquivo	*/

	/*	Recupera o endereço IPv4 do nó	*/
	system("echo \"IPv4: \" >> nodeinfo.txt");
    system("ip -4 addr | grep \"scope global\" >> nodeinfo.txt");		/*	Coloca o endereço IPv4 do nó em um arquivo	*/

	/*	Recupera as informações de utiização de discos do nó	*/
	system("echo \"DISK USAGE: \" >> nodeinfo.txt");
    system("df -kh >> nodeinfo.txt");     								/*	Coloca as informações de disco do nó em um arquivo	*/

	int ret = handleNodeInfo("nodeinfo.txt", buffer);					/*	Chama a função que coloca no buffer as informações da máquina	*/

	/*	Remove pasta e arquivos	*/
	system("rm nodeinfo.txt");

	return ret;	
}

int
handleNodeInfo(const char *file, char *target)
{
	int fd = open(file, O_RDONLY);										/*	Abre o arquivo em modo de leitura	*/
	if(fd == -1)														/*	Verifica se o arquivo foi aberto corretamente	*/
	{
		perror("open-file: ");											/*	Em caso negativo, imprime erro	*/
		return fd;														/*	e retorna erro */
	}

	memset(target, 0, sizeof(char)*MAX_SIZE);							/*	Coloca caracteres nulos no espaço reservado para o buffer	*/

	int len = read(fd, target, MAX_SIZE);								/*	Lê do arquivo as informações da máquina	*/
	target[len] = '\0';													/*	Encerra o buffer	*/

	close(fd);															/*	Fecha o arquivo aberto	*/

	return len;															/*	Retorna a quantidade de caracteres escritos	*/
}

void
printSNMP(const char *received, const char *from)
{
	fprintf(stdout, "Received answer from %s.\n\n", from);				/*	Imprime de quem foram recebidas as informações	*/
	fprintf(stdout, "Monitor info:\n%s\n", received);					/*	Imprime as informações recebidas do monitor	*/
}

