/*  SIMULADOR DE MEMÓRIA VIRTUAL
*   ALUNOS:
*   AURÉLIO SHIZUO MATSUNAGA
*   JÚLIO DO LAGO MÜELLER
*   TADS NOITE
 */

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <string.h>

using namespace std;

typedef struct Process
{
    string Name;
    int Size;
    int NumberOfPages;
}PROCESS;

typedef struct Page
{
    string Address;
    Page *next;
}PAGE;

int memorySize, pageSize, numberOfPages, usedPages = 0, writes = 0, reads = 0, hits=0, misses=0, writebacks=0;
float faults=0;
PAGE *first, *last;

void AddNewPage(string address){
    PAGE *current;
	if ((current = (PAGE*)malloc(sizeof(PAGE))))
    {
        current->Address = address;
        current->next = NULL;
        if(usedPages == 0){
            first = current;
            last = first;
        }
        else {
            last->next = current;
            last = current;
        }
        
        if(usedPages < numberOfPages)
            usedPages++;
        
        writes++;
    }
    else
    {
        cout << "Erro de alocacao!" << endl;
    }
}

void LRU(string address){
	AddNewPage(address);
	if(usedPages == numberOfPages)
		first = first->next;
}

void LRU_BIT(string address){
	AddNewPage(address);
	if(usedPages == numberOfPages)
		first = first->next;
}

void FIFO(string address){
	AddNewPage(address);
	if(usedPages == numberOfPages)
		first = first->next;
}

bool Find(string address, string algType){
	Page *tmp = first, *prev = NULL;
	while(tmp != NULL){
		if(tmp->Address.compare(address) == 0){
			if(algType.compare("LRU") == 0){
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

void ReplacePage(string address, string algType){
	if(algType.compare("LRU") == 0){
		LRU(address);
	}
	else if(algType.compare("LRU_BIT") == 0){
		LRU_BIT(address);
	}
	else if(algType.compare("FIFO") == 0){
		FIFO(address);
	}
	writebacks++;
}

void WriteAddress(string address, string algType){
    cout << "Numero de paginas usadas: " << usedPages << "/" << numberOfPages << endl;
    cout << "Escrevendo no endereco " << address << " - ";
	if(usedPages < numberOfPages){
		AddNewPage(address);
        cout << "sucesso!" << endl;
	}
	else
    {
		faults++;
        cout << "erro!" << endl;
		ReplacePage(address, algType);
	}
}

void SimulateVirtualMemory(string algType)
{
    cout << "Simulando memoria utilizando o algoritmo: " << algType << endl;

    int numberOfProcesses = 0;

    vector<PROCESS> processes;
    
    ifstream infile("processos.txt");

    while (!infile.eof())
    {
        string name, operation, unit;
        
        infile >> name >> operation;

        if (operation.compare("C") == 0)
        {
            int processSize;
            
            int numPages;

            infile >> processSize >> unit;

            cout << "Processo " << name << " criado - Tamanho: " << processSize << " " << unit << endl;

            if (unit.compare("KB") == 0)
            {
                numPages = processSize / pageSize;
            }
            else if (unit.compare("MB") == 0)
            {
                numPages = (processSize * 1000) / pageSize;
            }
            else
            {
                cout << "Unidade " << unit << " nao suportada!" << endl;
                continue;
            }

            if (processSize % pageSize)
            {
                numPages +=1;
            }
            
            PROCESS p{name, processSize, numPages};

            processes.push_back(p);

            numberOfProcesses++;
        }
        else
        {
            string address;

            infile >> address;

            if (operation.compare("W") == 0)
            {
                cout << name << " escrita em memoria" << endl;
                WriteAddress(address, algType);
            }
            else if (operation.compare("R") == 0)
            {
                cout << name << " leitura na posicao " << address << " de memoria - ";

                if (Find(address, algType))
                {
                    hits++;
                    cout << "encontrado!" << endl;
                }
                else
                {
                    misses++;
                    cout << "nao encontrado!" << endl;
                    WriteAddress(address, algType);
                }
                reads++;
            }
        }
    }

    float pFaults = faults/writes;

    cout << "Operacoes de leitura: " << reads << endl;
    cout << "Operacoes de escrita: " << writes << endl;
    cout << "Page hits: " << hits << endl;
    cout << "Page misses: " << misses << endl;
    cout << "Writebacks: " << writebacks << endl;
    cout << "Page fault: " << pFaults << "%" << endl;

    infile.close();
    
    processes.clear();
}

void FreeMemory(){
	Page *tmp = first;
	while(tmp != NULL){
		free(tmp);
		tmp = tmp->next;
	}
}

int main(int argc, char *argv[])
{
    string pSize, mSize;
    pageSize = atoi(argv[1]);
    memorySize = atoi(argv[2]);

    if (pageSize <= 0 || memorySize <= 0)
    {
        cout << "ERRO: Argumentos invalidos!";
        return 1;
    }
    else if (memorySize % pageSize != 0)
    {
        cout << "ERRO: Argumentos devem ser multiplos entre si!";
        return 1;
    }
    else if (memorySize < pageSize)
    {
        return 1;
        cout << "ERRO: Memoria deve ser maior que o quadro!";
    }

    numberOfPages = memorySize / pageSize;

    cout << "Tamanho da memoria: " << memorySize << endl;
    cout << "Tamanho dos quadros: " << pageSize << endl;
    cout << "Quantidade de quadros: " << numberOfPages << endl;

    SimulateVirtualMemory("LRU");
    usedPages = 0, faults = 0, writes=0, reads = 0, hits=0, misses=0, writebacks=0, faults = 0;
    FreeMemory();

    SimulateVirtualMemory("LRU_BIT");
    usedPages = 0, faults = 0, writes=0, reads = 0, hits=0, misses=0, writebacks=0, faults = 0;
    FreeMemory();

    SimulateVirtualMemory("FIFO");
    usedPages = 0, faults = 0, writes=0, reads = 0, hits=0, misses=0, writebacks=0, faults = 0;
    FreeMemory();
    return 0;
}