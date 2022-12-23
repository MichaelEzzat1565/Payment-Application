#include "app.h"
#include <stdio.h>
#include<string.h>
#include "../Card/card.h"
#include "../Terminal/terminal.h"
#include "../Server/server.h"
#define  Max_Transactions_numbure 255

ST_cardData_t cardData;         // creating variable of ST_cardData_t structer which has 3 members(cardHolderName, primaryAccountNumber, cardExpirationDate)
ST_terminalData_t termData;     // creating variable of terminal-data structure which has 3 members (trans. amount, max. trans. amount, trans. date)
EN_cardError_t card_state;        // // creating variable in EN_cardError_t enum, it's members (CARD_OK, WRONG_NAME, WRONG_EXP_DATE, WRONG_PAN) to get functions' return
EN_terminalError_t terminal_state;    // // creating variable in EN_terminalError_t enum, it's members (TERMINAL_OK, WRONG_DATE, EXPIRED_CARD, INVALID_CARD, INVALID_AMOUNT, EXCEED_MAX_AMOUNT, INVALID_MAX_AMOUNT) to get functions' return
ST_transaction_t transData;     //creating variable of trans-data structer which has 4 members(cardHolderData[name, pan, exp.date],terminalData[amount, date, maxamount], transState, transactionSequenceNumber)
EN_transStat_t transaction_state;  // creating variable in EN_transStat_t enum to get functions' return , it's members (APPROVED, DECLINED_INSUFFECIENT_FUND, DECLINED_STOLEN_CARD, FRAUD_CARD, INTERNAL_SERVER_ERROR)
EN_serverError_t server_state;    //creating variable in EN_transStat_t enum to get functions' return, it's members (SERVER_OK, SAVING_FAILED, TRANSACTION_NOT_FOUND, ACCOUNT_NOT_FOUND, LOW_BALANCE, BLOCKED_ACCOUNT)
EN_AppStat_t app_state;     //to make checks to the data entered by the user
void appStart(void)
{
    welcomText();
    for (uint16_t i = 0; i < Max_Transactions_numbure; i++)
    {
                                /*===========================Card======================*/
        //get the name of the card's owner
        app_state = checkName();
        if (app_state == APP_ERROR)
        {
            endTransText();
            break;
        }
        // to get the expiration date of the card
        app_state = checkExpDate();
        if (app_state == APP_ERROR)
        {
            endTransText();
            break;
        }
        // get card's primary account number
        app_state = checkPAN();
        if (app_state == APP_ERROR)
        {
            endTransText();
            break;
        }
                            /*===========================Terminal======================*/
        // get the transaction date from the system , storing and printing it
        getTransactionDate(&termData);
        // set the maximum amount of server to 5000
        setMaxAmount(&termData);
        /* comparing the transaction date with card expiration date to check if the card
           expired or not , if expired the transaction will declined */
        terminal_state = isCardExpired(&cardData, &termData);
        if (terminal_state == EXPIRED_CARD)
        {
            expiredCardState();
            printf("Do you want to retry ? (y/n) :");
            fseek(stdin, 0, SEEK_END);
            if (getchar() == 'y')
                continue;
            else
                break;
        }
        // to get transaction amount from the user
        app_state = checkTransAmount();
        if (app_state == APP_ERROR)
        {
            endTransText();
            break;
        }
        //comparing transaction amount with maximum trans. amount stored
        terminal_state = isBelowMaxAmount(&termData);
        if (terminal_state == EXCEED_MAX_AMOUNT)
        {
            exceededMaxAmountState();
            printf("Do you want to retry ? (y/n) :");
            fseek(stdin, 0, SEEK_END);
            if (getchar() == 'y')
                continue;
            else
                break;
        }
                                 /*===========================Server======================*/
        //stoting cardData and termData in trasData
        storeTransactionData();
        // calling recieveTransactionData fun. to all the transaction data
        transaction_state = recieveTransactionData(&transData);
        // if the account does not exsist in the data base
        if (transaction_state == FRAUD_CARD)
        {
            fraudCardState();
            printf("Do you want to retry ? (y/n) :");
            fseek(stdin, 0, SEEK_END);
            if (getchar() == 'y')
                continue;
            else
                break;
        }
        // if the amount is more than the balance
        else if (transaction_state == DECLINED_INSUFFECIENT_FUND)
        {
            insuffecientFaundState();
            printf("Do you want to retry ? (y/n) :");
            fseek(stdin, 0, SEEK_END);
            if (getchar() == 'y')
                continue;
            else
                break;
        }
        // if the account is blocked
        else if (transaction_state == DECLINED_STOLEN_CARD)
        {
            stolenCardState();
            printf("Do you want to retry ? (y/n) :");
            fseek(stdin, 0, SEEK_END);
            if (getchar() == 'y')
                continue;
            else
                break;
        }
        printf("Do you want to make another transaction ? (y/n) :");
        fseek(stdin, 0, SEEK_END);
        if (getchar() == 'y')
            continue;
        else
            break;
    }

    printf(" ====== Thanks =====");
    return;
}

