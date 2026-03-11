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

void logTransaction(int accNo, char type[], float amount)
{
    FILE *fp = fopen("transactions.txt","a");

    time_t t;
    time(&t);

    fprintf(fp,"%d %s %.2f %s",accNo,type,amount,ctime(&t));

    fclose(fp);
}

void createAccount()
{
    struct Account a;

    FILE *fp = fopen("accounts.dat","ab");

    printf("\nEnter Account Number: ");
    fflush(stdout);
    scanf("%d",&a.accNo);

    printf("Enter Name: ");
    fflush(stdout);
    scanf("%s",a.name);

    printf("Enter Initial Balance: ");
    fflush(stdout);
    scanf("%f",&a.balance);

    if(a.balance < 0)
    {
        printf("Balance cannot be negative\n");
        return;
    }

    fwrite(&a,sizeof(a),1,fp);

    fclose(fp);

    printf("\nAccount Created Successfully\n");
}

void deposit()
{
    int acc;
    float amount;
    struct Account a;

    FILE *fp = fopen("accounts.dat","rb+");

    printf("\nEnter Account Number: ");
    fflush(stdout);
    scanf("%d",&acc);

    printf("Enter Amount: ");
    fflush(stdout);
    scanf("%f",&amount);

    if(amount <= 0)
    {
        printf("Invalid amount\n");
        return;
    }

    while(fread(&a,sizeof(a),1,fp))
    {
        if(a.accNo == acc)
        {
            a.balance += amount;

            fseek(fp,-sizeof(a),SEEK_CUR);
            fwrite(&a,sizeof(a),1,fp);

            logTransaction(acc,"Deposit",amount);

            printf("\nDeposit Successful\n");
            fclose(fp);
            return;
        }
    }

    printf("\nAccount Not Found\n");
    fclose(fp);
}

void withdraw()
{
    int acc;
    float amount;
    struct Account a;

    FILE *fp = fopen("accounts.dat","rb+");

    printf("\nEnter Account Number: ");
    fflush(stdout);
    scanf("%d",&acc);

    printf("Enter Amount: ");
    fflush(stdout);
    scanf("%f",&amount);

    while(fread(&a,sizeof(a),1,fp))
    {
        if(a.accNo == acc)
        {
            if(a.balance < amount)
            {
                printf("\nInsufficient Balance\n");
                fclose(fp);
                return;
            }

            a.balance -= amount;

            fseek(fp,-sizeof(a),SEEK_CUR);
            fwrite(&a,sizeof(a),1,fp);

            logTransaction(acc,"Withdraw",amount);

            printf("\nWithdrawal Successful\n");
            fclose(fp);
            return;
        }
    }

    printf("\nAccount Not Found\n");
    fclose(fp);
}

void searchAccount()
{
    int acc;
    struct Account a;

    FILE *fp = fopen("accounts.dat","rb");

    printf("\nEnter Account Number: ");
    fflush(stdout);
    scanf("%d",&acc);

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

    printf("\nAccount Not Found\n");
    fclose(fp);
}

void displaySummary()
{
    struct Account a;

    FILE *fp = fopen("accounts.dat","rb");

    printf("\n----- Account Summary -----\n");

    while(fread(&a,sizeof(a),1,fp))
    {
        printf("\nAccNo: %d",a.accNo);
        printf("\nName: %s",a.name);
        printf("\nBalance: %.2f\n",a.balance);
    }

    fclose(fp);
}

void showLastTransactions()
{
    int acc;
    char line[200];
    int count = 0;

    printf("\nEnter Account Number: ");
    fflush(stdout);
    scanf("%d",&acc);

    FILE *fp = fopen("transactions.txt","r");

    printf("\nRecent Transactions:\n");

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
        printf("\n\n===== Mini Banking System =====\n");
        printf("1. Create Account\n");
        printf("2. Deposit\n");
        printf("3. Withdraw\n");
        printf("4. Search Account\n");
        printf("5. Account Summary\n");
        printf("6. Last 5 Transactions\n");
        printf("7. Exit\n");

        printf("Enter choice: ");
        fflush(stdout);
        scanf("%d",&choice);

        switch(choice)
        {
            case 1: createAccount(); break;
            case 2: deposit(); break;
            case 3: withdraw(); break;
            case 4: searchAccount(); break;
            case 5: displaySummary(); break;
            case 6: showLastTransactions(); break;
            case 7: exit(0);
            default: printf("Invalid choice\n");
        }
    }

    return 0;
}