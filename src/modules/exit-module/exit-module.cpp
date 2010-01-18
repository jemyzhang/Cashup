// Modules.cpp : 定义 DLL 的初始化例程。
//

#include "exit-module.h"

#include <MzCommon.h>
using namespace MzCommon;

#include <commondef.h>

#pragma comment(lib,"mzfc.lib")
#ifdef _DEBUG
#pragma comment(lib,"MzCommond.lib")
#else
#pragma comment(lib,"MzCommon.lib")
#endif

LPWSTR name = 0;

HINSTANCE instHandle = 0;
BOOL APIENTRY DllMain( HANDLE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved){
      instHandle = (HINSTANCE)hModule;
      return true;
}

BOOL WINAPI modules_createOjbect(void **pobj){
	*pobj = new ExitModule;
	return *pobj != NULL;
}


ExitModule::ExitModule(){
	MzGetApp()->EnableNeverQuit(true);	//never quit while press Mkey
}

ImagingHelper* ExitModule::GetIconImage() {
	return ModuleGetResImage(IDB_PNG_ICON); 
}

LPCTSTR ExitModule::GetModuleName() {
	if(name == 0){
		C::newstrcpy(&name,ModuleGetLngResString(IDS_MODULE_NAME).C_Str());
	}
    return name;
}

LPWSTR ExitModule::GetModuleIntro() {
	return L"Exit Application";
}

LPWSTR ExitModule::GetModuleAuthor() {
	return L"JEMYZHANG";
}


void ExitModule::Show(HWND m_hWnd) {
    //显示导航栏名称
    ::PostMessageW(m_hWnd,MZ_MW_CHANGE_TITLE,IDS_MODULE_NAME,(LPARAM)instHandle);
    DateTime::waitms(1);

	if(MzMessageBoxEx(m_hWnd,L"Exit Now?",L"Warning",MZ_YESNO) != 1){
		return;
	}else{
		MzGetApp()->EnableNeverQuit(false);
		::PostQuitMessage(0);
	}
}

ModuleVersion_st ExitModule::GetModuleVersion(){
	ModuleVersion_st ver = { 1, 0, 0 };
	return ver;
}
