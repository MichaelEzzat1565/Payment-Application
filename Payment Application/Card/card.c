#include "card.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//============================================================
/* 1.This function will ask for the cardholder's nameand store it into card data.
   2.Cardholder name is 24 alphabetic charactersstring max and 20 min.
   3.If the cardholder name is NULL , less than 20characters or more than 24
   will return a WRONG_NAME error, else returnCARD_OK. */
EN_cardError_t getCardHolderName(ST_cardData_t* cardData)
{
    if (cardData == NULL)       //Making sure the pointer is not NULL
        return WRONG_PAN;
    fseek(stdin, 0, SEEK_END);    //Ignoring any past input
    char str[40];     // creating new array to check the name
    printf("Please enter card's holder name [20-25] alphabetic : ");
    fgets(str, 40, stdin);     //scan the name and store it in str array
    // check if the string is not null or start with space or '\n'
    if ((str[0] == '\n') || (str[0] == '\0') || (str[0] == ' '))
        return WRONG_NAME;
    // check that all chars are alphabetic characters and spaces
    for (uint16_t i = 0; i < strlen(str) - 1; i++)
    {
        if ((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z') || (str[i] == ' '))
            continue;
        else
            return WRONG_NAME;
    }
    //check the the name is between 25-20, [strlen(str)-1] to egnore \n
    if ((strlen(str)-1 > 24) || (strlen(str)-1 < 20))
        return WRONG_NAME;
    // give all chars in cardHolderName array the NULL character
    int16_t i = 0;
    for (i = 0; i < 24; i++)
    {
        cardData->cardHolderName[i] = '\0';
    }
    //store the name to our original structure array
    i = 0;
    while (str[i] != '\n')
    {
        cardData->cardHolderName[i] = str[i];
        i++;
    }

    return CARD_OK;
}
//============================================================
/* 1. This function will ask for the card expiry date and store it in card data.
   2. Card expiry date is 5 characters string in the format "MM/YY", e.g "05/25".
   3. If the card expiry date is NULL , less or more than 5 characters, or has the
   wrong format will return the WRONG_EXP_DATE error, else return CARD_OK .
*/
EN_cardError_t getCardExpiryDate(ST_cardData_t* cardData)
{
    if (cardData == NULL)    //Making sure the pointer is not NULL
        return WRONG_PAN;
    fseek(stdin, 0, SEEK_END);    //Ignoring any past input
    char str[20];     // creating new array to check the expiration date
    printf("Please enter the card's EXP date [mm/yy]: ");
    fgets(str, 20, stdin);
    //storing array's element an new variables to check if the exp. date is ok
    uint16_t m1 = str[0] - '0';
    uint16_t m2 = str[1] - '0';
    uint16_t y1 = str[3] - '0';
    uint16_t y2 = str[4] - '0';
    uint16_t exp_month = (m1 * 10) + m2; // put it the form of MM
    uint16_t exp_year = (y1 * 10) + y2;  // put it in the form of YY
    // check if the string is not null or start with space or '\n'
    if ((str[0] == '\n') || (str[0] == '\0') || (str[0] == ' '))
    {
        return WRONG_EXP_DATE;
    }
    // checking that the number of characters is 5 + /n
    if (strlen(str) != 6)
    {
        return WRONG_EXP_DATE;
    }
    //chek if the date have the wrong format
    if (exp_month < 1 || exp_month > 12 || str[2] != '/' || (exp_year > 99 || exp_year < 0))
    {
        return WRONG_EXP_DATE;
    }
    //store the Exp. date in our original array
    strncpy_s(cardData->cardExpirationDate, 6, str, _TRUNCATE);
    return CARD_OK;

}
//============================================================
/*1. This function will ask for the card's Primary Account Number and store it in card data.
  2. PAN is 20 numeric characters string, 19 character max, and 16 character min. 
  3. If the PAN is NULL , less than 16 or more than 19 characters, will return the WRONG_PAN error,
    else return CARD_OK .*/
EN_cardError_t getCardPAN(ST_cardData_t* cardData)
{
    if (cardData == NULL) //Making sure the pointer is not NULL
        return WRONG_PAN;
    fseek(stdin, 0, SEEK_END); //Ignoring any past input
    char str[50]; // creating a new arrat to store the pan and check it
    printf("Please enter the card's Primary Account Number [16-19] : ");
    fgets(str, 50, stdin);
    // check if the string is not null or start with space or '\n'
    if ((str[0] == '\n') || (str[0] == '\0') || (str[0] == ' '))
        return WRONG_PAN;
     //check thet all chars are numeric characters string
    for (uint16_t i = 0; i < strlen(str) - 1; i++)
    {
        if (isdigit(str[i]) != 0)
            continue;
        else
            return WRONG_PAN;
    }
    //check that the PAN is from 16 to 19
    if ((strlen(str) - 1 > 19) || (strlen(str) - 1 < 16))
        return WRONG_PAN;
    int16_t i = 0;
    for (i = 0; i < 20; i++)
    {
        cardData->primaryAccountNumber[i] = '\0';
    }
    i = 0;
    while (str[i] != '\n')
    {
        cardData->primaryAccountNumber[i] = str[i];
        i++;
    }

    return CARD_OK;
}
