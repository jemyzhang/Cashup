#ifndef _UI_TOOLS_H
#define _UI_TOOLS_H

#include <ui_base.h>
#include <pluginloader.h>

// Main window derived from CMzWndEx

class Ui_AppendWnd : public Ui_BaseWnd {
    MZ_DECLARE_DYNAMIC(Ui_AppendWnd);
public:
	Ui_AppendWnd();
	~Ui_AppendWnd();
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC window messages handler
    LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
private:
    virtual void DelayShow();
private:
	int plugin_cnt;
};


#endif /*_UI_TOOLS_H*/