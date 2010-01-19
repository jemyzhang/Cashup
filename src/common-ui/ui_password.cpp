#include "ui_password.h"
#include <MzCommon.h>
using namespace MzCommon;

#include "resource.h"

class UiControlBackground :
	public UiWin
{
public:
	virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
		UiWin::PaintWin(hdcDst, prcWin, prcUpdate);
		MzDrawGridPopupMenuBG_480(hdcDst,prcWin);
	}
};



MZ_IMPLEMENT_DYNAMIC(Ui_PasswordWnd)

#define MZ_IDC_TOOLBAR_CALENDAR 101
#define MZ_IDC_Edit_YEAR 102
#define MZ_IDC_Edit_MONTH 103
#define MZ_IDC_Edit_DAY 104

extern CMzString getLngResString(int strid);

Ui_PasswordWnd::Ui_PasswordWnd(void)
{
	_uiMode = 0;
}

Ui_PasswordWnd::~Ui_PasswordWnd(void)
{
    delete m_pBackground;
}

void Ui_PasswordWnd::getPassword(wchar_t** p,int* plen){
	if(this->m_EdtPassword.GetText().IsEmpty()){
		*p = 0;
		*plen = 0;
        return;
	}
    C::newstrcpy(p,this->m_EdtPassword.GetText().C_Str());
	*plen = m_EdtPassword.GetText().Length();
}

BOOL Ui_PasswordWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
    int y = 10;
    m_pBackground = new UiControlBackground;
	m_pBackground->SetPos(0, 0, GetWidth(), GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR);
	AddUiWin(m_pBackground);

	m_EdtPassword.SetPos(5, y, GetWidth() - 10, MZM_HEIGHT_SINGLELINE_EDIT);
    m_EdtPassword.SetMaxChars(32);
    m_EdtPassword.SetSipMode(IM_SIP_MODE_GEL_LETTER, MZM_HEIGHT_TEXT_TOOLBAR);
	if(!_uiMode){
		m_EdtPassword.EnablePassWord();
	}
	if(_uiMode){
	    m_EdtPassword.SetTip(getLngResString(IDS_STR_PWD_TIP).C_Str());
	}else{
	    m_EdtPassword.SetTip(getLngResString(IDS_STR_PWD_LOCKED).C_Str());
	}
    m_pBackground->AddChild(&m_EdtPassword);

    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
    m_Toolbar.SetButton(0, true, true, getLngResString(IDS_STR_CANCEL).C_Str());
    m_Toolbar.SetButton(2, true, true, getLngResString(IDS_STR_OK).C_Str());
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_CALENDAR);
    AddUiWin(&m_Toolbar);

	m_EdtPassword.SetFocus(true);
    return TRUE;
}

void Ui_PasswordWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
        case MZ_IDC_TOOLBAR_CALENDAR:
        {
            int nIndex = lParam;
			if(nIndex == 0){	//取消
				if(_uiMode){
					EndModal(ID_CANCEL);
				}else{
					m_EdtPassword.SetFocus(false);
					EndModal(ID_CANCEL);
				}
				return;
			}
			if(nIndex == 2){	//确定
				EndModal(ID_OK);
				return;
			}
		}
	}
}

