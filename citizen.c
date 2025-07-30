#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "MAIN.h"
#include "CITIZEN.h"

static citizen *citizens = NULL;
static int citizensCount = 0;
static int citizensCapacity = 0;
static int citizensModified = 0;

enum MenuOptions {
    insertCtz = 1,
    updateCtz = 2,
    deleteCtz = 3,
    listCtz = 4,
    showCtz = 5,
    exitMenuCtz = 0
};

// Menu controllers
void menuCitizen()
{
    printf("+--------------------------------------------------+\n");
    printf("| Menu das pessoas                                 |\n");
    printf("+---+----------------------------------------------+\n");
    printf("| 1 | INSERIR                                      |\n");
    printf("| 2 | ALTERAR                                      |\n");
    printf("| 3 | DELETAR                                      |\n");
    printf("| 4 | LISTAR PESSOAS                               |\n");
    printf("| 5 | MOSTRAR PESSOA                               |\n");
    printf("| 0 | SAIR                                         |\n");
    printf("+---+----------------------------------------------+\n");
    printf("Escolha uma opcao: ");
}
void stateMachineCitizen()
{
    int op;
    do
    {
        menuCitizen();
        if (scanf("%d", &op) != 1)
        {
            cleanerKeyboard();
            continue;
        }
        cleanerKeyboard();
        printf("\n");

        switch (op)
        {
        case insertCtz:
            createCitizen();
            break;
        case updateCtz:
            updateCitizen();
            break;
        case deleteCtz:
            deleteCitizen();
            break;
        case listCtz:
            showCitizen();
            break;
        case showCtz:
            showCitizenByVoterNumber();
            break;
        case exitMenuCtz:
            saveCitizens();
            break;
        default:
            printf("Opcao invalida.\n\n");
        }
    } while (op != 0);
}

// File management
void loadCitizens()
{
    ensureDataDir();
    errno = 0; // Reset errno to check for errors later
    FILE *f = fopen(FILENAMECITIZEN, "rb");
    if (!f)
    {
        if (errno != ENOENT)
            printf("Erro ao abrir arquivo de pessoas: %s\n", strerror(errno)); // Print error if not file not found

        return;
    }

    free(citizens);
    citizens = NULL;
    citizensCount = citizensCapacity = 0;

    citizen tmp;
    while (fread(&tmp, sizeof(citizen), 1, f) == 1)
        if (!tmp.deleted)
            pushCitizen(&tmp);

    fclose(f);
    citizensModified = 0;
}
void saveCitizens()
{
    if (!citizensModified)
        return;

    ensureDataDir();
    FILE *f = fopen(FILENAMECITIZEN, "wb");
    if (!f)
    {
        printf("Erro ao salvar pessoas em %s\n", FILENAMECITIZEN);
        return;
    }

    fwrite(citizens, sizeof(citizen), citizensCount, f);
    fclose(f);
    citizensModified = 0;
}

