// db-engine.h : db-engine DLL 的主头文件
//

#pragma once
#include <MzCommonDll.h>

//显示密码框
//mode: 0,输入密码 1,设置密码
//retval: true: ID_OK false: ID_CANCEL
bool MzPasswordDialog(LPWSTR *psz, int *plen, HWND parent = 0, int mode = 0);

//显示日历
//y,m,d: in&out 年月日初始值&返回值
//retval: true: ID_OK false: ID_CANCEL
bool MzCalendarDialog(int &y = 0, int &m = 0, int &d = 0);
//显示计算器
//val: in&out 初始值&返回值
//retval: true: ID_OK false: ID_CANCEL
bool MzCalculatorDialog(double &val = 0, HWND parent = 0);
