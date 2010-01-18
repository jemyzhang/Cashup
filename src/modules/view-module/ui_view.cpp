#include <MzCommon.h>
using namespace MzCommon;

#include "ui_view.h"
#include "resource.h"
#include <commondef.h>

extern HINSTANCE instHandle;

MZ_IMPLEMENT_DYNAMIC(Ui_ViewWnd)

Ui_ViewWnd::Ui_ViewWnd(){
}

Ui_ViewWnd::~Ui_ViewWnd(){
}

BOOL Ui_ViewWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!Ui_BaseWnd::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window

    return TRUE;
}

void Ui_ViewWnd::DelayShow(){
    ::PostMessage(GetParent(),MZ_MW_CHANGE_TITLE,IDS_MODULE_NAME,(LPARAM)instHandle);
	DateTime::waitms(1);
}

LRESULT Ui_ViewWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    if(message == MZ_MW_CHANGE_TITLE || message == MZ_MW_CHANGE_TOPWND) {
        ::PostMessageW(GetParent(),message,wParam,lParam);  //收到子窗体发来的消息后，继续往导航窗体发送
        return 1;
    }
    return Ui_BaseWnd::MzDefWndProc(message, wParam, lParam);
}

void Ui_ViewWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
}

