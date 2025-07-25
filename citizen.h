#ifndef CITIZEN_H
#define CITIZEN_H

// File name for storing Citizen records
#define FILENAMECITIZEN "data/citizen.data"

// Citizen schema
typedef struct {
    char cpf[12]; // CPF (11 digits + '\0')
    char voterNumber[14]; // Voter number (14 digits + '\0')
    char name[100];
    char phone[20]; // Phone number (including area code)
    char address[200];
    char birthdate[11]; // Birth date in "DD-MM-YYYY" format
    
    int deleted;
} citizen;

// Menu controllers
void menuCitizen(); // Display the citizen menu
void stateMachineCitizen(); // Control the citizen menu flow

// File Management
void loadCitizens(); // Load all citizens from file
void saveCitizens(); // Save all citizens to file

// CRUD functions
void createCitizen(); // Create a new citizen
void updateCitizen(); // Update an existing citizen
void deleteCitizen(); // Mark a citizen as deleted
void showCitizen(); // Display all active citizens
void showCitizenByCPF(); // Display a specific citizen by CPF
void showCitizenByVoterNumber(); // Display a specific citizen by voter number

// Utils
void pushCitizen(const citizen *pushedCitizen); // Add a citizen to the in-memory array

void readCPFCitizen(citizen *tmp, const char *prompt); // Read and validate the CPF
void readVoterNumber(citizen *tmp, const char *prompt); // Read and validate the title
void readName(citizen *tmp, const char *prompt); // Read and validate the full name
void readPhone(citizen *tmp, const char *prompt); // Read and validate the phone number
void readAddress(citizen *tmp, const char *prompt); // Read and validate the address
void readBirthdate(citizen *tmp, const char *prompt); // Read and validate the birth date

int searchCitizenByCPF(const char *cpf); // Returns index or -1 if not found
int searchCitizenByVoterNumber(const char *title); // Returns index or -1 if not found

void printShowCitizenHeader(const char *header); // Print the header for showing citizens
void printShowCitizenBorder(); // Print the border for showing citizens
void printShowCitizenUI(const citizen *p); // Print the UI for showing a citizen

citizen *getSpecificCitizen(int idx); // Get a specific citizen by index

void freeCitizens(); // Free the memory allocated for citizens

#endif