// plugins.h : plugins DLL 的主头文件
//

#pragma once

// include the MZFC library header file
#include <pluginbase.h>
#include "resource.h"

class ImExPlugin : public CPluginBase
{
public:
	virtual LPWSTR GetPluginName();
	virtual LPWSTR GetPluginIntro();
	virtual LPWSTR GetPluginAuthor();
	virtual ImagingHelper* GetIconImage();
	virtual PluginVersion_st GetPluginVersion();

	virtual void Show(HWND m_hWnd = 0);
	virtual void Interface(int k) {}
	virtual void Release() {}
};