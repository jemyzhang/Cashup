#include <cMzCommon.h>
using namespace cMzCommon;

#include "ui_main.h"
#include "resource.h"
#include <UiIconButton.h>
#include "commondef.h"

#define	MZ_IDC_TOOLBAR_MAIN			101
#define	MZ_IDC_SCROLLWIN			102
#define	MZ_IDC_TITLE			103


#ifdef _DEBUG
#define COMPILEM	L"D"
#else
#define COMPILEM	L"R"
#endif

#ifdef MZFC_STATIC
#define COMPILEL	L"s"
#else
#define COMPILEL	L"d"
#endif

#define VER_STRING L"3.00"COMPILEL
#define BUILD_STRING	L"20100118" COMPILEM

extern ImagingHelper *pimg[IDB_PNG_END - IDB_PNG_BEGIN + 1];
extern HINSTANCE LangresHandle;

class UiModuleItem : public UiIconButton{
public:
	UiModuleItem(CModuleBase* obj, int nID) : UiIconButton() {
		this->SetText(obj->GetModuleName());
		this->SetImage(obj->GetIconImage());
		this->SetID(nID);
	}
};


MZ_IMPLEMENT_DYNAMIC(Ui_MainWnd)

Ui_MainWnd::Ui_MainWnd(){
	module_cnt = 0;
}

Ui_MainWnd::~Ui_MainWnd(){
	::UnLoadModules(true);
}

BOOL Ui_MainWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!Ui_BaseWnd::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
	int y = GetHeight() - 120;
	m_TextAbout.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX*2);
	m_TextAbout.SetEnable(false);
	m_TextAbout.SetTextColor(RGB(255-64,255-64,255-64));
	m_TextAbout.SetDrawTextFormat(DT_RIGHT);
	m_TextAbout.SetTextSize(20);
	CMzString sAbout;
	wchar_t sa[256];
	wsprintf(sa,LOADSTRING(IDS_STR_APPAUTHOR).C_Str(),L"JEMYZHANG");
	sAbout = sa;
	sAbout = sAbout + L"\n";
	wsprintf(sa,LOADSTRING(IDS_STR_APPVERSION).C_Str(),VER_STRING,BUILD_STRING);
	sAbout = sAbout + sa;
	sAbout = sAbout + L"\n";
	sAbout = sAbout + L"Email: jemyzhang@163.com\n";
	wsprintf(sa,LOADSTRING(IDS_STR_COPYRIGHT).C_Str(),2009,L"JEMYZHANG");
	sAbout = sAbout + sa;
	m_TextAbout.SetText(sAbout.C_Str());
	AddUiWin(&m_TextAbout);

    return TRUE;
}

void Ui_MainWnd::DelayShow(){
	ShowModules();
}

void Ui_MainWnd::AppendModule(CModuleBase *obj){
	UiModuleItem *item = new UiModuleItem(obj,IDC_MODULE_BEGIN + module_cnt);
	item->SetPos(30 + 110 * (module_cnt%4), 10 + 140 * (module_cnt / 4), 90, 90+25);
	m_ScrollWin.AddChild(item);
	item->Invalidate();
	item->Update();
	module_cnt ++;
}

void Ui_MainWnd::ShowModules(){
    PostMessageW(MZ_MW_CHANGE_TITLE,IDS_MODULE_LOADING,(LPARAM)(MzGetInstanceHandle()));
	DateTime::waitms(1);
	::LoadModulesWithCallback(reinterpret_cast<void*>(this),&Ui_MainWnd::static_callback);
    PostMessageW(MZ_MW_CHANGE_TITLE,IDS_TTL_MAIN,(LPARAM)(MzGetInstanceHandle()));
	DateTime::waitms(1);
}

LRESULT Ui_MainWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    if(message == MZ_MW_CHANGE_TITLE || message == MZ_MW_CHANGE_TOPWND) {
        ::PostMessageW(GetParent(),message,wParam,lParam);  //收到子窗体发来的消息后，继续往导航窗体发送
        return 1;
    }
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

void Ui_MainWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
	if(id >= IDC_MODULE_BEGIN && id <= IDC_MODULE_BEGIN + 16){
		MODULE_ST module = m_arrModuleObj.at(id - IDC_MODULE_BEGIN);
		//m_Title.SetText(module.pObj->GetModuleName());
		module.pObj->Show(m_hWnd);
        PostMessageW(MZ_MW_CHANGE_TITLE,IDS_TTL_MAIN,(LPARAM)MzGetInstanceHandle());
	}
}
