#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "main.h"
#include "election.h"

static election *elections = NULL;
static int electionsCount = 0;
static int electionsCapacity = 0;
static int electionsModified = 0;

enum MenuOptions {
    insert = 1,
    update = 2,
    delete = 3,
    list = 4,
    show = 5,
    exitMenu = 0
};

// Menu controllers
void menuElection()
{
    printf("+--------------------------------------------------+\n");
    printf("| Menu eleicao                                     |\n");
    printf("+---+----------------------------------------------+\n");
    printf("| 1 | INSERIR ELEICAO                              |\n");
    printf("| 2 | ATUALIZAR ELEICAO                            |\n");
    printf("| 3 | EXCLUIR ELEICAO                              |\n");
    printf("| 4 | LISTAR ELEICOES                              |\n");
    printf("| 5 | VISUALIZAR ELEICAO                           |\n");
    printf("| 0 | SAIR                                         |\n");
    printf("+---+----------------------------------------------+\n");
    printf("Escolha uma opcao: ");
}
void stateMachineElection()
{
    int option;
    do
    {
        menuElection();
        if (scanf("%d", &option) != 1)
        {
            cleanerKeyboard();
            continue;
        }
        cleanerKeyboard();
        printf("\n");
        switch (option)
        {
        case insert:
            createElection();
            break;
        case update:
            updateElection();
            break;
        case delete:
            deleteElection();
            break;
        case list:
            listElections();
            break;
        case show:
            viewElection();
            break;
        case exitMenu:
            saveElections();
            break;
        default:
            printf("Opcao invalida.\n\n");
        }
    } while (option != 0);
}

// File Management
void loadElections()
{
    ensureDataDir();
    errno = 0; // Reset errno to check for errors later
    FILE *f = fopen(FILENAMEELECTION, "rb");
    if (!f)
    {
        if (errno != ENOENT)
            printf("Erro ao abrir arquivo de eleicoes: %s\n", strerror(errno)); // Print error if not file not found

        return;
    }

    free(elections);
    elections = NULL;
    electionsCount = electionsCapacity = 0;

    election tmp;

    while (fread(&tmp, sizeof(election), 1, f) == 1)
        if (!tmp.deleted)
            pushElection(&tmp);

    fclose(f);
    electionsModified = 0;
}
void saveElections()
{
    if (!electionsModified)
        return;

    ensureDataDir();
    FILE *f = fopen(FILENAMEELECTION, "wb");
    if (!f)
    {
        printf("Erro ao salvar eleicoes em %s\n", FILENAMEELECTION);
        return;
    }

    fwrite(elections, sizeof(election), electionsCount, f);
    fclose(f);
    electionsModified = 0;
}

