#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#include "MAIN.h"
#include "CANDIDATE.h"
#include "UF.h"
#include "CITIZEN.h"

static candidate *candidates = NULL;
static int candidatesCount = 0;
static int candidatesCapacity = 0;
static int candidatesModified = 0;

// Menu controllers
void menuCandidate()
{
    printf("+--------------------------------------------------+\n");
    printf("| Menu de candidatos                               |\n");
    printf("+---+----------------------------------------------+\n");
    printf("| 1 | INSERIR                                      |\n");
    printf("| 2 | EXCLUIR                                      |\n");
    printf("| 3 | LISTAR POR UF E ANO                          |\n");
    printf("| 4 | LISTAR POR ANO                               |\n");
    printf("| 0 | SAIR                                         |\n");
    printf("+---+----------------------------------------------+\n");
    printf("Escolha uma opcao: ");
}
void stateMachineCandidate()
{
    int op;
    do
    {
        menuCandidate();
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
            createCandidate();
            break;
        case 2:
            deleteCandidate();
            break;
        case 3:
            showCandidatesByUFAndYear();
            break;
        case 4:
            showCandidatesByYear();
            break;
        case 0:
            saveCandidates();
            break;
        default:
            printf("Opcao invalida. Tente novamente:");
            break;
        }
    } while (op != 0);
}

// File management
void loadCandidates()
{
    ensureDataDir();
    errno = 0;
    FILE *f = fopen(FILENAMECANDIDATE, "rb");
    if (!f)
    {
        if (errno != ENOENT)
            printf("Erro ao abrir arquivo de candidatos: %s\n", strerror(errno));
        return;
    }

    free(candidates);
    candidates = NULL;
    candidatesCount = candidatesCapacity = 0;

    candidate tmp;
    while (fread(&tmp, sizeof(candidate), 1, f) == 1)
        if (!tmp.deleted)
            pushCandidate(&tmp);

    fclose(f);
    candidatesModified = 0;
}
void saveCandidates()
{
    if (!candidatesModified)
        return;

    ensureDataDir();
    FILE *f = fopen(FILENAMECANDIDATE, "wb");
    if (!f)
    {
        printf("Erro ao salvar candidatos em %s\n", FILENAMECANDIDATE);
        return;
    }

    fwrite(candidates, sizeof(candidate), candidatesCount, f);
    fclose(f);
    candidatesModified = 0;
}

// CRUD functions
void createCandidate()
{
    candidate tmp;
    memset(&tmp, 0, sizeof(candidate));

    readYear(&tmp, "Digite o ano da eleicao: *");
    readUFCode(&tmp, "Digite o codigo da UF: *");
    if (searchUF(tmp.ufCode) < 0)
    {
        printf("Codigo de UF nao encontrado.\n");
        return;
    }

    readCPFCandidate(&tmp, "Digite o CPF do candidato (11 digitos): *");
    if (searchCitizenByCPF(tmp.cpf) < 0)
    {
        printf("CPF nao encontrado.\n");
        return;
    }

    if (searchCandidateByPK(tmp.year, tmp.ufCode, tmp.cpf) >= 0)
    {
        printf("Candidato ja cadastrado.\n");
        return;
    }

    readNumber(&tmp, "Digite o numero do candidato (2 digitos): *");

    tmp.deleted = 0;
    pushCandidate(&tmp);
    candidatesModified = 1;

    printf("Candidato criado com sucesso!\n");
    printf("Pressione Enter para continuar...\n");
    cleanerKeyboard();
}
void deleteCandidate()
{
    int year;
    int uf;
    char cpf[12];

    printf("Digite o ano da eleicao: ");
    scanf("%d", &year);
    cleanerKeyboard();

    printf("Digite o codigo da UF: ");
    scanf("%d", &uf);
    cleanerKeyboard();

    printf("Digite o CPF do candidato (11 digitos): ");
    scanf("%11s", cpf);
    cleanerKeyboard();

    int idx = searchCandidateByPK(year, uf, cpf);
    if (idx < 0)
    {
        printf("Candidato nao encontrado.\n");
        return;
    }

    printf("Tem certeza que deseja excluir? (s/N): ");
    int ch = getchar();
    cleanerKeyboard();
    if (ch != 's' && ch != 'S')
    {
        printf("Exclusão cancelada.\n");
        printf("Pressione Enter para continuar...\n");
        cleanerKeyboard();
        return;
    }

    candidates[idx].deleted = 1;
    candidatesModified = 1;

    printf("Candidato excluido com sucesso.\n");
    printf("Pressione Enter para continuar...\n");
    cleanerKeyboard();
}
void showCandidatesByUFAndYear()
{
    if (candidatesCount == 0)
    {
        printf("Nenhum candidato cadastrado.\n");
        return;
    }

    int year;
    int uf;
    printf("Digite o ano da eleicao: ");
    scanf("%d", &year);
    cleanerKeyboard();

    printf("Digite o codigo da UF: ");
    scanf("%d", &uf);
    cleanerKeyboard();

    printf("\n");

    printShowCandidateHeader("Candidatos por UF e Ano");

    for (int i = 0; i < candidatesCount; i++)
    {
        if (!candidates[i].deleted &&
            candidates[i].year == year &&
            candidates[i].ufCode == uf)
        {
            printShowCandidateUI(&candidates[i]);
        }
    }

    printShowCandidateBorder();
    printf("Pressione Enter para continuar...\n");
    cleanerKeyboard();
}
void showCandidatesByYear()
{
    if (candidatesCount == 0)
    {
        printf("Nenhum candidato cadastrado.\n");
        return;
    }

    int year;
    printf("Digite o ano da eleicao: ");
    scanf("%d", &year);
    cleanerKeyboard();

    // Collect matching indices
    int *idxs = malloc(candidatesCount * sizeof(int));
    int cnt = 0;
    for (int i = 0; i < candidatesCount; i++)
    {
        if (!candidates[i].deleted && candidates[i].year == year)
            idxs[cnt++] = i;
    }

    if (cnt == 0)
    {
        printf("Nenhum candidato encontrado para o ano %d.\n", year);
        free(idxs);
        return;
    }

    // Sort by UF code
    for (int i = 0; i < cnt - 1; i++)
    {
        for (int j = i + 1; j < cnt; j++)
        {
            if (candidates[idxs[i]].ufCode > candidates[idxs[j]].ufCode)
            {
                int tmpi = idxs[i];
                idxs[i] = idxs[j];
                idxs[j] = tmpi;
            }
        }
    }

    printShowCandidateHeader("Candidatos por ano");

    for (int k = 0; k < cnt; k++)
    {
        int i = idxs[k];
        printShowCandidateUI(&candidates[i]);
    }

    printShowCandidateBorder();
    printf("Pressione Enter para continuar...\n");
    cleanerKeyboard();

    free(idxs);
}

