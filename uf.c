#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "MAIN.h"
#include "UF.h"

static uf *ufs = NULL;
static int ufsCount = 0;
static int ufsCapacity = 0;
static int ufsModified = 0;

// Menu controllers
void menuUF()
{
    printf("+--------------------------------------------------+\n");
    printf("| Menu UF                                          |\n");
    printf("+---+----------------------------------------------+\n");
    printf("| 1 | INSERIR                                      |\n");
    printf("| 2 | ALTERAR                                      |\n");
    printf("| 3 | DELETAR                                      |\n");
    printf("| 4 | LISTAR ESTADOS                               |\n");
    printf("| 5 | MOSTRAR ESTADO                               |\n");
    printf("| 0 | SAIR                                         |\n");
    printf("+---+----------------------------------------------+\n");
}
void stateMachineUF()
{
    int op;
    do
    {
        menuUF();
        if (scanf("%d", &op) != 1)
        {
            cleanerKeyboard();
            continue;
        }
        cleanerKeyboard();

        switch (op)
        {
        case 1:
            createUF();
            break;
        case 2:
            updateUF();
            break;
        case 3:
            deleteUF();
            break;
        case 4:
            showUF();
            break;
        case 5:
            showSpecificUF();
            break;
        case 0:
            saveUFs();
            break;
        default:
            printf("Escolha uma opção válida.\n");
            break;
        }
    } while (op != 0);
}

// File management
void loadUFs()
{
    ensureDataDir();
    FILE *f = fopen(FILENAMEUF, "rb");
    if (!f)
    {
        printf("Erro ao carregar pessoas de %s\n", FILENAMEUF);
        return;
    }

    free(ufs);
    ufs = NULL;
    ufsCount = ufsCapacity = 0;

    uf tmp;
    while (fread(&tmp, sizeof(uf), 1, f) == 1) 
        if (!tmp.deleted) pushUF(&tmp);

    fclose(f);
    ufsModified = 0;
}
void saveUFs()
{
    if (!ufsModified)
        return;

    ensureDataDir();
    FILE *f = fopen(FILENAMEUF, "wb");
    if (!f)
    {
        printf("Erro ao salvar UFs em %s\n", FILENAMEUF);
        return;
    }

    fwrite(ufs, sizeof(uf), ufsCount, f);
    fclose(f);
    ufsModified = 0;
}

// CRUD functions
void createUF()
{
    uf tmp;
    memset(&tmp, 0, sizeof(tmp));
    tmp.code = getNextCode();

    printf("\n");
    readAcronym(&tmp, -1, "Digite a sigla da UF (2 letras): ");
    readDescription(&tmp, "Digite a descricao da UF: ");

    tmp.deleted = 0;
    pushUF(&tmp);
    ufsModified = 1;

    printShowUFHeader("UF criada com sucesso!");
    printShowUFUI(&tmp);
    printShowUFBorder();

    printf("Pressione Enter para continuar...\n");
    cleanerKeyboard();
}
void updateUF()
{
    uf tmp;

    printf("Digite o codigo do UF que deseja editar: ");
    scanf("%d", &tmp.code);
    cleanerKeyboard();
    printf("\n");

    int ufIndex = searchUF(tmp.code);
    if (ufIndex < 0)
    {
        printf("O UF com codigo %d nao existe ou foi deletado \n\n", tmp.code);
        return;
    }

    tmp = ufs[ufIndex];

    readAcronym(&tmp, ufIndex, "Digite a nova sigla: ");
    readDescription(&tmp, "Digite a nova descricao: ");

    ufs[ufIndex] = tmp;
    ufsModified = 1;

    printShowUFHeader("UF atualizada com sucesso!");
    printShowUFUI(&tmp);
    printShowUFBorder();

    printf("Pressione Enter para continuar...\n");
    cleanerKeyboard();
}
void deleteUF()
{
    int dCode;

    printf("\nDigite o codigo da UF que deseja excluir: ");
    scanf("%d", &dCode);
    cleanerKeyboard();
    printf("\n");

    for (int i = 0; i < ufsCount; i++)
    {
        if (ufs[i].code == dCode && !ufs[i].deleted)
        {
            ufs[i].deleted = 1;
            ufsModified = 1;

            printShowUFHeader("UF deletada com sucesso!");
            printShowUFUI(&ufs[i]);
            printShowUFBorder();

            printf("Pressione Enter para continuar...\n");
            cleanerKeyboard();
            return;
        }
    }

    printf("UF com codigo %d nao encontrada ou ja deletada.\n", dCode);

    printf("Pressione Enter para continuar...\n");
    cleanerKeyboard();
}
void showUF()
{
    if (ufsCount == 0)
    {
        printf("Nenhuma UF cadastrada.\n");
        return;
    }

    printShowUFHeader("Lista de UFs:");

    for (int i = 0; i < ufsCount; i++)
        if (!ufs[i].deleted)
            printShowUFUI(&ufs[i]);

    printShowUFBorder();
    
    printf("Pressione Enter para continuar...\n");
    cleanerKeyboard();
}
void showSpecificUF()
{
    if (ufsCount == 0)
    {
        printf("Nenhuma UF cadastrada.\n");
        return;
    }

    int sCode;
    printf("\nDigite o codigo da UF que deseja ver: ");
    scanf("%d", &sCode);
    cleanerKeyboard();
    printf("\n");

    int found = 0;
    for (int i = 0; i < ufsCount; i++)
    {
        if (!(ufs[i].code == sCode))
            continue;

        if (!ufs[i].deleted)
        {
            printShowUFHeader("UF encontrada!");
            printShowUFUI(&ufs[i]);
            printShowUFBorder();

            printf("Pressione Enter para continuar...\n");
            cleanerKeyboard();
        }
        else printf("UF com codigo %d nao encontrada.\n", sCode);

        found = 1;
        break;
    }

    if (!found)
        printf("UF com codigo %d nao encontrada.\n", sCode);
}

