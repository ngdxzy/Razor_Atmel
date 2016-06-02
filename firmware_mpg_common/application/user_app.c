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
  PWMAudioSetFrequency(BUZZER1, 0);
  PWMAudioOn(BUZZER1);
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
#define quite 0
#define do 440
#define re 493
#define mi 554
#define fa 587
#define so 659
#define la 739
#define si 830
#define do1 440*2
#define re1 493*2
#define mi1 554*2
#define fa1 587*2
#define so1 659*2
#define la1 739*2
#define si1 830*2
#define Meter 500
#define OneM Meter
#define HalfM Meter/2
#define QuarM Meter/4
#define DouM Meter*2
static void UserAppSM_Idle(void)
{
  static u16 u16MusicData[]={
    mi1,OneM, mi1,HalfM,  mi1,HalfM,  fa1,OneM, so1,OneM, mi1,DouM, re1,DouM,
    do1,OneM,do1,HalfM,do1,HalfM,re1,OneM,mi1,OneM,mi1,DouM,si,DouM,
    la1,OneM,mi1,OneM,re1,DouM,la1,OneM,mi1,OneM,re1,DouM,
    la1,OneM,mi1,OneM,re1,OneM+HalfM,do1,HalfM,do1,DouM+OneM+HalfM,
    quite,DouM+OneM,
    mi1,HalfM,re1,HalfM,so1,DouM+OneM,fa1,HalfM,mi1,HalfM,re1,HalfM+DouM,
    so1,HalfM,fa1,HalfM,mi1,OneM,fa1,HalfM,so1,HalfM+OneM,mi1,OneM,re1,DouM+OneM,
    quite,HalfM,do1,HalfM,la,OneM,mi1,OneM,re1,OneM+HalfM,
    do1,HalfM,so,OneM,re1,OneM,do1,DouM-QuarM,quite,QuarM,
    fa1,HalfM,mi1,HalfM,fa1,HalfM,mi1,HalfM,do1,OneM+HalfM,quite,QuarM,
    fa1,HalfM,mi1,HalfM,fa1,HalfM,mi1,HalfM,do1,OneM+HalfM,re1,DouM,do1,DouM+DouM,
    quite,DouM*3
    };
  static u16 u16SizeOfMusic = sizeof(u16MusicData)/2;
  static u16 u16Timecounter = 0;
  static u8 u8PresentSound = 0;
  static bool bFlag = 1;
  u16Timecounter++;
  /* Just set initial statement */
  if(bFlag)
  {
    bFlag = 0;
    u8PresentSound = 1;
    PWMAudioSetFrequency(BUZZER1, u16MusicData[u8PresentSound-1]);
  }
  /* quite down at the end of one sound so that the music can have more rhythm sensation*/
  if(u16Timecounter == u16MusicData[u8PresentSound]-50)
    PWMAudioSetFrequency(BUZZER1,quite);
  /* Control the last time of per sound*/
  if(u16Timecounter == u16MusicData[u8PresentSound])
  {
    u16Timecounter = 0;
    u8PresentSound++;
    /* When the music ends,restart */
    if(u8PresentSound == u16SizeOfMusic)
    {
      u8PresentSound = 0;
      PWMAudioSetFrequency(BUZZER1, u16MusicData[u8PresentSound]);
    }
    PWMAudioSetFrequency(BUZZER1, u16MusicData[u8PresentSound]);
    u8PresentSound++;
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
