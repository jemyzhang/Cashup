#include "ui_about.h"

MZ_IMPLEMENT_DYNAMIC(UI_AboutWnd)

#define MZ_IDC_TOOLBAR_ABOUT 101
#define VER_STRING L"1.82"
#define BUILD_STRING L"20091203"
#define APPNAME L"掌上农历"


BOOL UI_AboutWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
    int y = 0;
    m_CaptionTitle.SetPos(0, 0, GetWidth(), MZM_HEIGHT_CAPTION);
	m_CaptionTitle.SetText(L"关于");
    AddUiWin(&m_CaptionTitle);

	y += MZM_HEIGHT_CAPTION*3;
    m_TextName.SetPos(0, y, GetWidth(), MZM_HEIGHT_CAPTION);
	m_TextName.SetTextSize(m_TextName.GetTextSize()*2);
	m_TextName.SetText(APPNAME);
    AddUiWin(&m_TextName);

	y += MZM_HEIGHT_CAPTION;
	wchar_t version[128];
	wsprintf(version,L"版本 %s Build.%s",VER_STRING,BUILD_STRING);
	m_TextVersion.SetPos(0, y, GetWidth(), MZM_HEIGHT_CAPTION);
	m_TextVersion.SetText(version);
    AddUiWin(&m_TextVersion);

	y += MZM_HEIGHT_CAPTION;
	m_TextAddtional.SetPos(0, y, GetWidth(), MZM_HEIGHT_CAPTION * 3);
	m_TextAddtional.SetText(
		L"               Email: jemy.zhang@gmail.com\n"
		L"            (C)2009 JEMYZHANG  保留所有权利");
	m_TextAddtional.SetReadOnly(true);
	m_TextAddtional.SetTextSize(20);
    AddUiWin(&m_TextAddtional);

	m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
    m_Toolbar.SetButton(0, true, true, L"返回");
    m_Toolbar.EnableLeftArrow(true);
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_ABOUT);
    AddUiWin(&m_Toolbar);

    return TRUE;
}

void UI_AboutWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
        case MZ_IDC_TOOLBAR_ABOUT:
        {
            int nIndex = lParam;
            if (nIndex == 0) {
                // exit the modal dialog
                EndModal(ID_OK);
                return;
            }
        }
    }
}