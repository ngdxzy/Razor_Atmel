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
#include "Command.h"
/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserAppFlags;                       /* Global state flags */


static u32 UserApp_u32DataMsgCount = 0;             /* Counts the number of ANT_DATA packets received */
static u32 UserApp_u32TickMsgCount = 0;             /* Counts the number of ANT_TICK packets received */

static fnCode_type UserApp_StateMachine;            /* The state machine function pointer */
static u32 UserApp_u32Timeout;      
/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern AntSetupDataType G_stAntSetupData;                         /* From ant.c */

extern u32 G_u32AntApiCurrentDataTimeStamp;                       /* From ant_api.c */
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;    /* From ant_api.c */
extern u8 G_au8AntApiCurrentData[ANT_APPLICATION_MESSAGE_BYTES];  /* From ant_api.c */

extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern volatile CMD sPresentCMD;
/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp_StateMachine;            /* The state machine function pointer */
static u32 UserApp_u32Timeout;                      /* Timeout counter used across states */

static u8 u8AntState = 0xff;

u8 u8PairedCount = 1;
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
  u8 au8WelcomeMessage[] = "ANT Master";

  /* Write a weclome message on the LCD */
  /* Set a message up on the LCD. Delay is required to let the clear command send. */
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, au8WelcomeMessage);
  LedOn(YELLOW);
 /* Configure ANT for this application */
  G_stAntSetupData.AntChannel          = ANT_CHANNEL_USERAPP;
  G_stAntSetupData.AntSerialLo         = ANT_SERIAL_LO_USERAPP;
  G_stAntSetupData.AntSerialHi         = ANT_SERIAL_HI_USERAPP;
  G_stAntSetupData.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  G_stAntSetupData.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  G_stAntSetupData.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  G_stAntSetupData.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
  G_stAntSetupData.AntFrequency        = ANT_FREQUENCY_USERAPP;
  G_stAntSetupData.AntTxPower          = ANT_TX_POWER_USERAPP;


  /* If good initialization, set state to Idle */
  if( AntChannelConfig(ANT_MASTER) )
  {
    UserApp_StateMachine = UserAppSM_Close;
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
static void UserApp_SendMessage()
{
  static u8 u8SendBuf[] = {0,0,0,0,0,0,0,0};
  static u8 au8DataContent[] = "xxxxxxxxxxxxxxxx";
  static bool bIsFirstMsg = 1;
  u8 u8MsgPlace = 0;
  static u8 u8CMDCp = 0;
  if( AntReadData() )
  {
     /* New data message: check what it is */
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      UserApp_u32DataMsgCount++;
      
      for(u8 i = 0; i < ANT_APPLICATION_MESSAGE_BYTES; i++)
      {
        au8DataContent[2 * i] = HexToASCIICharUpper(G_au8AntApiCurrentData[i] / 16);
        au8DataContent[2 * i + 1] = HexToASCIICharUpper(G_au8AntApiCurrentData[i] % 16); 
      }
      LCDClearChars(LINE2_START_ADDR,20);
      LCDMessage(LINE2_START_ADDR,au8DataContent);
      
    } /* end if(G_eAntApiCurrentMessageClass == ANT_DATA) */
    
    else if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {
      UserApp_u32TickMsgCount++;

      /* Look at the TICK contents to check the event code and respond only if it's different */
      if(u8AntState != G_au8AntApiCurrentData[ANT_TICK_MSG_EVENT_CODE_INDEX])
      {
        /* The state changed so update u8LastState and queue a debug message */
        u8AntState = G_au8AntApiCurrentData[ANT_TICK_MSG_EVENT_CODE_INDEX];
      } /* end if (u8LastState != G_au8AntApiCurrentData[ANT_TICK_MSG_EVENT_CODE_INDEX]) */
      
      if(sPresentCMD.bValued)
      {
        bool bGetEnd = 0;
        u8SendBuf[u8MsgPlace++] = sPresentCMD.u8DivAddr;
        if(bIsFirstMsg)
        {
          bIsFirstMsg = 0;
          u8SendBuf[u8MsgPlace++] = 0xFC;
          u8SendBuf[u8MsgPlace++] = sPresentCMD.u8CMDType;
        }
        else
        {
          u8SendBuf[u8MsgPlace++] = 0xCC;
        }
        while(u8MsgPlace < 8)
        {
          if(sPresentCMD.u8CMDDetail[u8CMDCp] != 0xFF )
          {
            u8SendBuf[u8MsgPlace++] = sPresentCMD.u8CMDDetail[u8CMDCp++];
          }
          else
          {
            bGetEnd = 1;
            u8CMDCp = 0;
            u8SendBuf[u8MsgPlace] = 0xCF;
            break;
          }
        }
        if(bGetEnd)
        {
          bIsFirstMsg = 1;
          sPresentCMD.bValued = 0;
        }
     
        AntQueueBroadcastMessage(u8SendBuf);
      }
      
    } /* end else if(G_eAntApiCurrentMessageClass == ANT_TICK) */
    
  } /* end AntReadData() */
}
static void UserApp_SearchingNewDiv()
{
  static u8 u8TollMessage[] = {0,0,0,0,0,0,0,1};
  static bool bGetNew = 0;
  static bool bClk = 0;
  static u8 au8DataContent[] = "xxxxxxxxxxxxxxxx";
  if( AntReadData() )
  {
    bClk=!bClk;
     /* New data message: check what it is */
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      UserApp_u32DataMsgCount++;
      if((G_au8AntApiCurrentData[0] == 0) && (G_au8AntApiCurrentData[1] == u8PairedCount) &&(G_au8AntApiCurrentData[7] == 0xFF))
      {
        u8PairedCount++;
      }
      for(u8 i = 0; i < ANT_APPLICATION_MESSAGE_BYTES; i++)
      {
        au8DataContent[2 * i] = HexToASCIICharUpper(G_au8AntApiCurrentData[i] / 16);
        au8DataContent[2 * i + 1] = HexToASCIICharUpper(G_au8AntApiCurrentData[i] % 16); 
      }
      LCDClearChars(LINE2_START_ADDR,20);
      LCDMessage(LINE2_START_ADDR,au8DataContent);
    } /* end if(G_eAntApiCurrentMessageClass == ANT_DATA) */
    
    else if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {
      UserApp_u32TickMsgCount++;
      
      /* Look at the TICK contents to check the event code and respond only if it's different */
      if(u8AntState != G_au8AntApiCurrentData[ANT_TICK_MSG_EVENT_CODE_INDEX])
      {
        /* The state changed so update u8LastState and queue a debug message */
        u8AntState = G_au8AntApiCurrentData[ANT_TICK_MSG_EVENT_CODE_INDEX];
      } /* end if (u8LastState != G_au8AntApiCurrentData[ANT_TICK_MSG_EVENT_CODE_INDEX]) */
      
      u8TollMessage[7] = u8PairedCount;
      u8TollMessage[6]++;
      if(bClk)
      {
        u8TollMessage[0]=0;
        AntQueueBroadcastMessage(u8TollMessage);
       // bSendBefore=1;
      }
      else
      {
        u8TollMessage[0]=u8PairedCount;
        AntQueueBroadcastMessage(u8TollMessage);
      }
    } /* end else if(G_eAntApiCurrentMessageClass == ANT_TICK) */
    
  } /* end AntReadData() */
}

