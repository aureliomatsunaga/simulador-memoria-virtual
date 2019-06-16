// SIMULADOR DE MEMÓRIA VIRTUAL
// ALUNOS: AURÉLIO SHIZUO MATSUNAGA
//         JÚLIO DO LAGO MÜELLER

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "stdbool.h"

struct Page 
{
	char address[9];
	Page *next;	
} typedef Page;

struct Process
{
	char Name[4];
	unsigned long int Size;
	int numPages;
} typedef PROCESS;


char *alg, *filePath, *tmpLine, line[20], tmpAddress[9], proc[3];
int pageSize, memSize, numPages, operations=0, reads=0, writes=0, hits=0, misses=0, writebacks=0, usedPages=0;
float faults=0;
FILE *file;
Page *first, *last;

void AddNewPage(char value[9]){
	Page *current = (Page*)malloc(sizeof(Page));
	strcpy(current->address, value);
	current->next = NULL;
	
	if(usedPages == 0){
		first = current;
		last = first;
	}
	else {
		last->next = current;
		last = current;
	}
	
	if(usedPages < numPages)
		usedPages++;
	
	writes++;
}

void LRU(char value[9]){
	AddNewPage(value);
	if(usedPages == numPages)
		first = first->next;
}

void FIFO(char value[9]){
	AddNewPage(value);
	if(usedPages == numPages)
		first = first->next;
}

bool Find(char value[9]){
	Page *tmp = first, *prev = NULL;
	while(tmp != NULL){
		if(strcmp(tmp->address, value)==0){
			if(strcmp(alg, "LRU") == 0){
				if(prev != NULL){
					if(tmp->next != NULL)
						prev->next = tmp->next;							
				}
				else {
					first = first->next;
				}
				last->next = tmp;
				last = tmp;
				tmp->next = NULL;
			}

			return true;
		}
		prev = tmp;	
		tmp = tmp->next;
	}
	return false;
}

void ReplacePage(char value[9]){
	if(strcmp(alg, "LRU") == 0){
		LRU(value);
	}
	else if(strcmp(alg, "FIFO") == 0){
		FIFO(value);
	}
	writebacks++;
}

void WriteAddress(char value[9]){
	if(usedPages < numPages){
		AddNewPage(tmpAddress);
	}
	else{
		faults++;
		ReplacePage(tmpAddress);
	}
}

void FreeMemory(){
	Page *tmp = first;
	while(tmp != NULL){
		free(tmp);
		tmp = tmp->next;
	}
	fclose(file);
}

int SimulateVirtualMemory(){
	printf("\nSimulador de memoria virtual utilizando %s\n", alg);
	printf("Tamanho da memoria fisica: %iKB\n", memSize);
	printf("Tamanho das paginas: %iKB\n", pageSize);
	printf("Numero total de paginas: %i\n", numPages);
	if(strlen(filePath) > 0){
		file = fopen(filePath, "r");
		while(fgets(line, 20, file) != NULL){
			operations++;
			tmpLine = line + 5;
			strncpy(tmpAddress, tmpLine, 7);
			tmpAddress[9] = '\0';
			strncpy(proc, line, 2);
			proc[2] = '\0';
			if(line[3] == 'W'){
				printf(proc);
				printf(" escrita no endereco de memoria ");
				printf(tmpAddress);
				WriteAddress(tmpAddress);
			}
			else if(line[3] == 'R'){
				printf(proc);
				printf(" leitura no endereco de memoria ");
				printf(tmpAddress);
				if(Find(tmpAddress)){
					hits++;	
				}
				else{
					misses++;
					WriteAddress(tmpAddress);
				}
				reads++;
			}
			else if (line[3] == 'C'){
				printf(proc);
				printf(" criado");
				printf("\n");
			}	
		}
	}
	else{
		printf("ERRO: Arquivo de entrada inválido.");
		return 0;
	}
	
	printf("\nTotal de operacoes: %i\n", operations);
	printf("Operacoes de leitura: %i\n", reads);
	printf("Operacoes de escrita: %i\n", writes);
	printf("Page hits: %i\n", hits);
	printf("Page misses: %i\n", misses);
	printf("Numero de writebacks: %i\n", writebacks);
	printf("Taxa de page fault: %f%% \n", faults/writes*100);
	
	FreeMemory();
	return 0;
}

int main(int argc, char *argv[]){
	pageSize = atoi(argv[1]);
	memSize = atoi(argv[2]);
	filePath = argv[3];

	if(pageSize <= 0 || memSize <= 0){	
		printf("ERRO: Argumentos invalidos!");
		return 0;
	}
		
	if(memSize % pageSize != 0){
		printf("ERRO: Argumentos devem ser multiplos entre si!");
		return 0;
	}
	if(memSize < pageSize){
		printf("ERRO: Memoria deve ser maior que o tamanho da pagina!");
	}
	
	numPages = memSize/pageSize;

	alg = "LRU";
	SimulateVirtualMemory();
	printf("\nPressione ENTER para continuar\n");
	getchar();

	operations=0, reads=0, writes=0, hits=0, misses=0, writebacks=0, usedPages=0, faults=0;
	alg = "FIFO";
	SimulateVirtualMemory();
	printf("\nPressione ENTER para encerrar\n");
	getchar();
		
	return 0;
}