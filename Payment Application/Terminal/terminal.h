#ifndef TERMINAL_H
#define TERMINAL_H
#include "../Def.h"
#include "../Card/card.h"

typedef struct ST_terminalData_t
{
	float transAmount;               // to store transaction amount entered by the user
	float maxTransAmount;            // to store maximum transaction amount determined by the server
	uint8_t transactionDate[11];     // to the date of the transaction from the sys.
}ST_terminalData_t;

typedef enum EN_terminalError_t
{
	TERMINAL_OK, WRONG_DATE, EXPIRED_CARD, INVALID_CARD, INVALID_AMOUNT, EXCEED_MAX_AMOUNT, INVALID_MAX_AMOUNT
}EN_terminalError_t;

EN_terminalError_t getTransactionDate(ST_terminalData_t* termData);
EN_terminalError_t isCardExpired(ST_cardData_t* cardData, ST_terminalData_t* termData);
EN_terminalError_t isValidCardPAN(ST_cardData_t* cardData);
EN_terminalError_t getTransactionAmount(ST_terminalData_t* termData);
EN_terminalError_t isBelowMaxAmount(ST_terminalData_t* termData);
EN_terminalError_t setMaxAmount(ST_terminalData_t* termData);
#endif