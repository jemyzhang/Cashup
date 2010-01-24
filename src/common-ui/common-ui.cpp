// db-engine.cpp : 定义 DLL 的初始化例程。
//

#include "resource.h"
#include <cMzCommon.h>
using namespace cMzCommon;

#include "ui_calendar.h"
#include "ui_calculator.h"
#include "ui_accounts.h"
#include "ui_categories.h"

#ifdef _DEBUG
#pragma comment(lib,"cMzCommond.lib")
#pragma comment(lib,"mzfc.lib")
#else
#pragma comment(lib,"cMzCommon.lib")
#pragma comment(lib,"mzfc.lib")
#endif

HINSTANCE hHandle = 0;

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

bool MzCalendarDialog(int &y, int &m, int &d, HWND parent){
    bool bret = false;
    Ui_CalendarWnd *pdlg = new Ui_CalendarWnd;
    pdlg->SetDate(y,m,d);
    RECT rcWork = MzGetWorkArea();
    pdlg->EnableImageBackground(false);
    pdlg->SetSizeMode(false);
    pdlg->Create(rcWork.left + 40, rcWork.top + RECT_HEIGHT(rcWork)/4, RECT_WIDTH(rcWork)-80, RECT_HEIGHT(rcWork)*1/2,
        parent, 0, WS_POPUP);
    if(pdlg->DoModal() == ID_OK){
        pdlg->GetDate(y,m,d);
        bret = true;
    }
    delete pdlg;
    return bret;
}
bool MzCalculatorDialog(double &val, HWND parent){
    bool bret = false;
    Ui_CalculatorWnd *pdlg = new Ui_CalculatorWnd;
    pdlg->EnableImageBackground(false);
    pdlg->SetSizeMode(false);
    RECT rcWork = MzGetWorkArea();
    pdlg->Create(rcWork.left, rcWork.top + RECT_HEIGHT(rcWork)/4, RECT_WIDTH(rcWork), 320,
        parent, 0, WS_POPUP);
    pdlg->SetInitOprand(val);
    if(pdlg->DoModal() == ID_OK){
        val = pdlg->GetResult();
        bret = true;
    }
    delete pdlg;
    return bret;
}

bool MzAccountsDialog(int &id, int mode, HWND parent){
    bool bret = false;
    Ui_AccountsWnd *pdlg = new Ui_AccountsWnd;
    pdlg->EnableImageBackground(false);
    pdlg->SetMode(mode);
    RECT rcWork = MzGetWorkArea();
    pdlg->Create(rcWork.left, rcWork.top + RECT_HEIGHT(rcWork)/4, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork)*3/4,
        parent, 0, WS_POPUP);
    if(pdlg->DoModal() == ID_OK){
        id = pdlg->GetSelectionIndex();
        bret = true;
    }
    delete pdlg;
    return bret;
}

bool MzCategoriesDialog(int &id, int mode, HWND parent){
    bool bret = false;
    Ui_CategoriesWnd *pdlg = new Ui_CategoriesWnd;
    pdlg->EnableImageBackground(false);
    //pdlg->SetMode(mode);
    RECT rcWork = MzGetWorkArea();
    pdlg->Create(rcWork.left, rcWork.top + RECT_HEIGHT(rcWork)/4, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork)*3/4,
        parent, 0, WS_POPUP);
    if(pdlg->DoModal() == ID_OK){
        //id = pdlg->GetSelectionIndex();
        bret = true;
    }
    delete pdlg;
    return bret;
}