void welcomText(void)
{
    printf("===Welcom To Our Server===\n");
    printf("= please enter your data to make  transaction .... \n");

}

void endTransText(void)
{
    printf("====You Entered Wrong Data More Than 5 Times ====\n");
    printf("============== Please Try Again Later =============\n");
}

EN_AppStat_t checkName(void)
{
    // for preventing the user from trying more than 5 times
    for (uint8_t i = 0; i < 5; i++)
    {
        //get the name of the card's owner
        card_state = getCardHolderName(&cardData); //calling getCardHolderName fun. to get the name
        if (card_state == WRONG_NAME)
        {
            printf("You have enterd a Wrong name\n");
        }
        else if (card_state == CARD_OK)
        {
            printf("Accepted Name \n");
            return APP_OK;
        }
    }
    return APP_ERROR;
}

EN_AppStat_t checkExpDate(void)
{
    for (uint8_t i = 0; i < 5; i++)
    {
        //get the name of the card's owner
        card_state = getCardExpiryDate(&cardData); //calling getCardHolderName fun. to get the name
        if (card_state == WRONG_EXP_DATE)
        {
            printf("You have enterd a Wrong Expiration Date\n");
        }
        else if (card_state == CARD_OK)
        {
            printf("Accepted Expitation Date \n");
            return APP_OK;
        }
    }
    return APP_ERROR;
}

EN_AppStat_t checkPAN(void)
{
    for (uint8_t i = 0; i < 5; i++)
    {
        //get the name of the card's owner
        card_state = getCardPAN(&cardData); //calling getCardHolderName fun. to get the name
        terminal_state = isValidCardPAN(&cardData); // luhn number check
        if (card_state == WRONG_PAN || terminal_state == INVALID_CARD)
        {
            printf("You have enterd a Wrong Primary Account Number\n");
        }
        else if (card_state == CARD_OK && terminal_state == TERMINAL_OK)
        {
            printf("Accepted Primary Account Number\n");
            return APP_OK;
        }
    }
    return APP_ERROR;
}

EN_AppStat_t checkTransAmount(void)
{
    for (uint8_t i = 0; i < 5; i++)
    {
        //get the name of the card's owner
        terminal_state = getTransactionAmount(&termData); //calling getCardHolderName fun. to get the name
        if (terminal_state == INVALID_AMOUNT)
        {
            printf("You have enterd a Wrong Amount\n");
        }
        else if (terminal_state == TERMINAL_OK)
        {
            printf("Accepted Amount \n");
            return APP_OK;
        }
    }
    return APP_ERROR;
}
 
void expiredCardState(void)
{
    printf("===================================\n");
    printf("      Your card is Expired\n");
    printf("===================================\n");
    transData.cardHolderData = cardData;
    transData.terminalData = termData;
    transData.transState = DECLINED_EXPIRED_CARD;
    server_state = SERVER_ERROR;
    transaction_state = DECLINED_EXPIRED_CARD; // to prevent saveTransaction function from updating the balance and balance after transaction
    saveTransaction(&transData);
}

