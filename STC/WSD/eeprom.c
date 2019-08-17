#include "eeprom.h"



//��������������ISP/IAP 
void Open_IAP(void)
{
      //IAPEN   SWBS    SWRST   CMD_FAIL -      WT2     WT1     WT0
	  IAP_CONTR = 0x83;           //����ISP/IAP��ϵͳʱ��<20MHzʱ�����õȴ�ʱ��WT2,WT1,WT0(010)

}


//������������ֹISP/IAP
void Close_IAP(void)
{
		IAP_CONTR = 0x00;           //��ֹISP/IAP
	
	   	IAP_ADDRH = 0xFF;
 		IAP_ADDRL = 0xFF;
}

//������������EEPROMָ���ĵ�Ԫ��ȡһ���ֽ�����

char Read_IAP_Byte(uint addr)
{
	 	//-       -       -        -      -       -       MS1     MS0
		IAP_CMD = 0x01;
		
		IAP_ADDRH = (addr & 0xFF00)>>8;
		IAP_ADDRL = addr & 0x00FF;
		
		IAP_TRIG = 0x5A;
		IAP_TRIG = 0xA5;                     //��IAP_TRIG��д0x5A��д0xA5��ISP/IAP����Ż���Ч
		
		return IAP_DATA;
}


//������������һ���ֽ�����д��EEPROMָ���ĵ�Ԫ,д������ǰӦ�Ȳ�������

void Write_IAP_Byte(uint addr, char writeVal)
{
		//-       -       -        -      -       -       MS1     MS0
		IAP_CMD = 0x02;
		
		IAP_ADDRH = (addr & 0xFF00)>>8;
		IAP_ADDRL = addr & 0x00FF;
		
		IAP_DATA = writeVal;
		
	    IAP_TRIG = 0x5A;
		IAP_TRIG = 0xA5;                     //��IAP_TRIG��д0x5A��д0xA5��ISP/IAP����Ż���Ч

}

//������������������,û���ֽڲ���
void Erase_IAP_Sector(uint addr)
{
		//-       -       -        -      -       -       MS1     MS0
		IAP_CMD = 0x03;

		IAP_ADDRH = (addr & 0xFF00)>>8;
		IAP_ADDRL = addr & 0x00FF;

		IAP_TRIG = 0x5A;
		IAP_TRIG = 0xA5;                     //��IAP_TRIG��д0x5A��д0xA5��ISP/IAP����Ż���Ч
}

//��������������ָ����Ԫ����
void Erase_IAP_Byte(uint addr)
{
	//STC�ڲ���չ1K(0x0000��0x03FF)ROM
	//��ʱ���EEPROMһ������512Byte����
	uchar xdata EEPROM_sector[512] = {0};
	uint sectorAddr;            //�����׵�ַ
	uint unit;                      //ĳ��������ĳ����Ԫ0��511
	uint i;
	sectorAddr = ((int)(addr / 512)) * 512;
	unit = addr % 512;
	for (i=0; i<512; i++)
	{
	        EEPROM_sector[i] = Read_IAP_Byte(sectorAddr+i);        //��ȡ����
	}

	Erase_IAP_Sector(sectorAddr);                  //��������
    EEPROM_sector[unit] = 0xFF;                         //Ҫ�����ĵ�Ԫ��Ϊ0xFF
	for (i=0; i<512; i++)
	{
		 Write_IAP_Byte(sectorAddr+i, EEPROM_sector[i]);         //����д������
	}

}



