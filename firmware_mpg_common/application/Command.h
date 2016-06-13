static void CMDSM_WaitCMD(void);
static void CMDSM_CheckCMD(void);

struct 
{
  u8 u8DivAddr = 0;
  u8 u8CMDType = 0;
  u8 u8CMDDetail[40] = {0}; 
}CMD;
