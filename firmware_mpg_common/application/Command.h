#ifndef _COMMAND_H
#define _COMMAND_H

typedef struct
{
  bool bValued;
  u8 u8DivAddr;
  u8 u8CMDType;
  u8 u8CMDDetail[40]; 
}CMD;



static void CMDSM_WaitCMD(void);
static void CMDSM_CheckCMD(void);
static void CMDSM_BuildCMD(void);
#endif
