#ifndef _UI_NAVI_H
#define _UI_NAVI_H

// include the MZFC library header file
#include <mzfc_inc.h>
#include <UiImage.h>
#include "ui_main.h"

// Main window derived from CMzWndEx

class Ui_NaviWnd : public CMzWndEx {
    MZ_DECLARE_DYNAMIC(Ui_NaviWnd);
public:
	Ui_NaviWnd();
	~Ui_NaviWnd();
    virtual BOOL Create(int xPos, int yPos, int width, int height, HWND hwndParent=NULL, int uID = NULL, DWORD style=WS_CHILD, DWORD exstyle=0);
private:
	UiImage	m_Logo;
	UiButton_Image m_Title;
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();
	virtual void OnTimer(UINT_PTR nIDEvent);

    // override the MZFC window messages handler
    LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
private:
	vector<HWND> topWnd;	//正在显示的Wnd
private:
    Ui_MainWnd *pMainWnd;
    void ShowMainWindow();
public:
    void SetNaviTitle(LPCTSTR pszTitle);
};


#endif /*_UI_NAVI_H*/