#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "MAIN.h"
#include "UF.h"
#include "ELECTION.h"
#include "CITIZEN.h"
#include "ATTENDANCE.h"

static attendance *attendances = NULL;
static int attendCount = 0;
static int attendCapacity = 0;

void loadAttendance()
{
    ensureDataDir();
    errno = 0;
    FILE *f = fopen(FILENAMEATTENDANCE, "rb");
    if (!f)
    {
        if (errno != ENOENT)
            printf("Erro ao abrir arquivo de comparecimento: %s\n", strerror(errno));
        return;
    }

    free(attendances);
    attendances = NULL;
    attendCount = attendCapacity = 0;

    attendance tmp;
    while (fread(&tmp, sizeof(attendance), 1, f) == 1)
        pushAttendance(&tmp);

    fclose(f);
}

void showAttendanceCountByUFAndYear()
{
    if (!attendCount)
    {
        printf("Nenhum registro de comparecimento.\n");
        return;
    }

    int year, uf;
    printf("Digite o ano da eleicao: ");
    scanf("%d", &year);
    cleanerKeyboard();
    if (searchElectionByYear(year) < 0)
    {
        printf("Nao existe nenhuma eleicao nesse ano!\n");
        return;
    }

    printf("Digite o codigo da UF:");
    scanf("%d", &uf);
    cleanerKeyboard();
    if (findElectionIndex(year, uf) < 0)
    {
        printf("No ano selecionado, essa UF nao possui eleicao cadastrada!\n");
        return;
    }

    int cnt = 0;
    for (int i = 0; i < attendCount; i++)
        if (attendances[i].year == year && attendances[i].ufCode == uf)
            cnt++;

    printf("\nTotal de comparecimentos no ano %d UF %d: %d\n", year, uf, cnt);
}
void showAttendanceByYear()
{
    if (!attendCount)
    {
        printf("Nenhum registro de comparecimento.\n");
        return;
    }
    int year;

    printf("Digite o ano da eleicao: ");
    scanf("%d", &year);
    cleanerKeyboard();
    if (searchElectionByYear(year) < 0)
    {
        printf("Nao existe nenhuma eleicao nesse ano!\n");
        return;
    }

    int *idxs = malloc(attendCount * sizeof(int)), cnt = 0;
    for (int i = 0; i < attendCount; i++)
        if (attendances[i].year == year)
            idxs[cnt++] = i;

    if (!cnt)
    {
        printf("Nenhum comparecimento para ano %d.\n", year);
        free(idxs);
        return;
    }

    for (int i = 0; i < cnt - 1; i++)
        for (int j = i + 1; j < cnt; j++)
            if (attendances[idxs[i]].ufCode > attendances[idxs[j]].ufCode)
            {
                int t = idxs[i];
                idxs[i] = idxs[j];
                idxs[j] = t;
            }

    printf("\nComparecimentos no ano %d ordenados por UF:\n", year);
    printf("+------+----+-------------+\n");
    printf("| %-4s | %-3s | %-11s |\n", "Ano", "UF", "CPF");
    printf("+------+----+-------------+\n");

    for (int k = 0; k < cnt; k++)
    {
        int i = idxs[k];
        printf("| %-4d | %-3d | %-11s |\n",
               attendances[i].year, attendances[i].ufCode, attendances[i].cpf);
    }

    printf("+------+----+-------------+\n");
    free(idxs);
}

void pushAttendance(const attendance *a)
{
    if (attendCount >= attendCapacity)
    {
        attendCapacity = attendCapacity ? attendCapacity + 10 : 10;
        attendance *tmp = realloc(attendances, attendCapacity * sizeof(attendance));
        if (!tmp)
        {
            printf("Erro alocar memoria comparecimento\n");
            exit(-1);
        }
        attendances = tmp;
    }
    attendances[attendCount++] = *a;
}