void exceededMaxAmountState(void)
{
    printf("===================================\n");
    printf("Exceeded Server's Maximum Amount\n");
    printf("===Transaction DECLINED====\n");
    printf("===================================\n");
    transData.cardHolderData = cardData;
    transData.terminalData = termData;
    transData.transState = DECLINED_EXEEDED_MAX_AMOUNT;
    server_state = SERVER_ERROR;  // to prevent saveTransaction function from updating the balance and balance after transaction
    transaction_state = DECLINED_EXEEDED_MAX_AMOUNT;  // to prevent saveTransaction function from updating the balance and balance after transaction
    saveTransaction(&transData);
}

void storeTransactionData(void)
{
    transData.cardHolderData = cardData;
    transData.terminalData = termData;
    transData.transactionSequenceNumber = 0;
    transData.transState = 0;
}

void fraudCardState(void)
{
    printf("===================================\n");
    printf("This Account Doesn't Exist\n");
    printf("===Transaction DECLINED====\n");
    printf("===================================\n");
    transData.transState = FRAUD_CARD;
    saveTransaction(&transData);  
}

void insuffecientFaundState(void)
{
    printf("===================================\n");
    printf("Insuffecient Funds\n");
    printf("===Transaction DECLINED====\n");
    printf("===================================\n");
    transData.transState = DECLINED_INSUFFECIENT_FUND;
    saveTransaction(&transData);
}

void stolenCardState(void)
{
    printf("===================================\n");
    printf("This account is blocked\n");
    printf("===Transaction DECLINED====\n");
    printf("===================================\n");
    transData.transState = DECLINED_STOLEN_CARD;
    saveTransaction(&transData);
}

                                                      /* ============ test functions ============= */

void testgetCardHolderName(void)
{
    uint8_t* test_cases[5] = { "michael ezzat tanyous", "michael ezzat","michael 3zzat tanyous","878733322","NULL" };
    uint8_t* expected_result[5] = { "CARD_OK", "WRONG_NAME","WRONG_NAME","WRONG_NAME","WRONG_NAME" };
    printf("Tester name: admin\n");
    printf("Function Name: getCardExpiryDate\n");
    for (uint16_t i = 0; i < 5; i++)
    {
        printf("Test case %d:\n", i + 1);
        printf("Input data:%s\n", test_cases[i]);
        printf("Expected result:%s\n", expected_result[i]);
        card_state = getCardHolderName(&cardData);
        printf("Actual result: ");
        if (card_state == WRONG_NAME)
            printf("WRONG_NAME\n");
        else if (card_state == CARD_OK)
            printf("CARD_OK\n");
        printf("====================\n");

    }

}

void testgetCardExpiryDate(void)
{
    uint8_t* test_cases[5] = { "12/25", "12 25","12*12","12525","12/255" };
    uint8_t* expected_result[5] = { "CARD_OK", "WRONG_EXP_DATE","WRONG_EXP_DATE","WRONG_EXP_DATE","WRONG_EXP_DATE" };
    printf("Tester name: admin\n");
    printf("Function Name: testgetCardExpiryDate\n");
    for (uint16_t i = 0; i < 5; i++)
    {
        printf("Test case %d:\n", i + 1);
        printf("Input data:%s\n", test_cases[i]);
        printf("Expected result:%s\n", expected_result[i]);
        card_state = getCardExpiryDate(&cardData);
        printf("Actual result: ");
        if (card_state == WRONG_EXP_DATE)
            printf("WRONG_EXP_DATE\n");
        else if (card_state == CARD_OK)
            printf("CARD_OK\n");
        printf("====================\n");

    }

}

void testgetgetCardPAN(void)
{
    uint8_t* test_cases[5] = { "1471490470778326", "147149047077832","147149047077832m","147149047077832658789","NULL" };
    uint8_t* expected_result[5] = { "CARD_OK", "WRONG_PAN","WRONG_PAN","WRONG_PAN","WRONG_PAN" };
    printf("Tester name: admin\n");
    printf("Function Name: testgetgetCardPAN\n");
    for (uint16_t i = 0; i < 5; i++)
    {
        printf("Test case %d:\n", i + 1);
        printf("Input data:%s\n", test_cases[i]);
        printf("Expected result:%s\n", expected_result[i]);
        card_state = getCardPAN(&cardData);
        printf("Actual result: ");
        if (card_state == WRONG_PAN)
            printf("WRONG_PAN\n");
        else if (card_state == CARD_OK)
            printf("CARD_OK\n");
        printf("====================\n");

    }

}

