// db-engine.cpp : 定义 DLL 的初始化例程。
//

#include "resource.h"
#include <MzCommon.h>
using namespace MzCommon;

#include "ui_password.h"
#include "ui_calendar.h"
#include "ui_password.h"
#include "ui_calculator.h"

#pragma comment(lib,"MzCommon.lib")
#pragma comment(lib,"mzfc.lib")

static HINSTANCE hHandle = 0;

BOOL APIENTRY DllMain( HANDLE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved){
      hHandle = (HINSTANCE)hModule;
      return true;
}

CMzString getLngResString(int nID){
	return MzLoadString(nID,hHandle);
}

ImagingHelper *getResImage(int nID){
	return ImagingHelper::GetImageObject(hHandle,nID);
}

bool MzPasswordDialog(LPWSTR *psz, int *plen, HWND parent, int mode){
    if(psz == 0 || plen == 0) return false;    //无法返回密码值，弹出框无效

    Ui_PasswordWnd *pdlg = new Ui_PasswordWnd;
    pdlg->setMode(mode);

    RECT rcWork = MzGetWorkArea();
    pdlg->Create(rcWork.left, rcWork.top + RECT_HEIGHT(rcWork)/4, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork)*3/4,
        parent, 0, WS_POPUP);
    // set the animation of the window
    pdlg->SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_BOTTOM_TO_TOP_2);
    pdlg->SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_TOP_TO_BOTTOM_1);
    if(pdlg->DoModal() == ID_OK){
        pdlg->getPassword(psz,plen);
        return true;
    }else{
        return false;
    }
}

bool MzCalendarDialog(int &y, int &m, int &d, HWND parent){
    Ui_CalendarWnd *pdlg = new Ui_CalendarWnd;
    pdlg->SetDate(y,m,d);
    RECT rcWork = MzGetWorkArea();
    pdlg->Create(rcWork.left, rcWork.top + RECT_HEIGHT(rcWork)/4, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork)*3/4,
        parent, 0, WS_POPUP);
    // set the animation of the window
    pdlg->SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_BOTTOM_TO_TOP_2);
    pdlg->SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_TOP_TO_BOTTOM_1);
    if(pdlg->DoModal() == ID_OK){
        pdlg->GetDate(y,m,d);
        return true;
    }else{
        return false;
    }
}
bool MzCalculatorDialog(double &val, HWND parent){
    Ui_CalculatorWnd *pdlg = new Ui_CalculatorWnd;
    pdlg->SetInitOprand(val);
    RECT rcWork = MzGetWorkArea();
    pdlg->Create(rcWork.left, rcWork.top + RECT_HEIGHT(rcWork)/4, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork)*3/4,
        parent, 0, WS_POPUP);
    // set the animation of the window
    pdlg->SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_BOTTOM_TO_TOP_2);
    pdlg->SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_TOP_TO_BOTTOM_1);
    if(pdlg->DoModal() == ID_OK){
        val = pdlg->GetResult();
        return true;
    }else{
        return false;
    }
}
