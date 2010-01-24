#include <cMzCommon.h>
using namespace cMzCommon;

#include "ui_append.h"
#include "resource.h"
#include <UiIconButton.h>
#include <commondef.h>
#include <common-ui.h>

extern HINSTANCE instHandle;

MZ_IMPLEMENT_DYNAMIC(Ui_AppendWnd)

Ui_AppendWnd::Ui_AppendWnd(){
}

Ui_AppendWnd::~Ui_AppendWnd(){
}

BOOL Ui_AppendWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!Ui_BaseWnd::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window

    return TRUE;
}

void Ui_AppendWnd::DelayShow(){
    ::PostMessage(GetParent(),MZ_MW_CHANGE_TITLE,IDS_MODULE_NAME,(LPARAM)instHandle);
	DateTime::waitms(1);
    int id = 0;
    int y = 0,m = 0,d = 0;
    MzCategoriesDialog(id,0,m_hWnd);
    //::MzAccountsDialog(id,0,m_hWnd);
    //::MzCalendarDialog(y,m,d,m_hWnd);
    //double val = 0.0;
    //::MzCalculatorDialog(val,m_hWnd);
}

LRESULT Ui_AppendWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    if(message == MZ_MW_CHANGE_TITLE || message == MZ_MW_CHANGE_TOPWND) {
        ::PostMessageW(GetParent(),message,wParam,lParam);  //收到子窗体发来的消息后，继续往导航窗体发送
        return 1;
    }
    return Ui_BaseWnd::MzDefWndProc(message, wParam, lParam);
}

void Ui_AppendWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
}

