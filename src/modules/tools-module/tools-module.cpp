// Modules.cpp : 定义 DLL 的初始化例程。
//

#include "tools-module.h"
#include "ui_tools.h"

#include <cMzCommon.h>
using namespace cMzCommon;
#include <commondef.h>

#pragma comment(lib,"mzfc.lib")
#pragma comment(lib,"cMzCommon.lib")
#pragma comment(lib,"pluginloader.lib")
#pragma comment(lib,"common-ui.lib")

Ui_ToolsWnd *pdlg = 0;
LPWSTR name = 0;

HINSTANCE instHandle = 0;
BOOL APIENTRY DllMain( HANDLE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved){
      instHandle = (HINSTANCE)hModule;
      return true;
}

BOOL WINAPI modules_createOjbect(void **pobj){
	*pobj = new ToolsModule;
	return *pobj != NULL;
}

ImagingHelper* ToolsModule::GetIconImage() {
	return PluginGetResImage(IDB_PNG_ICON); 
}

LPCTSTR ToolsModule::GetName() {
	if(name == 0){
		C::newstrcpy(&name,PluginGetLngResString(IDS_MODULE_NAME).C_Str());
	}
    return name;
}

LPCTSTR ToolsModule::GetIntro() {
	return L"External Tools";
}

LPCTSTR ToolsModule::GetAuthor() {
	return L"JEMYZHANG";
}


void ToolsModule::Show(HWND m_hWnd) {
	if(pdlg == NULL){
		pdlg = new Ui_ToolsWnd;
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

PluginVersion_st ToolsModule::GetVersion(){
	PluginVersion_st ver = { 1, 0, 0 };
	return ver;
}
