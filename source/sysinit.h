#include<ioCC2530.h>


void init_timer1()
{
	T1CTL = 0x0e;//定时器1,128分频，模计数  
	T1CC0L = 62500/256;
	T1CC0H = 62500%256; //定时1ms
	T1CCTL1 |= 0x04 ;//启动定时器0,并使用比较模式
}

/*    当要使用何种中断时,就在这里开*/
void ISR()	      
{
	T1IE = 1;    //开定时器T0的中断
	EA = 1;
}