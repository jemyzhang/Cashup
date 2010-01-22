// db-engine.h : db-engine DLL 的主头文件
//

#pragma once
// include the MZFC library header file
#include <mzfc_inc.h>
#include "database.h"
using namespace cashdatabase;

db_connection* createDatabaseOjbect();
void releaseDatabaseObject();

void DatabaseSetPassword();
void resetPassword();
bool DatabaseEncrypted();
void createDefaultDatabase();
bool setDatabaseFile(LPWSTR f);
LPWSTR DatabaseFile();

//显示密码框
//mode: 0,输入密码 1,设置密码
//retval: true: ID_OK false: ID_CANCEL
bool MzPasswordDialog(LPWSTR *psz, int *plen, int mode = 0, HWND parent = 0);
