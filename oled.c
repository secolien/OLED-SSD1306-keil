#include "oled.h"


uint8_t Swap8(uint8_t byte){return (((0xF0&byte)/16)|((0x0F&byte)*16));}

void OLED_Command(uint8_t byte)
{
	uint8_t buffer[2]={0x00,byte};
	HAL_I2C_Master_Transmit(OLED_I2C, OLED_Address, buffer, 2, 5);
}

void OLED_InitializeDevice()
{
	OLED_Command(0xAE);
	OLED_Command(0xA8); OLED_Command(63); 													//multiplex
	OLED_Command(0xD5); OLED_Command(0x80); 													//freq
	//OLED_Command(0xA3); OLED_Command(0x00); OLED_Command(64);
	OLED_Command(0xD3); OLED_Command(0x00); 
	OLED_Command(0x40); 
	OLED_Command(0x20); OLED_Command(0x00);
	//OLED_Command(0xA0); 
        OLED_Command(0xA1);
	//OLED_Command(0xC0); 
        OLED_Command(0xC8); 
	OLED_Command(0xDA); OLED_Command(0x12); 
	OLED_Command(0x81); OLED_Command(0xFF);
	OLED_Command(0xA4);
	OLED_Command(0xA6); 																							//normal display
        OLED_Command(0x8D); OLED_Command(0x14);													  //charge pump
	OLED_Command(0xAF); 																							//display on
	
}

void OLED_CursorPosition (uint8_t row, uint8_t column) //row 0-7 ; column 0-127
{
	column+=1;
	OLED_Command(0xB0|(row)); //set page
	OLED_Command(Swap8(column<<4));
	OLED_Command(0x10|(column>>4));
}


void OLED_SetContrast(uint8_t data) //0-255 steps
{
	OLED_Command(0x81); 
	OLED_Command(data); //contrast
}

void OLED_ReverseDisplay(_Bool state) //0-normal , 1-reverse
{
	OLED_Command(0xA6+state);
}

void OLED_Display(_Bool state) //0-off , 1-on
{
	OLED_Command(0xA4+state);
}

uint8_t OLED_ReadFrame (uint8_t row, uint8_t column) //row 1-6 ; column 0-127
{
	column+=1;row-=1;
	return OLED.Frame[row-1][column];
}

void OLED_WriteFrame (uint8_t data, uint8_t row, uint8_t column) //row 1-6 ; column 0-127
{
	column+=1;row-=1;
	OLED.Frame[row-1][column]=data;
}

void OLED_SetPixel (_Bool state, uint8_t x_coordinate, uint8_t y_coordinate) 	//row 0-47 ; column 0-127
{
	uint8_t data=OLED_ReadFrame(x_coordinate/8, y_coordinate);
	uint8_t data_bit=0x01;
	if(state){ (data_bit<<=((x_coordinate%8))); data=data|data_bit;}
	else {data_bit=0xFE; data_bit<<=((x_coordinate%8));data=data&data_bit;}
	OLED.Frame[x_coordinate/8][y_coordinate]=data;
}

_Bool OLED_ReadPixel (uint8_t x_coordinate, uint8_t y_coordinate)  		//row 0-47 ; column 0-127
{return ((OLED_ReadFrame(x_coordinate/8, y_coordinate)>>(7-(x_coordinate%8)))&0x01);}

void OLED_FillFrame (_Bool state){
   for(uint8_t i=0; i<=5; i++)
   {
     for(uint8_t k=0; k<=128; k++){ if(state){OLED.Frame[i][k]=0xFF;} else{OLED.Frame[i][k]=0x00;}}
   }
}


/////////////////////////////////////////////////////////////////////////////////////////////////


void OLED_WriteBattery_ChargeLevelIcon(uint8_t level)
{
	level++;
	uint8_t counter=level;
	uint8_t Buffer[15];
	for(uint8_t i=14; i>>0; i--)
	{
	  Buffer[i]=Battery_ChargeLevelIcon[i-1];
	  if(level-counter && counter*level){Buffer[i]=0x7E;} 
	  if(counter){counter--;}
 	}
	Buffer[0]=0x40;
        OLED_CursorPosition(0,113);
  	HAL_I2C_Master_Transmit(OLED_I2C, OLED_Address, Buffer,15,10);
  	OLED.LastSymbolLine=110;
}

void OLED_WriteBatteryCharge_Icon(){
	OLED.IconLocation[0]=OLED.LastSymbolLine;
	uint8_t Buffer[6];
	for(uint8_t i=0; i<=4; i++)
	{
	  Buffer[i+1]=Battery_ChargeIndicator[i];

	}
	Buffer[0]=0x40;
        OLED_CursorPosition(0,OLED.LastSymbolLine-5);
  	HAL_I2C_Master_Transmit(OLED_I2C, OLED_Address, Buffer,6,10);
	OLED.LastSymbolLine-=7;
}



void OLED_WriteGPS_Icon()
{
	OLED.IconLocation[1]=OLED.LastSymbolLine;
	uint8_t Buffer[6];
	for(uint8_t i=0; i<=4; i++)
	{
	  Buffer[i+1]=GPS_Icon[i];

	}
	Buffer[0]=0x40;
        OLED_CursorPosition(0,OLED.LastSymbolLine-5);
  	HAL_I2C_Master_Transmit(OLED_I2C, OLED_Address, Buffer,6,10);
	OLED.LastSymbolLine-=7;
}


