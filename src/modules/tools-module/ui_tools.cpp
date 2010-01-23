#include <cMzCommon.h>
using namespace cMzCommon;

#include "ui_tools.h"
#include "resource.h"
#include <UiIconButton.h>
#include <commondef.h>

#define	MZ_IDC_SCROLLWIN			102

extern HINSTANCE instHandle;

class UiPluginItem : public UiIconButton{
public:
	UiPluginItem(CPluginBase* obj, int nID) : UiIconButton() {
		this->SetText(obj->GetPluginName());
		this->SetImage(obj->GetIconImage());
		this->SetID(nID);
	}
};


MZ_IMPLEMENT_DYNAMIC(Ui_ToolsWnd)

Ui_ToolsWnd::Ui_ToolsWnd(){
	plugin_cnt = 0;
}

Ui_ToolsWnd::~Ui_ToolsWnd(){
	::UnLoadPlugins(true);
}

BOOL Ui_ToolsWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!Ui_BaseWnd::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window

    return TRUE;
}

void Ui_ToolsWnd::AppendPlugin(CPluginBase *obj){
	UiPluginItem *item = new UiPluginItem(obj,IDC_MODULE_BEGIN + plugin_cnt);
	item->SetPos(30 + 110 * (plugin_cnt%4), 10 + 140 * (plugin_cnt / 4), 90, 90+25);
	m_ScrollWin.AddChild(item);
	item->Invalidate();
	item->Update();
	plugin_cnt ++;
}

void Ui_ToolsWnd::ShowPlugins(){
        //显示导航栏名称
    ::PostMessage(GetParent(),MZ_MW_CHANGE_TITLE,IDS_LOADING,(LPARAM)instHandle);
	DateTime::waitms(1);
	::LoadPluginsWithCallback(reinterpret_cast<void*>(this),&Ui_ToolsWnd::static_callback);
    ::PostMessage(GetParent(),MZ_MW_CHANGE_TITLE,IDS_MODULE_NAME,(LPARAM)instHandle);
	DateTime::waitms(1);
}

LRESULT Ui_ToolsWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    if(message == MZ_MW_CHANGE_TITLE || message == MZ_MW_CHANGE_TOPWND) {
        ::PostMessageW(GetParent(),message,wParam,lParam);  //收到子窗体发来的消息后，继续往导航窗体发送
        return 1;
    }
    return Ui_BaseWnd::MzDefWndProc(message, wParam, lParam);
}

void Ui_ToolsWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
	if(id >= IDC_MODULE_BEGIN && id <= IDC_MODULE_BEGIN + 16){
		PLUGIN_ST plug = m_arrPluginObj.at(id - IDC_MODULE_BEGIN);
		plug.pObj->Show(m_hWnd);
        ::PostMessage(GetParent(),MZ_MW_CHANGE_TITLE,IDS_MODULE_NAME,(LPARAM)instHandle);
	}
}

