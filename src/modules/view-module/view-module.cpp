// Modules.cpp : 定义 DLL 的初始化例程。
//

#include "view-module.h"
#include "ui_view.h"

#include <cMzCommon.h>
using namespace cMzCommon;
#include <commondef.h>

#pragma comment(lib,"mzfc.lib")
#pragma comment(lib,"cMzCommon.lib")
#pragma comment(lib,"common-ui.lib")

Ui_ViewWnd *pdlg = 0;
LPWSTR name = 0;

HINSTANCE instHandle = 0;
BOOL APIENTRY DllMain( HANDLE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved){
      instHandle = (HINSTANCE)hModule;
      return true;
}

BOOL WINAPI modules_createOjbect(void **pobj){
	*pobj = new ViewModule;
	return *pobj != NULL;
}

ImagingHelper* ViewModule::GetIconImage() {
	return PluginGetResImage(IDB_PNG_ICON); 
}

LPCTSTR ViewModule::GetName() {
	if(name == 0){
		C::newstrcpy(&name,PluginGetLngResString(IDS_MODULE_NAME).C_Str());
	}
    return name;
}

LPCTSTR ViewModule::GetIntro() {
	return L"View Transactions";
}

LPCTSTR ViewModule::GetAuthor() {
	return L"JEMYZHANG";
}


void ViewModule::Show(HWND m_hWnd) {
	if(pdlg == NULL){
		pdlg = new Ui_ViewWnd;
		RECT rcWork = MzGetWorkArea();
		pdlg->Create(rcWork.left, rcWork.top + 129, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork) - 129,
			m_hWnd, 0, WS_POPUP);
		// set the animation of the window
		pdlg->SetAnimateType_Show(MZ_ANIMTYPE_NONE);
		pdlg->SetAnimateType_Hide(MZ_ANIMTYPE_NONE);

		pdlg->DoModal();
		delete pdlg;
		pdlg = NULL;
	}
}

PluginVersion_st ViewModule::GetVersion(){
	PluginVersion_st ver = { 1, 0, 0 };
	return ver;
}
