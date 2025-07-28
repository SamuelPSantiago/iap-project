#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "MAIN.h"
#include "UF.h"
#include "ELECTION.h"
#include "CITIZEN.h"
#include "CANDIDATE.h"
#include "VOTE.h"
#include "ATTENDANCE.h"
#include "RESULT.h"

int main()
{
    menu();

    loadUFs();
    loadElections();
    loadCitizens();
    loadCandidates();
    loadVotes();
    loadAttendances();

    int op;
    do
    {
        scanf("%d", &op);
        cleanerKeyboard();

        switch (op)
        {
        case 1:
            printf("\n");
            stateMachineUF();
            menu();
            break;
        case 2:
            printf("\n");
            stateMachineElection();
            menu();
            break;
        case 3:
            printf("\n");
            stateMachineCitizen();
            menu();
            break;
        case 4:
            printf("\n");
            stateMachineCandidate();
            menu();
            break;
        case 5:
            printf("\n");
            stateMachineVote();
            menu();
            break;
        case 6:
            printf("\n");
            stateMachineResult();
            menu();
            break;
        case 0:
            break;
        default:
            printf("Opcao invalida. Tente novamente:");
        }
    } while (op != 0);
}

void menu()
{
    printf("+--------------------------------------------------+\n");
    printf("| Menu do Programa                                 |\n");
    printf("+---+----------------------------------------------+\n");
    printf("| 1 | UF                                           |\n");
    printf("| 2 | ELEICAO                                      |\n");
    printf("| 3 | PESSOA                                       |\n");
    printf("| 4 | CANDIDATO                                    |\n");
    printf("| 5 | VOTO                                         |\n");
    printf("| 6 | RESULTADO                                    |\n");
    printf("| 0 | SAIR                                         |\n");
    printf("+---+----------------------------------------------+\n");
    printf("Escolha uma opcao: ");
}
void ensureDataDir()
{
    struct stat st = {0};
    if (stat("data", &st) == -1)
    {
        if (mkdir("data") != 0)
        {
            printf("Erro ao criar diretorio data\n");
            exit(-1);
        }
    }
}
void cleanerKeyboard()
{
    while (getchar() != '\n')
        ;
}
void formatBigString(const char *str, size_t width, char *outBuf, size_t outBufSize)
{
    size_t len = strlen(str);
    if (len > width && width > 3)
        snprintf(outBuf, outBufSize, "%.*s...", (int)(width - 3), str);
    else if (len > width)
    {
        size_t dots = width < outBufSize - 1 ? width : outBufSize - 1;
        memset(outBuf, '.', dots);
        outBuf[dots] = '\0';
    }
    else
        snprintf(outBuf, outBufSize, "%s", str);
}
void freeMemory()
{
    freeUFs();
    freeElections();
    freeCitizens();
    freeCandidates();
}