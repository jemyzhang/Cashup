#include <MzCommon.h>
using namespace MzCommon;

#include "database.h"
#include "resource.h"
extern CMzString getLngResString(int strid);

namespace cashdatabase {
	//创建：默认分类
	bool db_category::create(){
		bool bRet = true;
		TRY{	//create table
			if(this->conn.sqlconn.executeint(
				L"select count(*) from sqlite_master where name='"
				TABLE_CATEGORY
				L"';"
				) > 0) return false;	//table 已经存在

			this->conn.sqlconn.executenonquery(
				L"create table if not exists '"
				TABLE_CATEGORY
				L"' (ID numeric primary key,NAME text NOT NULL,TYPE integer NOT NULL,PARENTID numeric NOT NULL,LEVEL integer);"
				);
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}

		if(!bRet) return bRet;	//创建table失败

		CONST_CATEGORY default_categories[] = {
			{0, IDS_STR_EXPENSE, CT_OUTGOING, -1, 0},
			{1, IDS_STR_INCOME, CT_INCOME, -1, 0},
			{2, IDS_STR_TRANSFER, CT_TRANSFER, -1, 2},	//设置为无子目录
		};
		int aSets = sizeof (default_categories) / sizeof (CONST_CATEGORY);
		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"insert into '"
				TABLE_CATEGORY
				L"' (ID,NAME,TYPE,PARENTID,LEVEL) "
				L"values(?,?,?,?,?);"
				);
			for(int i = 0; i < aSets; i++){
				cmd.bind(1,static_cast<int>(default_categories[i].id));
				cmd.bind(2,
					getLngResString(default_categories[i].nameID).C_Str(),
					getLngResString(default_categories[i].nameID).Length()*2);
				cmd.bind(3,static_cast<int>(default_categories[i].type));
				cmd.bind(4,static_cast<int>(default_categories[i].parentid));
				cmd.bind(5,static_cast<int>(default_categories[i].level));
				cmd.executenonquery();
			}
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}
		return bRet;
	}

	//恢复：默认分类
	bool db_category::restore() {
		CONST_CATEGORY default_categories[] = {
			{0, IDS_STR_EXPENSE, CT_OUTGOING, -1, 0},
			{1, IDS_STR_INCOME, CT_INCOME, -1, 0},
			{2, IDS_STR_TRANSFER, CT_TRANSFER, -1, 2},	//设置为无子目录
		};

		bool bRet = true;
		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"update '"
				TABLE_CATEGORY
				L"' set NAME=? where LEVEL=? and TYPE=? and PARENTID=-1;"
				);
			for(int i = 0; i < sizeof(default_categories)/sizeof(CONST_CATEGORY); i++){
				cmd.bind(1,getLngResString(default_categories[i].nameID).C_Str(),
					getLngResString(default_categories[i].nameID).Length()*2);
				cmd.bind(2,static_cast<int>(default_categories[i].level));
				cmd.bind(3,static_cast<int>(default_categories[i].type));
				cmd.executenonquery();
			}
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}
		return bRet;
	}
	//新增：分类
	bool db_category::append(LPCATEGORY cat) {
		bool bRet = true;
		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"insert into '"
				TABLE_CATEGORY
				L"' (ID,NAME,TYPE,PARENTID,LEVEL)"
				L" values(?,?,?,?,?);");

			cmd.bind(1,static_cast<int>(cat->id));
			cmd.bind(2,cat->name, lstrlen(cat->name)*2);
			cmd.bind(3,static_cast<int>(cat->type));
			cmd.bind(4,static_cast<int>(cat->parentid));
			cmd.bind(5,static_cast<int>(cat->level));

			cmd.executenonquery();
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}

		return bRet;
	}

	//删除：对应ID：分类
	bool db_category::remove(int id){
		//检查是否有子类
		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"select count(*) from '"
				TABLE_CATEGORY
				L"' where PARENTID=?");
			cmd.bind(1,id);
			if(cmd.executeint() > 0){	//有子类
				return false;	//有子类，无法删除
			}
		}CATCH(exception &ex){
			db_out(ex.what());
		}

		//检查是否正在被使用
		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"select count(*) from '"
				TABLE_TRANSACTION
				L"' where CATEGORYID=?");
			cmd.bind(1,id);
			if(cmd.executeint() > 0){
				return false;	//正在被使用，无法删除
			}
		}CATCH(exception &ex){
			db_out(ex.what());
		}

		//从数据库中删除分类
		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"delete from '"
				TABLE_CATEGORY
				L"' where ID=?");
			cmd.bind(1,id);
			cmd.executenonquery();
		}CATCH(exception &ex){
			db_out(ex.what());
		}

		return true;
	}

	//更新：分类
	bool db_category::update(LPCATEGORY a){
		if(a == NULL) return false;

		bool bRet = true;

		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"update '"
				TABLE_CATEGORY
				L"' set NAME=?,LEVEL=?,PARENTID=?,TYPE=? where ID=?");
			cmd.bind(1,a->name,lstrlen(a->name)*2);
			cmd.bind(2,static_cast<int>(a->level));
			cmd.bind(3,static_cast<int>(a->parentid));
			cmd.bind(4,static_cast<int>(a->type));
			cmd.bind(5,static_cast<int>(a->id));
			cmd.executenonquery();
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}

		return bRet;
	}

	//载入：分类
	bool db_category::load() {
		bool bRet = true;
		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"select * from '"
				TABLE_CATEGORY
				L"' order by NAME collate pinyin;");
			bRet = search(cmd);
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}

		return bRet;
	}

	//根据ID获得分类
	LPCATEGORY db_category::category(int id) {
		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"select * from '"
				TABLE_CATEGORY
				L"' where ID=?;");
			cmd.bind(1,id);

			if(search(cmd)){
				return v_categories.at(0);
			}
		}CATCH(exception &ex){
			db_out(ex.what());
		}

		return NULL;
	}

	//根据ID获取分类名称
	wchar_t* db_category::name(int id) {
		LPCATEGORY pcategory = category(id);
		return (pcategory == NULL ? NULL : pcategory->name);
	}

	//根据ID获取分类全名（包括父类）
	//TODO: 返回值必须在使用完毕后销毁
	wchar_t* db_category::fullname(int id) {
		wchar_t* fullname;
		wchar_t* nodename[3]; //child max depth = 3
		int depth = 0;
		int szfullname = 0;

		int nodeidx = id;
		LPCATEGORY c;
		do {
			c = category(nodeidx);
			if(c != NULL){
				nodename[depth++] = c->name;
				nodeidx = c->parentid;
				szfullname += lstrlen(c->name);
			}else{
				nodeidx = -1;	//当不能找到对应category时结束
			}
		} while (nodeidx != -1);

		//分配字符串空间
		fullname = new wchar_t[szfullname + 1];

		for (int i = depth-1; i >= 0; i--) {
			lstrcat(fullname, nodename[i]);
			if (i != 0) {
				lstrcat(fullname, L" - ");
			}
		}
		return fullname;
	}

	//清除搜索结果：分类
	void db_category::clear_result(){
		TRY{
			for(int i = 0; i < v_categories.size(); i++){
				delete v_categories.at(i);
			}
			v_categories.clear();
		}CATCH(exception &ex){
			db_out(ex.what());
		}
	}

	//搜索：分类
	bool db_category::search(sqlite3_command& cmd){
		bool rc = true;

		//do some clear work
		clear_result();

		bool bRet = true;
		TRY{
			sqlite3_reader reader = cmd.executereader();
			while(reader.read()){
				LPCATEGORY r = new CATEGORY;
				r->id = reader.getint(0);
				C::newstrcpy(&r->name,reader.getstring16(1).c_str());
				r->type = static_cast<TRANSACT_TYPE>(reader.getint(2));
				r->parentid = reader.getint(3);
				r->level = reader.getint(4);

				v_categories.push_back(r);
			}
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}

		if(v_categories.empty()) bRet = false;
		return bRet;
	}

	//检查是否为子分类
	bool db_category::ischild(int childId, int parentId){
		if(childId == parentId) return false;


		LPCATEGORY child = category(childId);
		LPCATEGORY parent = category(parentId);

		if(child->level <= parent->level || child->parentid == -1) return false;

		if(child->level == 2 && parent->level == 0){
			LPCATEGORY parent1 = category(child->parentid);
			if(parent1->parentid == parent->id){
				return true;
			}else{
				return false;
			}
		}

		if(child->parentid == parent->id){
			return true;
		}
		return false;
	}

	//检查是否重复分类：按照name判断
	int db_category::duplicated(LPCATEGORY pc, bool *bconflict){
		if(pc == NULL || pc->name == NULL) return -1;

		int retid = -1;

		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"select count(*),ID,TYPE from '"
				TABLE_CATEGORY
				L"' where NAME=? and LEVEL=? and PARENTID=?;");
			cmd.bind(1,pc->name,lstrlen(pc->name)*2);
			cmd.bind(2,static_cast<int>(pc->level));
			cmd.bind(3,static_cast<int>(pc->parentid));
			sqlite3_reader reader = cmd.executereader();
			if(reader.getint(0) != 0){
				retid = reader.getint(1);
				if(reader.getint(2) != pc->type){
					if(bconflict) *bconflict = true;
				}
			}
		}CATCH(exception &ex){
			db_out(ex.what());
		}

		return retid;
	}

	//获取分类：根据级别
	bool db_category::category_list(int level){
		bool bRet = true;

		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"select * from '"
				TABLE_CATEGORY
				L"' where LEVEL=? order by NAME collate pinyin;");
			bRet = search(cmd);
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}

		return bRet;
	}

	//获取分类数量
	int db_category::size(){
		int retval = 0;

		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"select count(ID) from '"
				TABLE_CATEGORY
				L"';");
			retval = cmd.executeint();
		}CATCH(exception &ex){
			db_out(ex.what());
		}

		return retval;
	}

	//获取分类ID最大值
	int db_category::maxid(){
		int retval = 0;

		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"select max(ID) from '"
				TABLE_CATEGORY
				L"';");
			retval = cmd.executeint();
		}CATCH(exception &ex){
			db_out(ex.what());
		}

		return retval;
	}
}




