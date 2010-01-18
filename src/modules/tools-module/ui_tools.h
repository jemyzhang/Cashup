#ifndef _UI_TOOLS_H
#define _UI_TOOLS_H

#include "..\..\baseui\ui_base.h"
#include <pluginloader.h>

// Main window derived from CMzWndEx

class Ui_ToolsWnd : public Ui_BaseWnd {
    MZ_DECLARE_DYNAMIC(Ui_ToolsWnd);
public:
	Ui_ToolsWnd();
	~Ui_ToolsWnd();
private:
	void ShowPlugins();
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC window messages handler
    LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
public:
	void AppendPlugin(CPluginBase *obj);
	static void static_callback(void* obj, CPluginBase *plg)
	{
		if(obj == NULL) return;
		return reinterpret_cast<Ui_ToolsWnd*>(obj)->AppendPlugin(plg);
	}
private:
    virtual void DelayShow() { ShowPlugins(); }
private:
	int plugin_cnt;
};


#endif /*_UI_TOOLS_H*/