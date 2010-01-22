#include <MzCommon.h>
using namespace MzCommon;

#include "database.h"
#include "resource.h"

extern CMzString getLngResString(int strid);

namespace cashdatabase {
	//创建：默认人员
	bool db_person::create(){
		bool bRet = true;
		TRY{	//create table
			if(this->conn.sqlconn.executeint(
				L"select count(*) from sqlite_master where name='"
				TABLE_PERSON
				L"';"
				) > 0) return false;	//table 已经存在

			this->conn.sqlconn.executenonquery(
				L"create table if not exists '"
				TABLE_PERSON
				L"' (ID integer primary key not null, NAME text not null, TYPE numeric not null);"
				);
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}

		if(!bRet) return bRet;	//创建table失败

		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"insert into '"
				TABLE_PERSON
				L"' (ID,NAME,TYPE) "
				L"values(0,?,?);"
				);
			cmd.bind(1,
				getLngResString(IDS_STR_PERSON_DEFAULT).C_Str(),
				getLngResString(IDS_STR_PERSON_DEFAULT).Length()*2);
			cmd.bind(2,static_cast<int>(PRSN_FAMILY));
			cmd.executenonquery();
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}
		return bRet;
	}

	bool db_person::load(){
		return person_list(PRSN_ALL);
	}

	//新增：人员
	bool db_person::append(LPPERSON p){
		if(p == NULL || p->name == NULL) return false;

		bool bRet = true;
		TRY{
			p->id = maxid() + 1;
			//check if is being used
			sqlite3_command cmd(this->conn.sqlconn,
				L"insert into '"
				TABLE_TRANSACTION
				L"' (ID, NAME, TYPE) values(?,?,?);");
			cmd.bind(1,static_cast<int>(p->id));
			cmd.bind(2,p->name,lstrlen(p->name)*2);
			cmd.bind(3,static_cast<int>(p->type));
			cmd.executenonquery();
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}

		return bRet;
	}

	//删除：人员：根据ID
	bool db_person::remove(int id){
		bool bRet = true;

		TRY{
			//检查是否正在被使用
			sqlite3_command cmd(this->conn.sqlconn,
				L"select count(*) from '"
				TABLE_TRANSACTION
				L"' where PERSON_ID=?;");
			cmd.bind(1,id);
			if(cmd.executeint() > 0){
				bRet = false;
			}
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}

		if(!bRet) return bRet;

		TRY{
			//从数据库中删除
			sqlite3_command cmd(this->conn.sqlconn,
				L"delete from '"
				TABLE_PERSON
				L"' where ID=?;");
			cmd.bind(1,id);
			cmd.executenonquery();
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}

		return bRet;
	}

	//更新：人员
	bool db_person::update(LPPERSON p){
		if(p == NULL || p->name == NULL) return false;

		bool bRet = true;

		TRY{
			//check if is being used
			sqlite3_command cmd(this->conn.sqlconn,
				L"update '"
				TABLE_TRANSACTION
				L"' set NAME=?, TYPE=?, ID=?;");
			cmd.bind(1,p->name,lstrlen(p->name)*2);
			cmd.bind(2,static_cast<int>(p->type));
			cmd.bind(3,static_cast<int>(p->id));
			cmd.executenonquery();
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}

		return bRet;
	}

	//获取：人员：根据ID
	LPPERSON db_person::person(int id){
		LPPERSON pPerson = NULL;
		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"select * from '"
				TABLE_PERSON
				L"' where ID=?");
			cmd.bind(1,id);
			if(search(cmd)){
				pPerson = v_persons.at(0);
			}
		}CATCH(exception ex){
			db_out(ex.what());
		}
		return pPerson;
	}

	//获取：人员名称：根据ID
	wchar_t* db_person::name(int id){
		LPPERSON p = person(id);
		if(p){
			return p->name;
		}
		return NULL;
	}

	//获取：人员：最大编号
	int db_person::maxid(){
		int nRet = 0;

		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"select max(id) from '"
				TABLE_PERSON
				L"';");
			nRet = cmd.executeint();
		}CATCH(exception &ex){
			db_out(ex.what());
		}

		return nRet;
	}

	//人员类型列表名称ID
	const int PersonTypeStrID[] = {
		IDS_STR_PERSON_FAMILY,
		IDS_STR_PERSON_CONTACT,
		IDS_STR_PERSON_INSTITUTION
	};

	//获取：人员类型：名称
	int db_person::typenameid(PERSON_TYPE tid){
		int tsize = sizeof(PersonTypeStrID)/sizeof(PersonTypeStrID[0]);
		if(tid >= tsize) tid = PRSN_FAMILY;
		return PersonTypeStrID[tid];
	}

	//获取：人员：类型名称列表
	const int* db_person::typenameids(int *nsize){
		if(nsize) *nsize = sizeof(PersonTypeStrID)/sizeof(PersonTypeStrID[0]);
		return PersonTypeStrID;
	}

	//获取：人员列表：根据类型
	bool db_person::person_list(PERSON_TYPE t){
		bool bRet = false;
		TRY{
			if( t == PRSN_ALL ){
				sqlite3_command cmd(this->conn.sqlconn,
					L"select * from '"
					TABLE_PERSON
					L"' order by type");
				bRet = search(cmd);
			}else{
				sqlite3_command cmd(this->conn.sqlconn,
					L"select * from '"
					TABLE_PERSON
					L"' where TYPE=?");
				cmd.bind(1,static_cast<int>(t));
				bRet = search(cmd);
			}
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}
		return bRet;
	}

	//清除：人员：搜索结果列表
	void db_person::clear_result(){
		TRY{
			for(int i = 0; i < v_persons.size(); i++){
				delete v_persons.at(i);
			}
			v_persons.clear();
		}CATCH(exception &ex){
			db_out(ex.what());
		}
	}

	//搜索：人员
	bool db_person::search(sqlite3_command& cmd){
		int resCount = 0;

		//do some clear work
		clear_result();

		bool bRet = true;
		TRY{
			sqlite3_reader reader = cmd.executereader();
			while(reader.read()){
				LPPERSON r = new PERSON;

				r->id = reader.getint(0);
				C::newstrcpy(&r->name,reader.getstring16(1).c_str());
				r->type = static_cast<PERSON_TYPE>(reader.getint(2));

				v_persons.push_back(r);
			}
			//当无记录时
			if(v_persons.empty()) bRet = false;
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}

		return bRet;
	}

	//检查：人员名称：是否已存在
	//如果存在则返回ID
	int	db_person::duplicated(LPPERSON pP){
		if(pP == NULL || pP->name == NULL) return -1;

		int nRet = -1;
		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"select count(*),ID from '"
				TABLE_PERSON
				L"' where NAME=? collate nocase;");
			cmd.bind(1,pP->name);
			sqlite3_reader reader = cmd.executereader();
			while(reader.read()){
				if(reader.getint(0)){
					nRet = reader.getint(1);
				}
			}
		}CATCH(exception &ex){
			db_out(ex.what());
			nRet = -1;
		}

		return nRet;
	}

}

