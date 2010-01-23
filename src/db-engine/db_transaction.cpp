#include <cMzCommon.h>
using namespace cMzCommon;

#include "database.h"
#include "resource.h"

namespace cashdatabase {
	//创建：默认交易明细
	bool db_transaction::create(){
		bool bRet = true;
		TRY{	//create table
			if(this->conn.sqlconn.executeint(
				L"select count(*) from sqlite_master where name='"
				TABLE_TRANSACTION
				L"';"
				) > 0) return false;	//table 已经存在

			this->conn.sqlconn.executenonquery(
				L"create table if not exists '"
				TABLE_TRANSACTION
				L"' (TRANSACTID numeric primary key,DATE text not null,AMOUNT integer not null,NOTE text,"
				L" ACCOUNTID numeric not null,CATGORYID numeric not null,TOACCOUNTID numeric,ISTRANSFER integer not null,"
				L" PERSON_ID numeric not null default 0);"
				);
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}

		return bRet;
	}
	//新增：交易明细
	bool db_transaction::append(LPTRANSACT r) {
		bool bRet = true;
		TRY{
			//获取最大的交易ID
			r->transid = maxid() + 1;

			sqlite3_command cmd(this->conn.sqlconn,
				L"insert into '"
				TABLE_TRANSACTION
				L"' (TRANSACTID,DATE,AMOUNT,NOTE,ACCOUNTID,CATGORYID,TOACCOUNTID,ISTRANSFER,PERSON_ID)"
				L" values(?,?,?,?,?,?,?,?,?);");

			cmd.bind(1,static_cast<int>(r->transid));
			cmd.bind(2,r->date, lstrlen(r->date)*2);
			cmd.bind(3,static_cast<int>(r->amount));
			cmd.bind(4,r->note,lstrlen(r->note)*2);
			cmd.bind(5,static_cast<int>(r->accountid));
			cmd.bind(6,static_cast<int>(r->categoryid));
			cmd.bind(7,static_cast<int>(r->toaccountid));
			cmd.bind(8,static_cast<int>(r->isTransfer));
			cmd.bind(9,static_cast<int>(r->personid));

			cmd.executenonquery();
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}

		return bRet;
	}

	//载入：交易明细
	bool db_transaction::load() {
		bool bRet = true;
		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"select * from '"
				TABLE_TRANSACTION
				L"';");

