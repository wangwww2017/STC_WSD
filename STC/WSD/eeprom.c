#include "eeprom.h"



//功能描述：允许ISP/IAP 
void Open_IAP(void)
{
      //IAPEN   SWBS    SWRST   CMD_FAIL -      WT2     WT1     WT0
	  IAP_CONTR = 0x83;           //充许ISP/IAP，系统时钟<20MHz时，设置等待时间WT2,WT1,WT0(010)

}


//功能描述：禁止ISP/IAP
void Close_IAP(void)
{
		IAP_CONTR = 0x00;           //禁止ISP/IAP
	
	   	IAP_ADDRH = 0xFF;
 		IAP_ADDRL = 0xFF;
}

//功能描述：从EEPROM指定的单元读取一个字节数据

char Read_IAP_Byte(uint addr)
{
	 	//-       -       -        -      -       -       MS1     MS0
		IAP_CMD = 0x01;
		
		IAP_ADDRH = (addr & 0xFF00)>>8;
		IAP_ADDRL = addr & 0x00FF;
		
		IAP_TRIG = 0x5A;
		IAP_TRIG = 0xA5;                     //对IAP_TRIG先写0x5A再写0xA5，ISP/IAP命令才会生效
		
		return IAP_DATA;
}


//功能描述：把一个字节数据写入EEPROM指定的单元,写入数据前应先擦除扇区

void Write_IAP_Byte(uint addr, char writeVal)
{
		//-       -       -        -      -       -       MS1     MS0
		IAP_CMD = 0x02;
		
		IAP_ADDRH = (addr & 0xFF00)>>8;
		IAP_ADDRL = addr & 0x00FF;
		
		IAP_DATA = writeVal;
		
	    IAP_TRIG = 0x5A;
		IAP_TRIG = 0xA5;                     //对IAP_TRIG先写0x5A再写0xA5，ISP/IAP命令才会生效

}

//功能描述：擦除扇区,没有字节擦除
void Erase_IAP_Sector(uint addr)
{
		//-       -       -        -      -       -       MS1     MS0
		IAP_CMD = 0x03;

		IAP_ADDRH = (addr & 0xFF00)>>8;
		IAP_ADDRL = addr & 0x00FF;

		IAP_TRIG = 0x5A;
		IAP_TRIG = 0xA5;                     //对IAP_TRIG先写0x5A再写0xA5，ISP/IAP命令才会生效
}

//功能描述：擦除指定单元内容
void Erase_IAP_Byte(uint addr)
{
	//STC内部扩展1K(0x0000～0x03FF)ROM
	//临时存放EEPROM一个扇区512Byte数据
	uchar xdata EEPROM_sector[512] = {0};
	uint sectorAddr;            //扇区首地址
	uint unit;                      //某个扇区的某个单元0～511
	uint i;
	sectorAddr = ((int)(addr / 512)) * 512;
	unit = addr % 512;
	for (i=0; i<512; i++)
	{
	        EEPROM_sector[i] = Read_IAP_Byte(sectorAddr+i);        //读取扇区
	}

	Erase_IAP_Sector(sectorAddr);                  //擦除扇区
    EEPROM_sector[unit] = 0xFF;                         //要擦除的单元置为0xFF
	for (i=0; i<512; i++)
	{
		 Write_IAP_Byte(sectorAddr+i, EEPROM_sector[i]);         //重新写入扇区
	}

}



