#include "pluginLoader.h"
#include <cMzCommon.h>
using namespace cMzCommon;

vector<PLUGIN_ST> m_arrPluginObj;

bool CreatePlugins(CMzString szPlug){
	PLUGIN_ST stPs;
	ZeroMemory(&stPs, sizeof(stPs));
	stPs.hIns = LoadLibrary(szPlug.C_Str());
	PFN_Plug_CreateObject pFunc = (PFN_Plug_CreateObject)GetProcAddress(stPs.hIns, _T("plugins_createOjbect"));
	if (pFunc && pFunc((void **)&stPs.pObj)){
		C::newstrcpy(&stPs.pfName,szPlug.C_Str());
		m_arrPluginObj.push_back(stPs);
		return true;
	}
	return false;
}

void LoadPlugins(){
	wchar_t filename[MAX_PATH];
	File::GetCurrentPath(filename); // 获取应用程序路径
	CMzString strPath = filename; //设定当前目录下的子目录PLUGINS
	strPath = strPath + L"plugins\\";
	CMzString strFindFile = strPath + L"*.plg";
	// 搜寻子目录PLUGINS下的所有动态链接库
	WIN32_FIND_DATA wfd;
	HANDLE hf = FindFirstFile(strFindFile.C_Str(), &wfd); //寻找第一个
	if (hf != INVALID_HANDLE_VALUE)
	{
		// 如发现插件就将其插入到本应用程序
		CreatePlugins(strPath + wfd.cFileName); 
		while (FindNextFile(hf, &wfd)) //继续寻找下一个
			CreatePlugins(strPath + wfd.cFileName);
		FindClose(hf); // 结束搜寻
	}
}

void LoadPluginsWithCallback(void* obj, void (*callback)(void*,CPluginBase *)){
	wchar_t filename[MAX_PATH];
	File::GetCurrentPath(filename); // 获取应用程序路径
	CMzString strPath = filename; //设定当前目录下的子目录PLUGINS
	strPath = strPath + L"plugins\\";
	CMzString strFindFile = strPath + L"*.plg";
	// 搜寻子目录PLUGINS下的所有动态链接库
	WIN32_FIND_DATA wfd;
	HANDLE hf = FindFirstFile(strFindFile.C_Str(), &wfd); //寻找第一个
	if (hf != INVALID_HANDLE_VALUE)
	{
		// 如发现插件就将其插入到本应用程序
		if(CreatePlugins(strPath + wfd.cFileName) && callback){
			(*callback)(obj,m_arrPluginObj.back().pObj);
		}
		while (FindNextFile(hf, &wfd)) //继续寻找下一个
		{
			if(CreatePlugins(strPath + wfd.cFileName) && callback){
				(*callback)(obj,m_arrPluginObj.back().pObj);
			}
		}
		FindClose(hf); // 结束搜寻
	}
}

void UnLoadPlugins(bool bfree){
	for(int i = 0; i < m_arrPluginObj.size(); i++){
		PLUGIN_ST plug = m_arrPluginObj.at(i);
		delete plug.pObj;
		if(bfree) FreeLibrary(plug.hIns);
	}
	m_arrPluginObj.clear();
}

bool UnloadPlugin(int idx){
	if(idx < 0 || idx >= m_arrPluginObj.size()) return false;
	PLUGIN_ST plug = m_arrPluginObj.at(idx);
	delete plug.pObj;
	plug.pObj = NULL;
	plug.hIns = 0;
	//FreeLibrary(plug.hIns);
	return true;
}

bool ReloadPlugin(int idx){
	if(idx < 0 || idx >= m_arrPluginObj.size()) return false;
	PLUGIN_ST plug = m_arrPluginObj.at(idx);
	if(plug.pObj == NULL && plug.pfName != NULL){
		plug.hIns = LoadLibrary(plug.pfName);
		PFN_Plug_CreateObject pFunc = (PFN_Plug_CreateObject)GetProcAddress(plug.hIns, _T("plugins_createOjbect"));
		if (pFunc) pFunc((void **)&plug.pObj);
		return true;
	}
	return false;
}