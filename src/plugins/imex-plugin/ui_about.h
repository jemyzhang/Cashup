#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>

// Popup window derived from CMzWndEx

class UI_AboutWnd : public CMzWndEx {
    MZ_DECLARE_DYNAMIC(UI_AboutWnd);
public:
protected:
    UiToolbar_Text m_Toolbar;
    UiCaption m_CaptionTitle;
	UiStatic m_TextName;
	UiStatic m_TextVersion;
#ifdef USE_903SDK
	UiMultiLineEdit m_TextAddtional;
#else
	UiEdit m_TextAddtional;
#endif
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
private:
};
