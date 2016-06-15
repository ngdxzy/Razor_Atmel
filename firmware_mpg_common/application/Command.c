#include "configuration.h"
#include "Command.h"

/* Privite variable define */
static fnCode_type CMD_pfnStateMachine;

/* Variables from other files */
extern u8 G_au8DebugScanfBuffer[DEBUG_SCANF_BUFFER_SIZE]; /* From debug.c */
extern volatile u8 G_u8DebugScanfCharCount; /* From debug.c */
extern volatile u8 u8PairedCount;  /* From user_app.c */
extern volatile bool bIsChannelOpen; /* From user_app.c */
extern volatile bool bWaitResPond;  /* From user_app.c */
/* Gloable variables define */
bool bCommandFound = FALSE;
CMD sPresentCMD = {FALSE,0,0,{0}};
bool bGetNewCmd = 0;

/* Public functions define */

/* This function was build to check whether a two byte string can be changed to a hex number 
 before calling StringToHex() function .
  Its input *ptr_ should be the start address of the two byte string ,and it returns TRUE when the change
is allowed.
   */

bool CheckValue(u8* ptr_)
{
  bool Value = 0;
  u8 u8TempVar = *ptr_;
  /* From 'A' to 'F'? */
  if(u8TempVar > 96 && u8TempVar < 103)
  {
    Value = 1;
  }
  /* From 'a' to 'f'? */
  else if(u8TempVar > 64 && u8TempVar < 71)
  {
    Value = 1;
  }
  /* From '0' to '9'? */
  else if(u8TempVar > 47 && u8TempVar < 58)
  {
    Value = 1;
  }
  
  u8TempVar = *(ptr_ + 1);
  if(u8TempVar > 96 && u8TempVar < 103)
  {
    Value = 1;
  }
  else if(u8TempVar > 64 && u8TempVar < 71)
  {
    Value = 1;
  }
  else if(u8TempVar > 47 && u8TempVar < 58)
  {
    Value = 1;
  }
  
  return Value;
}
/* This funtion change a two byte String to a u8 Number as is refered before.
Its input *ptr_ should be the start address of the two byte string ,and it returns the number 
in type of hex. */
u8 StringToHex(u8* ptr_)
{
  u8 u8TempVar;
  u8 u8Ans;
  u8TempVar = *ptr_;
  if(u8TempVar > 96 && u8TempVar < 103)
  {
    u8Ans = (u8TempVar-87) * 10;
  }
  else if(u8TempVar > 64 && u8TempVar < 71)
  {
    u8Ans = (u8TempVar-55) * 10;
  }
  else if(u8TempVar > 47 && u8TempVar < 58)
  {
    u8Ans = (u8TempVar-48) * 10;
  }
  
  u8TempVar = *(ptr_ + 1);
  if(u8TempVar > 96 && u8TempVar < 103)
  {
    u8Ans += u8TempVar - 87;
  }
  else if(u8TempVar > 64 && u8TempVar < 71)
  {
    u8Ans += u8TempVar - 55;
  }
  else if(u8TempVar > 47 && u8TempVar < 58)
  {
    u8Ans += u8TempVar - 48;
  }
  
  return u8Ans;
}
/* Initialize the Command state machine, only called before start running the system */
void CMD_Initialize()
{
    CMD_pfnStateMachine = CMDSM_WaitCMD;
}
/* Active The CMD state machine ,called in while(1) in main().
 As running the statement need information provided by debug.c the state machine
should only be called after calling DebugRunActiveState() but before UserAppRunActiveState() */
void CMD_ActiveState()
{
  CMD_pfnStateMachine();
}

/* The state is waitting the user to input a command .when received a ENTER from uart,
the state machine will be changed to check whether the command inputted being valid.
*/
static void CMDSM_WaitCMD(void)
{
  static bool bPrintBefore = 0;
  /* Get the newly inputted character */
  u8 u8CurrentByte = 0;
  if(G_u8DebugScanfCharCount)
    u8CurrentByte = G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1];
  else
    u8CurrentByte = G_au8DebugScanfBuffer[G_u8DebugScanfCharCount];
  
  
  if(bPrintBefore && !sPresentCMD.bValid)
  {
    bPrintBefore = 0;
    DebugPrintf("\n\rThe system is free,you can input now .\n\r");
  }
  /* Get new command as Enter was pressed , and the command will be reject when the program is sendding a command*/
  if((u8CurrentByte == ASCII_CARRIAGE_RETURN) && !sPresentCMD.bValid)
  {
    
    /* Set the flag variable for next state to use. */
    if(bIsChannelOpen)
    {
      bCommandFound = TRUE;
      CMD_pfnStateMachine = CMDSM_CheckCMD;
    }
    else
    {
      G_au8DebugScanfBuffer[0] = '\0';
      G_u8DebugScanfCharCount = 0;
      DebugPrintf("\n\rThe channel has not be opend yet,you cannot send command now!\n\r");
    }
  }
  else if(sPresentCMD.bValid && !bPrintBefore)
  {
    bPrintBefore = 1;
    DebugPrintf("\n\rThe system is busy,please wait for a moment .\n\r");
  }
}

