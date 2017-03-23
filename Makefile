#Makefile - TP2_SNMP-DCC
#	Autores:	Felipe Tadeu Costa Ribeiro    - 2013066028
#			Tomás Henrique Batista Aguiar - 2013066346

all: snmpdcc agente monitor clean

snmpdcc: snmpdcc.h
	#Gera o arquivo objeto para snmpdcc
	gcc -Wall -Werror -pedantic -std=c11 -c snmpdcc.c						

agente: snmpdcc.o agente.c
	#Gera o executável do programa do agente
	gcc -Wall -Werror -pedantic -std=c11 snmpdcc.o agente.c -o agente		

monitor: snmpdcc.o monitor.c
	#Gera o executável do programa do monitor
	gcc -Wall -Werror -pedantic -std=c11 snmpdcc.o monitor.c -o monitor		

clean:
	#Remove os arquivos objeto
	rm *.o						
	#Mostra os arquivos (em lista) da pasta atual
	ls -l						
