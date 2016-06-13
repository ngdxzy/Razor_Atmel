#include "configuration.h"

static fnCode_type CMD_pfnStateMachine;

bool bGetNewCmd = 0;

static void CMDSM_WaitCMD(void)
{
  bool bCommandFound = FALSE;
  u8 u8CurrentByte;
  
}