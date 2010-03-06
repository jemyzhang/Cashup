#ifndef _UI_MAIN_H
#define _UI_MAIN_H

#include <ui_base.h>
#include <pluginloader.h>


// Main window derived from CMzWndEx

class Ui_MainWnd : public Ui_BaseWnd {
    MZ_DECLARE_DYNAMIC(Ui_MainWnd);
public:
	Ui_MainWnd();
	~Ui_MainWnd();
private:
#ifdef USE_903SDK
	UiMultiLineEdit m_TextAbout;
#else
	UiEdit m_TextAbout;
#endif
private:
	void ShowModules();
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC window messages handler
    LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
private:
	virtual void DelayShow();
public:
	void AppendModule(CPluginBase *obj);
	static void static_callback(void* obj, CPluginBase *mol)
	{
		if(obj == NULL) return;
		return reinterpret_cast<Ui_MainWnd*>(obj)->AppendModule(mol);
	}
private:
	int module_cnt;
};


#endif /*_UI_MAIN_H*/