// CRUD functions
void createCitizen()
{
    citizen tmp;
    memset(&tmp, 0, sizeof(citizen));

    readCPFCitizen(&tmp, "Digite o CPF (11 digitos): *");
    if (searchCitizenByCPF(tmp.cpf) >= 0)
    {
        printf("CPF ja cadastrado.\n");
        return;
    }

    readVoterNumber(&tmp, "Digite o titulo de eleitor (12 ou 13 digitos): *");
    if (searchCitizenByVoterNumber(tmp.voterNumber) >= 0)
    {
        printf("Titulo ja cadastrado.\n");
        return;
    }

    readName(&tmp, "Digite o nome completo: *");
    readPhone(&tmp, "Digite o telefone (com DDD, ex: 11911111111): ");
    readAddress(&tmp, "Digite o endereco: ");
    readBirthdate(&tmp, "Digite a data de nascimento (DD-MM-YYYY): *");

    tmp.deleted = 0;
    pushCitizen(&tmp);
    citizensModified = 1;

    printf("\n");

    printShowCitizenHeader("Pessoa criada com sucesso!");
    printShowCitizenUI(&tmp);
    printShowCitizenBorder();

    printf("Pressione Enter para continuar...\n");
    cleanerKeyboard();
}
void updateCitizen()
{
    citizen tmp;

    printf("Digite o CPF da pessoa que deseja alterar: ");
    scanf("%12s", tmp.cpf);
    cleanerKeyboard();

    if (strlen(tmp.cpf) != 11 || !isdigit(tmp.cpf[0]) || !isdigit(tmp.cpf[1]) ||
        !isdigit(tmp.cpf[2]) || !isdigit(tmp.cpf[3]) || !isdigit(tmp.cpf[4]) ||
        !isdigit(tmp.cpf[5]) || !isdigit(tmp.cpf[6]) || !isdigit(tmp.cpf[7]) ||
        !isdigit(tmp.cpf[8]) || !isdigit(tmp.cpf[9]) || !isdigit(tmp.cpf[10]))
    {
        printf("CPF invalido. Tente novamente.\n");
        return;
    }

    printf("\n");

    int index = searchCitizenByCPF(tmp.cpf);
    if (index < 0)
    {
        printf("Pessoa com CPF %s nao encontrada.\n\n", tmp.cpf);
        return;
    }

    tmp = citizens[index];

    int op;
    do
    {
        printShowCitizenHeader("Pessoa sendo editada:");
        printShowCitizenUI(&tmp);
        printShowCitizenBorder();

        printf("\n");

        printf("+--------------------------------------------------+\n");
        printf("| Menu de edicao                                   |\n");
        printf("+---+----------------------------------------------+\n");
        printf("| 2 | NOME                                         |\n");
        printf("| 3 | TELEFONE                                     |\n");
        printf("| 4 | ENDERECO                                     |\n");
        printf("| 5 | DATA DE NASCIMENTO                           |\n");
        printf("| 0 | CONCLUIR EDICAO                              |\n");
        printf("+---+----------------------------------------------+\n");

        scanf("%d", &op);
        cleanerKeyboard();
        printf("\n");

        switch (op)
        {
        case 1:
            readName(&tmp, "Digite o novo nome completo: *");
            break;
        case 2:
            readPhone(&tmp, "Digite o novo telefone (com DDD): ");
            break;
        case 3:
            readAddress(&tmp, "Digite o novo endereco: ");
            break;
        case 4:
            readBirthdate(&tmp, "Digite a nova data de nascimento (DD-MM-YYYY): *");
            break;
        case 0:
            break;
        default:
            printf("Opcao invalida. Tente novamente.\n");
        }
    } while (op != 0);

    citizens[index] = tmp;
    citizensModified = 1;

    printShowCitizenHeader("Pessoa atualizada com sucesso!");
    printShowCitizenUI(&tmp);
    printShowCitizenBorder();

    printf("Pressione Enter para continuar...\n");
    cleanerKeyboard();
}
void deleteCitizen()
{
    char cpf[12];

    printf("Digite o CPF da pessoa que deseja deletar: ");
    scanf("%12s", cpf);
    cleanerKeyboard();

    for (int i = 0; i < citizensCount; i++)
    {
        if (strcmp(citizens[i].cpf, cpf) == 0 && !citizens[i].deleted)
        {
            printShowCitizenHeader("Pessoa encontrada:");
            printShowCitizenUI(&citizens[i]);
            printShowCitizenBorder();

            printf("Tem certeza que deseja excluir? (s/N): ");
            int ch = getchar();
            cleanerKeyboard();
            if (ch != 's' && ch != 'S')
            {
                printf("\nExclusao cancelada.\n");

                printf("Pressione Enter para continuar...\n");
                cleanerKeyboard();
                return;
            }

            citizens[i].deleted = 1;
            citizensModified = 1;

            printf("\nPessoa deletada com sucesso.\n");
            printf("Pressione Enter para continuar...\n");
            cleanerKeyboard();
            return;
        }
    }

    printf("Pessoa com CPF %s nao encontrado ou ja deletado.\n", cpf);

    printf("Pressione Enter para continuar...\n");
    cleanerKeyboard();
}
void showCitizen()
{
    if (citizensCount == 0)
    {
        printf("Nenhuma pessoa cadastrada.\n");
        return;
    }

    printShowCitizenHeader("Lista de pessoas:");

    for (int i = 0; i < citizensCount; i++)
        if (!citizens[i].deleted)
            printShowCitizenUI(&citizens[i]);

    printShowCitizenBorder();

    printf("Pressione Enter para continuar...\n");
    cleanerKeyboard();
}
void showCitizenByCPF()
{
    if (citizensCount == 0)
    {
        printf("Nenhuma pessoa cadastrada.\n");
        return;
    }

    char cpf[12];
    printf("\nDigite o CPF da pessoa que deseja ver: ");
    scanf("%11s", cpf);
    cleanerKeyboard();
    printf("\n");

    int found = 0;
    for (int i = 0; i < citizensCount; i++)
    {
        if (!strcmp(citizens[i].cpf, cpf) == 0)
            continue;

        if (!citizens[i].deleted)
        {
            printShowCitizenHeader("Dados da pessoa:");
            printShowCitizenUI(&citizens[i]);
            printShowCitizenBorder();

            printf("Pressione Enter para continuar...\n");
            cleanerKeyboard();
        }
        else
            printf("Pessoa com CPF %s nao encontrada.\n", cpf);

        found = 1;
        break;
    }

    if (!found)
        printf("Pessoa com CPF %s nao encontrada\n", cpf);
}
void showCitizenByVoterNumber()
{
    if (citizensCount == 0)
    {
        printf("Nenhuma pessoa cadastrada.\n");
        printf("Pressione Enter para continuar...\n");
        cleanerKeyboard();
        return;
    }

    char title[14];
    printf("\nDigite o titulo de eleitor da pessoa que deseja ver: ");
    scanf("%13s", title);
    cleanerKeyboard();
    printf("\n");

    int idx = searchCitizenByVoterNumber(title);
    if (idx < 0)
    {
        printf("Pessoa com titulo %s nao encontrada.\n", title);
        printf("Pressione Enter para continuar...\n");
        cleanerKeyboard();
        return;
    }

    if (!citizens[idx].deleted)
    {
        printShowCitizenHeader("Dados da pessoa:");
        printShowCitizenUI(&citizens[idx]);
        printShowCitizenBorder();
    }
    else
    {
        printf("Pessoa com titulo %s nao encontrada.\n", title);
    }

    printf("Pressione Enter para continuar...\n");
    cleanerKeyboard();
}

