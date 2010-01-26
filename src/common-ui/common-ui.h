// db-engine.h : db-engine DLL 的主头文件
//

#pragma once
#include <cMzCommonDll.h>

//显示日历
//y,m,d: in&out 年月日初始值&返回值
//retval: true: ID_OK false: ID_CANCEL
bool MzCalendarDialog(int &y, int &m, int &d, HWND parent = 0);

//显示计算器
//val: in&out 初始值&返回值
//retval: true: ID_OK false: ID_CANCEL
bool MzCalculatorDialog(double &val, HWND parent = 0);

//显示账户列表
//mode: 0:普通模式 1:单选择模式（不可编辑删除新增）
//retval: true: ID_OK false: ID_CANCEL
bool MzAccountsDialog(int &id, int mode = 0, HWND parent = 0);

//显示分类列表
//mode: 0:普通模式 1:单选择模式（不可编辑删除新增）
//retval: true: ID_OK false: ID_CANCEL
bool MzCategoriesDialog(int &id, int mode = 0, HWND parent = 0);

//显示人员列表
//mode: 0:普通模式 1:单选择模式（不可编辑删除新增）
//retval: true: ID_OK false: ID_CANCEL
bool MzPersonDialog(int &id, int mode = 0, HWND parent = 0);
