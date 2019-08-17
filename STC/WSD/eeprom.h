#ifndef _EEPROM_H_
#define _EEPROM_H_


#include "global.h"


extern void Open_IAP(void);
extern void Close_IAP(void);
extern char Read_IAP_Byte(uint addr);
extern void Write_IAP_Byte(uint addr, char writeVal);
extern void Erase_IAP_Sector(uint addr);
extern void Erase_IAP_Byte(uint addr);


#endif
