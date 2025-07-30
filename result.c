#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MAIN.h"
#include "VOTE.h"
#include "ATTENDANCE.h"
#include "RESULT.h"

enum MenuOptions {
    showResultR,
    showAttendanceR,
    exitMenuR
};

// Menu controllers
void menuResult()
{
    printf("+--------------------------------------------------+\n");
    printf("| Menu resultados                                  |\n");
    printf("+---+----------------------------------------------+\n");
    printf("| 1 | VER RESULTADO DE UMA ELEICAO                 |\n");
    printf("| 2 | VER COMPARECIMENTO DE UMA ELEICAO            |\n");
    printf("| 0 | SAIR                                         |\n");
    printf("+---+----------------------------------------------+\n");
    printf("Escolha uma opcao: ");
}
void stateMachineResult()
{
    int option;
    do
    {
        menuResult();
        if (scanf("%d", &option) != 1)
        {
            cleanerKeyboard();
            continue;
        }
        cleanerKeyboard();
        printf("\n");
        switch (option)
        {
        case showResultR:
            showElectionResults();
            break;
        case showAttendanceR:
            showElectionAttendance();
            break;
        case exitMenuR: break;
        default:
            printf("Opcao invalida.\n\n");
        }
    } while (option != 0);
}

void showElectionResults() {
    int year, uf;

    printf("Digite o ano da eleicao: ");
    scanf("%d", &year);

    printf("Digite o codigo da UF: ");
    scanf("%d", &uf);
    cleanerKeyboard();

    printf("\n");

    int totalVotes = 0;
    candidateResult *results = NULL;
    int resultsCount = 0, resultsCapacity = 0;

    const vote *votes = getVotes();
    int votesCount = getVotesCount();

    for (int i = 0; i < votesCount; i++) {
        if (votes[i].year == year && votes[i].ufCode == uf) {
            totalVotes++;

            int found = 0;
            for (int j = 0; j < resultsCount; j++) {
                if (results[j].number == votes[i].candidateNumber) {
                    results[j].count++;
                    found = 1;
                    break;
                }
            }

            if (!found) {
                if (resultsCount >= resultsCapacity) {
                    resultsCapacity = resultsCapacity ? resultsCapacity + 10 : 10;
                    candidateResult *tmp = realloc(results, resultsCapacity * sizeof(candidateResult));
                    if (!tmp) {
                        printf("Erro ao alocar resultados\n");
                        exit(-1);
                    }
                    results = tmp;
                }
                results[resultsCount].number = votes[i].candidateNumber;
                results[resultsCount].count = 1;
                resultsCount++;
            }
        }
    }

    if (totalVotes == 0) {
        printf("\nNenhum voto registrado nessa eleicao.\n");

        printf("Pressione Enter para continuar...\n");
        cleanerKeyboard();

        free(results);
        return;
    }

    char title[100];
    sprintf(title, "Resultados da eleicao %d - UF %d", year, uf);
    printShowResultHeader(title);

    for (int i = 0; i < resultsCount; i++)
        printShowResultUI(&results[i], totalVotes);

    printShowResultBorder();

    printf("Pressione Enter para continuar...\n");
    cleanerKeyboard();
    free(results);
}
void showElectionAttendance() {
    int year, uf;

    printf("Digite o ano da eleicao: ");
    scanf("%d", &year);

    printf("Digite o codigo da UF: ");
    scanf("%d", &uf);
    cleanerKeyboard();

    int count = 0;
    const attendance *attendances = getAttendances();
    int attendancesCount = getAttendancesCount();

    for (int i = 0; i < attendancesCount; i++) {
        if (attendances[i].year == year && attendances[i].ufCode == uf) {
            count++;
        }
    }

    printf("\nTotal de comparecimentos na eleicao %d - UF %d: %d\n", year, uf, count);
    
    printf("Pressione Enter para continuar...\n");
    cleanerKeyboard();
}

void printShowResultHeader(const char *title)
{
    printf("+-------------------------------------------+\n");
    printf("| %-41s |\n", title);
    printShowResultBorder();
    printf("| %-20s | %-5s | %-10s |\n",
        "Numero do Candidato",
        "Votos",
        "Percentual");
    printShowResultBorder();
}
void printShowResultBorder()
{
    printf("+----------------------+-------+------------+\n");
}
void printShowResultUI(const candidateResult *result, int totalVotes)
{
    double perc = (result->count * 100.0) / totalVotes;
    printf("| %-20d | %-5d | %9.2f%% |\n", result->number, result->count, perc);
}