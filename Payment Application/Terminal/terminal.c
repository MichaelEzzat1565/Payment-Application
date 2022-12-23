#include "terminal.h"
#include<stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#pragma warning(disable : 4996)    // to stop the warning about localtime function

//============================================================
/* 1. This function will ask for the transaction date and store it in terminal data. 
   2. Transaction date is 10 characters string in the format DD/MM/YYYY, e.g 25/06/2022.
   3. If the transaction date is NULL or is less than 10 characters or wrong format will
   return the WRONG_DATE error, else return TERMINAL_OK .*/
EN_terminalError_t getTransactionDate(ST_terminalData_t* termData)
{
    if (termData == NULL) //Making sure the pointer is not NULL
        return WRONG_DATE;
    time_t t;
    t = time(NULL);
    struct tm tm = *localtime(&t); //  localtime() returns the local time of system
    // creat new variable to store the year in the format of YYYY
    uint32_t year = tm.tm_year + 1900;  // The tm_year field is relative to 1900 on all POSIX-compliant platforms.
    // creat new variable to store the month in the format of MM
    uint16_t month = tm.tm_mon + 1;     // The tm_mon displayed starting from 1 rather with 0
    // creat new variable to store the month in the format of DD
    uint16_t day = tm.tm_mday;
    // storing the date from the system in transactionDate array of chars in the form of DD/MM/YYYY
    termData->transactionDate[0] = day / 10 + '0'; // (day / 10 ) to store the first number , (+ '0') to convert the int to char
    termData->transactionDate[1] = day % 10 + '0'; // (day % 10 ) to store the secend number
    termData->transactionDate[2] = '/';            // to store a (/) not a (-)
    termData->transactionDate[3] = month / 10 + '0';  // (month / 10 ) to store the first number
    termData->transactionDate[4] = month % 10 + '0';  // (monthe % 10) to store the second nunber
    termData->transactionDate[5] = '/';
    termData->transactionDate[6] = '0' + year / 1000;
    termData->transactionDate[7] = '0' + (year % 1000) / 100;
    termData->transactionDate[8] = '0' + (year % 100) / 10;
    termData->transactionDate[9] = '0' + year % 10;
    termData->transactionDate[10] = '\0'; // the last number is null character
    printf("Transction date is : %s\n", termData->transactionDate);
    return TERMINAL_OK;
}
//============================================================
/* 1. This function compares the card expiry date with the transaction date.
  2. If the card expiration date is before the transaction date will return 
   EXPIRED_CARD , else return TERMINAL_OK .
*/
EN_terminalError_t isCardExpired(ST_cardData_t* cardData, ST_terminalData_t* termData)
{
    /*  creat new variable to get cardExpirationDate and transactionDate in the form MM/YYYY and 
    convet it from chars to int to compare it with transaction date */
    if ( termData == NULL || cardData == NULL)
        return EXPIRED_CARD;
    uint16_t card_exp_date_month = (cardData->cardExpirationDate[0] - '0') * 10 + cardData->cardExpirationDate[1] - '0';  //(+ '0') to convert char to int
    uint16_t card_exp_date_year = (cardData->cardExpirationDate[3] - '0') * 10 + cardData->cardExpirationDate[4] - '0' + 2000;
    uint16_t trans_date_month = (termData->transactionDate[3] - '0') * 10 + termData->transactionDate[4] - '0';
    uint16_t trans_date_year = (termData->transactionDate[6] - '0') * 1000 + (termData->transactionDate[7] - '0') * 100 + (termData->transactionDate[8] - '0') * 10 + +termData->transactionDate[9] - '0';
    if (trans_date_year > card_exp_date_year)
    {
        return EXPIRED_CARD;
    }
    if (trans_date_year == card_exp_date_year && trans_date_month > card_exp_date_month)
    {
        return EXPIRED_CARD;
    }

        return TERMINAL_OK;
}
//============================================================
/* 1. This function asks for the transaction amount and saves it into terminal data.
   2. If the transaction amount is less than or equal to 0 will return INVALID_AMOUNT 
   , else return TERMINAL_OK .*/
EN_terminalError_t getTransactionAmount(ST_terminalData_t* termData)
{
    if (termData != NULL)
    {
        float TransAmount;
        printf("Please enter transaction amount: ");
        fseek(stdin, 0, SEEK_END);
        scanf("%f", &TransAmount);
        if (TransAmount <= 0)
        {
            return INVALID_AMOUNT;
        }
        termData->transAmount = TransAmount;
        return TERMINAL_OK;
    }
    else
    {
        return INVALID_AMOUNT;
    }

}
//============================================================
/* 1. This function takes the maximum allowed amount and stores it into terminal data.
   2. Transaction max amount is a float number.
   3. If transaction max amount less than or equal to 0 will return the 
      INVALID_MAX_AMOUNT error, else return TERMINAL_OK .  */
EN_terminalError_t setMaxAmount(ST_terminalData_t* termData)
{
    if (termData == NULL) //Making sure the pointer is not NULL
        return WRONG_DATE;
    float max_amount = 5000;  //maximum transaction can user get per one transaction
    if (max_amount <= 0) {
        return INVALID_MAX_AMOUNT;
    }
    termData->maxTransAmount = max_amount;
    return TERMINAL_OK;
}
//============================================================
/* 1. This function compares the transaction amount with the terminal max allowed amount.
   2. If the transaction amount is larger than the terminal max allowed amount will return
      EXCEED_MAX_AMOUNT , else return TERMINAL_OK .*/
EN_terminalError_t isBelowMaxAmount(ST_terminalData_t* termData)
{
    if (termData == NULL)
        return EXCEED_MAX_AMOUNT;
    if (termData->transAmount > termData->maxTransAmount)
    {
        return EXCEED_MAX_AMOUNT;
    }

    return TERMINAL_OK;
}

EN_terminalError_t isValidCardPAN(ST_cardData_t* cardData)
{
    uint16_t PAN[20] = { 0 };
    uint8_t p = strlen(cardData->primaryAccountNumber);
    uint32_t sum = 0;
    for (uint8_t i = 0; i < p; i++)
    {
        PAN[i] = cardData->primaryAccountNumber[i] - '0';
    }
    for (int16_t i = (p - 2); i >= 0; i--)
    {
        PAN[i] = PAN[i] * 2;
        i--;
    }

    for (uint16_t i = 0; i < 20; i++)
    {
        if (PAN[i] > 9)
        {
            PAN[i] = PAN[i] - 9;
        }
    }
    for (uint16_t i = 0; i < 20; i++)
    {
        sum = sum + PAN[i];
    }
    if ((sum % 10) != 0)
        return INVALID_CARD;

    return TERMINAL_OK;

}


