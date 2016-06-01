/**********************************************************************************************************************
File: user_app.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app as a template:
 1. Copy both user_app.c and user_app.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserAppInitialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserAppRunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserAppFlags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp_StateMachine;            /* The state machine function pointer */
static u32 UserApp_u32Timeout;                      /* Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: UserAppInitialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserAppInitialize(void)
{
   /* All discrete LEDs to off */
  LedOff(WHITE);
  LedOff(PURPLE);
  LedOff(BLUE);
  LedOff(CYAN);
  LedOff(GREEN);
  LedOff(YELLOW);
  LedOff(ORANGE);
  LedOff(RED);
  
  /* Backlight to white */  
  LedOn(LCD_RED);
  LedOn(LCD_GREEN);
  LedOn(LCD_BLUE);
  
  
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp_StateMachine = UserAppSM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp_StateMachine = UserAppSM_FailedInit;
  }

} /* end UserAppInitialize() */


/*----------------------------------------------------------------------------------------------------------------------
Function UserAppRunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserAppRunActiveState(void)
{
  UserApp_StateMachine();

} /* end UserAppRunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void UserAppSM_Idle(void)
{
  static u16 u16BlinkCounter = 0;
  static u8 u8ColorIndex = 0;
  static bool u8Flag = 1;
  static LedNumberType aeCurrentLed[]  = {WHITE, PURPLE, BLUE, CYAN, GREEN, YELLOW, ORANGE, RED};
  u16BlinkCounter++;
  if(u16BlinkCounter == 333)
  {
    u16BlinkCounter = 0;
    /* Set direction */
    if(u8Flag)
    {
      u8ColorIndex++;
      if(u8ColorIndex == 7)
        u8Flag = 0;
    }
    else
    {
      u8ColorIndex--;
      if(u8ColorIndex == 0)
        u8Flag = 1;
    }
    /* Set leds according to state */

    switch(u8ColorIndex)
    {
    case 0:
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex],15);
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex+1],5);
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex+2],1);
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex+3],0);
           break;
     case 1:if(u8Flag)
          {
            LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex],20);
            LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex-1],5);
            LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex+1],0);
          }
         else
          {
            LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex],15);
            LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex+1],5);
            LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex+2],1);
            LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex+3],0);
          }
          break;
     case 2:
       if(u8Flag)
            {
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex],15);
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex-1],5);
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex-2],1);
            }
           else
            {
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex],15);
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex+1],5);
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex+2],1);
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex+3],0);
            }
           break;
     case 3:
     case 4:
     case 5:if(u8Flag)
            {
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex],15);
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex-1],5);
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex-2],1);
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex-3],0);
            }
           else
            {
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex],15);
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex+1],5);
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex+2],1);
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex+3],0);
            }
           break;
      case 6:if(u8Flag)
            {
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex],15);
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex-1],5);
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex-2],1);
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex-3],0);
            }
           else
            {
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex],20);
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex+1],5);
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex-1],0);
            }
           break;
     case 7:
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex],15);
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex-1],5);
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex-2],1);
              LedPWM((LedNumberType)aeCurrentLed[u8ColorIndex-3],0);
           break;
     default:if(u8Flag)
            {}
           else
            {}
           break;
    }
  }
} /* end UserAppSM_Idle() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserAppSM_Error(void)          
{
  
} /* end UserAppSM_Error() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserAppSM_FailedInit(void)          
{
    
} /* end UserAppSM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
