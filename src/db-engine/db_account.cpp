#include <MzCommon.h>
using namespace MzCommon;

#include "database.h"
#include "resource.h"
extern CMzString getLngResString(int strid);

namespace cashdatabase {
	//创建：默认账户
	bool db_account::create(){
		bool bRet = true;
		
		TRY{	//create table
			if(this->conn.sqlconn.executeint(
				L"select count(*) from sqlite_master where name='"
				TABLE_ACCOUNT
				L"';"
				) > 0) return false;	//table 已经存在

			this->conn.sqlconn.executenonquery(
				L"create table if not exist '"
				TABLE_ACCOUNT
				L"' (ID numeric primary key,NAME text NOT NULL,INITIALVALUE integer,NOTE text,ISFIX integer);"
				);
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}

		if(!bRet) return bRet;	//创建table失败

		CONST_ACCOUNT default_accounts[] = {
			{0, IDS_STR_CASH, 0, IDS_STR_CASH, 1},
			{1, IDS_STR_CREDIT, 0, IDS_STR_CREDIT, 1},
			{2, IDS_STR_SAVING, 0, IDS_STR_SAVING, 1},
		};
		int aSets = sizeof (default_accounts) / sizeof (CONST_ACCOUNT);
		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"insert into '"
				TABLE_ACCOUNT
				L"' (ID,NAME,INITIALVALUE,NOTE,ISFIX) "
				L"values(?,?,?,?,?);"
				);
			for(int i = 0; i < aSets; i++){
				cmd.bind(1,static_cast<int>(default_accounts[i].id));
				cmd.bind(2,
					getLngResString(default_accounts[i].nameID).C_Str(),
					getLngResString(default_accounts[i].nameID).Length()*2);
				cmd.bind(3,default_accounts[i].initval);
				cmd.bind(4,
					getLngResString(default_accounts[i].noteID).C_Str(),
					getLngResString(default_accounts[i].noteID).Length()*2);
				cmd.bind(5,static_cast<int>(default_accounts[i].isfix));
				cmd.executenonquery();
			}
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}
		return bRet;
	}

	//新增：账户
	bool db_account::append(LPACCOUNT acc) {
		bool bRet = true;
		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"insert into '"
				TABLE_ACCOUNT
				L"' (ID,NAME,INITIALVALUE,NOTE,ISFIX)"
				L" values(?,?,?,?,?);");

			cmd.bind(1,static_cast<int>(acc->id));
			cmd.bind(2,acc->name, lstrlen(acc->name)*2);
			cmd.bind(3,static_cast<int>(acc->initval));
			cmd.bind(4,acc->note,lstrlen(acc->note)*2);
			cmd.bind(5,static_cast<int>(acc->isfix));

			cmd.executenonquery();
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}
		return bRet;
	}

	//删除：账户：对应ID
	bool db_account::remove(int id){
		//检查是否正在被使用
		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"select count(*) from '"
				TABLE_TRANSACTION
				L"' where ACCOUNTID=? or TOACCOUNTID=?");

			cmd.bind(1,id); cmd.bind(2,id);
			if(cmd.executeint() > 0){
				return false;
			}
		}CATCH(exception &ex){
			db_out(ex.what());
		}

		//从数据库中删除分类
		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"delete from '"
				TABLE_ACCOUNT
				L"' where ID=?");
			cmd.bind(1,id);
			cmd.executenonquery();
		}CATCH(exception &ex){
			db_out(ex.what());
		}

		return true;
	}

	//更新账户
	bool db_account::update(LPACCOUNT a){
		if(a == NULL) return false;
		bool bRet = true;

		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"update '"
				TABLE_ACCOUNT
				L"' set NAME=?, NOTE=?, INITIALVALUE=? where ID=?");
			cmd.bind(1,a->name,lstrlen(a->name)*2);
			cmd.bind(2,a->note,lstrlen(a->note)*2);
			cmd.bind(3,a->initval);
			cmd.bind(4,static_cast<int>(a->id));
			cmd.executenonquery();
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}

		return bRet;
	}

	//载入：账户
	bool db_account::load() {
		return query_all();
	}

	//根据id获得帐号
	LPACCOUNT db_account::account(int id) {
		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"select * from '"
				TABLE_ACCOUNT
				L"' where ID=?;");
			cmd.bind(1,id);

			if(search(cmd)){
				return v_accounts.at(0);
			}
		}CATCH(exception &ex){
			db_out(ex.what());
		}

		return NULL;
	}

	//根据ID获取账户名称
	wchar_t* db_account::name(int id) {
		LPACCOUNT paccount = account(id);
		return (paccount == NULL ? NULL : paccount->name);
	}

	//清除搜索结果：账户
	void db_account::clear_result(){
		TRY{
			for(int i = 0; i < v_accounts.size(); i++){
				delete v_accounts.at(i);
			}
			v_accounts.clear();
		}CATCH(exception &ex){
			db_out(ex.what());
		}
	}

	//搜索：账户
	bool db_account::search(sqlite3_command& cmd){
		//do some clear work
		clear_result();

		bool bRet = true;
		TRY{
			sqlite3_reader reader = cmd.executereader();
			while(reader.read()){
				LPACCOUNT r = new ACCOUNT;
				r->id = reader.getint(0);
				C::newstrcpy(&r->name,reader.getstring16(1).c_str());
				r->initval = reader.getint(2);
				C::newstrcpy(&r->note,reader.getstring16(3).c_str());
				r->isfix = reader.getint(4);

				v_accounts.push_back(r);
			}
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}

		if(v_accounts.empty()) bRet = false;
		return bRet;
	}

	//检查是否重复账户：按照name判断
	int db_account::duplicated(LPACCOUNT pa, bool *bconflict){
		if(pa == NULL || pa->name == NULL) return -1;

		int retid = -1;
		bool bconf = false;

		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"select * from '"
				TABLE_ACCOUNT
				L"' where NAME=? COLLATE NOCASE;");
			cmd.bind(1,pa->name,lstrlen(pa->name)*2);

			if(search(cmd)){
				LPACCOUNT r = v_accounts.at(0);
				retid = r->id;	//返回存在的记录id号

				if(r->initval != pa->initval || 
					r->isfix != pa->isfix ||
					lstrcmp(r->note,pa->note) != 0){	//有冲突
						bconf = true;
				}
				if(bconflict){
					*bconflict = bconf;
				}
			}
		}CATCH(exception &ex){
			db_out(ex.what());
		}

		return retid;
	}

	//获取账户数量
	int db_account::size(){
		int retval = 0;
		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"select count(ID) from '"
				TABLE_ACCOUNT
				L"';");
			retval = cmd.executeint();
		}CATCH(exception &ex){
			db_out(ex.what());
		}
		return retval;
	}

	//获取账户ID最大值
	int db_account::maxid(){
		int retval = 0;
		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"select max(ID) from '"
				TABLE_ACCOUNT
				L"';");
			retval = cmd.executeint();
		}CATCH(exception &ex){
			db_out(ex.what());
		}
		return retval;
	}

	bool db_account::query_all() {
		bool bRet = true;
		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"select * from '"
				TABLE_ACCOUNT
				L"';");
			bRet = search(cmd);
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}

		return bRet;
	}
}


