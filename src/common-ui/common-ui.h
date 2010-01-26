// db-engine.h : db-engine DLL ����ͷ�ļ�
//

#pragma once
#include <cMzCommonDll.h>

//��ʾ����
//y,m,d: in&out �����ճ�ʼֵ&����ֵ
//retval: true: ID_OK false: ID_CANCEL
bool MzCalendarDialog(int &y, int &m, int &d, HWND parent = 0);

//��ʾ������
//val: in&out ��ʼֵ&����ֵ
//retval: true: ID_OK false: ID_CANCEL
bool MzCalculatorDialog(double &val, HWND parent = 0);

//��ʾ�˻��б�
//mode: 0:��ͨģʽ 1:��ѡ��ģʽ�����ɱ༭ɾ��������
//retval: true: ID_OK false: ID_CANCEL
bool MzAccountsDialog(int &id, int mode = 0, HWND parent = 0);

//��ʾ�����б�
//mode: 0:��ͨģʽ 1:��ѡ��ģʽ�����ɱ༭ɾ��������
//retval: true: ID_OK false: ID_CANCEL
bool MzCategoriesDialog(int &id, int mode = 0, HWND parent = 0);

//��ʾ��Ա�б�
//mode: 0:��ͨģʽ 1:��ѡ��ģʽ�����ɱ༭ɾ��������
//retval: true: ID_OK false: ID_CANCEL
bool MzPersonDialog(int &id, int mode = 0, HWND parent = 0);
