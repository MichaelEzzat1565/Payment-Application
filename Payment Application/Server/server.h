#ifndef SERVER_H
#define SERVER_H
#include "../Def.h"
#include "../Terminal/terminal.h"

typedef enum EN_transState_t
{
	APPROVED, DECLINED_INSUFFECIENT_FUND, DECLINED_STOLEN_CARD, FRAUD_CARD, INTERNAL_SERVER_ERROR , DECLINED_EXPIRED_CARD
	, DECLINED_EXEEDED_MAX_AMOUNT


}EN_transStat_t;

typedef struct ST_transaction_t
{
	ST_cardData_t cardHolderData;   // a structure variable of three members : cardHolderName, PrimaryAccountNumber, CerdExpirationDate
	ST_terminalData_t terminalData;  // a structure variable of three members : transAmount , transMaxAmount, transDate
	EN_transStat_t transState;        // enum variable of , it's members : APPROVED, DECLINED_INSUFFECIENT_FUND, DECLINED_STOLEN_CARD, FRAUD_CARD, INTERNAL_SERVER_ERROR
	uint32_t transactionSequenceNumber;  // number of the transaction stored in the data base
	float balance_after_trans;           // only for printing the balance after transaction on listSavedTransactions function
}ST_transaction_t;

typedef enum EN_serverError_t
{
	SERVER_OK, SAVING_FAILED, TRANSACTION_NOT_FOUND, ACCOUNT_NOT_FOUND, LOW_BALANCE, BLOCKED_ACCOUNT ,SERVER_ERROR
}EN_serverError_t;

typedef enum EN_accountState_t
{
	RUNNING,
	BLOCKED
}EN_accountState_t;
//used to creat the accounts data base and store the account data to do some checks on it
typedef struct ST_accountsDB_t
{
	float balance;
	EN_accountState_t state;
	uint8_t primaryAccountNumber[20];
}ST_accountsDB_t;

EN_transStat_t recieveTransactionData(ST_transaction_t* transData);
EN_serverError_t isValidAccount(const ST_cardData_t* cardData, ST_accountsDB_t* accountRefrence);
EN_serverError_t isBlockedAccount(ST_accountsDB_t* accountRefrence);
EN_serverError_t isAmountAvailable(ST_terminalData_t* termData, ST_accountsDB_t* accountRefrence);
EN_serverError_t saveTransaction(ST_transaction_t* transData);
void listSavedTransactions(uint32);



#endif