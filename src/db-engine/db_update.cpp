#include <cMzCommon.h>
using namespace cMzCommon;

#include "database.h"
#include "resource.h"

#if 0
namespace cashdatabase {
	////数据库更新
	bool db_update::checkDatabaseVersion_v1(){
		bool rc = false;
		wsprintf(sqlcmdw,L"select count(*) from sqlite_master where name=\"%s\"",V1_TABLE_TRANSACTION);
		if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK) {
			while (sqlite3_step(pStmt) == SQLITE_ROW) {
				int count = sqlite3_column_int(pStmt, 0);
				if(count == 1){
					rc = true;
				}
			}
		}else{
			rc = false;
		}
		sqlite3_finalize(pStmt);
		return rc;
	}

	void db_update::updateDatabaseVersion_v1(){
		beginTrans();
		wsprintf(sqlcmdw,L"ALTER TABLE %s RENAME TO %s",V1_TABLE_TRANSACTION,V2_TABLE_TRANSACTION);
		int rc = sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail);
		if(rc == SQLITE_OK){
			rc = sqlite3_step(pStmt);
		}else{
			return;
		}
		sqlite3_finalize(pStmt);
		commitTrans();

		if(loadTransactions()){
			list<CASH_TRANSACT_ptr>::iterator i = list_search_record.begin();
			beginTrans();
			for(; i != list_search_record.end(); i++){
				CASH_TRANSACT_ptr r = *i;
				wchar_t newdate[32];
				wsprintf(newdate,L"%0s 00:00:00",r->date);
				C::newstrcpy(&r->date,newdate);
				//更新日期格式
				wsprintf(sqlcmdw,L"UPDATE %s SET DATE=\"%s\" WHERE TRANSACTID=%d",TABLE_TRANSACTION,r->date,r->transid);
				rc = sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail);
				if(rc == SQLITE_OK){
					rc = sqlite3_step(pStmt);
				}
				sqlite3_finalize(pStmt);
			}
			commitTrans();
			//清理工作
			clean();
		}
	}

	bool db_update::TableExists(wchar_t* tablename){
		bool nRet = false;

		wsprintf(sqlcmdw,L"select count(*) from sqlite_master where type='table' and name = '%s'",tablename);
		if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
			if(sqlite3_step(pStmt) == SQLITE_ROW){
				nRet = (sqlite3_column_int(pStmt, 0) != 0);
			}
		}
		sqlite3_finalize(pStmt);

		return nRet;
	}

	bool db_update::addFieldPerson(){
		//检查是否存在PERSON_ID字段
		bool bfield = false;
		int rc = 0;
		wsprintf(sqlcmdw,L"select PERSON_ID from %s",TABLE_TRANSACTION);
		if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK) {
			rc = sqlite3_step(pStmt);
			if(rc == SQLITE_ROW || rc == SQLITE_DONE) {
				bfield = true;
			}
		}
		sqlite3_finalize(pStmt);
		//字段不存在，增加字段
		bool retval = false;
		if(!bfield){
			wsprintf(sqlcmdw,
				L"ALTER TABLE '%s' ADD COLUMN PERSON_ID NUMERIC NOT NULL  DEFAULT 0",
				TABLE_TRANSACTION);
			if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK) {
				if(sqlite3_step(pStmt) == SQLITE_DONE) {
					retval = true;
				}
			}
			sqlite3_finalize(pStmt);
		}
		return retval;
	}
	
	void db_update::versionUpdate(HWND hWnd){
		//检查记录版本
		if(checkDatabaseVersion_v1()){
			MzAutoMsgBoxEx(hWnd,L"数据库升级中，请稍后",1000);
			MzBeginWaitDlg(hWnd);
			updateDatabaseVersion_v1();
			conn.reorg();
			MzEndWaitDlg();
		}
		addFieldPerson();
	}
}
#endif
