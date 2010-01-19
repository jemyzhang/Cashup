// db-engine.h : db-engine DLL ����ͷ�ļ�
//

#pragma once
#include <MzCommonDll.h>

//��ʾ�����
//mode: 0,�������� 1,��������
//retval: true: ID_OK false: ID_CANCEL
bool MzPasswordDialog(LPWSTR *psz, int *plen, HWND parent = 0, int mode = 0);

//��ʾ����
//y,m,d: in&out �����ճ�ʼֵ&����ֵ
//retval: true: ID_OK false: ID_CANCEL
bool MzCalendarDialog(int &y = 0, int &m = 0, int &d = 0);
//��ʾ������
//val: in&out ��ʼֵ&����ֵ
//retval: true: ID_OK false: ID_CANCEL
bool MzCalculatorDialog(double &val = 0, HWND parent = 0);
