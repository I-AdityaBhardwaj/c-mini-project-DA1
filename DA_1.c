/*
Project Title: Mini Banking System with Transaction Log
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* STRUCTURES */

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


/* FUNCTION PROTOTYPES */

void createAccount();
void depositOrWithdraw(int isDeposit);
void searchAccount();
void viewLastTransactions();
void logTransaction(int accNo,const char *type,float amount);
int findAccount(int accNo,Account *acc);


/* FILE NAMES */

const char *accFile="accounts.dat";
const char *logFile="transactions.log";


/* MAIN PROGRAM */

int main(){

    int choice;

    setvbuf(stdout,NULL,_IONBF,0);

    do{

        printf("\n--- Mini Banking System ---\n");
        printf("1. Create Account\n");
        printf("2. Deposit Money\n");
        printf("3. Withdraw Money\n");
        printf("4. Search Account / View Summary\n");
        printf("5. Display Last 5 Transactions\n");
        printf("6. Exit\n");

        printf("Enter your choice: ");

        if(scanf("%d",&choice)!=1){
            printf("Invalid input. Please enter a number.\n");
            while(getchar()!='\n');
            continue;
        }

        switch(choice){

            case 1:
                createAccount();
                break;

            case 2:
                depositOrWithdraw(1);
                break;

            case 3:
                depositOrWithdraw(0);
                break;

            case 4:
                searchAccount();
                break;

            case 5:
                viewLastTransactions();
                break;

            case 6:
                printf("Exiting System...\n");
                break;

            default:
                printf("Invalid choice!\n");
        }

    }while(choice!=6);

    return 0;
}


/* CREATE ACCOUNT */

void createAccount(){

    Account acc;
    Account temp;

    FILE *fp=fopen(accFile,"ab");

    if(fp==NULL){
        printf("File error!\n");
        return;
    }

    printf("Enter Account Number: ");

    if(scanf("%d",&acc.accNo)!=1){
        printf("Invalid account number.\n");
        fclose(fp);
        return;
    }

    if(findAccount(acc.accNo,&temp)){
        printf("Account already exists!\n");
        fclose(fp);
        return;
    }

    printf("Enter Account Holder Name: ");
    scanf(" %[^\n]",acc.name);

    printf("Enter Initial Balance: ");
    scanf("%f",&acc.balance);

    if(acc.balance<0){
        printf("Balance cannot be negative.\n");
        fclose(fp);
        return;
    }

    fwrite(&acc,sizeof(Account),1,fp);

    fclose(fp);

    printf("Account created successfully!\n");

    if(acc.balance>0)
        logTransaction(acc.accNo,"Initial Deposit",acc.balance);
}


/* FIND ACCOUNT */

int findAccount(int accNo,Account *acc){

    FILE *fp=fopen(accFile,"rb");

    if(fp==NULL)
        return 0;

    while(fread(acc,sizeof(Account),1,fp)){

        if(acc->accNo==accNo){

            fclose(fp);
            return 1;
        }
    }

    fclose(fp);

    return 0;
}


/* DEPOSIT / WITHDRAW */

void depositOrWithdraw(int isDeposit){

    int accNo;
    float amount;
    Account acc;

    int found=0;

    printf("Enter Account Number: ");

    if(scanf("%d",&accNo)!=1)
        return;

    FILE *fp=fopen(accFile,"rb+");

    if(fp==NULL){

        printf("No records found!\n");
        return;
    }

    while(fread(&acc,sizeof(Account),1,fp)){

        if(acc.accNo==accNo){

            found=1;

            printf("Current Balance: %.2f\n",acc.balance);

            printf("Enter amount to %s: ",isDeposit?"deposit":"withdraw");

            if(scanf("%f",&amount)!=1)
                break;

            if(amount<=0){

                printf("Amount must be positive.\n");
            }

            else if(!isDeposit && acc.balance<amount){

                printf("Insufficient balance!\n");
            }

            else{

                if(isDeposit)
                    acc.balance+=amount;
                else
                    acc.balance-=amount;

                fseek(fp,-sizeof(Account),SEEK_CUR);

                fwrite(&acc,sizeof(Account),1,fp);

                printf("Transaction successful!\n");
                printf("New Balance: %.2f\n",acc.balance);

                logTransaction(accNo,isDeposit?"Deposit":"Withdrawal",amount);
            }

            break;
        }
    }

    if(!found)
        printf("Account not found!\n");

    fclose(fp);
}


/* SEARCH ACCOUNT */

void searchAccount(){

    int accNo;
    Account acc;

    printf("Enter Account Number: ");

    if(scanf("%d",&accNo)!=1)
        return;

    if(findAccount(accNo,&acc)){

        printf("\n--- Account Summary ---\n");

        printf("Account No: %d\n",acc.accNo);
        printf("Name: %s\n",acc.name);
        printf("Balance: %.2f\n",acc.balance);
    }

    else{

        printf("Account not found!\n");
    }
}


/* LOG TRANSACTION */

void logTransaction(int accNo,const char *type,float amount){

    FILE *fp=fopen(logFile,"a");

    if(fp==NULL)
        return;

    time_t t=time(NULL);

    struct tm tm=*localtime(&t);

    fprintf(fp,"%d,%s,%.2f,%d-%02d-%02d\n",
            accNo,
            type,
            amount,
            tm.tm_year+1900,
            tm.tm_mon+1,
            tm.tm_mday);

    fclose(fp);
}


/* VIEW LAST 5 TRANSACTIONS */

void viewLastTransactions(){

    int accNo;

    printf("Enter Account Number: ");

    if(scanf("%d",&accNo)!=1)
        return;

    FILE *fp=fopen(logFile,"r");

    if(fp==NULL){

        printf("No transaction records found.\n");
        return;
    }

    char lines[100][200];
    int count=0;

    while(fgets(lines[count],200,fp)){

        count++;

        if(count>=100)
            break;
    }

    fclose(fp);

    printf("\nLast Transactions:\n");

    int start=count-5;

    if(start<0)
        start=0;

    for(int i=start;i<count;i++){

        if(strstr(lines[i],",")!=NULL)
            printf("%s",lines[i]);
    }
}