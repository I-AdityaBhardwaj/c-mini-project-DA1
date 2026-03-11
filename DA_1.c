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
    FILE *fp;
    fp = fopen("transactions.dat", "a");

    time_t t;
    time(&t);

    fprintf(fp,"%d %s %.2f %s",accNo,type,amount,ctime(&t));

    fclose(fp);
}

void createAccount()
{
    struct Account a;
    FILE *fp;

    fp = fopen("accounts.dat","a");

    printf("Enter Account Number: ");
    scanf("%d",&a.accNo);

    printf("Enter Name: ");
    scanf("%s",a.name);

    printf("Enter Initial Balance: ");
    scanf("%f",&a.balance);

    if(a.balance < 0)
    {
        printf("Balance cannot be negative\n");
        return;
    }

    fwrite(&a,sizeof(a),1,fp);

    fclose(fp);

    printf("Account Created Successfully\n");
}

void deposit()
{
    int acc;
    float amount;
    struct Account a;
    FILE *fp;

    printf("Enter Account Number: ");
    scanf("%d",&acc);

    printf("Enter Amount: ");
    scanf("%f",&amount);

    if(amount<=0)
    {
        printf("Invalid amount\n");
        return;
    }

    fp = fopen("accounts.dat","r+");

    while(fread(&a,sizeof(a),1,fp))
    {
        if(a.accNo==acc)
        {
            a.balance += amount;

            fseek(fp,-sizeof(a),SEEK_CUR);
            fwrite(&a,sizeof(a),1,fp);

            fclose(fp);

            logTransaction(acc,"Deposit",amount);

            printf("Deposit Successful\n");
            return;
        }
    }

    fclose(fp);
    printf("Account Not Found\n");
}

void withdraw()
{
    int acc;
    float amount;
    struct Account a;
    FILE *fp;

    printf("Enter Account Number: ");
    scanf("%d",&acc);

    printf("Enter Amount: ");
    scanf("%f",&amount);

    fp = fopen("accounts.dat","r+");

    while(fread(&a,sizeof(a),1,fp))
    {
        if(a.accNo==acc)
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

            fclose(fp);

            logTransaction(acc,"Withdraw",amount);

            printf("Withdrawal Successful\n");
            return;
        }
    }

    fclose(fp);
    printf("Account Not Found\n");
}

void searchAccount()
{
    int acc;
    struct Account a;
    FILE *fp;

    printf("Enter Account Number: ");
    scanf("%d",&acc);

    fp = fopen("accounts.dat","r");

    while(fread(&a,sizeof(a),1,fp))
    {
        if(a.accNo==acc)
        {
            printf("\nAccount Found\n");
            printf("Name: %s\n",a.name);
            printf("Balance: %.2f\n",a.balance);

            fclose(fp);
            return;
        }
    }

    fclose(fp);
    printf("Account Not Found\n");
}

void displaySummary()
{
    struct Account a;
    FILE *fp;

    fp = fopen("accounts.dat","r");

    printf("\n----Account List----\n");

    while(fread(&a,sizeof(a),1,fp))
    {
        printf("AccNo: %d  Name: %s  Balance: %.2f\n",a.accNo,a.name,a.balance);
    }

    fclose(fp);
}

void showLastTransactions()
{
    int acc;
    int count=0;

    printf("Enter Account Number: ");
    scanf("%d",&acc);

    FILE *fp = fopen("transactions.dat","r");

    char line[200];

    printf("\nRecent Transactions:\n");

    while(fgets(line,sizeof(line),fp))
    {
        int fileAcc;

        sscanf(line,"%d",&fileAcc);

        if(fileAcc==acc)
        {
            printf("%s",line);
            count++;
        }

        if(count==5)
        break;
    }

    fclose(fp);
}

int main()
{
    int choice;

    while(1)
    {
        printf("\n----- Mini Banking System -----\n");

        printf("1 Create Account\n");
        printf("2 Deposit\n");
        printf("3 Withdraw\n");
        printf("4 Search Account\n");
        printf("5 Account Summary\n");
        printf("6 Show Last Transactions\n");
        printf("7 Exit\n");

        printf("Enter choice: ");
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

            default: printf("Invalid Choice\n");
        }
    }
}