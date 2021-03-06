#include "flightmode.h"

#include "control.h"
#include "stdio.h"
#include "RC.h"
#include "altHold.h"

#define USE_TOE_IN_UNLOCK 1
vu8 Mode = Vert;   //初始化机身为垂直
vu8 FlightMode = Stabilze; //默认飞行模式为自稳
vu8 State = Armed;  //初始化飞行状态为锁定

void go_arm()
{
    if(State == DisArmed) // 如果没有解锁
    {
        State = Armed;

    }
    else if(State == Armed)
    {
        //do something
    }

}

void go_disarm()
{
    if(State == Armed)
    {
        State = DisArmed;
    }
    else if(State == DisArmed)
    {
            
        GravityAcc = getGravityAcc();  //得到重力加速度
        
        //do something
    }
}




s16 ready_cnt = 0;
/*----------------------------------------------------------
 + 实现功能：解锁判断
 + 调用参数：两次调用时间差 单位：秒
----------------------------------------------------------*/

void FlightStateTask(float T)
{
    if( CHdata[2] < 1300 ) //油门控制信号小于1200
    {
        if( !State && ready_cnt != -1 ) //解锁完成，且已退出解锁上锁过程
        {

        }
        /* 内八解锁方式*/

        if( CHdata[0] < 1300 )
        {

            if( CHdata[1] < 1300 )

                if( CHdata[3] < 1300 )

                    if( ready_cnt != -1 )	//内八满足且退出解锁上锁过程，防止连续切换

                        ready_cnt +=  100 * T; //解锁计时

        }

        /* 已经解锁或上锁了*/
        else if( ready_cnt == -1 )
   
            ready_cnt = 0; //清零解锁计时
     
    }
    else//不符合解锁或上锁条件
    {
        ready_cnt = 0; //清零解锁计时
    }


    if( ready_cnt > 1000 ) // 清零数1000次以上
    {
        ready_cnt = -1;//防止连续切换解锁
        if( !State )//若锁定
        {
            go_disarm();//解锁

        }
        else//若已经上锁
        {
            go_arm(); //加锁
        }
    }

}

void ModeTask(vs16 AUX_Mode, int8_t tow)
{
    if(tow * (AUX_Mode - 1500) > 0 ) Mode = Vert;
    else Mode = Aero;  //使用第7通道改变飞机状态
}

void FlightTask(vs16 AUX_Flight, int8_t tow) //飞行模式
{
    if(tow * (AUX_Flight -1700) > 0 ) 
    {
        FlightMode = Stabilze;
        rate = 0;
    }
    else 
    {
        FlightMode = AltHold;
         barAltHoldHeight.Desire = 0;
         initialThrottleHold = CHdata[THR];
         barAltHoldHeight.I = 0;
         barAltRateOut=0;
    }
}

void SafeTask(void)
{
    static uint8_t safe_check_cnt = 0;
    if(!(loop_cnt %10))
      safe_check_cnt++;
    if(safe_check_cnt == 100)
    {
        go_arm();
        safe_check_cnt = 0;
    }
}