void testgetTransactionDate(void)
{
    uint8_t* test_cases[] = { "date of current day from sys. " };
    uint8_t* expected_result[] = { "date of current day from sys." };
    printf("Tester name: admin\n");
    printf("Function Name: getTransactionDate\n");
    printf("Test case: 1 of 1\n");
    printf("Input data:%s\n", test_cases[0]);
    printf("Expected result:%s\n", expected_result[0]);
    terminal_state = getTransactionDate(&termData);
    printf("Actual result: %s", termData.transactionDate);

}

void testisCardExpired(void)
{
    uint8_t* test_cases[5] = { "12/25", "10/22","01/21","11/01","01/02" };
    uint8_t* expected_result[5] = { "TERMINAL_OK", "EXPIRED_CARD","EXPIRED_CARD","EXPIRED_CARD","EXPIRED_CARD" };
    printf("Tester name: admin\n");
    printf("Function Name: isCardExpired\n");
    for (uint16_t i = 0; i < 5; i++)
    {
        strncpy_s(cardData.cardExpirationDate, 6, test_cases[i], _TRUNCATE);
        strncpy_s(termData.transactionDate, 11, "20/11/2022", _TRUNCATE);

        printf("Test case %d:\n", i + 1);
        printf("Input data:%s\n", test_cases[i]);
        printf("Expected result:%s\n", expected_result[i]);
        terminal_state = isCardExpired(&cardData, &termData);
        printf("Actual result: ");
        if (terminal_state == EXPIRED_CARD)
            printf("EXPIRED_CARD\n");
        else if (terminal_state == TERMINAL_OK)
            printf("TERMINAL_OK\n");
        printf("====================\n");

    }
}

void testisgetTransactionAmount()
{
    uint8_t* test_cases[5] = { "1000", "-1000","-1","mm","-125.2658" };
    uint8_t* expected_result[5] = { "TERMINAL_OK", "INVALID_AMOUNT","INVALID_AMOUNT","INVALID_AMOUNT","INVALID_AMOUNT" };
    printf("Tester name: admin\n");
    printf("Function Name: getTransactionAmount\n");
    for (uint16_t i = 0; i < 5; i++)
    {
        printf("Test case %d:\n", i + 1);
        printf("Input data:%s\n", test_cases[i]);
        printf("Expected result:%s\n", expected_result[i]);
        terminal_state = getTransactionAmount(&termData);
        printf("Actual result: ");
        if (terminal_state == INVALID_AMOUNT)
            printf("INVALID_AMOUNT\n");
        else if (terminal_state == TERMINAL_OK)
            printf("TERMINAL_OK\n");
        printf("====================\n");

    }
}


void testsetMaxAmount(void)
{
    float test_case = 5000;
    uint8_t* expected_result[] = { "5000 Constant maximum amount by the server" };
    printf("Tester name: admin\n");
    printf("Function Name: setMaxAmount\n");
    printf("Test case: 1 of 1\n");
    printf("Input data: %f\n", test_case);
    printf("Expected result:%s\n", expected_result[0]);
    terminal_state = setMaxAmount(&termData);
    printf("Actual result: %f", termData.maxTransAmount);

}

void testisBelowMaxAmount(void)
{
    float test_cases[3] = { 4500, 5001 , 5500 };
    uint8_t* expected_result[3] = { "TERMINAL_OK", "EXCEED_MAX_AMOUNT","EXCEED_MAX_AMOUNT" };
    printf("Tester name: admin\n");
    printf("Function Name: isBelowMaxAmount\n");
    for (uint16_t i = 0; i < 3; i++)
    {
        termData.maxTransAmount = 5000;
        termData.transAmount = test_cases[i];

        printf("Test case %d:\n", i + 1);
        printf("Input data:%f\n", test_cases[i]);
        printf("Expected result:%s\n", expected_result[i]);
        terminal_state = isBelowMaxAmount(&termData);
        printf("Actual result: ");
        if (terminal_state == EXCEED_MAX_AMOUNT)
            printf("EXCEED_MAX_AMOUNT\n");
        else if (terminal_state == TERMINAL_OK)
            printf("TERMINAL_OK\n");
        printf("====================\n");

    }

}

