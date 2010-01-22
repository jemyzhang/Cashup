// db-engine.h : db-engine DLL ����ͷ�ļ�
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

//��ʾ�����
//mode: 0,�������� 1,��������
//retval: true: ID_OK false: ID_CANCEL
bool MzPasswordDialog(LPWSTR *psz, int *plen, int mode = 0, HWND parent = 0);
