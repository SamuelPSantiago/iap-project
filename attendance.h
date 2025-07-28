#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#define FILENAMEATTENDANCE "data/attendance.data"

typedef struct {
    char cpf[12];
    int year;
    int ufCode;
} attendance;

void loadAttendances();
void saveAttendances();
void pushAttendance(const attendance *a);

int hasAlreadyVoted(const char *cpf, int year, int ufCode);
void registerAttendance(const char *cpf, int year, int ufCode);
void showAttendanceCount();

const attendance *getAttendances();
int getAttendancesCount();

#endif