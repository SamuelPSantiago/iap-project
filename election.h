#ifndef ELECTION_H
#define ELECTION_H

#include "uf.h"

// File name for storing Election records
#define FILENAMEELECTION "data/election.data"

// Election schema
typedef struct {
    int year;
    int ufCode;
    char description[100];
    
    int deleted;
} election;

// Menu controllers
void menuElection();
void stateMachineElection();

// File Management
void loadElections();
void saveElections();

// CRUD functions
void createElection();
void updateElection();
void deleteElection();
void listElections();
void viewElection();

// Utils
void pushElection(const election *item);
void readElectionYear(int *year, const char *prompt);
void readElectionUfCode(int *ufCode, const char *prompt);
void readElectionDescription(election *item, const char *prompt);
int findElectionIndex(int year, int ufCode);

void printElectionHeader(const char *title);
void printElectionBorder();
void printElectionRow(const election *item);

void freeElections();

#endif