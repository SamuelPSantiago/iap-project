#ifndef CANDIDATE_H
#define CANDIDATE_H

// File name for storing candidate records
#define FILENAMECANDIDATE "data/candidate.data"

// Candidate schema
typedef struct {
    int year; // Election year
    int ufCode; // UF code (integer, PK references UF)
    char cpf[12]; // Citizen CPF (11 digits + '\0')
    int number; // Candidate number (2 digits)

    int deleted;
} candidate;

// Menu controllers
void menuCandidate(); // Display the candidate menu
void stateMachineCandidate(); // Control the candidate menu flow

// File management
void loadCandidates(); // Load all candidates from file
void saveCandidates(); // Save all candidates to file

// CRUD functions
void createCandidate(); // Create a new candidate
void deleteCandidate(); // Delete (mark) a candidate

// Display functions
void showCandidatesByUFAndYear(); // Display candidates filtered by UF and year
void showCandidatesByYear(); // Display candidates by year ordered by UF

// Utils
void pushCandidate(const candidate *p); // Add a candidate to the in‑memory array
int  searchCandidateByPK(int year, int ufCode, const char *cpf); // Return index or -1
int  searchCandidateBySK(int ufCode, int year, int number); // Search by UF code, year, and number

void readYear(candidate *tmp, const char *prompt); // Read and validate year
void readUFCode(candidate *tmp, const char *prompt); // Read and validate UF code
void readCPFCandidate(candidate *tmp, const char *prompt); // Read and validate CPF
void readNumber(candidate *tmp, const char *prompt); // Read and validate candidate number

void printShowCandidateHeader(const char *title); // Print header for candidate records
void printShowCandidateBorder(); // Print table border
void printShowCandidateUI(const candidate *item); // Print a single candidate record

void freeCandidates(); // Free allocated memory

#endif