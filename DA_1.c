/*
Project Title: Mini Banking System with Transaction Log (Web-Compatible)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // Required for usleep()

// Structure definitions
typedef struct {
    int accNo;
    char name[50];
    float balance;
} Account;

typedef struct {
    int accNo;
    char type[20];
    float amount;
    char date[50];
} Transaction;

// Function Prototypes
void createAccount();
void depositOrWithdraw(int isDeposit);
void searchAccount();
void viewLastTransactions();
void logTransaction(int accNo, const char* type, float amount);
int findAccount(int accNo, Account *acc);

const char* accFile = "accounts.dat";
const char* logFile = "transactions.log";

int main() {
    int choice;
    int res;

    // Disable output buffering so text appears instantly in the browser
    setvbuf(stdout, NULL, _IONBF, 0);

    do {
        printf("\n--- Mini Banking System ---\n");
        printf("1. Create Account\n");
        printf("2. Deposit Money\n");
        printf("3. Withdraw Money\n");
        printf("4. Search Account / View Summary\n");
        printf("5. Display Last 5 Transactions\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        
        // This is the CRITICAL FIX for the "Unresponsive" error
        res = scanf("%d", &choice);
        
        if (res == EOF) {
            // If no input is waiting, sleep for 100ms and try again.
            // This prevents the CPU from hitting 100% and freezing the tab.
            usleep(100000); 
            continue;
        }

        if (res != 1) {
            printf("Invalid input. Please enter a number.\n");
            while(getchar() != '\n'); // clear buffer
            continue;
        }

        switch(choice) {
            case 1: createAccount(); break;
            case 2: depositOrWithdraw(1); break;
            case 3: depositOrWithdraw(0); break;
            case 4: searchAccount(); break;
            case 5: viewLastTransactions(); break;
            case 6: printf("Exiting System...\n"); break;
            default: printf("Invalid choice! Please try again.\n");
        }
    } while(choice != 6);

    return 0;
}

// 1. Function to create a new account
void createAccount() {
    Account acc;
    FILE *fp = fopen(accFile, "ab"); 
    if (fp == NULL) { printf("Error opening file!\n"); return; }

    printf("Enter Account Number: ");
    if(scanf("%d", &acc.accNo) != 1) return;
    
    Account temp;
    if (findAccount(acc.accNo, &temp)) {
        printf("Account Number already exists!\n");
        fclose(fp);
        return;
    }

    printf("Enter Account Holder Name: ");
    scanf(" %[^\n]s", acc.name);
    
    printf("Enter Initial Balance: ");
    scanf("%f", &acc.balance);

    if (acc.balance < 0) {
        printf("Initial balance cannot be negative!\n");
        fclose(fp);
        return;
    }

    fwrite(&acc, sizeof(Account), 1, fp);
    fclose(fp);
    printf("Account created successfully!\n");
    
    if(acc.balance > 0) {
        logTransaction(acc.accNo, "Initial Deposit", acc.balance);
    }
}

int findAccount(int accNo, Account *acc) {
    FILE *fp = fopen(accFile, "rb");
    if (fp == NULL) return 0;
    while(fread(acc, sizeof(Account), 1, fp)) {
        if(acc->accNo == accNo) { fclose(fp); return 1; }
    }
    fclose(fp);
    return 0; 
}

void depositOrWithdraw(int isDeposit) {
    int accNo;
    float amount;
    Account acc;
    int found = 0;
    
    printf("Enter Account Number: ");
    if(scanf("%d", &accNo) != 1) return;

    FILE *fp = fopen(accFile, "rb+");
    if (fp == NULL) { printf("No records found!\n"); return; }

    while(fread(&acc, sizeof(Account), 1, fp)) {
        if(acc.accNo == accNo) {
            found = 1;
            printf("Current Balance: %.2f\n", acc.balance);
            printf("Enter amount to %s: ", isDeposit ? "deposit" : "withdraw");
            if(scanf("%f", &amount) != 1) break;

            if (amount <= 0) {
                printf("Amount must be positive!\n");
            } else if (!isDeposit && (acc.balance - amount < 0)) {
                printf("Error: Insufficient balance.\n");
            } else {
                if (isDeposit) acc.balance += amount;
                else acc.balance -= amount;

                fseek(fp, -sizeof(Account), SEEK_CUR);
                fwrite(&acc, sizeof(Account), 1, fp);
                printf("Success! New Balance: %.2f\n", acc.balance);
                logTransaction(accNo, isDeposit ? "Deposit" : "Withdrawal", amount);
            }
            break;
        }
    }
    if(!found) printf("Account not found!\n");
    fclose(fp);
}

void searchAccount() {
    int accNo;
    Account acc;
    printf("Enter Account Number: ");
    if(scanf("%d", &accNo) != 1) return;

    if(findAccount(accNo, &acc)) {
        printf("\n--- Account Summary ---\n");
        printf("Acc: %d | Name: %s | Bal: %.2f\n", acc.accNo, acc.name, acc.balance);
    } else {
        printf("Account not found!\n");
    }
}

void logTransaction(int accNo, const char* type, float amount) {
    FILE *fp = fopen(logFile, "a");
    if (fp == NULL) return;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(fp, "%d,%s,%.2f,%d-%02d-%02d\n", accNo, type, amount, tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday);
    fclose(fp);
}

void viewLastTransactions() {
    int searchAccNo;
    printf("Enter Account Number: ");
    if(scanf("%d", &searchAccNo) != 1) return;

    FILE *fp = fopen(logFile, "r");
    if (fp == NULL) { printf("No logs found!\n"); return; }

    char lines[100][150], buffer[150];
    int count = 0, accNo;
    while (fgets(buffer, sizeof(buffer), fp)) {
        sscanf(buffer, "%d,", &accNo);
        if (accNo == searchAccNo) strcpy(lines[count++], buffer);
    }
    fclose(fp);

    if (count == 0) { printf("No records.\n"); return; }
    printf("\n--- Recent Activity ---\n");
    int start = (count > 5) ? count - 5 : 0;
    for (int i = start; i < count; i++) printf("%s", lines[i]);
}