
#include "ds1820.h"


sbit DQ=P5^2;

extern uchar t_flag;

void delay_us(uint us)	//(us*9+16)us
{

   while(us--){
   		_nop_();
		_nop_();
   }
//   while(us--);
 //  while(us--);

}

//18b20����
uchar Reset_18b20() 
{
	uchar count=0;
	DQ=1;
	DQ=0;
	delay_us(500);  //(55*9+16��=us
	DQ=1;
	delay_us(60);   //�ȴ�DS18B20�ĵ͵�ƽ 25us
	while(DQ)
	{
		count++;
		if(count==60)
			return 0;
	}
	delay_us(420);
	return 1;
/*
	DQ = 1;
	delay_us(55);
	DQ = 0;
	delay_us(500);
	DQ = 1;
	delay_us(60);
	count = DQ;
	return count;
*/
}

//������дһ�ֽ�
void WriteByte(uchar dat)
{
	uchar i=0,w;
	for(i=0;i<8;i++)
	{
	  w=dat&0x01;
	  dat=dat>>1;
		if(w)	   //�ж���д1����д0
		{
			DQ=0;
			//��ʱ15us;
		  delay_us(1);
			//д1
			DQ=1;									
			delay_us(60);		;
		}

		else
		{
			DQ=0;
			 delay_us(1);
			//д0
			DQ=0;
		    delay_us(60);		
		}
	  DQ=1;	  //ÿ����һλ���������1US
	  delay_us(1);
		
	 }
}

//�ӵ����߶�һ�ֽ�
uchar ReadByte()
{
	 uchar tempdat=0,i=0;
	 DQ=1; //������ 
	 for(i=0;i<8;i++)
	 {	
	    tempdat=tempdat>>1;	
	 	DQ=0;
		 delay_us(1);
		DQ=1;
		 delay_us(1);	 
		/* */
		if(DQ==1)
			tempdat =tempdat|0x80;
		else  
			tempdat =tempdat|0x00;
		delay_us(60);  

	   DQ=1;	  //ÿ����һλ����������1US
	   delay_us(1);
	 }
	 return tempdat;
}


//��ȡ�����¶�
float ReadOne_T()
{
	int t;
	float t1;
	uchar tempH=0;
	uchar tempL=0;	
	if(!Reset_18b20()) 	//
		return 13;

	Reset_18b20();      //��λ	
	WriteByte(0xcc);	//����ROMָ��	
	WriteByte(0x44); 	//ִ���¶�ת���洢������ָ��
	delay_us(1000);		//�ȴ�>500us�¶�ת��ʱ��
	
	Reset_18b20();		//��λ
	WriteByte(0xcc);	//����ROMָ��	
	WriteByte(0xbe); 	//ִ�ж�RAM�Ĵ洢������ָ��
	  
	tempL=ReadByte();	//������--�¶ȵĵ�4λ�ֽ�
	tempH=ReadByte(); 	//������--�¶ȵĸ�4λ�ֽ�
	//����ȡ�¶�ת��������
	//t=tempH*256+tempL;
	t=tempH;
	t=t<<8;
	t=t|tempL;
	if(t<0x800)
	{
		t_flag = 0;
	}
	else if(t>=0x800)
	{
		t_flag=1;
		t=~t+1;
	}

	t1=t*0.625+0.5;
	return t1;
} 




