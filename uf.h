#ifndef UF_H
#define UF_H

// File name for storing UFs
#define FILENAMEUF "data/uf.data"

// UF schema
typedef struct {
    int code;
    char acronym[3];
    char description[100];

    int deleted;
} uf;

// Menu controllers
void menuUF(); // Show the menu
void stateMachineUF(); // Control the state machine for UF operations

// File Management
void loadUFs(); // Load UFs from file
void saveUFs(); // Save UFs to file

// CRUD functions
void createUF(); // Creates a new state
void updateUF(); // Updates an existing state
void deleteUF(); // Deletes a state
void showUF(); // Show all states
void showSpecificUF(); // Show a specific state by code

//Utils
void pushUF(const uf *pushedUf); // Push a UF to the array

void readAcronym(uf *tmp, const int ufIndex,const char *prompt); // Read and validate the acronym for a UF
void readDescription(uf *tmp, const char *prompt); // Read and validate the description for a UF

int searchUF(const int cod); // Search for a UF by code
int searchAcronym(const char *sAcronym); // Search for a UF by acronym

int getNextCode(); // Get the next available code for a new UF

void printShowUFHeader(const char *header); // Print the header for showing UFs
void printShowUFBorder(); // Print the border for showing UFs
void printShowUFUI(const uf *p); // Print the UI for showing a UF

void freeUFs(); // Free the allocated memory for UFs

#endif