# Implementation-of-Smart-Home-System-Based-on-STM32F103VET6-STM32F103VET6-
本次课设题目为智能家居，设计采取本地控制和巴法云平台控制两种方式，设计使用LCD屏幕与按键进行人机交互，方便控制以及排查bug，其中本地控制部分设计刷卡开门、开关灯、窗帘开关以及温湿度、烟雾、火灾传感器检测部分，巴法云平台设计远程开关门、窗帘、灯光，以及远程读取温湿度和烟雾浓度数据，并且可以使用相应app控制
具体效果演示已发至bilibili：https://www.bilibili.com/video/BV1aRKHz2EMi/?share_source=copy_web&vd_source=f5ddef819f3f384f292c35111804982b
/**********************************************************************************************************
 * @author      DylanChan
 * @version     V1.0
 * @date        2025-06-24
 * @brief       智能家居的实现以及本地菜单的控制
 * @details     本次课设题目为智能家居，设计采取本地控制和巴法云平台控制两种方式；
 *				-设计使用LCD屏幕与按键进行人机交互，方便控制以及排查bug；
 * 				-其中本地控制部分设计刷卡开门、开关灯、窗帘开关以及温湿度、烟雾、火灾传感器检测部分；
 *				-巴法云平台设计远程开关门、窗帘、灯光，以及远程读取温湿度和烟雾浓度数据，并且可以使用相应app控制
 * 				使用的设备：
 *				-STM32F103VET6主控；
 *				-SG90舵机（或其他舵机）模拟门锁；
 *				-TT马达电机（模拟窗帘）；
 *				-RFID-RC522无线射频模块用于识别卡序列号和刷卡；
 *				-ESP01SWIFI模块；
 *				-MQ2烟雾传感器；
 *				-三线火焰传感器；
 *				-LCD显示屏ST7789；
 *				-DHT11温湿度传感器；
 *				-按键若干，LED灯若干，蜂鸣器
 * @note	具体接线：
 *				-SG90舵机信号线——PC6；
 *				-TT马达电机A相——PC9；B相——PC7；
 *				-RFID-RC522模块：
 *        SDA(CS)---PD7
 *        SCK---PB3
 *	      MOSI--PB5
 *	      MISO--PB4
 *	      空
 *	      GND--GND
 *	      RST---PB6
 *	      3.3V-3.3V
 *				-ESP01S：ESP RXD——PA2；ESP TXD——PA3；
 *				-MQ2烟雾传感器AO——PB0；
 *				-三线火焰传感器DO——PC10；
 *				-ST7789：
 *        LCD_Back		PD15
 *        LCD_DC		PD14
 *        LCD_CS		PD13
 *        LCD_CLK		PD12
 *        LCD_data		PD11
 *        LCD_Rest		PD10
 *				-DHT11温湿度传感器：DATA——PD2；NC——悬空
 *				-KEY1——PC2；KEY2——PC3；KEY3——PA0；KEY4——PA1；
 *				-LED1——PB8；LED2——PB9；LED3——PE0；LED4——PED1；
 *				-蜂鸣器BEEP——PC8
 **********************************************************************************************************
 */
