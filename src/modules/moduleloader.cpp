#include "moduleLoader.h"
#include <cMzCommon.h>
using namespace cMzCommon;

vector<MODULE_ST> m_arrModuleObj;

bool CreateModules(CMzString szModule){
	MODULE_ST stPs;
	ZeroMemory(&stPs, sizeof(stPs));
	stPs.hIns = LoadLibrary(szModule.C_Str());
	PFN_Module_CreateObject pFunc = (PFN_Module_CreateObject)GetProcAddress(stPs.hIns, _T("modules_createOjbect"));
	if (pFunc && pFunc((void **)&stPs.pObj)){
		C::newstrcpy(&stPs.pfName,szModule.C_Str());
		m_arrModuleObj.push_back(stPs);
		return true;
	}
	return false;
}

void LoadModules(){
	wchar_t filename[MAX_PATH];
	File::GetCurrentPath(filename); // ��ȡӦ�ó���·��
	CMzString strPath = filename; //�趨��ǰĿ¼�µ���Ŀ¼PLUGINS
	strPath = strPath + L"modules\\";
	CMzString strFindFile = strPath + L"*.mol";
	// ��Ѱ��Ŀ¼MODULES�µ����ж�̬���ӿ�
	WIN32_FIND_DATA wfd;
	HANDLE hf = FindFirstFile(strFindFile.C_Str(), &wfd); //Ѱ�ҵ�һ��
	if (hf != INVALID_HANDLE_VALUE)
	{
		// �緢�ֲ���ͽ�����뵽��Ӧ�ó���
		CreateModules(strPath + wfd.cFileName); 
		while (FindNextFile(hf, &wfd)) //����Ѱ����һ��
			CreateModules(strPath + wfd.cFileName);
		FindClose(hf); // ������Ѱ
	}
}

void LoadModulesWithCallback(void* obj, void (*callback)(void*,CModuleBase *)){
	wchar_t filename[MAX_PATH];
	File::GetCurrentPath(filename); // ��ȡӦ�ó���·��
	CMzString strPath = filename; //�趨��ǰĿ¼�µ���Ŀ¼PLUGINS
	strPath = strPath + L"modules\\";
	CMzString strFindFile = strPath + L"*.mol";
	// ��Ѱ��Ŀ¼MODULES�µ����ж�̬���ӿ�
	WIN32_FIND_DATA wfd;
	HANDLE hf = FindFirstFile(strFindFile.C_Str(), &wfd); //Ѱ�ҵ�һ��
	if (hf != INVALID_HANDLE_VALUE)
	{
		// �緢�ֲ���ͽ�����뵽��Ӧ�ó���
		if(CreateModules(strPath + wfd.cFileName) && callback){
			(*callback)(obj,m_arrModuleObj.back().pObj);
		}
		while (FindNextFile(hf, &wfd)) //����Ѱ����һ��
		{
			if(CreateModules(strPath + wfd.cFileName) && callback){
				(*callback)(obj,m_arrModuleObj.back().pObj);
			}
		}
		FindClose(hf); // ������Ѱ
	}
}

void UnLoadModules(bool bfree){
	for(int i = 0; i < m_arrModuleObj.size(); i++){
		MODULE_ST module = m_arrModuleObj.at(i);
		delete module.pObj;
		if(bfree) FreeLibrary(module.hIns);
	}
	m_arrModuleObj.clear();
}

bool UnloadModule(int idx){
	if(idx < 0 || idx >= m_arrModuleObj.size()) return false;
	MODULE_ST module = m_arrModuleObj.at(idx);
	delete module.pObj;
	module.pObj = NULL;
	module.hIns = 0;
	//FreeLibrary(module.hIns);
	return true;
}

bool ReloadModule(int idx){
	if(idx < 0 || idx >= m_arrModuleObj.size()) return false;
	MODULE_ST module = m_arrModuleObj.at(idx);
	if(module.pObj == NULL && module.pfName != NULL){
		module.hIns = LoadLibrary(module.pfName);
		PFN_Module_CreateObject pFunc = (PFN_Module_CreateObject)GetProcAddress(module.hIns, _T("modules_createOjbect"));
		if (pFunc) pFunc((void **)&module.pObj);
		return true;
	}
	return false;
}