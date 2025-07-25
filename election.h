#ifndef ELECTION_H
#define ELECTION_H

#include "UF.h"

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
void menuElection(); // Display the election menu
void stateMachineElection(); // State machine for election operations

// File Management
void loadElections(); // Load elections from file
void saveElections(); // Save elections to file

// CRUD functions
void createElection(); // Create a new election
void updateElection(); // Update an existing election
void deleteElection(); // Delete an election
void listElections(); // List all elections
void viewElection(); // View details of a specific election

// Utils
void pushElection(const election *item); // Add an election to the list
void readElectionYear(int *year, const char *prompt); // Read election year from user input
void readElectionUfCode(int *ufCode, const char *prompt); // Read UF code for the election from user input
void readElectionDescription(election *item, const char *prompt); // Read election description from user input

int findElectionIndex(int year, int ufCode); // Find the index of an election by year and UF code
int searchElectionByYear(int year); // Search for an election by year
int searchElectionByUfCode(int ufCode); // Search for an election by UF code

void printElectionHeader(const char *title); // Print header for election records
void printElectionBorder(); // Print header for election records
void printElectionRow(const election *item); // Print a single election record

void freeElections(); // Free memory allocated for elections

#endif