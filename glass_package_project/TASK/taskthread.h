#ifndef __TASKTHREAD_H
#define __TASKTHREAD_H
#include "motor.h"
#include "uart3_dataHandle.h"

#define GC_ROT_PACKAGE
//#define GC_VER_PACKAGE
#define GE_UP_DOWN_SENSOR
//#define GE_UP_SENSOR_BEFORE 

#define OVERTIME 20000 // 20 Second

// 任务线程状态
enum taskthread_state
{
	taskthread_emergency, //紧急暂停状态 
	taskthread_pause,	// 暂停状态
	taskthread_debug,	// 调试状态
	taskthread_error,	// 错误状态
	taskthread_boost,	// 启动状态
	taskthread_ready,	// 运行准备状态
	taskthread_running, // 运行进行状态
	taskthread_finsih,	// 运行结束状态
	taskthread_close,	// 关机状态
};

// 具体任务运行状态
enum task_state
{
	Ready,
	Running,
	Finish,
};

enum task_error
{
	Error_none = 0,
	Error_Sensor = 1 << 0, //  传感器错误
	Error_OverTime = 1 << 1,		//  任务超时错误
	Error_Slide_Glass = 1 << 2, // 缺少载玻片
	Error_In_Box = 1 << 3,		// 缺少装载盒
	Error_Out_Box = 1 << 4,		// 缺少存储盒
	Error_Cover_Glass = 1 << 5,		// 缺少盖玻片
	Error_Spray = 1 << 6,	// 缺少滴胶头
	Error_StorageFull = 1 << 7, // 存储槽满
	Error_Grap=1<<8, //夹取失败
	Error_Sucker=1<<9, //吸取失败 玻片损坏或者吸盘损坏 
};



// 玻片进入槽任务流程序号
enum glass_enter_task_index
{
	GE_none = 0,  // none 空任务
	/*************复用任务***********/
	GE_reset_on,  //  开机复位
	GE_reset_off, // 关机复位
	GE_Box_In,	  // 进缸
	GE_BOx_Out,	  // 出缸
	/****************************/
	GE_start,	   // 移动到原点位置
	#ifdef GE_UP_SENSOR_BEFORE
	GE_move_front, // 移动到装载槽前端
	GE_move_glass, // 移动到载玻片
	GE_move_back,  // 移动到装载槽后端
	#endif
	
	#ifdef GE_UP_DOWN_SENSOR
	GE_move_glass,
	#endif
	GE_finish,	   // 结束任务
	GE_error,	   // 异常处理任务

};

// 玻片夹手任务流程序号
enum glass_claw_task_index
{
	GC_none = 0,
	/*************复用任务***********/
	GC_ver_start, // 夹手复位到垂直原点位置 basic
	GC_rot_start, // 夹手复位到旋转原点位置 basic
	GC_release,	  // 夹手松开释放 basic
	GC_grab,	  // 夹手夹取 basic
	GC_reset_on,  // 开机复位
	GC_reset_off, // 关机复位
	/*****************************/
	GC_start,		// 开始运行
	GC_rot_down,	// 夹手旋转到垂直位置
	GC_move_down,	// 夹手下降到夹载玻片处
	GC_check_glass, // 检测是否夹取到玻片
	GC_move_up,		// 夹手上升回垂直原点
	#ifdef GC_ROT_PACKAGE
	GC_rot_up,		// 夹手回到旋转原点
	GC_rot_hor,		// 夹手旋转到水平位置
	#endif
	#ifdef GC_VER_PACKAGE
	GC_rot_hor,
	GC_move_package,
	#endif
	
	GC_finish,
	GC_error, // 异常处理任务
};

// 玻片封装任务流程序号
enum glass_package_task_index
{
	GP_none = 0,
	/**************复用任务****************/
	GP_spray_test,	 // 进行滴胶测试 basic
	GP_move_start,	 // 复位到轨道原点 basic
	GP_cyl_start,	 // 气缸复位到原点位置 basic
	GP_sucker_start, // 关闭吸盘 basic
	GP_reset_on,	 // 开机复位
	GP_reset_off,	 // 关机复位
	/*************************************/
	GP_start,		  // 封片初始化
	GP_move_glass,	  // 移动到盖玻片位置
	GP_sucker_down,	  // 吸盘下降
	GP_suck_glass,	  // 吸盘吸取盖玻片
	GP_sucker_up,	  // 吸盘上升
	GP_move_spray,	  // 移动到喷胶位置
	GP_move_package,  // 移动到封片位置
	GP_package,		  // 开始封片
	GP_finish,
	GP_error, // 异常处理任务

};