// CRUD functions
void createElection()
{
    election e;
    memset(&e, 0, sizeof(e));
    readElectionYear(&e.year, "Digite o ano da eleicao (4 digitos): ");

    readElectionUfCode(&e.ufCode, "Digite o codigo da UF (existente): ");
    if (findElectionIndex(e.year, e.ufCode) != -1)
    {
        printf("\nEleicao para %d / UF %d ja existe.\n", e.year, e.ufCode);
        printf("Pressione Enter para continuar...\n");
        cleanerKeyboard();
        return;
    }

    readElectionDescription(&e, "Digite a descricao: ");
    e.deleted = 0;
    pushElection(&e);
    electionsModified = 1;

    printElectionHeader("Eleicao criada com sucesso!");
    printElectionRow(&e);
    printElectionBorder();
    printf("Pressione Enter para continuar...\n");
    cleanerKeyboard();
}
void updateElection()
{
    int year, ufCode;
    printf("Digite o ano da eleicao a editar: ");
    scanf("%d", &year);
    cleanerKeyboard();
    printf("Digite o codigo da UF: ");
    scanf("%d", &ufCode);
    cleanerKeyboard();
    int idx = findElectionIndex(year, ufCode);
    if (idx < 0)
    {
        printf("Eleicao %d / UF %d nao encontrada.\n", year, ufCode);
        return;
    }
    election e = elections[idx];
    int option;
    do
    {
        printElectionHeader("Editando eleicao:");
        printElectionRow(&e);
        printElectionBorder();
        printf("\n+---+----------------------------------------------+\n");
        printf("| 1 | ANO                                          |\n");
        printf("| 2 | UF                                           |\n");
        printf("| 3 | DESCRICAO                                    |\n");
        printf("| 0 | CONCLUIR                                     |\n");
        printf("+---+----------------------------------------------+\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &option);
        cleanerKeyboard();
        printf("\n");
        switch (option)
        {
        case 1:
            readElectionYear(&e.year, "Digite o novo ano (4 digitos): ");
            break;
        case 2:
            readElectionUfCode(&e.ufCode, "Digite o novo codigo da UF: ");
            break;
        case 3:
            readElectionDescription(&e, "Digite a nova descricao: ");
            break;
        case 0:
            break;
        default:
            printf("Opcao invalida.\n");
        }
    } while (option != 0);
    if ((e.year != year || e.ufCode != ufCode) && findElectionIndex(e.year, e.ufCode) != -1)
    {
        printf("Outra eleicao para %d / UF %d ja existe.\n", e.year, e.ufCode);
        printf("Pressione Enter para continuar...\n");
        cleanerKeyboard();
        return;
    }
    elections[idx] = e;
    electionsModified = 1;
    printElectionHeader("Eleicao atualizada com sucesso!");
    printElectionRow(&e);
    printElectionBorder();
    printf("Pressione Enter para continuar...\n");
    cleanerKeyboard();
}
void deleteElection()
{
    int year, ufCode;
    printf("Digite o ano da eleicao a excluir: ");
    scanf("%d", &year);
    cleanerKeyboard();
    printf("Digite o codigo da UF: ");
    scanf("%d", &ufCode);
    cleanerKeyboard();
    for (int i = 0; i < electionsCount; i++)
    {
        if (elections[i].year == year && elections[i].ufCode == ufCode && !elections[i].deleted)
        {
            printElectionHeader("Eleicao encontrada:");
            printElectionRow(&elections[i]);
            printElectionBorder();
            printf("Confirmar exclusao? (s/N): ");
            int ch = getchar();
            cleanerKeyboard();
            if (ch != 's' && ch != 'S')
            {
                printf("Exclusao cancelada.\nPressione Enter para continuar...\n");
                cleanerKeyboard();
                return;
            }
            elections[i].deleted = 1;
            electionsModified = 1;
            printf("Eleicao excluida com sucesso.\nPressione Enter para continuar...\n");
            cleanerKeyboard();
            return;
        }
    }
    printf("Eleicao %d / UF %d nao encontrada.\n", year, ufCode);
}
void listElections()
{
    if (electionsCount == 0)
    {
        printf("Nenhuma eleicao cadastrada.\n");
        return;
    }

    printElectionHeader("Lista de eleicoes:");
    for (int i = 0; i < electionsCount; i++)
        if (!elections[i].deleted)
            printElectionRow(&elections[i]);
    printElectionBorder();

    printf("Pressione Enter para continuar...\n");
    cleanerKeyboard();
}
void viewElection()
{
    int year, ufCode;

    printf("Digite o ano da eleicao: ");
    scanf("%d", &year);
    cleanerKeyboard();

    printf("Digite o codigo da UF: ");
    scanf("%d", &ufCode);
    cleanerKeyboard();

    printf("\n");

    for (int i = 0; i < electionsCount; i++)
    {
        if (elections[i].year == year && elections[i].ufCode == ufCode)
        {
            if (!elections[i].deleted)
            {
                printElectionHeader("Detalhes da eleicao:");
                printElectionRow(&elections[i]);
                printElectionBorder();
            }
            else printf("Eleicao nao encontrada.\n");

            printf("Pressione Enter para continuar...\n");
            cleanerKeyboard();
            
            return;
        }
    }
    printf("Eleicao nao encontrada.\n");

    printf("Pressione Enter para continuar...\n");
    cleanerKeyboard();
}

// Utils
void pushElection(const election *item)
{
    if (electionsCount >= electionsCapacity)
    {
        electionsCapacity = (electionsCapacity == 0 ? 10 : electionsCapacity * 2);
        election *tmp = realloc(elections, electionsCapacity * sizeof(election));

        if (!tmp)
        {
            printf("Erro ao alocar memoria para eleicoes\n");
            exit(-1);
        }

        elections = tmp;
    }
    elections[electionsCount++] = *item;
}

void readElectionYear(int *year, const char *prompt)
{
    char buf[16];
    int len, invalid;
    do
    {
        invalid = 0;
        printf("%s", prompt);

        if (!fgets(buf, sizeof(buf), stdin))
        {
            invalid = 1;
            continue;
        }

        // Remove newline
        len = strcspn(buf, "\n");
        if (buf[len] != '\n')
            cleanerKeyboard();
        else
            buf[len] = '\0';

        // Check if length is exactly 4
        if (len != 4)
        {
            printf("Ano deve ter exatamente 4 digitos!\n");
            invalid = 1;
            continue;
        }

        // Check if all characters are digits
        for (int i = 0; i < 4; i++)
        {
            if (!isdigit((unsigned char)buf[i]))
            {
                printf("Ano deve ser numerico!\n");
                invalid = 1;
                break;
            }
        }
    } while (invalid);

    *year = atoi(buf);
}
void readElectionUfCode(int *ufCode, const char *prompt)
{
    int code, invalid;
    do
    {
        invalid = 0;
        printf("%s", prompt);

        if (scanf("%d", &code) != 1)
        {
            cleanerKeyboard();
            invalid = 1;
            continue;
        }

        cleanerKeyboard();

        if (searchUF(code) < 0)
        {
            printf("Codigo de UF nao cadastrado!\n\n");
            invalid = 1;
        }
    } while (invalid);
    *ufCode = code;
}
void readElectionDescription(election *item, const char *prompt)
{
    printf("%s", prompt);
    if (fgets(item->description, sizeof(item->description), stdin) != NULL)
        item->description[strcspn(item->description, "\n")] = '\0';
}

int findElectionIndex(int year, int ufCode)
{
    for (int i = 0; i < electionsCount; i++)
        if (elections[i].year == year && elections[i].ufCode == ufCode && !elections[i].deleted)
            return i;

    return -1;
}
int searchElectionByYear(int year)
{
    for (int i = 0; i < electionsCount; i++)
        if (elections[i].year == year && !elections[i].deleted)
            return i;

    return -1;
}
int searchElectionByUfCode(int ufCode)
{
    for (int i = 0; i < electionsCount; i++)
        if (elections[i].ufCode == ufCode && !elections[i].deleted)
            return i;

    return -1;
}

void printElectionHeader(const char *title)
{
    printf("+------------------------------------------------------+\n");
    printf("| %-52s |\n", title);
    printElectionBorder();
    printf("| %-4s | %-2s | %-40s |\n",
        "Ano",
        "UF",
        "Descricao");
    printElectionBorder();
}
void printElectionBorder()
{
    printf("+------+----+------------------------------------------+\n");
}
void printElectionRow(const election *item)
{
    char desc[31];
    formatBigString(item->description, 30, desc, sizeof(desc));

    printf("| %4d | %2d | %-40s |\n",
           item->year,
           item->ufCode,
           desc);
}

void freeElections()
{
    free(elections);
    elections = NULL;
    electionsCount = 0;
    electionsCapacity = 0;
    electionsModified = 0;
}