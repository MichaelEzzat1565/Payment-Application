#ifndef CARD_H
#define CARD_H
#include "../Def.h"

typedef struct ST_cardData_t
{
	uint8_t cardHolderName[25];          // to store card holder name from the user
	uint8_t primaryAccountNumber[20];    // to store card primary account number from the user
	uint8_t cardExpirationDate[6];       // to store card expriation date from the user
}ST_cardData_t;


typedef enum EN_cardError_t
{
	CARD_OK, WRONG_NAME, WRONG_EXP_DATE, WRONG_PAN
}EN_cardError_t; 

EN_cardError_t getCardHolderName(ST_cardData_t* cardData);
EN_cardError_t getCardExpiryDate(ST_cardData_t* cardData);
EN_cardError_t getCardPAN(ST_cardData_t* cardData);
#endif