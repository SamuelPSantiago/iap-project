#ifndef VOTE_H
#define VOTE_H

// File name for storing vote records
#define FILENAMEVOTE "data/vote.data"

// Vote schema
typedef struct {
    int year; // Election year (must exist in election records)
    int ufCode; // UF code (must exist in UF records)
    int candidateNumber; // Candidate number (must exist in candidate records)
    char dateTime[17]; // Date and time "DD-MM-YYYY HH:MM"
} vote;

// Menu controllers
void menuVote();              // Display vote menu
void stateMachineVote();      // Control vote menu flow

// File management
void loadVotes();             // Load votes from file
void saveVotes();             // Save votes to file

// CRUD functions
void createVote();            // Record a new vote
void showVotesByCandidateAndYear(); // Show votes by candidate for a UF/year
void showVotesByYear();            // Show all votes by year ordered by UF

// Utils
void pushVote(const vote *v);     // Add vote to in-memory array

void readYearVote(vote *tmp, const char *prompt); // Read election year
void readUFCodeVote(vote *tmp, const char *prompt); // Read UF code
void readCandidateNumber(vote *tmp, const char *prompt); // Read candidate number
void readDateTime(vote *tmp, const char *prompt); // Read date and time of vote

#endif