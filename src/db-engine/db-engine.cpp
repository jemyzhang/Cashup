// db-engine.cpp : 定义 DLL 的初始化例程。
//

#include "db-engine.h"
#include "ui_password.h"
#include "resource.h"
#include <MzCommon.h>
using namespace MzCommon;

static wchar_t g_password[256] = {0};
static int g_password_len = 0;
static db_connection* g_pldb = 0;
static bool g_bencypt = false;	//数据库是否加密
static LPWSTR g_dbpath = 0;

#ifdef _DEBUG
#define DEFAULT_DB		L"Program Files\\M8Cash\\cash.db"
#else
#define DEFAULT_DB		L"\\Disk\\Programs\\M8Cash\\cash.db"
#endif

#pragma comment(lib,"MzCommon.lib")
#pragma comment(lib,"mzfc.lib")

static HINSTANCE lngres = 0;

BOOL APIENTRY DllMain( HANDLE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved){
      lngres = (HINSTANCE)hModule;
      return true;
}


void setLanguageResHandle(HINSTANCE h){
	if(h == NULL) return;

	lngres = h;
}

CMzString getLngResString(int strid){
	return MzLoadString(strid,lngres);
}

void create(){
	if(g_pldb == NULL) return;
	db_account account(*g_pldb);
	db_category category(*g_pldb);
	db_transaction transaction(*g_pldb);
	db_person person(*g_pldb);

	account.create();
	category.create();
	transaction.create();
	person.create();
}

void restore(){
	db_category category(*g_pldb);
	category.restore();
}

bool setDatabaseFile(LPWSTR f){
	if(g_pldb) return false;	//数据库打开状态，不能更改
	if(f == NULL) return false;

	C::newstrcpy(&g_dbpath,f);
	return true;
	//...
}

LPWSTR DatabaseFile(){
	if(g_dbpath == NULL){
		wchar_t currpath[MAX_PATH];
		if(File::GetCurrentPath(currpath)){
			C::newstrcpy(&g_dbpath,currpath);
			wsprintf(currpath,L"%s\\cash.db",g_dbpath);
			C::newstrcpy(&g_dbpath,currpath);
		}else{
			C::newstrcpy(&g_dbpath,DEFAULT_DB);
		}
	}
	return g_dbpath;
}

db_connection* createDatabaseOjbect(){
	if(g_pldb == 0){
		bool bRet = false;
        //当数据文件不存在时
        if(!File::FileExists(DatabaseFile())){
		    g_pldb = new db_connection(DatabaseFile());
		    g_bencypt = false;
            bRet = true;
        }else{
            //当数据文件存在时
		    g_pldb = new db_connection(DatabaseFile());
		    g_bencypt = false;
		    if(!g_pldb->checkpwd(g_password,g_password_len)){
			    g_bencypt = true;
			    bRet = false;
			    wchar_t *p = 0; int len = 0;
			    while(MzPasswordDialog(&p,&len)){	//输入密码框获取密码
				    if(g_pldb->checkpwd(p,len)){	//检查密码正确性
					    bRet = true;
					    if(len > 0){
						    memcpy(g_password,p,sizeof(wchar_t)*len);
						    g_password_len = len;
					    }else{
						    g_password[0] = 0;
						    g_password_len = 0;
					    }
					    break;
				    }else{
					    MzAutoMsgBoxEx(0,getLngResString(IDS_STR_PWD_INCORRECT).C_Str(),2000);
				    }
			    }
		    }else{
			    bRet = true;
		    }
        }

		if(bRet){	//成功解密
			create();
			restore();
		}else{	//解密失败
			delete g_pldb;
			g_pldb = 0;
		}
	}
	return g_pldb;
}

//释放对象，但是密码保留
void releaseDatabaseObject(){
	if(g_pldb){
		delete g_pldb;
		g_pldb = NULL;
	}
	if(g_dbpath){
		delete g_dbpath;
		g_dbpath = NULL;
	}
}

void DatabaseSetPassword(){
	if(g_pldb == NULL) createDatabaseOjbect();

	wchar_t *p = 0; int len = 0;
	if(MzPasswordDialog(&p,&len,1)){
		if(g_pldb->encrypt(p,len)){
			if(len != 0){
				MzAutoMsgBoxEx(0,getLngResString(IDS_STR_PWD_SET_S).C_Str(),2000);
			}else{
				MzAutoMsgBoxEx(0,getLngResString(IDS_STR_PWD_CLEAR_S).C_Str(),2000);
			}
		}else{
			if(len != 0){
				MzAutoMsgBoxEx(0,getLngResString(IDS_STR_PWD_SET_F).C_Str(),2000);
			}else{
				MzAutoMsgBoxEx(0,getLngResString(IDS_STR_PWD_CLEAR_F).C_Str(),2000);
			}
		}
	}
}

void resetPassword(){
	memset(g_password,0,sizeof(g_password) * sizeof(g_password[0]));
	g_password_len = 0;
}

bool DatabaseEncrypted(){
	return g_bencypt;
}

bool MzPasswordDialog(LPWSTR *psz, int *plen, int mode, HWND parent){
    if(psz == 0 || plen == 0) return false;    //无法返回密码值，弹出框无效

    Ui_PasswordWnd *pdlg = new Ui_PasswordWnd;
    pdlg->setMode(mode);

    RECT rcWork = MzGetWorkArea();
    pdlg->Create(rcWork.left, rcWork.top + RECT_HEIGHT(rcWork)/4, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork)*3/4,
        parent, 0, WS_POPUP);
    // set the animation of the window
    pdlg->SetAnimateType_Show(MZ_ANIMTYPE_NONE);
    pdlg->SetAnimateType_Hide(MZ_ANIMTYPE_FADE);
    if(pdlg->DoModal() == ID_OK){
        pdlg->getPassword(psz,plen);
        return true;
    }else{
        return false;
    }
}