static void CheckAntState()
{
  switch (u8AntState)
  {
    /* If we are synced with a device, blue is solid */
    case RESPONSE_NO_ERROR:
    {
      LedOff(GREEN);
      LedOn(BLUE);
      break;
    }

    /* If we are paired but missing messages, blue blinks */
    case EVENT_RX_FAIL:
    {
      LedOff(GREEN);
      LedBlink(BLUE, LED_2HZ);
      break;
    }

    /* If we drop to search, LED is green */
    case EVENT_RX_FAIL_GO_TO_SEARCH:
    {
      LedOff(BLUE);
      LedOn(GREEN);
      break;
    }
    /* If the search times out, the channel should automatically close */
    case EVENT_RX_SEARCH_TIMEOUT:
    {
      DebugPrintf("Search timeout\r\n");
      break;
    }

    default:
    {
//      DebugPrintf("Unexpected Event\r\n");
      break;
    }
  } /* end switch (G_au8AntApiCurrentData) */
}

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void UserAppSM_Close(void)
{
  if(WasButtonPressed(BUTTON0))
  {
    /* Got the button, so complete one-time actions before next state */
    ButtonAcknowledge(BUTTON0);
    
    /* Queue open channel and change LED0 from yellow to blinking green to indicate channel is opening */
    AntOpenChannel();
    LedOff(YELLOW);
    LedBlink(GREEN, LED_2HZ);
    
    /* Set timer and advance states */
    UserApp_u32Timeout = G_u32SystemTime1ms;
    UserApp_StateMachine = UserAppSM_WaitChannelOpen;
  }
}
static void UserAppSM_WaitChannelClose(void)
{
  /* Monitor the channel status to check if channel is closed */
  if(AntRadioStatus() == ANT_CLOSED)
  {
    LedOff(GREEN);
    LedOn(YELLOW);
    UserApp_StateMachine = UserAppSM_Close;
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp_u32Timeout, TIMEOUT_VALUE) )
  {
    LedOff(GREEN);
    LedOff(YELLOW);
    LedBlink(RED, LED_4HZ);
    
    UserApp_StateMachine = UserAppSM_Error;
  }
}
static void UserAppSM_WaitChannelOpen(void)
{
  if(AntRadioStatus() == ANT_OPEN)
  {
    LedOn(GREEN);  
    UserApp_StateMachine = UserAppSM_Idle;
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp_u32Timeout, TIMEOUT_VALUE) )
  {
    AntCloseChannel();
    LedOff(GREEN);
    LedOn(YELLOW);  
    UserApp_StateMachine = UserAppSM_Close;
  }
} /* end UserAppSM_Idle() */

static void UserAppSM_Idle(void)
{
  if( WasButtonPressed(BUTTON1) )
  {
    u8 u8TempString[] = "   Board Paired\n\r";
    u8 u8TempReal = u8PairedCount -1;
    u8TempString[0] = HexToASCIICharUpper(u8TempReal / 16);
    u8TempString[1] = HexToASCIICharUpper(u8TempReal % 16); 
    ButtonAcknowledge(BUTTON1);
    DebugPrintf("\n\r");
    DebugPrintf(u8TempString);
  }
  if( WasButtonPressed(BUTTON0) )
  {
    ButtonAcknowledge(BUTTON0);
    AntCloseChannel();
    u8AntState = 0xff;

    LedOff(YELLOW);
    LedOff(BLUE);
    LedBlink(GREEN, LED_2HZ);
    /* Set timer and advance states */
    UserApp_u32Timeout = G_u32SystemTime1ms;
    UserApp_StateMachine = UserAppSM_WaitChannelClose;
  }
  else if(sPresentCMD.bValued)
  {
    UserApp_SendMessage();
  }
  else
  {
    UserApp_SearchingNewDiv();
  }
  CheckAntState();
}
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserAppSM_Error(void)          
{
  UserApp_StateMachine = UserAppSM_Close;
  
} /* end UserAppSM_Error() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserAppSM_FailedInit(void)          
{
    
} /* end UserAppSM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
