#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/time.h>

#include "MAIN.h"
#include "UF.h"
#include "ELECTION.h"
#include "CANDIDATE.h"
#include "VOTE.h"

static vote *votes = NULL;
static int votesCount = 0;
static int votesCapacity = 0;
static int votesModified = 0;

void menuVote()
{
    printf("+--------------------------------------------------+\n");
    printf("| Menu de votos                                    |\n");
    printf("+---+----------------------------------------------+\n");
    printf("| 1 | INSERIR VOTO                                 |\n");
    printf("| 2 | LISTAR POR CANDIDATO E ANO                   |\n");
    printf("| 3 | LISTAR POR ANO                               |\n");
    printf("| 0 | SAIR                                         |\n");
    printf("+---+----------------------------------------------+\n");
    printf("Escolha uma opcao: ");
}
void stateMachineVote()
{
    int op;
    do
    {
        menuVote();
        if (scanf("%d", &op) != 1)
        {
            cleanerKeyboard();
            continue;
        }
        cleanerKeyboard();
        printf("\n");
        switch (op)
        {
        case 1:
            createVote();
            break;
        case 2:
            showVotesByCandidateAndYear();
            break;
        case 3:
            showVotesByYear();
            break;
        case 0:
            saveVotes();
            break;
        default:
            printf("Opcao invalida. Tente novamente:");
        }
    } while (op != 0);
}

void loadVotes()
{
    ensureDataDir();
    errno = 0;
    FILE *f = fopen(FILENAMEVOTE, "rb");
    if (!f)
    {
        if (errno != ENOENT)
            printf("Erro ao abrir arquivo de votos: %s\n", strerror(errno));
        return;
    }

    free(votes);
    votes = NULL;
    votesCount = votesCapacity = votesModified = 0;

    vote tmp;
    while (fread(&tmp, sizeof(vote), 1, f) == 1)
        pushVote(&tmp);

    fclose(f);
    votesModified = 0;
}
void saveVotes()
{
    if (!votesModified)
        return;
    ensureDataDir();
    FILE *f = fopen(FILENAMEVOTE, "wb");
    if (!f)
    {
        printf("Erro ao salvar votos em %s\n", FILENAMEVOTE);
        return;
    }
    fwrite(votes, sizeof(vote), votesCount, f);
    fclose(f);
    votesModified = 0;
}

