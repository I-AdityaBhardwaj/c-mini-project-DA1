#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct Account
{
    int accNo;
    char name[50];
    float balance;
};

int getInt()
{
    char input[50];
    fgets(input, sizeof(input), stdin);
    int value;
    sscanf(input, "%d", &value);
    return value;
}

float getFloat()
{
    char input[50];
    fgets(input, sizeof(input), stdin);
    float value;
    sscanf(input, "%f", &value);
    return value;
}

void logTransaction(int accNo, char type[], float amount)
{
    FILE *fp = fopen("transactions.txt", "a");

    time_t t = time(NULL);

    fprintf(fp, "%d %s %.2f %s", accNo, type, amount, ctime(&t));

    fclose(fp);
}

void createAccount()
{
    struct Account a;

    FILE *fp = fopen("accounts.dat", "ab");

    printf("\nEnter Account Number: ");
    fflush(stdout);
    a.accNo = getInt();

    printf("Enter Name: ");
    fflush(stdout);
    fgets(a.name, sizeof(a.name), stdin);

    a.name[strcspn(a.name, "\n")] = 0;

    printf("Enter Initial Balance: ");
    fflush(stdout);
    a.balance = getFloat();

    if(a.balance < 0)
    {
        printf("Balance cannot be negative\n");
        return;
    }

    fwrite(&a, sizeof(a), 1, fp);

    fclose(fp);

    printf("Account Created Successfully\n");
}

void deposit()
{
    int acc;
    float amount;
    struct Account a;

    FILE *fp = fopen("accounts.dat", "rb+");

    printf("Enter Account Number: ");
    fflush(stdout);
    acc = getInt();

    printf("Enter Amount: ");
    fflush(stdout);
    amount = getFloat();

    while(fread(&a, sizeof(a), 1, fp))
    {
        if(a.accNo == acc)
        {
            a.balance += amount;

            fseek(fp, -sizeof(a), SEEK_CUR);

            fwrite(&a, sizeof(a), 1, fp);

            logTransaction(acc,"Deposit",amount);

            printf("Deposit Successful\n");

            fclose(fp);
            return;
        }
    }

    printf("Account Not Found\n");

    fclose(fp);
}

void withdraw()
{
    int acc;
    float amount;
    struct Account a;

    FILE *fp = fopen("accounts.dat", "rb+");

    printf("Enter Account Number: ");
    fflush(stdout);
    acc = getInt();

    printf("Enter Amount: ");
    fflush(stdout);
    amount = getFloat();

    while(fread(&a,sizeof(a),1,fp))
    {
        if(a.accNo == acc)
        {
            if(a.balance < amount)
            {
                printf("Insufficient Balance\n");
                fclose(fp);
                return;
            }

            a.balance -= amount;

            fseek(fp,-sizeof(a),SEEK_CUR);

            fwrite(&a,sizeof(a),1,fp);

            logTransaction(acc,"Withdraw",amount);

            printf("Withdrawal Successful\n");

            fclose(fp);
            return;
        }
    }

    printf("Account Not Found\n");

    fclose(fp);
}

void searchAccount()
{
    int acc;
    struct Account a;

    FILE *fp = fopen("accounts.dat","rb");

    printf("Enter Account Number: ");
    fflush(stdout);
    acc = getInt();

    while(fread(&a,sizeof(a),1,fp))
    {
        if(a.accNo == acc)
        {
            printf("\nAccount Found\n");
            printf("Name: %s\n",a.name);
            printf("Balance: %.2f\n",a.balance);

            fclose(fp);
            return;
        }
    }

    printf("Account Not Found\n");

    fclose(fp);
}

void displaySummary()
{
    struct Account a;

    FILE *fp = fopen("accounts.dat","rb");

    printf("\n--- Account Summary ---\n");

    while(fread(&a,sizeof(a),1,fp))
    {
        printf("\nAccNo: %d",a.accNo);
        printf("\nName: %s",a.name);
        printf("\nBalance: %.2f\n",a.balance);
    }

    fclose(fp);
}

void lastTransactions()
{
    int acc;
    char line[200];
    int count = 0;

    printf("Enter Account Number: ");
    fflush(stdout);
    acc = getInt();

    FILE *fp = fopen("transactions.txt","r");

    printf("\nLast Transactions:\n");

    while(fgets(line,sizeof(line),fp))
    {
        int fileAcc;

        sscanf(line,"%d",&fileAcc);

        if(fileAcc == acc)
        {
            printf("%s",line);
            count++;
        }

        if(count == 5)
        break;
    }

    fclose(fp);
}

int main()
{
    int choice;

    while(1)
    {
        printf("\n\n===== MINI BANKING SYSTEM =====\n");

        printf("1 Create Account\n");
        printf("2 Deposit\n");
        printf("3 Withdraw\n");
        printf("4 Search Account\n");
        printf("5 Account Summary\n");
        printf("6 Last 5 Transactions\n");
        printf("7 Exit\n");

        printf("Enter choice: ");
        fflush(stdout);

        choice = getInt();

        switch(choice)
        {
            case 1: createAccount(); break;
            case 2: deposit(); break;
            case 3: withdraw(); break;
            case 4: searchAccount(); break;
            case 5: displaySummary(); break;
            case 6: lastTransactions(); break;
            case 7: exit(0);

            default: printf("Invalid Choice\n");
        }
    }

    return 0;
}