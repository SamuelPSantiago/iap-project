#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MAIN.H"
#include "ATTENDANCE.h"

static attendance *attendances = NULL;
static int attendancesCount = 0;
static int attendancesCapacity = 0;
static int attendancesModified = 0;

// File management
void loadAttendances()
{
    ensureDataDir();
    errno = 0;
    FILE *f = fopen(FILENAMEATTENDANCE, "rb");
    if (!f) {
        if (errno != ENOENT)
            printf("Erro ao abrir arquivo de comparecimentos: %s\n", strerror(errno));
        return;
    }

    free(attendances);
    attendances = NULL;
    attendancesCount = attendancesCapacity = 0;

    attendance tmp;
    while (fread(&tmp, sizeof(attendance), 1, f) == 1)
        pushAttendance(&tmp);

    fclose(f);
    attendancesModified = 0;
}
void saveAttendances()
{
    if (!attendancesModified) return;

    ensureDataDir();
    FILE *f = fopen(FILENAMEATTENDANCE, "wb");
    if (!f) {
        printf("Erro ao salvar comparecimentos em %s\n", FILENAMEATTENDANCE);
        return;
    }

    fwrite(attendances, sizeof(attendance), attendancesCount, f);
    fclose(f);
    attendancesModified = 0;
}

// Utils
void pushAttendance(const attendance *a)
{
    if (attendancesCount >= attendancesCapacity) {
        attendancesCapacity = attendancesCapacity ? attendancesCapacity + 10 : 10;
        attendance *tmp = realloc(attendances, attendancesCapacity * sizeof(attendance));
        if (!tmp) {
            printf("Erro ao alocar comparecimentos\n");
            exit(-1);
        }
        attendances = tmp;
    }
    attendances[attendancesCount++] = *a;
    attendancesModified = 1;
}
int hasAlreadyVoted(const char *cpf, int year, int ufCode)
{
    for (int i = 0; i < attendancesCount; i++)
        if (strcmp(attendances[i].cpf, cpf) == 0 &&
            attendances[i].year == year &&
            attendances[i].ufCode == ufCode)
            return 1;
    return 0;
}
void registerAttendance(const char *cpf, int year, int ufCode)
{
    attendance a;
    strncpy(a.cpf, cpf, sizeof(a.cpf));
    a.year = year;
    a.ufCode = ufCode;
    pushAttendance(&a);
}
void showAttendanceCount()
{
    int year, uf;
    printf("Digite o ano da eleicao: ");
    scanf("%d", &year);
    printf("Digite o codigo da UF: ");
    scanf("%d", &uf);
    cleanerKeyboard();

    int count = 0;
    for (int i = 0; i < attendancesCount; i++)
        if (attendances[i].year == year && attendances[i].ufCode == uf)
            count++;

    printf("Comparecimentos em %d na UF %d: %d\n", year, uf, count);
    printf("Pressione Enter para continuar...\n");
    cleanerKeyboard();
}