// Utils
void pushUF(const uf *p)
{
    if (ufsCount >= ufsCapacity)
    {
        ufsCapacity = (ufsCapacity == 0 ? 10 : ufsCapacity + 10);

        uf *tmp = realloc(ufs, ufsCapacity * sizeof(uf));
        if (!tmp)
        {
            printf("Erro ao alocar memoria para UFs");
            exit(-1);
        }

        ufs = tmp;
    }
    ufs[ufsCount++] = *p;
}

void readAcronym(uf *tmp, const int ufIndex, const char *prompt)
{
    char buf[32];
    size_t len;
    int ch, invalid;

    do
    {
        invalid = 0;
        printf("%s", prompt);

        if (fgets(buf, sizeof(buf), stdin) == NULL)
        {
            printf("Erro ao ler entrada! Tente novamente.\n\n");
            invalid = 1;
            continue;
        }

        len = strcspn(buf, "\n");
        if (buf[len] != '\n')
            while ((ch = getchar()) != '\n' && ch != EOF)
                ;
        else
            buf[len] = '\0';

        if (len != 2)
        {
            printf("Sigla deve ter exatamente 2 letras!\n\n");
            invalid = 1;
            continue;
        }

        strncpy(tmp->acronym, buf, 3);

        int codeAcronym = searchAcronym(tmp->acronym);
        if (codeAcronym != -1 && codeAcronym != ufIndex)
        {
            printf("Ja existe um UF com esta sigla!\n\n");
            invalid = 1;
            continue;
        }

    } while (invalid);
}
void readDescription(uf *tmp, const char *prompt)
{
    printf("%s", prompt);
    if (fgets(tmp->description, sizeof(tmp->description), stdin) != NULL)
        tmp->description[strcspn(tmp->description, "\n")] = '\0';
}

int searchUF(const int sCode)
{
    for (int i = 0; i < ufsCount; i++)
        if (ufs[i].code == sCode && !ufs[i].deleted)
            return i;

    return -1;
}
int searchAcronym(const char *sAcronym)
{
    for (int i = 0; i < ufsCount; i++)
        if (!ufs[i].deleted && strcmp(ufs[i].acronym, sAcronym) == 0)
            return i;

    return -1;
}

int getNextCode()
{
    if (!ufsCount)
        return 1;
    return (ufs[ufsCount - 1].code + 1);
}

void printShowUFHeader(const char *header)
{
    printf("\n");
    printf("+--------------------------------------------------+\n");
    printf("| %-48s |\n", header);
    printShowUFBorder();
    printf("| %-6s | %-6s | %-30s |\n",
           "Codigo", "Sigla", "Descricao");
    printShowUFBorder();
}
void printShowUFBorder()
{
    printf("+--------+--------+--------------------------------+\n");
}
void printShowUFUI(const uf *p)
{
    char buf[31];

    size_t len = strlen(p->description);
    if (len > 30) snprintf(buf, sizeof buf, "%.*s...", 27, p->description);
    else snprintf(buf, sizeof buf, "%.30s", p->description);

    printf("| %6d | %-6s | %-30s |\n",
           p->code,
           p->acronym,
           buf);
}