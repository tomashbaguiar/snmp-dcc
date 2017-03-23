#ifndef SNMPDCC_H
	#define SNMPDCC_H

	#define MAX_SIZE 1024															/*	Tamanho máximo do buffer enviado	*/
	
	/*	Função para adquirir as informações da máquina em que está rodando	*/
	int getNodeInfo(char *buffer);    							

	/*	Função que recebe as informações, as trata e as coloca no buffer	*/
	int handleNodeInfo(const char *file, char *target);		

	/*	Procedimento que imprime as informações recebidas do monitor	*/
	void printSNMP(const char *received, const char *from);

#endif  /* SNMPDCC_H */
