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
extern u8 G_au8DebugScanfBuffer[];                     /* From debug.c */
extern u8 G_u8DebugScanfCharCount; 
/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp_StateMachine;            /* The state machine function pointer */
static u32 UserApp_u32Timeout;                      /* Timeout counter used across states */
static u8 UserApp_u8Name[] = "XuZhenyu";
static u8 u8SizeOfName = sizeof(UserApp_u8Name);
static u8 UserApp_u8OutputBuffer[DEBUG_SCANF_BUFFER_SIZE] = 0;
static u8 UserApp_u8OutputBufCount = 0;
static u8 UserApp_u8NameBuffer[sizeof(UserApp_u8Name)] = 0;
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
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR,"A3.");
  LCDMessage(LINE1_START_ADDR+4,UserApp_u8Name);
  
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
/* new created functions*/
/* Show the word on tera on the LCD,and the u8TempChar is used to confirm the input is not a backspace */
void LCDMessageSync(u8* u8ScanfCharCountBefore)
{
   static u8 u8TempVar = 0;
   static u8 u8TempStrng[DEBUG_SCANF_BUFFER_SIZE]= 0;
   /* Back to the begin when the LCD is full*/
   if(G_u8DebugScanfCharCount == 21)
   {
     G_au8DebugScanfBuffer[0] = G_au8DebugScanfBuffer[20];
     G_u8DebugScanfCharCount = 1;
     *u8ScanfCharCountBefore = 0;
   }
   G_au8DebugScanfBuffer[G_u8DebugScanfCharCount]= '\0';
   LCDClearChars(LINE2_START_ADDR,20);
   LCDMessage(LINE2_START_ADDR , G_au8DebugScanfBuffer);
}
/* To see whether the two character is same  */
bool Equal(u8 A,u8 B)
{
  return (A == (B - 32) )||(A == (B + 32)) ||A == B;
}
/* Check the inputed character with my name and save the character when they are equal */
/* Return 1 when a full name has been inputed */
bool CheckName()
{
    static u8 u8TimeCounter = 0;
    static u8 u8PositionOfName = 0;
    /* Call the function 'Equal'*/
    if(Equal(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1],UserApp_u8Name[u8PositionOfName]))
    {
      UserApp_u8NameBuffer[u8PositionOfName] = G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1];
      UserApp_u8NameBuffer[u8PositionOfName+1]='\0';
      u8PositionOfName++;
      /* return 1 if a full name has been inputed*/
      if(u8PositionOfName == (u8SizeOfName-1))
      {
        u8PositionOfName = 0;
        u8TimeCounter++;
        return 1;
      }
    }
    return 0;
}
/* Check the input from button and do the refered things */
u16 CheckButton(u16 u16InputCounter)
{
  /* Clear the input buffer */
    if( WasButtonPressed(BUTTON0) )
    {
    /* Be sure to acknowledge the button press */
      ButtonAcknowledge(BUTTON0);
      G_u8DebugScanfCharCount = 0;
      G_au8DebugScanfBuffer[0] = '\0';
    }
  /* Print the Number of input */
    if( WasButtonPressed(BUTTON1) )
    {
    /* Be sure to acknowledge the button press */
      ButtonAcknowledge(BUTTON1);
      DebugPrintf("\n\rCharacter count is:");
      DebugPrintNumber((u32)u16InputCounter & 0x0000FFFF);
      DebugPrintf("\n\r");
    }
    /* Clear the Number of input */
    if( WasButtonPressed(BUTTON2) )
    {
    /* Be sure to acknowledge the button press */
      ButtonAcknowledge(BUTTON2);
      u16InputCounter = 0;
      DebugPrintf("\n\rCharacter count has been cleared!\n\r");
    }
    /* Output the name Buffer */
    if( WasButtonPressed(BUTTON3) )
    {
    /* Be sure to acknowledge the button press */
      ButtonAcknowledge(BUTTON3);
      DebugPrintf("\n\r");
      DebugPrintf("Name Buffer:");
      DebugPrintf(UserApp_u8NameBuffer);
      DebugPrintf("\n\r");
    }
  return u16InputCounter;
}
/* When a full name has been inputed ,celebreat it buy doing something.
I just toggle a led as a example,and it can be more interesting too. */
void WelcomeShow(bool Flag)
{
  static u16 u16Counter = 0;
  
  if(Flag)
  {
    u16Counter++;
    if(u16Counter == 500)
    {
      u16Counter = 0;
      LedToggle(RED);
    }
  }
  else
  {
    LedOff(RED);
  }
}
/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
/*-------------------------------------------------------------------------------------------------------------------*/
static void UserAppSM_Idle(void)
{
    static u16 u16InputCounter = 0;
    static u8 u8ScanfCharCountBefore = 0;
    static u16 u16TimeCounter = 0;
    static bool bWelcomeFlag = 0;
    /* Compare the present number with the number before,so that the programe can 
      run only when a key pressed rather than keep checking it*/
    if(u8ScanfCharCountBefore != G_u8DebugScanfCharCount)
    {
      /* Call the function 'LCDMessageSync' */
      LCDMessageSync(&u8ScanfCharCountBefore);
      /* To confirm the key pressed is not a 'back space' */
      if(u8ScanfCharCountBefore < G_u8DebugScanfCharCount)
      {
        u16InputCounter++;
        /* Once a full name is inputed,Welcome! */
        /* The value of 'CheckName()' has been explained before,
        if it is confusing see the declaration of the function */
        if(CheckName())
        {
          if(bWelcomeFlag)
          {
            u16TimeCounter = 0;
          }
          else
          {
            bWelcomeFlag = 1;
          }
        }
      }
    }
    /* Update the number 'u8ScanfCharCountBefore' when everything is done  */
    u8ScanfCharCountBefore = G_u8DebugScanfCharCount;
    /* Call the function of CheckButton  */
    u16InputCounter = CheckButton(u16InputCounter);
    /* Control the time of welcoming */
    if(bWelcomeFlag)
    {
      u16TimeCounter++;
      if(u16TimeCounter == 5000)
      {
        u16TimeCounter = 0;
        bWelcomeFlag = 0;
      }
    }
    WelcomeShow(bWelcomeFlag);
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
