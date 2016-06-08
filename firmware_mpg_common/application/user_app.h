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
#define ANT_CHANNEL_USERAPP             (u8)0                 
#define ANT_SERIAL_LO_USERAPP           (u8)0xCF                 
#define ANT_SERIAL_HI_USERAPP           (u8)0x17                 
#define ANT_DEVICE_TYPE_USERAPP         (u8)1                 
#define ANT_TRANSMISSION_TYPE_USERAPP   (u8)1                 
#define ANT_CHANNEL_PERIOD_LO_USERAPP   (u8)0x00              
#define ANT_CHANNEL_PERIOD_HI_USERAPP   (u8)0x20              
#define ANT_FREQUENCY_USERAPP           (u8)50                
#define ANT_TX_POWER_USERAPP            RADIO_TX_POWER_0DBM


#define TIMEOUT_VALUE                   (u32)3000   //Time in ms for state time out
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
static void UserAppSM_Idle(void);    
static void UserAppSM_WaitChannelOpen(void);
static void UserAppSM_Error(void);         
static void UserAppSM_FailedInit(void);        
static void UserAppSM_WaitChannelClose(void);
static void UserAppSM_ChannelOpen(void);
#endif /* __USER_APP_H */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
