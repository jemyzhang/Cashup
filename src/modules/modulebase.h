// modulebase.h : plugins DLL ����ͷ�ļ�
//

#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>

typedef struct tagModuleVer{
	unsigned char major;
	unsigned char minor1;
	unsigned char minor2;
}ModuleVersion_st;

#define ModuleGetLngResString(nID)  MzLoadString(nID,instHandle)
#define ModuleGetResImage(nID) ImagingHelper::GetImageObject(instHandle,nID)

class CModuleBase 
{
public:
	CModuleBase(){ }
	virtual LPCTSTR GetModuleName() = 0;		//ģ�����ƣ�����10���֣�
	virtual LPWSTR GetModuleIntro() = 0;	//ģ���飨����20���֣�
	virtual LPWSTR GetModuleAuthor() = 0;	//ģ������
	virtual ModuleVersion_st GetModuleVersion() = 0; //ģ��汾��
	virtual ImagingHelper* GetIconImage() = 0;	//ģ��ͼ�� (80*80)


	virtual void Show(HWND m_hWnd = 0) = 0;
	virtual void Interface(int k) = 0;
	virtual void Release() = 0;
};

