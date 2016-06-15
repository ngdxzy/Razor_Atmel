#ifndef _COMMAND_H
#define _COMMAND_H

/* Definition of the CMD struct */
typedef struct
{
  bool bValid;   /* To stemp whether the CMD has been send */
  u8 u8DivAddr;  /* The address of target device */
  u8 u8CMDType;  /* The type of the CMD */
  u8 u8CMDDetail[40]; /* The Details of the CMD */
}CMD;

/* Public functions declaration */
bool CheckValue(u8* ptr_);
u8 StringToHex(u8* ptr_);
void CMD_Initialize();
void CMD_ActiveState();
/* State machien declaration */

static void CMDSM_WaitCMD(void);
static void CMDSM_CheckCMD(void);
static void CMDSM_BuildCMD(void);
#endif
