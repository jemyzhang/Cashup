// Modules.h : Modules DLL 的主头文件
//

#pragma once

// include the MZFC library header file
#include <pluginbase.h>
#include "resource.h"

class AppendModule : public CPluginBase
{
public:
	virtual LPCTSTR GetName();
	virtual LPCTSTR GetIntro();
	virtual LPCTSTR GetAuthor();
	virtual ImagingHelper* GetIconImage();
	virtual PluginVersion_st GetVersion();

	virtual void Show(HWND m_hWnd = 0);
	virtual void Interface(int k) {}
	virtual void Release() {}
};