// plugins.cpp : 定义 DLL 的初始化例程。
//

#include "backup-plugin.h"
#include "ui_backup.h"
#include "resource.h"

#include <cMzCommon.h>
using namespace cMzCommon;

#pragma comment(lib,"mzfc.lib")
#ifdef _DEBUG
#pragma comment(lib,"common-ui.lib")
#pragma comment(lib,"cMzCommond.lib")
#else
#pragma comment(lib,"common-ui.lib")
#pragma comment(lib,"cMzCommon.lib")
#endif

LPWSTR name = 0;
HINSTANCE instHandle = 0;
BOOL APIENTRY DllMain( HANDLE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved){
      instHandle = (HINSTANCE)hModule;
      return true;
}

BOOL WINAPI plugins_createOjbect(void **pobj){
	*pobj = new BackupPlugin;
	return *pobj != NULL;
}

ImagingHelper* BackupPlugin::GetIconImage() {
	return PluginGetResImage(IDB_PNG_ICON); 
}

LPWSTR BackupPlugin::GetPluginName() {
	if(name == 0){
		C::newstrcpy(&name,PluginGetLngResString(IDS_MODULE_NAME).C_Str());
	}
    return name;
}

LPWSTR BackupPlugin::GetPluginIntro() {
	return L"Backup Database";
}

LPWSTR BackupPlugin::GetPluginAuthor() {
	return L"JEMYZHANG";
}


void BackupPlugin::Show(HWND m_hWnd) {
	Ui_BackupWnd dlg;
	RECT rcWork = MzGetWorkArea();
	dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
		m_hWnd, 0, WS_POPUP);
	// set the animation of the window
	dlg.SetAnimateType_Show(MZ_ANIMTYPE_NONE);
	dlg.SetAnimateType_Hide(MZ_ANIMTYPE_NONE);
	dlg.DoModal();
}

PluginVersion_st BackupPlugin::GetPluginVersion(){
	PluginVersion_st ver = { 1, 0, 0 };
	return ver;
}

