#include <cMzCommon.h>
using namespace cMzCommon;

#include "ui_navi.h"
#include "resource.h"
#include "commondef.h"

#define	MZ_IDC_TITLE			101

extern ImagingHelper *pimg[IDB_PNG_END - IDB_PNG_BEGIN + 1];
extern HINSTANCE LangresHandle;


MZ_IMPLEMENT_DYNAMIC(Ui_NaviWnd)

Ui_NaviWnd::Ui_NaviWnd(){
    pMainWnd = NULL;
}

Ui_NaviWnd::~Ui_NaviWnd(){
    if(pMainWnd) delete pMainWnd;
}

BOOL Ui_NaviWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
    int y = 0;
	m_Logo.SetPos(0,y,GetWidth(),SZ_LOGO_HEIGHT);
	m_Logo.setupImage(pimg[IDB_PNG_LOGO - IDB_PNG_BEGIN]);
	AddUiWin(&m_Logo);

	y += SZ_LOGO_HEIGHT - 50 - 5;
	m_Title.SetPos(87,y,306,50);
	m_Title.SetImage_Pressed(pimg[IDB_PNG_TITLE00 - IDB_PNG_BEGIN]);
	m_Title.SetImage_Normal(pimg[IDB_PNG_TITLE01 - IDB_PNG_BEGIN]);
    //m_Title.EnableButtonHighlight(false);
	m_Title.SetTextColor(RGB(255,255,255));
	m_Title.SetTextSize(32);
	m_Title.SetID(MZ_IDC_TITLE);
	AddUiWin(&m_Title);

    ::SetTimer(m_hWnd,0x8001,10,NULL);
    return TRUE;
}

void Ui_NaviWnd::OnTimer(UINT_PTR nIDEvent){
	if(nIDEvent == 0x8001){
		::KillTimer(m_hWnd,0x8001);
		ShowMainWindow();
            dbg_printf("navi: %04x\n",m_hWnd);

	}
}

LRESULT Ui_NaviWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message){
		case MZ_WM_WND_ACTIVATE:
		{
			if(LOWORD(wParam)!=WA_INACTIVE){
				if(topWnd.size() != 0 && ::GetForegroundWindow() != topWnd.back()){
                    ::PostMessage(topWnd.back(),MZ_MW_REQ_CHILDWND_SHOW,0,0);
				}
			}
			break;
		}
		case MZ_MW_REQ_CHANGE_TITLE:
		{
			SetNaviTitle(MzLoadString(wParam,reinterpret_cast<HANDLE>(lParam)).C_Str());
			break;
		}
		case MZ_MW_REQ_CHANGE_TOPWND:
		{
			topWnd.push_back(reinterpret_cast<HWND>(wParam));
            dbg_printf("pushback: %04x\n",wParam);
			break;
		}
        case MZ_MW_ACK_WND_ISQUIT:
        {
            dbg_printf("pop_back: %04x\n",topWnd.back());
    		topWnd.pop_back();
            break;
        }
	}
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

void Ui_NaviWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
	if(id == MZ_IDC_TITLE){
		if(topWnd.size() < 1) return;
		::PostMessage(topWnd.back(),MZ_MW_REQ_CHILDWND_QUIT,0,0);
	}
}

BOOL Ui_NaviWnd::Create(int xPos, int yPos, int width, int height, HWND hwndParent, int uID, DWORD style, DWORD exstyle){
    RECT rcWork = MzGetWorkArea();
    return CMzWndEx::Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork),129,
        hwndParent,uID,style,exstyle);
}

void Ui_NaviWnd::ShowMainWindow(){
		RECT rcWork = MzGetWorkArea();
        if(pMainWnd == NULL){
            pMainWnd = new Ui_MainWnd;
            pMainWnd->Create(rcWork.left, rcWork.top + SZ_LOGO_HEIGHT, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork) - SZ_LOGO_HEIGHT,
                m_hWnd, 0, WS_POPUP);
        }
		// set the animation of the window
		pMainWnd->SetAnimateType_Show(MZ_ANIMTYPE_NONE);
		pMainWnd->SetAnimateType_Hide(MZ_ANIMTYPE_NONE);
        pMainWnd->Show();
}

void Ui_NaviWnd::SetNaviTitle(LPCTSTR pszTitle){
    m_Title.SetText(pszTitle);
    m_Title.Invalidate();
    m_Title.Update();
}
