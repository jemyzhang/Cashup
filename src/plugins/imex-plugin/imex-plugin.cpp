// plugins.cpp : 定义 DLL 的初始化例程。
//

#include "imex-plugin.h"
#include "ui_about.h"

#include <cMzCommon.h>
using namespace cMzCommon;
#include <afxwin.h>

#pragma comment(lib,"mzfc.lib")

BOOL WINAPI plugins_createOjbect(void **pobj){
	*pobj = new ImExPlugin;
	return *pobj != NULL;
}

ImagingHelper* ImExPlugin::GetIconImage() {
	HINSTANCE ImgresHandle = AfxGetInstanceHandle();	//AfxGetInstanceHandle
	return LOADIMAGE(IDB_PNG_ICON); 
}

LPWSTR ImExPlugin::GetPluginName() {
	return L"Import/Export";
}

LPWSTR ImExPlugin::GetPluginIntro() {
	return L"Import/Export Transaction Detail";
}

LPWSTR ImExPlugin::GetPluginAuthor() {
	return L"JEMYZHANG";
}

UI_AboutWnd *pdlg = 0;
void ImExPlugin::Show(HWND m_hWnd) {
	if(pdlg == NULL){
		pdlg = new UI_AboutWnd;
		RECT rcWork = MzGetWorkArea();
		pdlg->Create(rcWork.left, rcWork.top + 126, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork) - 126,
			m_hWnd, 0, WS_POPUP);
		// set the animation of the window
		pdlg->SetAnimateType_Show(MZ_ANIMTYPE_NONE);
		pdlg->SetAnimateType_Hide(MZ_ANIMTYPE_NONE);
		pdlg->DoModal();
		delete pdlg;
		pdlg = NULL;
	}

#if 0
	UI_AboutWnd dlg;
	RECT rcWork = MzGetWorkArea();
	dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
		m_hWnd, 0, WS_POPUP);
	// set the animation of the window
	dlg.SetAnimateType_Show(MZ_ANIMTYPE_NONE);
	dlg.SetAnimateType_Hide(MZ_ANIMTYPE_NONE);
	dlg.DoModal();
#endif
}

PluginVersion_st ImExPlugin::GetPluginVersion(){
	PluginVersion_st ver = { 1, 0, 0 };
	return ver;
}

