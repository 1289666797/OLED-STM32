//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//  �� �� ��   : main.c
//  �� �� ��   : v2.0
//  ��    ��   : north-sj
//  ��������   : 2016-0101
//  ����޸�   : 
//  ��������   : OLED SPI4�ӿ���ʾ����(STM32ϵ��)
//              ˵��: ������
//              ----------------------------------------------------------------
//              GND  �ӵ�Դ��
//              VCC  ��5V��3.3v��Դ
//              D0   ��PB13��SCL��
//              D1   ��PB15��SDA��
//              RES  ��PB11
//              DC   ��PB10
//              CS   ��PB12               
//              ----------------------------------------------------------------
// �޸���ʷ   :
// ��    ��   : 2016
// ��    ��   : north-sj
// �޸�����   : �����ļ�
// 
//
//
//******************************************************************************/

#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"
#include "usart.h"
#include "config.h"
#include "string.h"
#include "integer.h"
#include "TEST_FATFS.h"
/* 
*˵��: 
*PA0:LED1;PA1:LED2; 
*PA9:USART1_TX;PA10:USART1_RX 
*/  
#include "stm32f10x.h"  
#include "stm32f10x_rcc.h"  
#include "stm32f10x_gpio.h"  
#include "stm32f10x_tim.h"  
#include "stm32f10x_exti.h"  
#include "system_stm32f10x.h"  
#include "misc.h" 

/* Private variables ---------------------------------------------------------*/
FATFS fs;         				/* Work area (file system object) for logical drive */
FIL fsrc;         				/* file objects */   
FRESULT res;

UINT br;									//FatFs
/* Private function prototypes -----------------------------------------------*/
uint8_t textFileBuffer[] = "Thank you for using STM32F103 �� \r\n";	//����д���ַ���
char path[128]="0:";																								//·���洢
uint32_t file_byte;																									//�洢Rt.Dat�ļ���С
uint32_t ls_move=0;																									//�洢�ļ�ָ���ƶ�
unsigned char G_Bin1[1024];																					//BINͼ���Դ�1
unsigned char G_Bin2[1024];																					//BINͼ���Դ�2
extern int k_1;
extern int k_2;

int SD_TotalSize(void);
void USART_Configuration(void);
FRESULT scan_files (char* path);


void RCC_Configuration(void);  
void GPIO_Configuration(void);   
void TIM3_Configuration(void);  
void NVIC_Configuration(void);



void Read_Bin1(void)
{
		if(ls_move>file_byte-1024)
			{
				ls_move=0;
				printf(" \n");
				printf("������ϣ����ؿ�ͷ \r\n");
				printf(" \n");
			}
			br=1;
			res=f_lseek(&fsrc,ls_move);
			res=f_read(&fsrc,G_Bin1,sizeof(G_Bin1),&br);//��ȡBin���ݷ���G_Bin1������
			ls_move=ls_move+1024;
}


