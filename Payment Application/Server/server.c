#include "server.h"
#include <stdio.h>
#include<string.h>
#include <stdlib.h>
#define Def_Acc_Number 10         

EN_serverError_t server_state ;     // create a new variable in EN_serverError_t enum to get the fanction's return
ST_accountsDB_t accountRefrence; // creating variable in accounts-data structer which has 3 members(balance, state, PAN) to do some checks on it
uint32_t transaction_counter = 1;     //first transaction's sequence no. always equal 1 
uint32_t accountNumber = 0;        // to searsh and know the number of user's account in the data base
EN_transStat_t transaction_state;


ST_accountsDB_t accountsDB[255] = {
	{1450.000000,RUNNING,"1471490470778326"},
	{3000.5000,RUNNING,"1528711067713381"},
	{23000.520000,RUNNING,"9064430871219443"},
	{90.000000,BLOCKED,"9699988114694485"},
	{34567.900500,RUNNING,"1072060454141040"},
	{23404.320000,RUNNING,"3218927618802868"},
	{10000.000000,BLOCKED,"1087892437038874"},
	{7.200000,RUNNING,"9971272061194065"},
	{1.700000,BLOCKED,"8130775533233113"},
	{0.900000,RUNNING,"7690963180692997"}

};
// transactions data base is an array of structure variable
ST_transaction_t transactionsDB[255]= { 0 };
//============================================================
/*1. This function will take all transaction data and validate its data, it contains all server logic.
  2. It checks the account details and amount availability.
  3. If the account does not exist return FRAUD_CARD , if the amount is not available will return
     DECLINED_INSUFFECIENT_FUND , if the account is blocked will return DECLINED_STOLEN_CARD , if a
     transaction can't be saved will return INTERNAL_SERVER_ERROR , else returns APPROVED .*/
EN_transStat_t recieveTransactionData(ST_transaction_t* transData)
{   
	if (transData == NULL)
		INTERNAL_SERVER_ERROR;
	server_state = isValidAccount(&transData->cardHolderData,& accountRefrence);
	if (server_state == ACCOUNT_NOT_FOUND)
		return FRAUD_CARD;
	server_state = isBlockedAccount(&accountRefrence);
	if (server_state == BLOCKED_ACCOUNT)
		return DECLINED_STOLEN_CARD;
	server_state = isAmountAvailable(&transData->terminalData, &accountRefrence);
	if (server_state == LOW_BALANCE)
		return DECLINED_INSUFFECIENT_FUND;

	if (saveTransaction(transData) == SAVING_FAILED)
		return INTERNAL_SERVER_ERROR;

	
	return APPROVED;
}
//============================================================
/*1. This function will take card data and validate if the account related to this card exists or not.
2. It checks if the PAN exists or not in the server's database (searches for the card PAN in the DB). */
EN_serverError_t isValidAccount(const ST_cardData_t* cardData, ST_accountsDB_t* accountRefrence)
{
	
	if (cardData == NULL || accountRefrence == NULL || accountsDB == NULL)   //Making sure the pointer is not NULL
		return ACCOUNT_NOT_FOUND;

	for (accountNumber = 0; accountNumber < Def_Acc_Number; accountNumber++)  // searshing for the account in DB by the primary account number
	{
		// converting PAN from chars to float pont
		uint32_t reference = atof(&accountsDB[accountNumber].primaryAccountNumber);
		uint32_t query = atof(&cardData->primaryAccountNumber);
		// comparing the PAN entered by the user with the PAN in accounts data base
		if (query == reference) 
		{
			// if found store avery thing about the account in accountRefrence structure variable
			accountRefrence->balance = accountsDB[accountNumber].balance;  // storing the balance in account refernce
			accountRefrence->state = accountsDB[accountNumber].state;  // storing account state in account referenc
			for(uint8_t iter=0 ; iter<20; iter++)
				accountRefrence->primaryAccountNumber[iter] = accountsDB[accountNumber].primaryAccountNumber[iter]; // storing PAN in account reference
			return SERVER_OK;
		}
	}
	//if not found the account in the data base
	accountRefrence = NULL;
	return ACCOUNT_NOT_FOUND;
}
//============================================================
/* 1. This function takes a reference to the account into the database and verifies if it is blocked or not.
   2. If the account is running it will return SERVER_OK , else if the account is blocked it will return
       BLOCKED_ACCOUNT .*/
EN_serverError_t isBlockedAccount(ST_accountsDB_t* accountRefrence)
{
	if ( accountRefrence == NULL )  //Making sure the pointer is not NULL
		return BLOCKED_ACCOUNT;
	if (accountRefrence->state == BLOCKED)
	{
		return BLOCKED_ACCOUNT;
	}

	return SERVER_OK;
}
//============================================================
/* 1. This function will take terminal data and a reference to the account in the database and check
      if the account has a sufficient amount to withdraw or not.
   2. It checks if the transaction's amount is available or not.
   3. If the transaction amount is greater than the balance in the database will return LOW_BALANCE ,
      else will return SERVER_OK .*/
