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
	if(lpSuffix == NULL ||		//�������׺
		lpProcName == NULL)		//��������ڳ�������
		return;	

	wchar_t filename[MAX_PATH];
	File::GetCurrentPath(filename); // ��ȡӦ�ó���·��
	CMzString strPath = filename; //�趨��ǰĿ¼�µ���Ŀ¼PLUGINS

	if(lpSubFolder != NULL){	//�����Ѿ��趨��Ŀ¼
		strPath = strPath + lpSubFolder;
		strPath = strPath + L"\\";
	}
	CMzString strFindFile = strPath + L"*.";
	strFindFile = strFindFile + lpSuffix;
	// ��Ѱ��Ŀ¼PLUGINS�µ����ж�̬���ӿ�
	WIN32_FIND_DATA wfd;
	HANDLE hf = FindFirstFile(strFindFile.C_Str(), &wfd); //Ѱ�ҵ�һ��
	if (hf != INVALID_HANDLE_VALUE)
	{
		// �緢�ֲ���ͽ�����뵽��Ӧ�ó���
		if(CreatePlugins(strPath + wfd.cFileName,lpProcName) && callback){
			(*callback)(obj,m_arrPluginObj.back().pObj);
		}
		while (FindNextFile(hf, &wfd)) //����Ѱ����һ��
		{
			if(CreatePlugins(strPath + wfd.cFileName,lpProcName) && callback){
				(*callback)(obj,m_arrPluginObj.back().pObj);
			}
		}
		FindClose(hf); // ������Ѱ
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