// pluginbase.h : plugins DLL 的主头文件
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
	virtual LPWSTR GetPluginName() = 0;		//插件名称（少于10文字）
	virtual LPWSTR GetPluginIntro() = 0;	//插件简介（少于20文字）
	virtual LPWSTR GetPluginAuthor() = 0;	//插件作者
	virtual PluginVersion_st GetPluginVersion() = 0; //插件版本号
	virtual ImagingHelper* GetIconImage() = 0;	//插件图表 (80*80)


	virtual void Show(HWND m_hWnd = 0) = 0;
	virtual void Interface(int k) = 0;
	virtual void Release() = 0;
};