// 玻片出任务流程序号
enum glass_out_task_index
{
	GO_none = 0,
	/************复用任务************/
	GOV_start, // 复位到垂直原点位置 basic
	GOH_start, // 玻片复位到水平原点位置 basic
	GO_adjust, // 调整存储槽对准玻片 basic
	GO_reset_on,  // 开机复位
	GO_reset_off, // 关机复位
	GO_Box_In,	  // 进槽
	GO_Box_Out,	  // 出槽
	/*******************************/
	GO_start,	  // 开始运行
	GOH_package,  // 玻片托盘移动到水平封片位置
	GOH_end,	  // 玻片托盘移动到水平终点位置
	GO_out,		  // 玻片出料
	GO_next,	  // 移动到下一存储器
	GO_finish,
	GO_error, // 异常处理任务
};

struct glass_enter_struct
{
	volatile enum glass_enter_task_index task; // 任务序列
	volatile enum task_state sta;			   // 任务状态
	volatile motor_struct *motor;			   // 电机结构体指针
	enum glass_enter_task_index resume_task;   // 恢复任务序列
	u32 running_tim;						   // 运行时间 ms
	u32 GE_box_len;							   // 装载槽长度
	u32 GE_box_speed;
	u8 box_Exist;							   // 装载槽检测信号
	u8 glass_Exist;							   // 载玻片检测信号
	u8 subtask;								   // 子任务
	u8 Index;
	u8 WaitAck;
};

struct glass_claw_struct
{
	volatile enum glass_claw_task_index task;
	enum glass_claw_task_index main_task;
	enum glass_claw_task_index resume_task;
	volatile enum task_state sta;
	volatile motor_struct *motor_v; // 垂直电机
	volatile motor_struct *motor_r; // 旋转电机
	volatile u32 running_tim;		// 运行时间
	u16 GCR_hor_pos;				// 旋转水平位置
	u16 GCR_ver_pos;				// 旋转垂直位置
	u32 GCV_down_pos;				// 垂直下降位置
	u32 GCV_glass_len;		//检测是否夹取玻片成功上升高度
	u32 GCV_pacakge_pos;
	u8 subtask;						// 子任务
	u8 main_subtask;
	volatile u8 Glass_Ready;
};





struct glass_out_struct
{
	volatile enum glass_out_task_index task;
	enum glass_out_task_index main_task;
	volatile enum task_state sta;
	enum glass_out_task_index resume_task;
	volatile motor_struct *motor_h;
	volatile motor_struct *motor_v;
	volatile u32 running_tim;
	u32 GOH_mid_pos;
	u32 GOH_end_pos;
	u32 GOV_box_dis;
	u32 GOV_slot_dis;
	u32 GOV_box_len;
	u16 GOV_adjust;
	u16 GOV_adjust_start;
	u8 subtask;		 // 子任务
	u8 main_subtask; //
	u8 Index;
	u8 WaitAck;
	u8 Storage_full;
	u32 GOH_waste_pos; //废片位置
};

extern volatile enum task_error error_type;
extern enum taskthread_state TaskThread_State; // 任务线程运行状态 


extern struct glass_enter_struct GE;
extern struct glass_claw_struct GC;

extern struct glass_out_struct GO;

extern volatile u8 glass_signal;
extern volatile u8 box_signal;

// 任务线程结构体初始化 任务定时器TIM6初始化
void TaskThread_Init(void);
//GE任务线程
void GE_TaskThread(void);
//GC任务线程
void GC_TaskThread(void);
//GP任务线程
void GP_TaskThread(void);
//GO任务线程
void GO_TaskThread(void);

//错误处理
void Error_Set(enum task_error error, u8 error_sen);

//超时错误处理
void Error_OverTime_Set(enum Error error_task,u8 task_index);

//从错误暂停中恢复运行
u8 TaskThread_Resume_Error(void);

// 任务是否处于空闲状态
u8 TaskThread_CheckIdle(void);

//错误处理是否结束
u8 TaskThread_Check_ErrorDone(void);

//任务是否停止 任务不在运行中
u8 TaskThread_CheckStop(void);

// 检测机器是否可以准备运行
enum taskthread_state TaskThread_IsReady(void);

// 开机复位任务
u8 TaskThread_BootReset(void);

// 开始运行任务
u8 TaskThread_Start(void);

// 关闭运行任务
u8 TaskThread_Close(void);

// 紧急暂停任务
void TaskThread_Emergency(void);

//暂停任务
u8 TaskThread_Pause(void);

// 恢复任务
u8 TaskThread_Resume(void);

// 任务参数初始化
void TaskThread_Parm_Init(void);

//装载槽进入
u8 TaskThread_GEIn(void);
//装载槽推出
u8 TaskThread_GEOut(void);
//存储盒进入
u8 TaskThread_GOIn(void);
//存储盒推出
u8 TaskThread_GOOut(void);

//发送错误信息
void Error_Mesg_Send(void);

#endif