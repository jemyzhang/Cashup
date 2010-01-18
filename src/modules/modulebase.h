// modulebase.h : plugins DLL 的主头文件
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
	virtual LPCTSTR GetModuleName() = 0;		//模块名称（少于10文字）
	virtual LPWSTR GetModuleIntro() = 0;	//模块简介（少于20文字）
	virtual LPWSTR GetModuleAuthor() = 0;	//模块作者
	virtual ModuleVersion_st GetModuleVersion() = 0; //模块版本号
	virtual ImagingHelper* GetIconImage() = 0;	//模块图表 (80*80)


	virtual void Show(HWND m_hWnd = 0) = 0;
	virtual void Interface(int k) = 0;
	virtual void Release() = 0;
};

