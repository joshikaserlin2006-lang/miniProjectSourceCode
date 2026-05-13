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

// prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);

int main(int argc, char *argv[]) {
    FILE *cfPtr;
    unsigned int choice;

    // Use "rb+" for update, but "wb+" if the file doesn't exist yet
    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL) {
        // If file doesn't exist, try creating it
        if ((cfPtr = fopen("credit.dat", "wb+")) == NULL) {
            printf("File could not be opened.\n");
            exit(-1);
        }
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

void textFile(FILE *readPtr) {
    FILE *writePtr;
    struct clientData client = {0};

    if ((writePtr = fopen("accounts.txt", "w")) == NULL) {
        puts("File could not be opened.");
    } else {
        rewind(readPtr);
        fprintf(writePtr, "%-6s%-16s%-11s%-10s%-10s\n", "Acct", "Last Name", "First Name", "Balance", "Branch");

        while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1) {
            if (client.acctNum != 0) {
                fprintf(writePtr, "%-6d%-16s%-11s%10.2f %-10s\n", 
                        client.acctNum, client.lastName, client.firstName, client.balance, client.branch);
            }
        }
        fclose(writePtr);
        printf("Output saved to accounts.txt\n");
    }
}

void updateRecord(FILE *fPtr) {
    unsigned int account;
    double transaction;
    struct clientData client = {0};

    printf("Enter account to update ( 1 - 100 ): ");
    scanf("%u", &account);

    if (account < 1 || account > 100) {
        puts("Invalid account number.");
        return;
    }

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr); // FIXED: Removed negative size

    if (client.acctNum == 0) {
        printf("Account #%d has no information.\n", account);
    } else {
        printf("%-6d%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);
        printf("Enter charge (+) or payment (-): ");
        scanf("%lf", &transaction);
        client.balance += transaction;

        fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET); // Better to use SEEK_SET for clarity
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
        printf("Transaction complete. New balance: %.2f\n", client.balance);
    }
}

void deleteRecord(FILE *fPtr) {
    struct clientData client = {0};
    struct clientData blankClient = {0};
    unsigned int accountNum;

    printf("Enter account number to delete ( 1 - 100 ): ");
    scanf("%u", &accountNum);

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0) {
        printf("Account %d does not exist.\n", accountNum);
    } else {
        fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
        printf("Account #%d deleted.\n", accountNum);
    }
}

void newRecord(FILE *fPtr) {
    struct clientData client = {0};
    unsigned int accountNum;

    printf("Enter new account number ( 1 - 100 ): ");
    scanf("%u", &accountNum);

    if (accountNum < 1 || accountNum > 100) {
        puts("Invalid account number.");
        return;
    }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum != 0) {
        printf("Account #%d already exists.\n", client.acctNum);
    } else {
        client.acctNum = accountNum;
        printf("Enter Last Name, First Name, Balance, Account Type (Savings/Current), Branch:\n? ");
        scanf("%14s %9s %lf %9s %19s", client.lastName, client.firstName, &client.balance, client.accountType, client.branch);

        fseek(fPtr, (client.acctNum - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
    }
}

unsigned int enterChoice(void) {
    unsigned int menuChoice;
    printf("\n1 - Store text file\n2 - Update account\n3 - Add new account\n4 - Delete account\n5 - End program\n? ");
    if (scanf("%u", &menuChoice) != 1) return 5; 
    return menuChoice;
}