void testisValidAccount(void)
{
    uint8_t* test_cases[2] = { "1471490470778326", "1258798546598745" };
    uint8_t* expected_result[2] = { "SERVER_OK", "ACCOUNT_NOT_FOUND" };
    printf("Tester name: admin\n");
    printf("Function Name: isValidAccount\n");
    for (uint16_t i = 0; i < 2; i++)
    {
        ST_accountsDB_t accountRefrence;
        strncpy_s(transData.cardHolderData.primaryAccountNumber, 20, test_cases[i], _TRUNCATE);
        printf("Test case %d:\n", i + 1);
        printf("Input data:%s\n", test_cases[i]);
        printf("Expected result:%s\n", expected_result[i]);
        server_state = isValidAccount(&transData.cardHolderData, &accountRefrence);
        printf("Actual result: ");
        if (server_state == ACCOUNT_NOT_FOUND)
            printf("ACCOUNT_NOT_FOUND\n");
        else if (server_state == SERVER_OK)
            printf("SERVER_OK\n");
        printf("====================\n");
    }



}

void testisBlockedAccount(void)
{

    uint8_t* test_cases[2] = { "1471490470778326", "9699988114694485" };
    uint8_t* expected_result[2] = { "SERVER_OK", "BLOCKED_ACCOUNT" };

    printf("Tester name: admin\n");
    printf("Function Name: isBlockedAccount\n");
    ST_accountsDB_t accountRefrence;
    strncpy_s(accountRefrence.primaryAccountNumber, 20, test_cases[0], _TRUNCATE);
    accountRefrence.state = RUNNING;
    printf("Test case : 1 of 2\n");
    printf("Input data:%s\n", test_cases[0]);
    printf("Expected result:%s\n", expected_result[0]);
    server_state = isBlockedAccount(&accountRefrence);
    printf("Actual result: ");
    if (server_state == BLOCKED_ACCOUNT)
        printf("BLOCKED_ACCOUNT\n");
    else if (server_state == SERVER_OK)
        printf("SERVER_OK\n");
    printf("====================\n");

    strncpy_s(accountRefrence.primaryAccountNumber, 20, test_cases[1], _TRUNCATE);
    accountRefrence.state = BLOCKED;
    printf("Test case : 2 of 2\n");
    printf("Input data:%s\n", test_cases[1]);
    printf("Expected result:%s\n", expected_result[1]);
    server_state = isBlockedAccount(&accountRefrence);
    printf("Actual result: ");
    if (server_state == BLOCKED_ACCOUNT)
        printf("BLOCKED_ACCOUNT\n");
    else if (server_state == SERVER_OK)
        printf("SERVER_OK\n");
    printf("====================\n");

}


void testisAmountAvailable(void)
{
    float test_cases[2] = { 2000, 3000 };
    uint8_t* expected_result[2] = { "SERVER_OK", "LOW_BALANCE" };
    ST_accountsDB_t accountRefrence;

    accountRefrence.balance = 2500;
    termData.transAmount = 2000;
    printf("Tester name: admin\n");
    printf("Function Name: isAmountAvailable\n");
    printf("For account balance = 2500\n");
    printf("Test case : 1 of 2\n");
    printf("Input data:%f\n", test_cases[0]);
    printf("Expected result:%s\n", expected_result[0]);
    server_state = isAmountAvailable(&termData, &accountRefrence);
    printf("Actual result: ");
    if (server_state == LOW_BALANCE)
        printf("LOW_BALANCE\n");
    else if (server_state == SERVER_OK)
        printf("SERVER_OK\n");
    printf("====================\n");

    accountRefrence.balance = 2500;
    termData.transAmount = 3000;
    printf("Test case : 2 of 2\n");
    printf("For account balance = 2500\n");
    printf("Input data:%f\n", test_cases[1]);
    printf("Expected result:%s\n", expected_result[1]);
    server_state = isAmountAvailable(&termData, &accountRefrence);
    printf("Actual result: ");
    if (server_state == LOW_BALANCE)
        printf("LOW_BALANCE\n");
    else if (server_state == SERVER_OK)
        printf("SERVER_OK\n");
    printf("====================\n");

}
