/*
Project Title: Mini Banking System with Transaction Log
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // Added for usleep() to prevent browser freezing

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

    do {
        printf("\n--- Mini Banking System ---\n");
        printf("1. Create Account\n");
        printf("2. Deposit Money\n");
        printf("3. Withdraw Money\n");
        printf("4. Search Account / View Summary\n");
        printf("5. Display Last 5 Transactions\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        
        // FIX: Check for EOF and add a tiny delay
        // This stops the "Unresponsive" error by giving the browser a breather
        res = scanf("%d", &choice);
        
        if (res == EOF) {
            usleep(100000); // Wait 0.1 seconds if no input is ready
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
    
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

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

// Helper function to find an account
int findAccount(int accNo, Account *acc) {
    FILE *fp = fopen(accFile, "rb");
    if (fp == NULL) return 0;
    
    while(fread(acc, sizeof(Account), 1, fp)) {
        if(acc->accNo == accNo) {
            fclose(fp);
            return 1; 
        }
    }
    fclose(fp);
    return 0; 
}

// 2. & 3. Function to handle Deposits and Withdrawals
void depositOrWithdraw(int isDeposit) {
    int accNo;
    float amount;
    Account acc;
    int found = 0;
    
    printf("Enter Account Number: ");
    if(scanf("%d", &accNo) != 1) return;

    FILE *fp = fopen(accFile, "rb+");
    if (fp == NULL) {
        printf("No accounts found! Please create one first.\n");
        return;
    }

    while(fread(&acc, sizeof(Account), 1, fp)) {
        if(acc.accNo == accNo) {
            found = 1;
            printf("Current Balance: %.2f\n", acc.balance);
            printf("Enter amount to %s: ", isDeposit ? "deposit" : "withdraw");
            scanf("%f", &amount);

            if (amount <= 0) {
                printf("Amount must be greater than zero!\n");
                fclose(fp);
                return;
            }

            if (!isDeposit && (acc.balance - amount < 0)) {
                printf("Error: Insufficient balance.\n");
                fclose(fp);
                return;
            }

            if (isDeposit) acc.balance += amount;
            else acc.balance -= amount;

            fseek(fp, -sizeof(Account), SEEK_CUR);
            fwrite(&acc, sizeof(Account), 1, fp);
            
            printf("Success! New Balance: %.2f\n", acc.balance);
            logTransaction(accNo, isDeposit ? "Deposit" : "Withdrawal", amount);
            break;
        }
    }
    
    if(!found) printf("Account not found!\n");
    fclose(fp);
}

// 4. Function to search summary
void searchAccount() {
    int accNo;
    Account acc;
    printf("Enter Account Number: ");
    if(scanf("%d", &accNo) != 1) return;

    if(findAccount(accNo, &acc)) {
        printf("\n--- Account Summary ---\n");
        printf("Acc No: %d | Name: %s | Balance: %.2f\n", acc.accNo, acc.name, acc.balance);
    } else {
        printf("Account not found!\n");
    }
}

// 5. Function to log transactions
void logTransaction(int accNo, const char* type, float amount) {
    FILE *fp = fopen(logFile, "a");
    if (fp == NULL) return;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char dateStr[50];
    sprintf(dateStr, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    fprintf(fp, "%d,%s,%.2f,%s\n", accNo, type, amount, dateStr);
    fclose(fp);
}

// 6. Function to view last 5 transactions
void viewLastTransactions() {
    int searchAccNo;
    printf("Enter Account Number: ");
    if(scanf("%d", &searchAccNo) != 1) return;

    FILE *fp = fopen(logFile, "r");
    if (fp == NULL) {
        printf("No logs found!\n");
        return;
    }

    char lines[100][150];
    int count = 0, accNo;
    char type[20], dateStr[50], buffer[150];
    float amount;

    while (fgets(buffer, sizeof(buffer), fp)) {
        sscanf(buffer, "%d,%[^,],%f,%[^\n]", &accNo, type, &amount, dateStr);
        if (accNo == searchAccNo) {
            strcpy(lines[count++], buffer);
        }
    }
    fclose(fp);

    if (count == 0) {
        printf("No records found.\n");
        return;
    }

    printf("\n--- Last 5 Transactions ---\n");
    int start = (count > 5) ? count - 5 : 0;
    for (int i = start; i < count; i++) {
        sscanf(lines[i], "%d,%[^,],%f,%[^\n]", &accNo, type, &amount, dateStr);
        printf("%s | %-10s | %.2f\n", dateStr, type, amount);
    }
}