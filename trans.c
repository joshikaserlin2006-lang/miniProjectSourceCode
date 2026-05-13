#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ... [struct clientData remains the same] ...

// New helper function to clear trailing input
void clearBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Function to initialize file with 100 empty records
void initializeFile(FILE *fPtr) {
    struct clientData blankClient = {0, "", "", 0.0, "", "", "", "", 0};
    rewind(fPtr);
    for (int i = 0; i < 100; ++i) {
        fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
    }
}

int main(int argc, char *argv[]) {
    FILE *cfPtr;
    unsigned int choice;

    // Check if file exists first
    cfPtr = fopen("credit.dat", "rb+");
    
    if (cfPtr == NULL) {
        // If it doesn't exist, create it and initialize it
        if ((cfPtr = fopen("credit.dat", "wb+")) == NULL) {
            printf("File could not be opened.\n");
            exit(-1);
        }
        printf("Initializing new database file...\n");
        initializeFile(cfPtr);
    }

    while ((choice = enterChoice()) != 5) {
        switch (choice) {
            case 1: textFile(cfPtr); break;
            case 2: updateRecord(cfPtr); break;
            case 3: newRecord(cfPtr); break;
            case 4: deleteRecord(cfPtr); break;
            default: puts("Incorrect choice"); break;
        }
    }

    fclose(cfPtr);
    return 0;
}

// Updated newRecord with buffer clearing and better prompts
void newRecord(FILE *fPtr) {
    struct clientData client = {0};
    unsigned int accountNum;

    printf("Enter new account number (1 - 100): ");
    if (scanf("%u", &accountNum) != 1) {
        clearBuffer();
        return;
    }

    if (accountNum < 1 || accountNum > 100) {
        printf("Account #%u is out of range.\n", accountNum);
        return;
    }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum != 0) {
        printf("Account #%d already contains information.\n", client.acctNum);
    } else {
        client.acctNum = accountNum;
        
        printf("Enter Last Name: ");
        scanf("%14s", client.lastName);
        printf("Enter First Name: ");
        scanf("%9s", client.firstName);
        printf("Enter Balance: ");
        scanf("%lf", &client.balance);
        printf("Enter Account Type (Savings/Current): ");
        scanf("%9s", client.accountType);
        printf("Enter Branch: ");
        scanf("%19s", client.branch);
        printf("Enter 4-digit PIN: ");
        scanf("%d", &client.pin);
        
        clearBuffer(); // Clean up for next menu

        fseek(fPtr, (client.acctNum - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
        printf("Record created successfully.\n");
    }
}

// Updated updateRecord with input validation
void updateRecord(FILE *fPtr) {
    unsigned int account;
    double transaction;
    struct clientData client = {0};

    printf("Enter account to update (1 - 100): ");
    scanf("%u", &account);

    if (account < 1 || account > 100) {
        puts("Invalid account number.");
        return;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0) {
        printf("Account #%d is empty.\n", account);
    } else {
        printf("Current Record: %d %s %s | Balance: %.2f\n", 
               client.acctNum, client.firstName, client.lastName, client.balance);
        
        printf("Enter charge (+) or payment (-): ");
        if (scanf("%lf", &transaction) == 1) {
            client.balance += transaction;
            printf("New balance: %.2f\n", client.balance);

            fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
            fwrite(&client, sizeof(struct clientData), 1, fPtr);
        }
    }
    clearBuffer();
}

// ... [other functions remain largely the same, but ensure they use clearBuffer() after scanf] ...