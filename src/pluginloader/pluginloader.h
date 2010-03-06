//包含MZFC库的头文件
#include <pluginbase.h>

typedef struct tag_Plugins{
	CPluginBase * pObj;
	HINSTANCE hIns;
	LPWSTR pfName;
}PLUGIN, * LPPLUGIN;

void LoadPlugins(LPCTSTR lpSubFolder, LPCTSTR lpSuffix, LPCTSTR lpProcName,
				 void* obj = NULL, void (*callback)(void*,CPluginBase *) = NULL	//Callback
				 );
void UnLoadPlugins(bool bfree = false);
bool UnloadPlugin(int idx);
bool ReloadPlugin(int idx, LPCTSTR lpProcName);

int GetPluginSize();
PLUGIN GetPlugin(int idx);
