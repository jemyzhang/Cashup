// Modules.h : Modules DLL 的主头文件
//

#pragma once

// include the MZFC library header file
#include <modulebase.h>
#include "resource.h"

class SearchModule : public CModuleBase
{
public:
	virtual LPCTSTR GetModuleName();
	virtual LPWSTR GetModuleIntro();
	virtual LPWSTR GetModuleAuthor();
	virtual ImagingHelper* GetIconImage();
	virtual ModuleVersion_st GetModuleVersion();

	virtual void Show(HWND m_hWnd = 0);
	virtual void Interface(int k) {}
	virtual void Release() {}
};