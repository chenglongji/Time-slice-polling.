#include <ioCC2530.h>
#include "sys_init.h"
#include "uart.h"
#include"sysinit.h"

#define uchar unsigned char
#define uint unsigned  int

void TaskRermark();
void delay(uint);
void sys_init();
void TaskProcess();
void TaskLED1();
void TaskLED2();


uchar n = 0;


/*  任务结构体  */
typedef struct _TAST_COMPONENTS
{
	uchar Run;      //程序运行标志
	uchar Timer;  	//计时器
	uchar ItvTime;  //任务运行能使用的时间片
	void(* TaskHook)(void);   //任务指针,所要运行的任务函数
}TASK_COMPONENTS;

/*   任务结构体数组(** 如果要加功能,就在这里添加)   */
static TASK_COMPONENTS Taskcomps[] = 
{
	{0,2,2,TaskLED1},    //参数要与任务结构体相对
	{0,3,3,TaskLED2},
};

uint TASK_MAX = sizeof(Taskcomps)/sizeof(Taskcomps[0]);	     //sizeof操作符在编译时就算出来了,得到数组的地址容量,除以单个就得到数组数

void  sys_init()
{
	init_timer1();
        led_init();
        xtal_init();                          //外部32MHZ晶振
        uart0_init(0x00, 0x00);            //初始化串口：无奇偶校验，停止位为1位
	ISR();          //中断开启
} 

void main()
{
	sys_init();
	while(1)
	{
		TaskProcess();
	}
} 

/*********    中断处理服务子函数	*********/
#pragma vector = T1_VECTOR
__interrupt void T1_ISR()
{
         T1IE = 0;          //关定时器T1的中断
	 n++;
	 if(n >5)	   //5ms为一个时间片
	 {
	 	n = 0;
		TaskRermark();
	 }
	 T1CC0L = 62500/256;
	 T1CC0H = 62500%256; //定时1ms
	 T1IE = 1;    //开定时器T1的中断
}


/*   任务运行标志函数   */
void TaskRermark()
{
	uchar i;
	for(i=0;i<TASK_MAX;i++)		     //逐个任务处理
	{
		if(Taskcomps[i].Timer)		 //节拍数不为"0"时
		{
			Taskcomps[i].Timer--;
		}
		else
		{
		   Taskcomps[i].Timer = Taskcomps[i].ItvTime;   //恢复计时
		   Taskcomps[i].Run = 1;          //时间片到了,运行标志置1   
		}
	}
} 

/**      任务处理函数     **/
void TaskProcess()
{
	uchar i;
	for(i=0;i<TASK_MAX;i++)
	{
		if(Taskcomps[i].Run)		   //运行标志为"1"
		{
			Taskcomps[i].TaskHook();   //执行相应任务
			Taskcomps[i].Run = 0;      //运行之后清"0"
		}

	}
}

/*ms级软件延时函数*/
void delay(uint z)	
{
	uint x,y;
	for(x = z; x > 0; x--)
		for(y = 320; y > 0 ; y--);
} 

/**********   各个要执行的函数 *********/
void  TaskLED1()
{
	  D7 = 0;
	  delay(100);
          D7 = 1;
          delay(100);
}

void TaskLED2()
{
    D6 = 0;
    delay(50);
    D6 = 1;
    delay(50);   
}