void OLED_WriteWIFI_Icon()
{
	OLED.IconLocation[2]=OLED.LastSymbolLine;
	uint8_t Buffer[11];
	for(uint8_t i=0; i<=9; i++)
	{
	  Buffer[i+1]=WIFI_Icon[i];

	}
	Buffer[0]=0x40;
        OLED_CursorPosition(0,OLED.LastSymbolLine-10);
  	HAL_I2C_Master_Transmit(OLED_I2C, OLED_Address, Buffer,11,10);
	OLED.LastSymbolLine-=13;
} 


void OLED_IconBar_Delete()
{
	uint8_t Buffer[68];
	for(uint8_t i=0; i<=67; i++)
	{
	  Buffer[i]=0x00;

	}
	Buffer[0]=0x40;
        OLED_CursorPosition(0,60);
  	HAL_I2C_Master_Transmit(OLED_I2C, OLED_Address, Buffer,68,10);
}


void OLED_IconBar_IconRemap(uint8_t level)
{
	OLED_IconBar_Delete();
	OLED_WriteBattery_ChargeLevelIcon(level);
	OLED_WriteTime();
	if(OLED.IconState[0]){OLED_WriteBatteryCharge_Icon();}
	if(OLED.IconState[1]){OLED_WriteGPS_Icon();}
	if(OLED.IconState[2]){OLED_WriteWIFI_Icon();}
}

void OLED_EnterIcon(_Bool selection)
{  
	uint8_t Buffer[11];
     if(selection)
	{
	 for(uint8_t i=0; i<=9; i++){
	 Buffer[i+1]=~ENTER_ICON[i];}
	}
     else
	{
	 for(uint8_t i=0; i<=9; i++){
	 Buffer[i+1]=ENTER_ICON[i];}
	}
	Buffer[0]=0x40;
	OLED_CursorPosition(1,116);
	HAL_I2C_Master_Transmit(OLED_I2C, OLED_Address, Buffer,11,10);
}

void OLED_BackIcon(_Bool selection)
{  
	uint8_t Buffer[10];
     if(selection)
	{
	 for(uint8_t i=0; i<=8; i++){
	 Buffer[i+1]=~BACK_ICON[i];}
	}
     else
	{
	 for(uint8_t i=0; i<=8; i++){
	 Buffer[i+1]=BACK_ICON[i];}
	}
	Buffer[0]=0x40;
	OLED_CursorPosition(1,1);
	HAL_I2C_Master_Transmit(OLED_I2C, OLED_Address, Buffer,10,10);
}



void OLED_WriteTime ()
{
	auto RTC_TimeTypeDef sTime;
	uint8_t Buffer[30]={0x40};
	OLED_CursorPosition(0,1);
	HAL_I2C_Master_Transmit(OLED_I2C, OLED_Address, Buffer,30,10);
	HAL_RTC_GetTime(&hrtc,&sTime,RTC_FORMAT_BIN);
	sprintf(OLED.Text,"%02d:%02d",sTime.Hours,sTime.Minutes);
  OLED_Printf(0,1,OLED.Text);
}



void OLED_Header(char ptr[])
{
	uint8_t Size_of=0; while(ptr[Size_of]){Size_of++;}
	Size_of=(Size_of-1)*7;
	OLED_Printf(1, 64-Size_of/2, ptr);
}



void OLED_ClearScreen ()
{
	uint8_t Buffer[129];
	Buffer[0]=0x40;
	for(uint8_t j=0; j<=7; j++)
	{	OLED_CursorPosition(j,0);
		for(uint8_t i=1; i<=128; i++)
		{Buffer[i]=0x00;}
		HAL_I2C_Master_Transmit(OLED_I2C, OLED_Address, Buffer,129,10);
	}
	
}

void OLED_ClearRow (uint8_t row)
{
	uint8_t Buffer[129];
	OLED_CursorPosition(row,0);
	for(uint8_t i=1; i<=128; i++)
	{Buffer[i]=0x00;}
	Buffer[0]=0x40;
	HAL_I2C_Master_Transmit(OLED_I2C, OLED_Address, Buffer,129,10);
}


void OLED_Printf (uint8_t row, uint8_t column, char SendText[]) //0-7 row, 0-127 column
{ 
	uint8_t Size_of=0; while(SendText[Size_of]){Size_of++;}
	column+=1;
	uint8_t Buffer[7]={0x40};
    OLED_ClearRow(row);
	for(uint8_t Count_Text=0; Count_Text<=Size_of-1; Count_Text++)
	{
	  if(SendText[Count_Text]=='/' && SendText[Count_Text+1]=='n'){row++;  OLED_ClearRow(row); Count_Text+=2;column=1;}
		for(uint8_t i=0; i<=4; i++)
		{
			
			Buffer[i+1]=FONT[SendText[Count_Text]-32][i];
		}
		OLED_CursorPosition(row,column);
		HAL_I2C_Master_Transmit(OLED_I2C, OLED_Address, Buffer,7,10);
		column+=6; 
	}
}



void OLED_Start()
{
	OLED_InitializeDevice();
	OLED_ClearScreen();
	HAL_Delay(1000);
	OLED_ClearScreen();
	OLED.IconState[0]=TRUE;
	OLED.IconState[1]=TRUE;
	OLED.IconState[2]=TRUE;
	OLED.ChargeLevel=1;
	OLED_IconBar_IconRemap(OLED.ChargeLevel); 
	
}
