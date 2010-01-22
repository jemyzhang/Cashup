#include "cashup.h"

#include <MzCommon.h>
using namespace MzCommon;

#include "resource.h"

#ifdef _DEBUG
#pragma comment(lib,"mzfc.lib")
#pragma comment(lib,"MzCommon.lib")
#pragma comment(lib,"moduleloaderd.lib")
#pragma comment(lib,"common-uid.lib")
#else
#pragma comment(lib,"mzfc.lib")
#pragma comment(lib,"MzCommon.lib")
#pragma comment(lib,"moduleloader.lib")
#pragma comment(lib,"common-ui.lib")
#endif

// The global variable of the application.
CashupApp theApp;

ImagingHelper *pimg[IDB_PNG_END - IDB_PNG_BEGIN + 1];
ImagingHelper *imgArrow;
HINSTANCE LangresHandle;
HINSTANCE ImgresHandle;

void CashupApp::loadImageRes(){
	ImgresHandle = MzGetInstanceHandle();
	for(int i = 0; i < sizeof(pimg) / sizeof(pimg[0]); i++){
		pimg[i] = LOADIMAGE(IDB_PNG_BEGIN + i);
	}
	HINSTANCE MzresHandle = GetMzResModuleHandle();
	imgArrow = ImagingHelper::GetImageObject(MzresHandle, MZRES_IDR_PNG_ARROW_RIGHT, true);
}

void CashupApp::loadLanguageRes(){
	LangresHandle = LoadLibrary(L"language.dll");
	if(LangresHandle){
		isExternLangres = true;
	}else{
		LangresHandle = MzGetInstanceHandle();
		isExternLangres = false;
	}
}

int CashupApp::Done(){
	FreeMzResModule();
	if(isExternLangres) FreeLibrary(LangresHandle);
	if(m_pWnd) delete m_pWnd;
	return CMzApp::Done();
}

BOOL CashupApp::Init() {
	// Init the COM relative library.
	CoInitializeEx(0, COINIT_MULTITHREADED);
	m_pWnd = NULL;
	//载入资源
	loadLanguageRes();

	//正常启动程序
	//检测程序是否已经运行
	m_pWnd = new Ui_NaviWnd;
	HANDLE m_hCHDle = CreateMutex(NULL,true,L"M8Cashup");
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		HWND pWnd=FindWindow(m_pWnd->GetMzClassName(),NULL);
		//HWND pWnd=FindWindow(NULL,L"M8Cash");
		if(pWnd)
		{
			SetForegroundWindow(pWnd);
			PostMessage(pWnd,WM_NULL,NULL,NULL);
		}
		PostQuitMessage(0);
		return true; 
	}
	//载入图片
	loadImageRes();
	//SetForegroundWindow(m_pMainWnd->m_hWnd);
	//检测密码
	// Create the main window
	RECT rcWork = MzGetWorkArea();
	m_pWnd->Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork), 0, 0, 0);

	m_pWnd->Show();
	SetForegroundWindow(m_pWnd->m_hWnd);

	// return TRUE means init success.
	return TRUE;
}