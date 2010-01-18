#include <MzCommon.h>
using namespace MzCommon;

#include "ui_search.h"
#include "resource.h"
#include <commondef.h>

extern HINSTANCE instHandle;

MZ_IMPLEMENT_DYNAMIC(Ui_SearchWnd)

Ui_SearchWnd::Ui_SearchWnd(){
}

Ui_SearchWnd::~Ui_SearchWnd(){
}

BOOL Ui_SearchWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!Ui_BaseWnd::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window

    return TRUE;
}

void Ui_SearchWnd::DelayShow(){
    ::PostMessage(GetParent(),MZ_MW_CHANGE_TITLE,IDS_MODULE_NAME,(LPARAM)instHandle);
	DateTime::waitms(1);
}

LRESULT Ui_SearchWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    if(message == MZ_MW_CHANGE_TITLE || message == MZ_MW_CHANGE_TOPWND) {
        ::PostMessageW(GetParent(),message,wParam,lParam);  //�յ��Ӵ��巢������Ϣ�󣬼������������巢��
        return 1;
    }
    return Ui_BaseWnd::MzDefWndProc(message, wParam, lParam);
}

void Ui_SearchWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
}
