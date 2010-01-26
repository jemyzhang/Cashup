#ifndef _UI_PERSON_H
#define _UI_PERSON_H

#include <cMzCommonDll.h>
#include <ui_base.h>
#include <db-engine.h>

class PersonList;

class Ui_PersonsWnd : public Ui_BaseWnd {
    MZ_DECLARE_DYNAMIC(Ui_PersonsWnd);
public:
    Ui_PersonsWnd() {
		_mode = 0;
		_sel_idx = -1;
		_listpersonType = PRSN_ALL;
    }
    ~Ui_PersonsWnd();
public:
    void SetListPersonType(PERSON_TYPE t = PRSN_ALL){
		_listpersonType = t;
	}

    int GetSelectionID() {
        return _selection;
    }

	//1: selection mode
	void setMode(int m) { _mode = m; }
protected:
    UiButtonBar m_PersonTypeBar;
    PersonList *m_pList;

    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
    void SetupPersonTypeBar();
    virtual void DelayShow();
private:
	int _mode;
    int _selection;
	int _sel_idx;
    PERSON_TYPE _listpersonType;	//-1: all
};

#endif /*_UI_PERSON_H*/