/* This state is creatted to check whether a newly inputted command is valid.
a valid command should like:
con              01                      01                   details
Handle| space | target address |space  | CMD type  | space  | CMD details
target address should smaller than paired devices count.
CMD type is limmited in 256 but in this project we only defines 10 (00 ~ 09)types .
and when CMD¡¡do not follow the format,it is invalid as well as program cannot recognize is.

bCommandFound is setted to True before the step into the state ,and once a error occured, it will be 
setted to False;
*/

static void CMDSM_CheckCMD(void)
{
  static u8 u8CMDHandle[] = "con ";
  static u8 u8Temp;
  u8 u8PlaceCounter = 0;
  u8 i;
  /* Here is to check the format ,when the format is right, than the size of it should be more than 10 and 
  space should occur at certain place */
  if(G_u8DebugScanfCharCount < 10)
    bCommandFound = 0;
  if(G_au8DebugScanfBuffer[3] != ' ' || G_au8DebugScanfBuffer[6] != ' '||G_au8DebugScanfBuffer[9] != ' ')
    bCommandFound = 0;
  /* If it passed the format check ,now check the handle. */
  if(bCommandFound)
  {
    for(i=0;i<4;i++)
    {
      if(G_au8DebugScanfBuffer[i] != u8CMDHandle[i])
      {
        bCommandFound = 0;
        break;
      }
    }
  }
  /* If it still correct, than check whether the address is exist  */
  if(bCommandFound)
  {
    if(CheckValue(&G_au8DebugScanfBuffer[4]))
    {
      u8Temp = StringToHex(&G_au8DebugScanfBuffer[4]);
      if(u8PairedCount <= u8Temp)
        bCommandFound = 0;
    }
    else
      bCommandFound = 0;
    
  }
  /* If still passed ,check whether the CMD type is defined */
  if(bCommandFound)
  {
    if(CheckValue(&G_au8DebugScanfBuffer[7]))
    {
      u8Temp = StringToHex(&G_au8DebugScanfBuffer[7]);
      if(u8Temp > 9)
        bCommandFound = 0;
    }
    else
    {
      bCommandFound = 0;
    }
  }
  /* If still passed ,than passed */
  /* The state machine will step into the CMDSM_BuildCMD to take the message and packed it for ant to sent*/ 
  if(bCommandFound)
  {
    bCommandFound = 0;
    CMD_pfnStateMachine = CMDSM_BuildCMD;
  }
  /* If the CMD is not passed , clear the scanf buffer and back to wait */
  else
  {
    G_au8DebugScanfBuffer[0] = '\0';
    G_u8DebugScanfCharCount = 0;
    DebugPrintf("\n\rCMD error!\n\r");
    CMD_pfnStateMachine = CMDSM_WaitCMD;
  }
}

/* This state is to Build the CMD for userapp to use.
it only screen out the helpful information and copy them to the struct CMD */

static void CMDSM_BuildCMD()
{
  u8 i = 0;
  sPresentCMD.bValid = 1;
  sPresentCMD.u8DivAddr = StringToHex(&G_au8DebugScanfBuffer[4]);
  sPresentCMD.u8CMDType = StringToHex(&G_au8DebugScanfBuffer[7]);
  while(G_au8DebugScanfBuffer[i + 10] != '\r')
  {
    sPresentCMD.u8CMDDetail[i] = G_au8DebugScanfBuffer[i + 10];
    i++;
  }
  sPresentCMD.u8CMDDetail[i] = 0xFF;
  /* reset the Scanf Buffer for checking new CMD */
  G_au8DebugScanfBuffer[0] = '\0';
  G_u8DebugScanfCharCount = 0;
  CMD_pfnStateMachine = CMDSM_WaitCMD;
}
  