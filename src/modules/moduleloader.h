//包含MZFC库的头文件
#include <modulebase.h>
#include <vector>
using std::vector;

typedef struct tag_Modules{
	CModuleBase * pObj;
	HINSTANCE hIns;
	LPWSTR pfName;
}MODULE_ST, * LPMODULE_ST;

typedef BOOL(*PFN_Module_CreateObject)(void **) ;

extern vector<MODULE_ST> m_arrModuleObj;

void LoadModules();
void LoadModulesWithCallback(void* obj, void (*callback)(void*, CModuleBase *));
void UnLoadModules(bool bfree = false);
bool UnloadModule(int idx);
bool ReloadModule(int idx);