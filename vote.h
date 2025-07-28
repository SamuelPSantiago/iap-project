#ifndef VOTE_H
#define VOTE_H

// File name for storing vote records
#define FILENAMEVOTE "data/vote.data"

// Vote schema
typedef struct {
    int year; // Election year (must exist in election records)
    int ufCode; // UF code (must exist in UF records)
    int candidateNumber; // Candidate number (must exist in candidate records)
    char dateTime[20]; // Date and time "DD-MM-YYYY HH:MM:SS"
} vote;

// Menu controllers
void menuVote(); // Display vote menu
void stateMachineVote(); // Control vote menu flow

// File management
void loadVotes(); // Load votes from file
void saveVotes(); // Save votes to file

// CRUD functions
void createVote(); // Record a new vote
void showVotesByCandidateAndYear(); // Show votes by candidate for a UF/year
void showVotesByYear(); // Show all votes by year ordered by UF

// Utils
void pushVote(const vote *v); // Add vote to in-memory array

void readCPFVote(char *out, const char *prompt); // Read CPF for voting
void readYearVote(vote *tmp, const char *prompt); // Read election year
void readUFCodeVote(vote *tmp, const char *prompt); // Read UF code
void readCandidateNumber(vote *tmp, const char *prompt); // Read candidate number

void printShowVoteHeader(const char *header); // Print header for showing votes
void printShowVoteBorder(); // Print table border for showing votes
void printShowVoteUI(const vote *v); // Print a single vote record

void setCurrentDateTime(vote *tmp); // Set current date and time in vote

const vote *getVotes(); // Get pointer to votes array
int getVotesCount();  // Get count of votes

#endif