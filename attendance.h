#ifndef ATTENDANCE_H
#define ATTENDANCE_H

// File name for storing attendance records
#define FILENAMEATTENDANCE "data/attendance.data"

// Attendance schema
typedef struct {
    char cpf[12]; // Citizen CPF (11 digits + '\0')
    int  year; // Election year
    int  ufCode; // UF code
} attendance;

// File management
void loadAttendance(); // Load attendance from file

// Display functions
void showAttendanceCountByUFAndYear(); // Count attendance by UF and year
void showAttendanceByYear(); // List attendance by year ordered by UF

// Utils
void pushAttendance(const attendance *a); // Add to in-memory array

#endif