// Utils
void pushCandidate(const candidate *p)
{
    if (candidatesCount >= candidatesCapacity)
    {
        candidatesCapacity = (candidatesCapacity == 0 ? 10 : candidatesCapacity + 10);
        candidate *tmp = realloc(candidates, candidatesCapacity * sizeof(candidate));
        if (!tmp)
        {
            printf("Erro ao alocar memoria para candidatos\n");
            exit(-1);
        }
        candidates = tmp;
    }
    candidates[candidatesCount++] = *p;
}

int searchCandidateByPK(int year, int ufCode, const char *cpf)
{
    for (int i = 0; i < candidatesCount; i++)
    {
        if (!candidates[i].deleted &&
            candidates[i].year == year &&
            candidates[i].ufCode == ufCode &&
            strcmp(candidates[i].cpf, cpf) == 0)
            return i;
    }
    return -1;
}
int searchCandidateBySK(int year, int ufCode, int number)
{
    for (int i = 0; i < candidatesCount; i++)
    {
        if (!candidates[i].deleted &&
            candidates[i].year == year &&
            candidates[i].ufCode == ufCode &&
            candidates[i].number == number)
            return i;
    }
    return -1;
}

void readYear(candidate *tmp, const char *prompt)
{
    printf("%s", prompt);
    while (1)
    {
        if (scanf("%d", &tmp->year) != 1 || tmp->year <= 0)
        {
            cleanerKeyboard();
            printf("Ano invalido. Tente novamente: ");
        }
        else
        {
            cleanerKeyboard();
            break;
        }
    }
}
void readUFCode(candidate *tmp, const char *prompt)
{
    printf("%s", prompt);
    while (1)
    {
        if (scanf("%d", &tmp->ufCode) != 1 || tmp->ufCode < 1)
        {
            cleanerKeyboard();
            printf("Codigo de UF invalido. Tente novamente: ");
        }
        else
        {
            cleanerKeyboard();
            break;
        }
    }
}

void readCPFCandidate(candidate *tmp, const char *prompt)
{
    printf("%s", prompt);
    while (1)
    {
        scanf("%11s", tmp->cpf);
        cleanerKeyboard();
        if (strlen(tmp->cpf) == 11 &&
            isdigit(tmp->cpf[0]) && isdigit(tmp->cpf[1]) &&
            isdigit(tmp->cpf[2]) && isdigit(tmp->cpf[3]) &&
            isdigit(tmp->cpf[4]) && isdigit(tmp->cpf[5]) &&
            isdigit(tmp->cpf[6]) && isdigit(tmp->cpf[7]) &&
            isdigit(tmp->cpf[8]) && isdigit(tmp->cpf[9]) &&
            isdigit(tmp->cpf[10]))
            break;
        printf("CPF invalido. Tente novamente: ");
    }
}
void readNumber(candidate *tmp, const char *prompt)
{
    printf("%s", prompt);
    while (1)
    {
        if (scanf("%2d", &tmp->number) != 1 || tmp->number < 0 || tmp->number > 99)
        {
            cleanerKeyboard();
            printf("Numero invalido. Tente novamente: ");
        }
        else
        {
            cleanerKeyboard();
            break;
        }
    }
}

void printShowCandidateHeader(const char *title)
{
    printf("+----------------------------------------------------------------+\n");
    printf("| %-62s |\n", title);
    printShowCandidateBorder();
    printf("| %-4s | %-2s | %-11s | %-30s | %-3s |\n", "Ano", "UF", "CPF", "Nome", "Num");
    printShowCandidateBorder();
}
void printShowCandidateBorder()
{
    printf("+------+----+-------------+--------------------------------+-----+\n");
}
void printShowCandidateUI(const candidate *item)
{
    int idx = searchCitizenByCPF(item->cpf);
    citizen *c = getSpecificCitizen(idx);
    if (!c) return;

    char name[31];
    strncpy(name, c->name, sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';

    char nameFormatted[31];
    formatBigString(name, 30, nameFormatted, sizeof(nameFormatted));

    printf("| %-4d | %-2d | %-11s | %-30s | %-3d |\n",
           item->year,
           item->ufCode,
           item->cpf,
           nameFormatted,
           item->number);
}

void freeCandidates()
{
    free(candidates);
    candidates = NULL;
    candidatesCount = 0;
    candidatesCapacity = 0;
    candidatesModified = 0;
}