// Utils
void pushCitizen(const citizen *p)
{
    if (citizensCount >= citizensCapacity)
    {
        citizensCapacity = (citizensCapacity == 0 ? 10 : citizensCapacity + 10);

        citizen *tmp = realloc(citizens, citizensCapacity * sizeof(citizen));
        if (!tmp)
        {
            printf("Erro ao alocar memoria para UFs");
            exit(-1);
        }

        citizens = tmp;
    }
    citizens[citizensCount++] = *p;
}

void readCPFCitizen(citizen *tmp, const char *prompt)
{
    printf("%s", prompt);
    while (1)
    {
        scanf("%11s", tmp->cpf);
        cleanerKeyboard();

        if (strlen(tmp->cpf) == 11 && isdigit(tmp->cpf[0]) && isdigit(tmp->cpf[1]) &&
            isdigit(tmp->cpf[2]) && isdigit(tmp->cpf[3]) && isdigit(tmp->cpf[4]) &&
            isdigit(tmp->cpf[5]) && isdigit(tmp->cpf[6]) && isdigit(tmp->cpf[7]) &&
            isdigit(tmp->cpf[8]) && isdigit(tmp->cpf[9]) && isdigit(tmp->cpf[10]))
            break;

        printf("CPF invalido. Tente novamente: ");
    }
}
void readVoterNumber(citizen *tmp, const char *prompt)
{
    printf("%s", prompt);
    while (1)
    {
        scanf("%13s", tmp->voterNumber);
        cleanerKeyboard();

        int len = strlen(tmp->voterNumber);
        if (len == 12 || len == 13)
        {
            int allDigits = 1;
            for (int i = 0; i < len; i++)
            {
                if (!isdigit(tmp->voterNumber[i]))
                {
                    allDigits = 0;
                    break;
                }
            }

            if (allDigits) break;
        }

        printf("Titulo invalido. Tente novamente: ");
    }
}
void readName(citizen *tmp, const char *prompt)
{
    printf("%s", prompt);
    while (1)
    {
        fgets(tmp->name, sizeof(tmp->name), stdin);
        tmp->name[strcspn(tmp->name, "\n")] = 0; // Remove newline

        if (strlen(tmp->name) > 0 && strlen(tmp->name) < 100)
            break;

        printf("Nome invalido. Tente novamente: ");
    }
}
void readPhone(citizen *tmp, const char *prompt)
{
    printf("%s", prompt);
    while (1)
    {
        char buffer[32];
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            printf("Erro ao ler telefone. Tente novamente: ");
            continue;
        }

        buffer[strcspn(buffer, "\n")] = '\0';

        size_t len = strlen(buffer);
        if (len == 0)
        {
            tmp->phone[0] = '\0';
            break;
        }
        else if (len == 11)
        {
            int allDigits = 1;
            for (int i = 0; i < 11; i++)
            {
                if (!isdigit(buffer[i]))
                {
                    allDigits = 0;
                    break;
                }
            }

            if (allDigits)
            {
                strcpy(tmp->phone, buffer);
                break;
            }
        }

        printf("Telefone invalido. Digite 11 digitos numericos ou deixe em branco: ");
    }
}
void readAddress(citizen *tmp, const char *prompt)
{
    printf("%s", prompt);
    while (1)
    {
        if (fgets(tmp->address, sizeof(tmp->address), stdin) == NULL)
        {
            printf("Erro ao ler endereco. Tente novamente: ");
            continue;
        }

        size_t len = strcspn(tmp->address, "\n");
        if (tmp->address[len] == '\n')
            tmp->address[len] = '\0';
        else
        {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }

        if (strlen(tmp->address) < sizeof(tmp->address)) break;

        printf("Endereco invalido. Tente novamente: ");
    }
}
void readBirthdate(citizen *tmp, const char *prompt)
{
    printf("%s", prompt);
    while (1)
    {
        scanf("%10s", tmp->birthdate);
        cleanerKeyboard();

        // Validate DD-MM-YYYY pattern
        if (strlen(tmp->birthdate) == 10 &&
            isdigit(tmp->birthdate[0]) && isdigit(tmp->birthdate[1]) &&
            tmp->birthdate[2] == '-' &&
            isdigit(tmp->birthdate[3]) && isdigit(tmp->birthdate[4]) &&
            tmp->birthdate[5] == '-' &&
            isdigit(tmp->birthdate[6]) && isdigit(tmp->birthdate[7]) &&
            isdigit(tmp->birthdate[8]) && isdigit(tmp->birthdate[9]))
        {
            // Parse day, month, year
            int day = (tmp->birthdate[0] - '0') * 10 + (tmp->birthdate[1] - '0');
            int month = (tmp->birthdate[3] - '0') * 10 + (tmp->birthdate[4] - '0');
            int year = (tmp->birthdate[6] - '0') * 1000 + (tmp->birthdate[7] - '0') * 100 + (tmp->birthdate[8] - '0') * 10 + (tmp->birthdate[9] - '0');

            // Determine days in month, accounting for leap year
            int max_day = 0;
            if (month >= 1 && month <= 12)
            {
                switch (month)
                {
                case 1:
                case 3:
                case 5:
                case 7:
                case 8:
                case 10:
                case 12:
                    max_day = 31;
                    break;
                case 4:
                case 6:
                case 9:
                case 11:
                    max_day = 30;
                    break;
                case 2:
                    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
                        max_day = 29;
                    else
                        max_day = 28;
                    break;
                }
            }

            // Final validity check: month 1-12 and day within range
            if (month >= 1 && month <= 12 && day >= 1 && day <= max_day)
                break;
        }

        printf("Data de nascimento invalida. Tente novamente: ");
    }
}

