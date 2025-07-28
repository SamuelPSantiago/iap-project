#ifndef RESULT_H
#define RESULT_H

typedef struct {
    int number;
    int count;
} candidateResult;

void menuResult(); // Display the election results menu
void stateMachineResult(); // Control the result menu flow

void showElectionResults(); // Display election results for a specific year and UF
void showElectionAttendance(); // Display attendance count for a specific year and UF

void printShowResultHeader(const char *title); // Print header for showing results
void printShowResultBorder(); // Print border for showing results
void printShowResultUI(const candidateResult *result, int totalVotes); // Print a single result record

#endif