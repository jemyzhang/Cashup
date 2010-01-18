#include <MzCommon.h>
using namespace MzCommon;

#include "database.h"
#include "resource.h"

namespace cashdatabase {
	db_connection::db_connection(LPWSTR dbfile) {
		if(dbfile){
			swprintf(db_path,dbfile);
		}
		connect();	//构造时立即打开
	}

	db_connection::~db_connection() {
		disconnect();
	}

	//打开数据库
	bool db_connection::connect(){
		bool bRet = true;
		TRY{
			sqlconn.open(db_path);
			sqlconn.createPinyinSearchEngine();
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}
		return bRet;
	}

	//关闭数据库
	bool db_connection::disconnect(){
		bool bRet = true;
		TRY{
			sqlconn.close();
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}
		return bRet;
	}

	//测试密码
	bool db_connection::checkpwd(wchar_t* pwd,int len){
		bool bRet = false;

		if(pwd && len != 0){
			decrypt(pwd,len);
		}
		TRY{
			bRet = sqlconn.executeint(L"select count(*) from sqlite_master;") > 0;
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}

		if(!bRet){  //如果解密失败，则需要断开数据库后再连接
			disconnect();
			connect();
		}
		return bRet;
	}

	//解密数据库
	bool db_connection::decrypt(wchar_t* pwd, int len){
		char* temp = new char[len*2+1];
		int bytecnt = 0;
		wchar_t *p = pwd;
		char* b_pwd = temp;
		for(int i = 0; i < len; i++){
			wchar_t w = *p++;
			if(w&0xff){
				*b_pwd++ = w&0xff;
				bytecnt++;
			}
			if((w>>8)&0xff){
				*b_pwd++ = (w>>8)&0xff;
				bytecnt++;
			}
		}
		*b_pwd = '\0';

		bool bRet = true;
		TRY{
			sqlconn.decypt(temp,bytecnt);
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}
		delete temp;
		return bRet;
	}

	//加密数据库
	bool db_connection::encrypt(wchar_t* pwd, int len){
		char* temp = new char[len*2+1];
		int bytecnt = 0;
		wchar_t *p = pwd;
		char* b_pwd = temp;
		for(int i = 0; i < len; i++){
			wchar_t w = *p++;
			if(w&0xff){
				*b_pwd++ = w&0xff;
				bytecnt++;
			}
			if((w>>8)&0xff){
				*b_pwd++ = (w>>8)&0xff;
				bytecnt++;
			}
		}
		*b_pwd = '\0';

		bool bRet = true;
		TRY{
			sqlconn.encypt(temp,bytecnt);
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}
		delete temp;
		return bRet;
	}

	bool db_connection::begin(){
		bool bRet = true;
		TRY{
			sqlconn.executenonquery(L"begin;");
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}
		return bRet;
	}

	bool db_connection::commit(){
		bool bRet = true;
		TRY{
			sqlconn.executenonquery(L"commit;");
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}
		return bRet;
	}

	bool db_connection::reorg(){
		bool bRet = true;
		TRY{
			sqlconn.executenonquery(L"VACUUM");
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}
		return bRet;
	}

	void db_common::regular_date(LPTRSDATE date, LPTRSDATE datend,
		TRSDATE &d1, TRSDATE &d2){
			if(date){
				d1.Date.Year = date->Date.Year > 0 ? date->Date.Year : 0;
				d1.Date.Month = date->Date.Month > 0 ? date->Date.Month : 0;
				d1.Date.Day = date->Date.Day > 0 ? date->Date.Day : 0;
			}else{
				d1.Value = 0;
			}

			if(datend){
				d2.Date.Year = datend->Date.Year > 0 ? datend->Date.Year : 0xfff;
				d2.Date.Month = datend->Date.Month > 0 ? datend->Date.Month : 12;
				d2.Date.Day = datend->Date.Day > 0 ? datend->Date.Day : 31;
			}else{
				d2.Date.Year = 0xfff; d2.Date.Month = 12; d2.Date.Day = 31;
			}
	}
}