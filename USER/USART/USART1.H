#ifndef _USART_H_
#define _USART_H_
#include"config.h"
#include<stdio.h>
void UART1GPIO_config(void);//����IO������
void USART1_config(void);		//���ڳ�ʼ��
void UART1NVIC_config(void);//�����ж�
//int fputc(int ch, FILE *f);	//��������
//int fgetc(FILE *f); 				//��������
int USART_Scanf_Name(char * str);
#endif