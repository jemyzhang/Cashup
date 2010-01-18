//包含MZFC库的头文件
#include <pluginbase.h>
#include <vector>
using std::vector;

typedef struct tag_Plugins{
	CPluginBase * pObj;
	HINSTANCE hIns;
	LPWSTR pfName;
}PLUGIN_ST, * LPPLUGIN_ST;

typedef BOOL(*PFN_Plug_CreateObject)(void **) ;

extern vector<PLUGIN_ST> m_arrPluginObj;

void LoadPlugins();
void UnLoadPlugins(bool bfree = false);
bool UnloadPlugin(int idx);
bool ReloadPlugin(int idx);
void LoadPluginsWithCallback(void* obj, void (*callback)(void*,CPluginBase *));