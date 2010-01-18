#include "ui_navi.h"

// Application class derived from CMzApp

class CashupApp : public CMzApp {
public:
    // The main window of the app.
//    Ui_MainWnd *m_pMainWnd;
    Ui_NaviWnd *m_pWnd;

	void loadImageRes();
	void loadLanguageRes();
    // Initialization of the application
    virtual BOOL Init();
	virtual int Done();
private:
	bool isExternLangres;
};

