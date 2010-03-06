#include "pluginLoader.h"
#include <cMzCommon.h>
using namespace cMzCommon;
#include <vector>
using std::vector;


#pragma comment(lib,"cMzCommon.lib")
#pragma comment(lib,"mzfc.lib")

typedef BOOL(*PFN_Plug_CreateObject)(void **) ;
typedef vector<PLUGIN> PLUGINS;

static PLUGINS m_arrPluginObj;

int GetPluginSize(){
	return m_arrPluginObj.size();
}
PLUGIN GetPlugin(int idx){
	return m_arrPluginObj.at(idx);
}


bool CreatePlugins(CMzString szPlug, LPCTSTR lpProcName){
	PLUGIN stPs;
	ZeroMemory(&stPs, sizeof(stPs));
	stPs.hIns = LoadLibrary(szPlug.C_Str());
	PFN_Plug_CreateObject pFunc = 
		(PFN_Plug_CreateObject)GetProcAddress(stPs.hIns, lpProcName);
	if (pFunc && pFunc((void **)&stPs.pObj)){
		C::newstrcpy(&stPs.pfName,szPlug.C_Str());
		m_arrPluginObj.push_back(stPs);
		return true;
	}
	FreeLibrary(stPs.hIns);
	return false;
}

void LoadPlugins(LPCTSTR lpSubFolder, LPCTSTR lpSuffix, LPCTSTR lpProcName,
				 void* obj, void (*callback)(void*,CPluginBase *)){
	if(lpSuffix == NULL ||		//必须带后缀
		lpProcName == NULL)		//必须有入口程序名称
		return;	

	wchar_t filename[MAX_PATH];
	File::GetCurrentPath(filename); // 获取应用程序路径
	CMzString strPath = filename; //设定当前目录下的子目录PLUGINS

	if(lpSubFolder != NULL){	//假如已经设定子目录
		strPath = strPath + lpSubFolder;
		strPath = strPath + L"\\";
	}
	CMzString strFindFile = strPath + L"*.";
	strFindFile = strFindFile + lpSuffix;
	// 搜寻子目录PLUGINS下的所有动态链接库
	WIN32_FIND_DATA wfd;
	HANDLE hf = FindFirstFile(strFindFile.C_Str(), &wfd); //寻找第一个
	if (hf != INVALID_HANDLE_VALUE)
	{
		// 如发现插件就将其插入到本应用程序
		if(CreatePlugins(strPath + wfd.cFileName,lpProcName) && callback){
			(*callback)(obj,m_arrPluginObj.back().pObj);
		}
		while (FindNextFile(hf, &wfd)) //继续寻找下一个
		{
			if(CreatePlugins(strPath + wfd.cFileName,lpProcName) && callback){
				(*callback)(obj,m_arrPluginObj.back().pObj);
			}
		}
		FindClose(hf); // 结束搜寻
	}
}


void UnLoadPlugins(bool bfree){
	for(int i = 0; i < m_arrPluginObj.size(); i++){
		PLUGIN plug = m_arrPluginObj.at(i);
		delete plug.pObj;
		if(bfree) FreeLibrary(plug.hIns);
	}
	m_arrPluginObj.clear();
}

bool UnloadPlugin(int idx){
	if(idx < 0 || idx >= m_arrPluginObj.size()) return false;
	PLUGIN plug = m_arrPluginObj.at(idx);
	delete plug.pObj;
	plug.pObj = NULL;
	plug.hIns = 0;
	//FreeLibrary(plug.hIns);
	return true;
}

bool ReloadPlugin(int idx, LPCTSTR lpProcName){
	if(idx < 0 || idx >= m_arrPluginObj.size()) return false;
	PLUGIN plug = m_arrPluginObj.at(idx);
	if(plug.pObj == NULL && plug.pfName != NULL){
		plug.hIns = LoadLibrary(plug.pfName);
		PFN_Plug_CreateObject pFunc = 
			(PFN_Plug_CreateObject)GetProcAddress(plug.hIns, lpProcName);
		if (pFunc) pFunc((void **)&plug.pObj);
		return true;
	}
	return false;
}