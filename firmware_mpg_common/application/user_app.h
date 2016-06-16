/**********************************************************************************************************************
File: user_app.h                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app as a template:
1. Follow the instructions at the top of user_app.c
2. Use ctrl-h to find and replace all instances of "user_app" with "yournewtaskname"
3. Use ctrl-h to find and replace all instances of "UserApp" with "YourNewTaskName"
4. Use ctrl-h to find and replace all instances of "USER_APP" with "YOUR_NEW_TASK_NAME"
5. Add #include yournewtaskname.h" to configuration.h
6. Add/update any special configurations required in configuration.h (e.g. peripheral assignment and setup values)
7. Delete this text (between the dashed lines)
----------------------------------------------------------------------------------------------------------------------

Description:
Header file for yournewtaskname.c

**********************************************************************************************************************/

#ifndef __USER_APP_H
#define __USER_APP_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/


/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/
/* Required constants for ANT channel configuration */
#define ANT_CHANNEL_USERAPP             (u8)0                 // Channel 0 - 7
#define ANT_SERIAL_LO_USERAPP           (u8)0x26             // Low byte of two-byte Device #
#define ANT_SERIAL_HI_USERAPP           (u8)0x13             // High byte of two-byte Device #
#define ANT_DEVICE_TYPE_USERAPP         (u8)66                // 1 - 255
#define ANT_TRANSMISSION_TYPE_USERAPP   (u8)2                 // 1-127 (MSB is pairing bit)
#define ANT_CHANNEL_PERIOD_LO_USERAPP   (u8)0x00              // Low byte of two-byte channel period 0x0001 - 0x7fff
#define ANT_CHANNEL_PERIOD_HI_USERAPP   (u8)0x08              // High byte of two-byte channel period 
#define ANT_FREQUENCY_USERAPP           (u8)66                // 2400MHz + this number 0 - 99
#define ANT_TX_POWER_USERAPP            RADIO_TX_POWER_0DBM   // RADIO_TX_POWER_0DBM, RADIO_TX_POWER_MINUS5DBM, RADIO_TX_POWER_MINUS10DBM, RADIO_TX_POWER_MINUS20DBM

#define TIMEOUT_VALUE 3000
#define MAX_RESEND_TIMES 3
/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/
void UserAppInitialize(void);
void UserAppRunActiveState(void);


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
             
static void UserAppSM_Error(void);         
static void UserAppSM_FailedInit(void);        
static void UserAppSM_Close(void);
static void UserAppSM_WaitChannelOpen(void);
static void UserAppSM_SendMessage();
static void UserAppSM_SearchingNewDev();
static void UserAppSM_WaitForRspond();
static void UserAppSM_Idle(void);
static void UserAppSM_WaitChannelClose(void);
#endif /* __USER_APP_H */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