EN_serverError_t isAmountAvailable(ST_terminalData_t* termData, ST_accountsDB_t* accountRefrence)
{
	if (termData == NULL || accountRefrence == NULL )   //Making sure the pointer is not NULL
		return ACCOUNT_NOT_FOUND;
	if (termData->transAmount > accountRefrence->balance)
	{
		return LOW_BALANCE;
	}
	return SERVER_OK;
}
//============================================================
/* 1. This function will store all transaction data in the transactions database.
   2. It gives a sequence number to a transaction, this number is incremented once a transaction is
      processed into the server, you must check the last sequence number in the server to give the new
      transaction a new sequence number.
   3. It saves any type of transaction, APPROVED , DECLINED_INSUFFECIENT_FUND , DECLINED_STOLEN_CARD ,
      FRAUD_CARD , INTERNAL_SERVER_ERROR .
   4. It will list all saved transactions using the listSavedTransactions function.
   5. It will update the database with the new balance
   6. Assuming that the connection between the terminal and server is always connected, then it will return
      SERVER_OK .*/
EN_serverError_t saveTransaction(ST_transaction_t* transData)
{
	if (transactionsDB == NULL || transData == NULL)
		return SAVING_FAILED;
	//stroring transaction information in the data base
	//[transaction_counter - 1] to start storing in the element of [0] in the array of structure  transactionsDB
	for (uint8_t iter = 0; iter < 25; iter++)
		transactionsDB[transaction_counter - 1].cardHolderData.cardHolderName[iter] = transData->cardHolderData.cardHolderName[iter];
	for (uint8_t iter = 0; iter < 20; iter++)
		transactionsDB[transaction_counter - 1].cardHolderData.primaryAccountNumber[iter] = transData->cardHolderData.primaryAccountNumber[iter];
	for (uint8_t iter = 0; iter < 6; iter++)
		transactionsDB[transaction_counter - 1].cardHolderData.cardExpirationDate[iter] = transData->cardHolderData.cardExpirationDate[iter];
	for (uint8_t iter = 0; iter < 11; iter++)
		transactionsDB[transaction_counter - 1].terminalData.transactionDate[iter] = transData->terminalData.transactionDate[iter];

	transactionsDB[transaction_counter - 1].transState = transData->transState;  //transState
	if (server_state == SERVER_OK && transaction_state == APPROVED)
		transactionsDB[transaction_counter - 1].balance_after_trans = accountsDB[accountNumber].balance - transData->terminalData.transAmount; // balance_after_trans	
	else
		transactionsDB[transaction_counter - 1].balance_after_trans = 0;

	transactionsDB[transaction_counter - 1].terminalData.maxTransAmount = transData->terminalData.maxTransAmount; //maxTransAmount
	transactionsDB[transaction_counter - 1].transactionSequenceNumber = transaction_counter;  //transactionSequencNumber
	transactionsDB[transaction_counter - 1].terminalData.transAmount = transData->terminalData.transAmount;  //transAmount

	// if every thing about the server is ok then update the balance
	if (server_state == SERVER_OK && transaction_state == APPROVED)
		accountsDB[accountNumber].balance -= transData->terminalData.transAmount;
    // to list all transactions stored in the data base in the console 
    listSavedTransactions();
	// increment number of transaction in case if user want to do another one
	transaction_counter+=1;
	// always return 
	return SERVER_OK;
}
//============================================================
/* 1. This function prints all transactions found in the transactions DB. */
void listSavedTransactions(void)
{
	for (uint32_t j = 0; j < transaction_counter; j++)
	{
		printf("\n#########################\n");
		printf("Transaction Sequence Number: %d\n", transactionsDB[j].transactionSequenceNumber);
		printf("Transaction Date: %s\n", transactionsDB[j].terminalData.transactionDate);
		printf("Transaction Amount: %f\n", transactionsDB[j].terminalData.transAmount);
		printf("Transaction State: ");
		switch (transactionsDB[j].transState)
		{
		case DECLINED_EXPIRED_CARD:
		{
			printf("TRANSACTION DECLINED >>> EXPIRED_CARD\n");
			break;
		}
		case DECLINED_EXEEDED_MAX_AMOUNT:
		{
			printf("TRANSACTION DECLINED >>> EXEEDED_MAX_AMOUNT\n");
			break;
		}
		case FRAUD_CARD:
		{
			printf("TRANSACTION DECLINED >>> FRAUD CARD\n");
			break;
		}
		case DECLINED_INSUFFECIENT_FUND:
		{
			printf("TRANSACTION DECLINED >>> INSUFFECIENT FUND\n");
			break;
		}
		case DECLINED_STOLEN_CARD:
		{
			printf("TRANSACTION DECLINED >>> STOLEN_CARD\n");
			break;
		}
		case APPROVED:
		{
			printf("TRANSACTION APPROVED\n");
			break;
		}
		default:break;
		}
		printf("Terminal Max Amount : % f\n", transactionsDB[j].terminalData.maxTransAmount);
		printf("Card holder name : %s\n", transactionsDB[j].cardHolderData.cardHolderName);
		printf("PAN : %s\n", transactionsDB[j].cardHolderData.primaryAccountNumber);
		printf("Card Expiration Date: %s\n", transactionsDB[j].cardHolderData.cardExpirationDate);
	    printf("Balance After Transaction :%f\n", transactionsDB[j].balance_after_trans);
	   

		printf("#########################\n\n");

	}	
	
}