void Read_Bin2(void)
{
		if(ls_move>file_byte-1024)
			{
				ls_move=0;
				printf(" \n");
				printf("������ϣ����ؿ�ͷ \r\n");
				printf(" \n");
			}
			br=1;
			res=f_lseek(&fsrc,ls_move);
			res=f_read(&fsrc,G_Bin2,sizeof(G_Bin2),&br);//��ȡBin���ݷ���G_Bin2������
			ls_move=ls_move+1024;
}



 int main(void)
 {	
		//uint8_t t;
		//char strw[512]={"qwertyuiopasdfasddfghjk"},strr[512];
		//DWORD free=0;
	 
		/*******************************************************/
		uart_init(115200);		//��ʼ�����ڣ������ò�����Ϊ115200
		/*******************************************************/
		SDCard_SpiInit();				//SD����IO�ڼ�SPIģ���ʼ����PA2�����⣨H��>L��;PA3 CD_CSƬѡ��PA5 SCK��PA6 MISO��PA7 MOSI������ģʽ
		//												//SD_Init();//SD����ʼ�������ʹ�õ�FATFS����Բ�����䣬��ΪFATFS�ڴ��ļ��Ȳ���ʱ�Ѿ���������
		//Test_f_getfreem(&free);	//��ȡSD����ʣ��������λΪM
		//printf("TF��ʣ��ռ�Ϊ %d\n MB\n",(int)free);
		/*******************************************************/ 

	 	//printf("\r\n\r\n%48s\r\n","<δ��⵽����,����SD���Ƿ���...>");

	 
	 
		if( SD_Insert_Status() == 0 )
			{
				printf("---- SD���Ѿ���⵽ \r\n");
			}
			else
			{
				printf("---- ������SD�� \r\n");
				//while( SD_Insert_Status != 0 );
				printf("---- ��⵽SD�������� \r\n");
				delay_ms(1864);
			}

		f_mount(0,&fs);	

			//res = f_open( &fsrc , "0:/Demo.TXT" , FA_CREATE_ALWAYS | FA_WRITE);
				res = f_open( &fsrc , "0:/Demo.TXT" , FA_CREATE_NEW | FA_WRITE);	
					
		
    if ( res == FR_OK )
			{ 
				/* Write buffer to file */
				res = f_write(&fsrc, textFileBuffer, sizeof(textFileBuffer), &br);     
 
				printf("Demo.TXT �����ɹ�        \r\n");

				/*close file */
				f_close(&fsrc);      
			}
			else if ( res == FR_EXIST )
			{
				printf("Demo.TXT �Ѵ��ڴ���      \r\n");
			}

		scan_files(path);	//������Ŀ¼���ļ�
		SD_TotalSize();		//����ʣ��ռ��С

/********************* ��ȡbadapple.bin����  *********************/
		
			 res = f_open( &fsrc , "0:/badapple.bin" , FA_READ);	
			 file_byte=f_size(&fsrc);
		   if (res==FR_OK)
				{
					printf("�ڸ�Ŀ¼���ҵ�badapple.bin�ļ�  \r\n");
					printf("badapple.bin�ļ���СΪ%d�ֽ� \r\n",file_byte);//��ʾ�ļ���С
				  //res=f_read(&fsrc,testRead,sizeof(testRead),&br);		//��ȡ
				  //printf("\r\n %s ",testRead);
				 
				}
				else if(res==FR_NO_FILE)
					{
						printf("��Ŀ¼���Ҳ���badapple.bin�����ļ�  \r\n");
						printf(" \n");
					}

/****************************************************************/					
					
										
	 
		delay_init();							//��ʱ������ʼ��	  
		//NVIC_Configuration(); 	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
		//LED_Init();			     		//LED�˿ڳ�ʼ��
		OLED_Init();					   	//��ʼ��OLED  
		OLED_Clear(); 						//����	
		//t=' ';



		Read_Bin1();							//��ȡ��һ֡
		Read_Bin2();							//��ȡ�ڶ�֡

/************************����OLED��ʾ*************************/
		OLED_Clear();
		LED_ON;
		
		//OLED_ShowCHinese(0,0,0);	//��
		//OLED_ShowCHinese(25,0,1);	//��
		//OLED_ShowCHinese(50,0,2);	//��
		//OLED_ShowCHinese(75,0,3);	//��
		//OLED_ShowCHinese(100,0,4);//��			
		//OLED_ShowString(0,3,(u8*)"0.96' OLED TEST");
		////OLED_ShowString(8,2,"ZHONGJINGYUAN");  
	  ////	OLED_ShowString(20,4,"2014/05/01");  
		//OLED_ShowString(0,6,(u8*)"ASCII:");  
		//OLED_ShowString(63,6,(u8*)"CODE:");  
		//OLED_ShowChar(48,6,t);		//��ʾASCII�ַ�	   
		//t++;
		//if(t>'~')t=' ';
		//OLED_ShowNum(103,6,t,3,16);//��ʾASCII�ַ�����ֵ 	

		delay_ms(1000);
		LED_OFF;
		OLED_DrawBMP(0,0,128,8,BMP1);//ͼƬ��ʾ(ͼƬ��ʾ���ã����ɵ��ֱ�ϴ󣬻�ռ�ý϶�ռ䣬FLASH�ռ�8K��������)
		delay_ms(1000);
		delay_ms(1000);
		delay_ms(1000);
		OLED_DrawBMP(0,0,128,8,gImage_1);
		delay_ms(1000);
		//OLED_DrawBMP(0,0,128,8,G_Bin1);
		//delay_ms(1864);

/*************************************************************/	

/***********************�ж����ü���ʼ��**********************/
    SystemInit();  
    RCC_Configuration();  
    GPIO_Configuration();   
    TIM3_Configuration();  
    NVIC_Configuration(); 											//�ж�Ƶ��20Hz
/*************************************************************/
		GPIO_Configuration();
	while(1) 
	{		
		if(k_1==0)
			{
				Read_Bin1();
				k_1=1;
			}
		if(k_2==0)
			{
				Read_Bin2();
				k_2=1;
			}

	}	  
	
}




