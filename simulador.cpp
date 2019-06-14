/*  SIMULADOR DE MEMÓRIA VIRTUAL
*   ALUNOS:
*   AURÉLIO SHIZUO MATSUNAGA
*   JÚLIO DO LAGO MÜELLER
*   TADS NOITE
 */

#include <string>
#include <fstream>
#include <iostream>

using namespace std;

struct Process
{
    string Name;
    unsigned int *Pages;
    unsigned long int Size;
    unsigned int NumberOfPages;
} typedef PROCESS;

unsigned long int NormalizeUnit(unsigned long int size, string unit)
{
    if (unit.compare("B") == 0) return size;
    else if (unit.compare("KB") == 0) return size * 1000;
    else if (unit.compare("MB") == 0) return size * 1000000;
    else if (unit.compare("GB") == 0) return size * 1000000000;
    exit (-1);
}

int CountProcesses()
{
    unsigned int address;
    unsigned long int processSize;
    string name, operation, unit;
    ifstream infile("processos.txt");
    int numProcesses = 0;
    while (!infile.eof())
    {
        infile >> name >> operation;

        if (operation.compare("C") == 0)
        {
            infile >> processSize >> unit;
            numProcesses++;
        }
        else
        {
            infile >> unit;
        }
    }
    infile.close();
    return numProcesses;
}

void AlocateProcesses(PROCESS *process)
{
    unsigned int address;
    unsigned long int processSize;
    string name, operation, unit;
    ifstream infile("processos.txt");
    int i = 0;
    while (!infile.eof())
    {
        infile >> name >> operation;

        if (operation.compare("C") == 0)
        {
            infile >> processSize >> unit;
            process[i].Size = NormalizeUnit(processSize, unit);
            i++;
        }
        else
        {
            infile >> unit;
        }
    }
    infile.close();
    int numProcesses = i;
    for (i = 0; i < numProcesses; i++)
    {
        process[i].NumberOfPages = process[i].Size / pageSize;
        if ((process[i].Size % pageSize) > 0)
        {
            process[i].NumberOfPages += 1;
        }
        process[i].Pages = (unsigned int*)malloc(sizeof(unsigned int) * process[i].NumberOfPages);
    }
}

int memorySize, pageSize, frameQuantity;

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

    frameQuantity = memorySize / pageSize;

    cout << "Tamanho da memoria: " << memorySize << endl;
    cout << "Tamanho dos quadros: " << pageSize << endl;
    cout << "Quantidade de quadros: " << frameQuantity << endl;

    int numProcesses = CountProcesses();

    PROCESS *processes;

    processes = (PROCESS*)malloc(sizeof(PROCESS) * numProcesses);

    AlocateProcesses(processes);

    return 0;
}