void createVote() {
    vote tmp;
    memset(&tmp, 0, sizeof(tmp));

    readYearVote(&tmp, "Digite o ano da eleicao: *");
    if (searchElectionByYear(tmp.year) < 0) {
        printf("\nNao existe nenhuma eleicao nesse ano!\n");
        printf("Pressione Enter para continuar...\n");
        cleanerKeyboard();
        return;
    }

    readUFCodeVote(&tmp, "Digite o codigo da UF: *");
    if (findElectionIndex(tmp.year, tmp.ufCode) < 0) {
        printf("\nNo ano selecionado, essa UF nao possui eleicao cadastrada!\n");
        printf("Pressione Enter para continuar...\n");
        cleanerKeyboard();
        return;
    }

    printf("\n");

    int confirmed = 0;
    while (!confirmed) {
        readCandidateNumber(&tmp, "Digite o numero do candidato: *");

        int candidateIndex = searchCandidateBySK(tmp.ufCode, tmp.year, tmp.candidateNumber);
        if (candidateIndex < 0)
            tmp.candidateNumber = 0;

        int ch;
        if (tmp.candidateNumber == 0) { // Null vote
            do {
                printf("Confirma voto NULO? (S/N): ");
                ch = getchar();
                cleanerKeyboard();

                if (ch == 's' || ch == 'S') confirmed = 1;
                else if (ch == 'n' || ch == 'N') {
                    printf("Voto cancelado.\n\n");
                    break;
                } else printf("Opcao invalida. Tente novamente.\n");
            } while (1);

        } else {
            do {
                printf("Confirma voto para candidato %d? (S/N): ", tmp.candidateNumber);
                ch = getchar();
                cleanerKeyboard();

                if (ch == 's' || ch == 'S') confirmed = 1;
                else if (ch == 'n' || ch == 'N') {
                    printf("Voto cancelado.\n\n");
                    break;
                } else printf("Opcao invalida. Tente novamente.\n");
            } while (1);
        }
    }

    setCurrentDateTime(&tmp);
    pushVote(&tmp);
    votesModified = 1;

    printf("Voto registrado com sucesso!\n");
    printf("Pressione Enter para continuar...\n");
    cleanerKeyboard();
}
void showVotesByCandidateAndYear()
{
    if (!votesCount)
    {
        printf("Nenhum voto registrado.\n");
        return;
    }

    int year, uf;
    int num;

    printf("Digite o ano da eleicao: ");
    scanf("%d", &year);
    cleanerKeyboard();

    printf("Digite o codigo da UF: ");
    scanf("%d", &uf);
    cleanerKeyboard();

    printf("\n");

    printf("Digite numero do candidato: ");
    scanf("%2d", &num);
    cleanerKeyboard();
    
    printf("\n");

    char title[100];
    sprintf(title, "Votos do candidato %d no ano %d UF %d", num, year, uf);
    printShowVoteHeader(title);

    for (int i = 0; i < votesCount; i++)
        if (votes[i].year == year && votes[i].ufCode == uf && votes[i].candidateNumber == num)
        printShowVoteUI(&votes[i]);
    
    printShowVoteBorder();
    printf("Pressione Enter para continuar...\n");
    cleanerKeyboard();
}
void showVotesByYear()
{
    if (!votesCount)
    {
        printf("Nenhum voto registrado.\n");
        return;
    }
    int year;
    printf("Digite o ano da eleicao: ");
    scanf("%d", &year);
    cleanerKeyboard();
    // collect and sort by UF
    int *idxs = malloc(votesCount * sizeof(int)), cnt = 0;
    for (int i = 0; i < votesCount; i++)
        if (votes[i].year == year)
            idxs[cnt++] = i;
    if (!cnt)
    {
        printf("Nenhum voto para ano %d.\n", year);
        free(idxs);
        return;
    }
    for (int i = 0; i < cnt - 1; i++)
        for (int j = i + 1; j < cnt; j++)
            if (votes[idxs[i]].ufCode > votes[idxs[j]].ufCode)
            {
                int t = idxs[i];
                idxs[i] = idxs[j];
                idxs[j] = t;
            }
    printf("\nVotos no ano %d ordenados por UF:\n", year);
    printShowCandidateBorder();
    printf("| %-4s | %-3s | %-2s | %-16s |\n", "Ano", "UF", "Num", "DataHora");
    printShowCandidateBorder();
    for (int k = 0; k < cnt; k++)
    {
        int i = idxs[k];
        printf("| %-4d | %-3d | %-2d | %-16s |\n",
               votes[i].year, votes[i].ufCode,
               votes[i].candidateNumber, votes[i].dateTime);
    }
    printShowCandidateBorder();
    printf("Pressione Enter para continuar...\n");
    cleanerKeyboard();
    free(idxs);
}

void pushVote(const vote *v)
{
    if (votesCount >= votesCapacity)
    {
        votesCapacity = votesCapacity ? votesCapacity + 10 : 10;
        vote *tmp = realloc(votes, votesCapacity * sizeof(vote));
        if (!tmp)
        {
            printf("Erro alocar votos\n");
            exit(-1);
        }
        votes = tmp;
    }
    votes[votesCount++] = *v;
}

void readYearVote(vote *tmp, const char *prompt)
{
    printf("%s", prompt);
    while (scanf("%d", &tmp->year) != 1 || tmp->year <= 0)
    {
        cleanerKeyboard();
        printf("Ano invalido. Tente novamente: ");
    }
    cleanerKeyboard();
}
void readUFCodeVote(vote *tmp, const char *prompt)
{
    printf("%s", prompt);
    while (scanf("%d", &tmp->ufCode) != 1 || tmp->ufCode < 1)
    {
        cleanerKeyboard();
        printf("Codigo de UF invalido. Tente novamente: ");
    }
    cleanerKeyboard();
}
void readCandidateNumber(vote *tmp, const char *prompt)
{
    printf("%s", prompt);
    while (scanf("%2d", &tmp->candidateNumber) != 1 || tmp->candidateNumber < 0 || tmp->candidateNumber > 99)
    {
        cleanerKeyboard();
        printf("Numero invalido. Tente novamente: ");
    }
    cleanerKeyboard();
}

void printShowVoteHeader(const char *title)
{
    printf("+--------------------------------------------+\n");
    printf("| %-42s |\n", title);
    printShowCandidateBorder();
    printf("| %-4s | %-2s | %-11s | %-16s |\n", "Ano", "UF", "Num", "DataHora");
    printShowCandidateBorder();
}
void printShowVoteBorder()
{
    printf("+------+----+-------------+------------------+\n");
}
void printShowVoteUI(const vote *v)
{
    printf("| %-4d | %-2d | %-11d | %-16s |\n",
           v->year, v->ufCode, v->candidateNumber, v->dateTime);
}

void setCurrentDateTime(vote *tmp) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm *t = localtime((time_t *)&tv.tv_sec);
    strftime(tmp->dateTime, sizeof(tmp->dateTime), "%d-%m-%Y %H:%M:%S", t);
}