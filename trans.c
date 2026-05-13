#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// clientData structure definition
struct clientData {
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
    char accountType[10];
    char ifsc[15];
    char branch[20];
    char phone[15];
    int pin;
};

// Prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void clearBuffer(void);
void initializeFile(FILE *fPtr);

int main(int argc, char *argv[]) {
    FILE *cfPtr;
    unsigned int choice;

    // Try opening for read/write. If it fails, create it.
    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL) {
        if ((cfPtr = fopen("credit.dat", "wb+")) == NULL) {
            printf("Fatal Error: Could not create database file.\n");
            exit(1);
        } else {
            printf("New database detected. Formatting 100 records...\n");
            initializeFile(cfPtr);
        }
    }

    while ((choice = enterChoice()) != 5) {
        switch (choice) {
            case 1: textFile(cfPtr); break;
            case 2: updateRecord(cfPtr); break;
            case 3: newRecord(cfPtr); break;
            case 4: deleteRecord(cfPtr); break;
            default: puts("Incorrect choice. Please try again."); break;
        }
    }

    fclose(cfPtr);
    return 0;
}

// Clears the standard input buffer to prevent scanf infinite loops
void clearBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Fills the file with 100 blank records to prepare for random access
void initializeFile(FILE *fPtr) {
    struct clientData blankClient = {0, "", "", 0.0, "", "", "", "", 0};
    rewind(fPtr);
    for (int i = 0; i < 100; ++i) {
        fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
    }
}

void textFile(FILE *readPtr) {
    FILE *writePtr;
    struct clientData client = {0};

    if ((writePtr = fopen("accounts.txt", "w")) == NULL) {
        puts("Error: Could not create export file.");
    } else {
        rewind(readPtr);
        fprintf(writePtr, "%-6s%-16s%-11s%-10s%-15s\n", "Acct", "Last Name", "First Name", "Balance", "Branch");

        while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1) {
            if (client.acctNum != 0) {
                fprintf(writePtr, "%-6d%-16s%-11s%10.2f %-15s\n", 
                        client.acctNum, client.lastName, client.firstName, client.balance, client.branch);
            }
        }
        fclose(writePtr);
        printf("Success: Data exported to accounts.txt\n");
    }
}

void updateRecord(FILE *fPtr) {
    unsigned int account;
    double transaction;
    struct clientData client = {0};

    printf("Enter account to update (1 - 100): ");
    if (scanf("%u", &account) != 1) { clearBuffer(); return; }

    if (account < 1 || account > 100) {
        puts("Error: Account number must be between 1 and 100.");
        return;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0) {
        printf("Account #%u has no information.\n", account);
    } else {
        printf("%-6d%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);
        printf("Enter charge (+) or payment (-): ");
        scanf("%lf", &transaction);
        client.balance += transaction;

        fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
        printf("Record updated.\n");
    }
    clearBuffer();
}

void newRecord(FILE *fPtr) {
    struct clientData client = {0};
    unsigned int accountNum;

    printf("Enter new account number (1 - 100): ");
    if (scanf("%u", &accountNum) != 1) { clearBuffer(); return; }

    if (accountNum < 1 || accountNum > 100) {
        printf("Error: Account #%u is out of range.\n", accountNum);
        return;
    }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum != 0) {
        printf("Account #%d already contains information.\n", client.acctNum);
    } else {
        client.acctNum = accountNum;
        printf("Enter Last Name, First Name, Balance:\n? ");
        scanf("%14s %9s %lf", client.lastName, client.firstName, &client.balance);
        
        printf("Enter Account Type, Branch, Phone:\n? ");
        scanf("%9s %19s %14s", client.accountType, client.branch, client.phone);

        fseek(fPtr, (client.acctNum - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
        printf("Record created.\n");
    }
    clearBuffer();
}

void deleteRecord(FILE *fPtr) {
    struct clientData client = {0};
    struct clientData blankClient = {0};
    unsigned int accountNum;

    printf("Enter account number to delete (1 - 100): ");
    if (scanf("%u", &accountNum) != 1) { clearBuffer(); return; }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0) {
        printf("Account %d does not exist.\n", accountNum);
    } else {
        fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
        printf("Account #%d deleted.\n", accountNum);
    }
    clearBuffer();
}

unsigned int enterChoice(void) {
    unsigned int menuChoice;
    printf("\n1 - Export to text file\n2 - Update balance\n3 - Add new account\n4 - Delete account\n5 - End program\n? ");
    if (scanf("%u", &menuChoice) != 1) {
        clearBuffer();
        return 0;
    }
    return menuChoice;
}