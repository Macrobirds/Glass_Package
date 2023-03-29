#ifndef __TASKTHREAD_H
#define __TASKTHREAD_H
#include "motor.h"
#include "Globalconfig.h"

#define OVERTIME 3000


//任务运行状态
enum task_state
{
	 Ready,
	 Running,
	 Finish,
};

enum task_error
{
	Error_Slide_Glass=1<<0,
	Error_Cover_Glass=1<<1,
	Error_In_Box=1<<2,
	Error_Out_Box=1<<3,
	Error_Sensor=1<<4,
	Error_Spray=1<<5,
	Error_OverTime=1<<6,
};

//玻片进入槽任务流程序号
enum glass_enter_task_index
{
	GE_none,// none 空任务 
	GE_reset_on,//开机复位 进缸
	GE_reset_off,//关机复位 出缸
	GE_move_start, //移动到原点位置
	GE_move_front, //移动到装载槽前端
	GE_move_glass, //移动到载玻片
	GE_move_back, //移动到装载槽后端
	GE_finish, //结束任务
  GE_error,//异常上报任务
    
};

//玻片夹手任务流程序号
enum glass_claw_task_index
{
	GC_none,
	GC_reset_on, //开机复位
	GC_reset_off, //关机复位
	GC_ver_start, //夹手复位到垂直原点位置
	GC_rot_start, //夹手复位到旋转原点位置
	GC_rot_down, //夹手旋转到垂直位置
	GC_move_down, //夹手移动到夹载玻片处
	GC_grab,	//夹手夹取载玻片
	GC_move_up,	//夹手回到垂直原点
	GC_rot_up, //夹手旋转到旋转原点位置
	GC_rot_hor, //夹手旋转到水平位置
	GC_release, //夹手松开释放
	GC_finish, 
  GC_error,//异常上报任务
};

//玻片封装任务流程序号
enum glass_package_task_index
{
	GP_none,
	GP_reset_on,
	GP_reset_off,
	GP_start, //封片初始化
	GP_move_start, //移动到轨道原点
	GP_move_glass, //移动到盖玻片位置
	GP_sucker_down, //吸盘下降
	GP_suck_glass, //吸盘吸取盖玻片
	GP_sucker_up, //吸盘上升
	GP_move_spray, //移动到喷胶位置
	GP_move_package, //移动到封片位置
	GP_package, //开始封片
	GP_finish, 
  GP_error,//异常上报任务
};


//玻片出任务流程序号
enum glass_out_task_index
{
	GO_none,
	GO_reset_on,
	GO_reset_off,
	GO_start, //玻片托盘移动到原点位置
	GO_package, //玻片托盘移动到封片位置
	GO_end, //玻片托盘移动到终点位置
	GO_out, //玻片出料
	GO_next, //移动到下一存储器
	GO_adjust,  //调整存储槽对准玻片
	GO_finish,
  GO_error,//异常上报任务
};

struct glass_enter_struct{
	volatile enum glass_enter_task_index task; //任务序列
	volatile enum task_state sta; //任务状态
	volatile enum task_error error; //错误码
	motor_struct * motor; //电机结构体指针
	volatile u32 running_tim; //运行时间 ms
	u32 GE_box_len; //装载槽长度
	u32 GE_box_dis; //装载槽间距
	u8 box_Exist; //装载槽检测信号
	u8 glass_Exist; //载玻片检测信号
	u8 subtask; //子任务
	enum glass_enter_task_index resume_task; //恢复任务序列
};

struct glass_claw_struct{
	volatile enum glass_claw_task_index task;
	volatile enum task_state sta;
	volatile enum task_error error;
	motor_struct * motor_v; //垂直电机
	motor_struct * motor_r; //旋转电机
	volatile u32 running_tim; //运行时间
	u16 GCR_hor_pos; //旋转水平位置
	u16 GCR_ver_pos; //旋转垂直位置
	u32 GCV_down_pos; //垂直下降位置
	u8 subtask; //子任务
	enum glass_claw_task_index resume_task;//恢复任务序列
}; 

struct glass_package_struct{
	volatile enum glass_package_task_index task;
	volatile enum task_state sta;
	volatile enum task_error error;
	motor_struct * motor;
	volatile u32 running_tim;
	u16 delay_before;
	u16 delay_after;
	u16 sucker_delay;
	u32 sucker_pos;
	u32 spray_pos;
	u32 spray_len;
	u16 spray_speed;
	u8 spray_pressure;
	u8 subtask; 
	enum glass_package_task_index resume_task;//恢复任务序列
};

struct glass_out_struct{
	volatile enum glass_out_task_index task;
	volatile enum task_state sta;
	volatile enum task_error error;
	motor_struct * motor_h;
	motor_struct * motor_v;
	volatile u32 running_tim;
	u32 GOH_mid_pos;
	u32 GOH_end_pos;
	u32 GOV_box_dis;
	u32 GOV_slot_dis;
	u32 GOV_box_len;
	u8 subtask; //子任务
	volatile u8 glass_num;
	volatile u8 box_num;
	enum glass_out_task_index resume_task;//恢复任务序列
};

//任务准备状态
u8 Task_IsReady=FALSE;


extern struct glass_enter_struct GE;
extern struct glass_claw_struct GC;
extern struct glass_package_struct GP;
extern struct glass_out_struct GO;
//任务线程结构体初始化 任务定时器TIM6初始化
void TaskThread_Init(void); 
//检测机器是否可以准备运行
void TaskThread_IsReady(void); 
void GE_TaskThread(void);
void GC_TaskThread(void);
void GP_TaskThread(void);
void GO_TaksThread(void);

void Pause_TaskThread(void);
void Resume_TaskThread(void);



#endif