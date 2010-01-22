#ifndef _UI_ACCOUNTS_H
#define _UI_ACCOUNTS_H

// include the MZFC library header file
#include <MzCommonDll.h>
#include <db-engine.h>
#include <ui_base.h>

class AccountList;

class COMMON_API Ui_AccountsWnd : public Ui_BaseWnd {
    MZ_DECLARE_DYNAMIC(Ui_AccountsWnd);
public:

    Ui_AccountsWnd();
    ~Ui_AccountsWnd();
public:
	//1: selection mode
	void SetMode(int m) { _mode = m; }
    int GetSelectionIndex() { return _selection; }
protected:
    AccountList *m_pList;

    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
	//update display list
private:
	void UpdateUi();
    virtual void DelayShow() { UpdateUi(); }
private:
	int _mode;
    int _selection;
	int *idarray;
	int _sel_idx;
};

#endif /*_UI_ACCOUNTS_H*/