void RCC_Configuration(void)//RCC���ú���
{     
  //USART2��USART3����APB1�϶�USART1����APB2�ϵ�
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);     
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);     
}   
  
void GPIO_Configuration(void)//GPIO���ú���
{    
  GPIO_InitTypeDef GPIO_InitStructure;    
    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;    			//����PA0
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
  GPIO_Init(GPIOA, &GPIO_InitStructure);   
}  
  
void TIM3_Configuration(void)//TIM3���ú���
{  
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  
  TIM_TimeBaseStructure.TIM_Period = 82;						//Ƶ��:72MHz  per/2000Hz=72000000/36000  
  TIM_TimeBaseStructure.TIM_Prescaler = 35999;			//36000-1=35999  ʱ��Ƶ��=72/(ʱ��Ԥ��Ƶ+1)
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;  	//ʱ�ӷ�Ƶ
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  
     
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE );  
  TIM_Cmd(TIM3,ENABLE);

}  

 

/*******************************************************************************
* Function Name  : scan_files
* Description    : �����ļ�Ŀ¼�������ļ�
* Input          : - path: ��Ŀ¼
* Output         : None
* Return         : FRESULT
* Attention		   : ��֧�ֳ��ļ���
*******************************************************************************/
FRESULT scan_files (char* path)
{
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;
#if _USE_LFN
    static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
#endif

    res = f_opendir(&dir, path);
    if (res == FR_OK) {
        i = strlen(path);
        for (;;) {
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0) break;
            if (fno.fname[0] == '.') continue;
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if (fno.fattrib & AM_DIR) {
                sprintf(&path[i], "/%s", fn);
                res = scan_files(path);
                if (res != FR_OK) break;
                path[i] = 0;
            } else {
                printf("%s/%s \r\n", path, fn);
            }
        }
    }

    return res;
}




/*******************************************************************************
* Function Name  : SD_TotalSize
* Description    : �ļ��ռ�ռ�����
* Input          : None
* Output         : None
* Return         : ����1�ɹ� ����0ʧ��
* Attention		   : None
*******************************************************************************/
int SD_TotalSize(void)
{
    FATFS *fs;
    DWORD fre_clust;        

    res = f_getfree("0:", &fre_clust, &fs);  /* �����Ǹ�Ŀ¼��ѡ�����0 */
	
    if ( res==FR_OK ) 
    {
	  /* Print free space in unit of MB (assuming 512 bytes/sector) */
 			
#if 0			//
			printf("\r\n%d MB total drive space.\r\n"
           "%d MB available.\r\n",
           ( (fs->n_fatent - 2) * fs->csize ) / 2 /1024 , (fre_clust * fs->csize) / 2 /1024 );     
#endif			
			
			printf("\r\n%d MB ��������\r\n"
						 "%d MB ���ÿռ䡣\r\n",
						(uint32_t)( (fs->n_fatent - 2) * fs->csize ) / 2 /1024 , (uint32_t)(fre_clust * fs->csize) / 2 /1024 );
		
			return ENABLE;
		}	
			else 
				return DISABLE;   
}	 



