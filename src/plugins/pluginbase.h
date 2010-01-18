// pluginbase.h : plugins DLL ����ͷ�ļ�
//

#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>

typedef struct tagPluginVer{
	unsigned char major;
	unsigned char minor1;
	unsigned char minor2;
}PluginVersion_st;

#define PluginGetLngResString(nID)  MzLoadString(nID,instHandle)
#define PluginGetResImage(nID) ImagingHelper::GetImageObject(instHandle,nID)

class CPluginBase 
{
public:
	CPluginBase(){ }
	virtual LPWSTR GetPluginName() = 0;		//������ƣ�����10���֣�
	virtual LPWSTR GetPluginIntro() = 0;	//�����飨����20���֣�
	virtual LPWSTR GetPluginAuthor() = 0;	//�������
	virtual PluginVersion_st GetPluginVersion() = 0; //����汾��
	virtual ImagingHelper* GetIconImage() = 0;	//���ͼ�� (80*80)


	virtual void Show(HWND m_hWnd = 0) = 0;
	virtual void Interface(int k) = 0;
	virtual void Release() = 0;
};

