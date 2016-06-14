#include "configuration.h"
#include "Command.h"
static fnCode_type CMD_pfnStateMachine;
extern u8 G_au8DebugScanfBuffer[DEBUG_SCANF_BUFFER_SIZE]; /* Space to latch characters for DebugScanf() */
extern volatile u8 G_u8DebugScanfCharCount;
extern volatile u8 u8PairedCount;  
bool bCommandFound = FALSE;
CMD sPresentCMD = {FALSE,0,0,{0}};
/* Counter for # of characters in Debug_au8ScanfBuffer */
//static u8 *pDebugBufCurrentChar = &G_au8DebugScanfBuffer[0];
bool bGetNewCmd = 0;

bool CheckValue(u8* ptr_)
{
  bool Value = 0;
  u8 u8TempVar = *ptr_;
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

void CMD_Initialize()
{
    CMD_pfnStateMachine = CMDSM_WaitCMD;
}

void CMD_ActiveState()
{
  CMD_pfnStateMachine();
}

static void CMDSM_WaitCMD(void)
{
  u8 u8CurrentByte = 0;
  if(G_u8DebugScanfCharCount)
    u8CurrentByte = G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1];
  else
    u8CurrentByte = G_au8DebugScanfBuffer[G_u8DebugScanfCharCount];

  if((u8CurrentByte == ASCII_CARRIAGE_RETURN) && !sPresentCMD.bValued)
  {
    bCommandFound = TRUE;
    CMD_pfnStateMachine = CMDSM_CheckCMD;
  }
}

static void CMDSM_CheckCMD(void)
{
  static u8 u8CMDHandle[] = "con ";
  static u8 u8Temp;
  u8 u8PlaceCounter = 0;
  u8 i;
  if(G_u8DebugScanfCharCount < 10)
    bCommandFound = 0;
  if(G_au8DebugScanfBuffer[3] != ' ' || G_au8DebugScanfBuffer[6] != ' '||G_au8DebugScanfBuffer[9] != ' ')
    bCommandFound = 0;
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
  if(bCommandFound)
    CMD_pfnStateMachine = CMDSM_BuildCMD;
  else
  {
    G_au8DebugScanfBuffer[0] = '\0';
    G_u8DebugScanfCharCount = 0;
    CMD_pfnStateMachine = CMDSM_WaitCMD;
  }
}

static void CMDSM_BuildCMD()
{
  u8 i = 0;
  sPresentCMD.bValued = 1;
  sPresentCMD.u8DivAddr = StringToHex(&G_au8DebugScanfBuffer[4]);
  sPresentCMD.u8CMDType = StringToHex(&G_au8DebugScanfBuffer[7]);
  while(G_au8DebugScanfBuffer[i + 10] != '\r')
  {
    sPresentCMD.u8CMDDetail[i] = G_au8DebugScanfBuffer[i + 10];
    i++;
  }
  sPresentCMD.u8CMDDetail[i] = 0xFF;
  G_au8DebugScanfBuffer[0] = '\0';
  G_u8DebugScanfCharCount = 0;
  CMD_pfnStateMachine = CMDSM_WaitCMD;
}
  