			bRet = search(cmd);
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}

		return bRet;
	}

	//根据ID获得交易明细
	bool db_transaction::transaction(int id,LPTRANSACT pr) {
		bool bRet = true;
		if(pr == NULL) return false;	//指针错误

		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"select * from '"
				TABLE_TRANSACTION
				L"' where TRANSACTID=?;");
			cmd.bind(1,id);

			sqlite3_reader reader = cmd.executereader();
			while(reader.read()){
				pr->transid = reader.getint(0);
				C::newstrcpy(&pr->date,reader.getstring16(1).c_str());
				pr->amount = reader.getint(2);
				C::newstrcpy(&pr->note,reader.getstring16(3).c_str());
				pr->accountid = reader.getint(4);
				pr->categoryid = reader.getint(5);
				pr->toaccountid = reader.getint(6);
				pr->isTransfer = reader.getint(7);
				pr->personid = reader.getint(8);
			}
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}

		return bRet;
	}


	//删除：对应ID：交易明细
	bool db_transaction::delete_id(int id){
		bool bRet = true;
		//从数据库中删除分类
		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"delete from '"
				TABLE_TRANSACTION
				L"' where TRANSACTID=?");
			cmd.bind(1,id);
			cmd.executenonquery();
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}
		return bRet;
	}

	//更新：交易明细
	bool db_transaction::update(LPTRANSACT a){
		if(a == NULL) return false;

		bool bRet = true;

		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"update '"
				TABLE_CATEGORY
				L"' set NOTE=?,DATE=?,AMOUNT=?,ACCOUNTID=?,"
				L"CATGORYID=?,TOACCOUNTID=?,ISTRANSFER=?,PERSON_ID=? "
				L"where TRANSACTID=?");
			cmd.bind(1,a->note,lstrlen(a->note)*2);
			cmd.bind(2,a->date,lstrlen(a->date)*2);
			cmd.bind(3,a->amount);
			cmd.bind(4,static_cast<int>(a->accountid));
			cmd.bind(5,static_cast<int>(a->categoryid));
			cmd.bind(6,static_cast<int>(a->toaccountid));
			cmd.bind(7,static_cast<int>(a->isTransfer));
			cmd.bind(8,static_cast<int>(a->personid));
			cmd.bind(9,static_cast<int>(a->transid));
			cmd.executenonquery();
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}

		return bRet;
	}

	//清除搜索结果：交易明细
	void db_transaction::clear_result(){
		TRY{
			for(int i = 0; i < v_trans.size(); i++){
				delete v_trans.at(i);
			}
			v_trans.clear();
		}CATCH(exception &ex){
			db_out(ex.what());
		}
	}

	//搜索：交易明细
	bool db_transaction::search(sqlite3_command& cmd){
		bool bRet = true;
		//do some clear work
		clear_result();

		TRY{
			sqlite3_reader reader = cmd.executereader();
			while(reader.read()){
				LPTRANSACT r = new TRANSACT;

				r->transid = reader.getint(0);
				C::newstrcpy(&r->date,reader.getstring16(1).c_str());
				r->amount = reader.getint(2);
				C::newstrcpy(&r->note,reader.getstring16(3).c_str());
				r->accountid = reader.getint(4);
				r->categoryid = reader.getint(5);
				r->toaccountid = reader.getint(6);
				r->isTransfer = reader.getint(7);
				r->personid = reader.getint(8);

				v_trans.push_back(r);
			}
			if(v_trans.empty()) bRet = false;
		}CATCH(exception &ex){
			db_out(ex.what());
			bRet = false;
		}

		return bRet;
	}

	//获取明细数量
	int db_transaction::size(){
		int retval = 0;

		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"select count(TRANSACTID) from '"
				TABLE_TRANSACTION
				L"';");
			retval = cmd.executeint();
		}CATCH(exception &ex){
			db_out(ex.what());
		}

		return retval;
	}

	//获取明细ID最大值
	int db_transaction::maxid(){
		int retval = 0;
		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"select max(TRANSACTID) from '"
				TABLE_TRANSACTION
				L"';");
			retval = cmd.executeint();
		}CATCH(exception &ex){
			db_out(ex.what());
		}

		return retval;
	}

	//获取明细最小日期
	TRSDATE db_transaction::mindate(){
		TRSDATE retval;
		retval.Value = 0;

		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"select min(DATE) from '"
				TABLE_TRANSACTION
				L"';");
			const wchar_t *date = cmd.executestring16().c_str();
			if(date != 0){
				DWORD y,m,d;
				swscanf(date,L"%04d-%02d-%02d",&y,&m,&d);
				retval.Date.Year = y;
				retval.Date.Month = m;
				retval.Date.Day = d;
			}
		}CATCH(exception &ex){
			db_out(ex.what());
		}

		return retval;
	}

	//获取明细最大日期
	TRSDATE db_transaction::maxdate(){
		TRSDATE retval;
		retval.Value = 0;

		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"select max(DATE) from '"
				TABLE_TRANSACTION
				L"';");
			const wchar_t *date = cmd.executestring16().c_str();
			if(date != 0){
				DWORD y,m,d;
				swscanf(date,L"%04d-%02d-%02d",&y,&m,&d);
				retval.Date.Year = y;
				retval.Date.Month = m;
				retval.Date.Day = d;
			}
		}CATCH(exception &ex){
			db_out(ex.what());
		}

		return retval;
	}


	//检查是否是重复记录：按照date, amount, account id, category id判断
	int db_transaction::duplicated(LPTRANSACT pr){
		if(pr == NULL || pr->date == NULL) return -1;

		int retid = -1;	//返回存在的记录ID

		TRY{
			sqlite3_command cmd(this->conn.sqlconn,
				L"select count(*),TRANSACTID from '"
				TABLE_TRANSACTION
				L"' where DATE like ? and AMOUNT=? "
				L"and CATEGORYID=? and ACCOUNTID=? and TOACCOUNTID=? "
				L"and ISTRANSFER=?;");

			wchar_t sqldate[24];
			wcscpy(sqldate,pr->date);
			sqldate[10] = '%';	sqldate[11] = '\0';
			cmd.bind(1,sqldate,lstrlen(sqldate)*2);
			cmd.bind(2,pr->amount);
			cmd.bind(3,static_cast<int>(pr->categoryid));
			cmd.bind(4,static_cast<int>(pr->accountid));
			cmd.bind(5,static_cast<int>(pr->toaccountid));
			cmd.bind(6,static_cast<int>(pr->isTransfer));

			sqlite3_reader reader = cmd.executereader();
			if(reader.getint(0) > 0){
				retid = reader.getint(1);
			}
		}CATCH(exception &ex){
			db_out(ex.what());
		}
		return retid;
	}

	bool db_transaction::query_date(LPTRSDATE date,LPTRSDATE datend,UINT orderby){
		TRSDATE d1, d2;
		db_common::regular_date(date,datend,d1,d2);
		wchar_t *sql = new wchar_t[256];
		wsprintf(sql,
			L"select * from '"
			TABLE_TRANSACTION,
			L"' where DATE between ? and ? order by ");
		switch(orderby){
		case 1:
			wcscat(sql,L"ACCOUNTID,DATE DESC;");
			break;
		case 2:
			wcscat(sql,L"CATGORYID,DATE DESC;");
			break;
		case 0:
		default:
			wcscat(sql,L"DATE DESC;");
			break;
		}
		bool bRet = false;
		TRY{
			wchar_t date1[32];
			wchar_t date2[32];
			wsprintf(date1,L"%04d-%02d-%02d 00:00:00",d1.Date.Year,d1.Date.Month,d1.Date.Day);
			wsprintf(date2,L"%04d-%02d-%02d 23:59:59",d2.Date.Year,d2.Date.Month,d2.Date.Day);
			sqlite3_command cmd(this->conn.sqlconn,sql);
			cmd.bind(1,date1,lstrlen(date1)*2);	//date 1
			cmd.bind(2,date2,lstrlen(date2)*2);	//date 2

			bRet = search(cmd);
		}CATCH(exception &ex){
			db_out(ex.what());
		}
		delete sql;
		return bRet;
	}

	bool db_transaction::query_category(int id, LPTRSDATE date, LPTRSDATE datend){
		TRSDATE d1, d2;
		db_common::regular_date(date,datend,d1,d2);

		bool bRet = false;
		TRY{
			wchar_t date1[32];
			wchar_t date2[32];
			wsprintf(date1,L"%04d-%02d-%02d 00:00:00",d1.Date.Year,d1.Date.Month,d1.Date.Day);
			wsprintf(date2,L"%04d-%02d-%02d 23:59:59",d2.Date.Year,d2.Date.Month,d2.Date.Day);
			sqlite3_command cmd(this->conn.sqlconn,
				L"select * from '"
				TABLE_TRANSACTION
				L"' where CATGORYID=? and DATE between ? and ? order by DATE desc;");

			cmd.bind(1,id);
			cmd.bind(2,date1,lstrlen(date1)*2);	//date 1
			cmd.bind(3,date2,lstrlen(date2)*2);	//date 2

			bRet = search(cmd);
		}CATCH(exception &ex){
			db_out(ex.what());
		}

		return bRet;
	}
	bool db_transaction::query_account(int id, LPTRSDATE date, LPTRSDATE datend){
		TRSDATE d1, d2;
		db_common::regular_date(date,datend,d1,d2);

		bool bRet = false;
		TRY{
			wchar_t date1[32];
			wchar_t date2[32];
			wsprintf(date1,L"%04d-%02d-%02d 00:00:00",d1.Date.Year,d1.Date.Month,d1.Date.Day);
			wsprintf(date2,L"%04d-%02d-%02d 23:59:59",d2.Date.Year,d2.Date.Month,d2.Date.Day);
			sqlite3_command cmd(this->conn.sqlconn,
				L"select * from '"
				TABLE_TRANSACTION
				L"' where ACCOUNTID=? and DATE between ? and ? order by DATE desc;");

			cmd.bind(1,id);
			cmd.bind(2,date1,lstrlen(date1)*2);	//date 1
			cmd.bind(3,date2,lstrlen(date2)*2);	//date 2

			bRet = search(cmd);
		}CATCH(exception &ex){
			db_out(ex.what());
		}

		return bRet;
	}

	bool db_transaction::query_toaccount(int id, LPTRSDATE date, LPTRSDATE datend){
		TRSDATE d1, d2;
		db_common::regular_date(date,datend,d1,d2);

		bool bRet = false;
		TRY{
			wchar_t date1[32];
			wchar_t date2[32];
			wsprintf(date1,L"%04d-%02d-%02d 00:00:00",d1.Date.Year,d1.Date.Month,d1.Date.Day);
			wsprintf(date2,L"%04d-%02d-%02d 23:59:59",d2.Date.Year,d2.Date.Month,d2.Date.Day);
			sqlite3_command cmd(this->conn.sqlconn,
				L"select * from '"
				TABLE_TRANSACTION
				L"' where TOACCOUNTID=? and DATE between ? and ? order by DATE desc;");

			cmd.bind(1,id);
			cmd.bind(2,date1,lstrlen(date1)*2);	//date 1
			cmd.bind(3,date2,lstrlen(date2)*2);	//date 2

			bRet = search(cmd);
		}CATCH(exception &ex){
			db_out(ex.what());
		}

		return bRet;
	}
}

