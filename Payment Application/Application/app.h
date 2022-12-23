#ifndef APP_H
#define APP_H

typedef enum EN_AppStat_t
{
	APP_OK, APP_ERROR
}EN_AppStat_t;

void appStart(void);
void welcomText(void);
void endTransText(void);
EN_AppStat_t checkName(void);
EN_AppStat_t checkExpDate(void);
EN_AppStat_t checkPAN(void);
EN_AppStat_t checkTransAmount(void);
void expiredCardState(void);
void exceededMaxAmountState(void);
void storeTransactionData(void);
void fraudCardState(void);
void insuffecientFaundState(void);
void stolenCardState(void);
// test functions prototypes
void testgetCardHolderName(void);
void testgetCardExpiryDate(void);
void testgetgetCardPAN(void);
void testgetTransactionDate(void);
void testisCardExpired(void);
void testisgetTransactionAmount(void);
void testsetMaxAmount(void);
void testisBelowMaxAmount(void);
void testisValidAccount(void);
void testisBlockedAccount(void);
void testisAmountAvailable(void);
/* recieveTransactionData , saveTransaction , listSavedTransactions : are all conected 
to each other so we can chek them with app test cases */

#endif