int searchCitizenByCPF(const char *cpf)
{
    for (int i = 0; i < citizensCount; i++)
    {
        if (strcmp(citizens[i].cpf, cpf) == 0 && !citizens[i].deleted)
            return i;
    }
    return -1;
}
int searchCitizenByVoterNumber(const char *voterNumber)
{
    for (int i = 0; i < citizensCount; i++)
    {
        if (strcmp(citizens[i].voterNumber, voterNumber) == 0 && !citizens[i].deleted)
            return i;
    }
    return -1;
}

void printShowCitizenHeader(const char *header)
{
    printf("+---------------------------------------------------------------------------------------------------------------------------+\n");
    printf("| %-121s |\n", header);
    printShowCitizenBorder();
    printf("| %-11s | %-13s | %-30s | %-11s | %-30s | %-11s |\n",
           "CPF", "Title", "Name", "Phone", "Address", "Birthdate");
    printShowCitizenBorder();
}
void printShowCitizenBorder()
{
    printf("+-------------+---------------+--------------------------------+-------------+--------------------------------+-------------+\n");
}
void printShowCitizenUI(const citizen *p)
{
    char name[31];
    formatBigString(p->name, 30, name, sizeof(name));

    char address[31];
    formatBigString(p->address, 30, address, sizeof(address));

    printf("| %-11s | %-13s | %-30s | %-11s | %-30s | %-11s |\n",
           p->cpf,
           p->voterNumber,
           name,
           p->phone,
           address,
           p->birthdate);
}

citizen *getSpecificCitizen(int idx)
{
    if (idx < 0 || idx >= citizensCount) return NULL;
    if (citizens[idx].deleted) return NULL;
    return &citizens[idx];
}

void freeCitizens()
{
    free(citizens);
    citizens = NULL;
    citizensCount = 0;
    citizensCapacity = 0;
    citizensModified = 0;
}