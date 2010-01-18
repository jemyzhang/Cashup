#include <MzCommon.h>
using namespace MzCommon;

#include "database.h"
#include "resource.h"

namespace cashdatabase {
	void db_analysis::AccountInOutById(int &in, int &out, int id,LPTRSDATE date, LPTRSDATE datend){
		in = AccountInById(id,date,datend);
		out = AccountOutById(id,date,datend);
		return;
	}

	int db_analysis::AccountInById(int id,LPTRSDATE date, LPTRSDATE datend){
		TRSDATE d1,d2;
		int amount = 0;
		db_common::regular_date(date,datend,d1,d2);

		TRY{
			wchar_t date1[32];
			wchar_t date2[32];
			wsprintf(date1,L"%04d-%02d-%02d 00:00:00",d1.Date.Year,d1.Date.Month,d1.Date.Day);
			wsprintf(date2,L"%04d-%02d-%02d 23:59:59",d2.Date.Year,d2.Date.Month,d2.Date.Day);

			if(id == -1){
				sqlite3_command cmd(this->conn.sqlconn,
					L"select sum(amount) from '"
					TABLE_TRANSACTION
					L"' where categoryid in (select id from '"
					TABLE_CATEGORY
					L"' where type=0 and DATE between ? and ?;");
				cmd.bind(1,date1,lstrlen(date1)*2);	//date 1
				cmd.bind(2,date2,lstrlen(date2)*2);	//date 2

				amount = cmd.executeint();
			}else{
				sqlite3_command cmd(this->conn.sqlconn,
					L"select sum(amount) from '"
					TABLE_TRANSACTION
					L"' where ((accountid=? and categoryid in (select id from '"
					TABLE_CATEGORY
					L"' where type=0)) or (toaccountid=? and categoryid in (select id from '"
					TABLE_CATEGORY
					L"' where type=2))) and DATE between ? and ?;");
				cmd.bind(1,id);	//accountid
				cmd.bind(2,id);	//toaccountid
				cmd.bind(3,date1,lstrlen(date1)*2);	//date 1
				cmd.bind(4,date2,lstrlen(date2)*2);	//date 2

				amount = cmd.executeint();
			}
		}CATCH(exception &ex){
			db_out(ex.what());
		}

		return amount;
	}

	int db_analysis::AccountOutById(int id,LPTRSDATE date, LPTRSDATE datend){
		TRSDATE d1,d2;
		int amount = 0;
		db_common::regular_date(date,datend,d1,d2);

		TRY{
			wchar_t date1[32];
			wchar_t date2[32];
			wsprintf(date1,L"%04d-%02d-%02d 00:00:00",d1.Date.Year,d1.Date.Month,d1.Date.Day);
			wsprintf(date2,L"%04d-%02d-%02d 23:59:59",d2.Date.Year,d2.Date.Month,d2.Date.Day);

			if(id == -1){
				sqlite3_command cmd(this->conn.sqlconn,
					L"select sum(amount) from '"
					TABLE_TRANSACTION
					L"' where categoryid in (select id from '"
					TABLE_CATEGORY
					L"' where type=1 and DATE between ? and ?;");
				cmd.bind(1,date1,lstrlen(date1)*2);	//date 1
				cmd.bind(2,date2,lstrlen(date2)*2);	//date 2

				amount = cmd.executeint();
			}else{
				sqlite3_command cmd(this->conn.sqlconn,
					L"select sum(amount) from '"
					TABLE_TRANSACTION
					L"' where accountid=? and categoryid in (select id from '"
					TABLE_CATEGORY
					L"' where type!=0) and DATE between ? and ?;");
				cmd.bind(1,id);	//accountid
				cmd.bind(3,date1,lstrlen(date1)*2);	//date 1
				cmd.bind(4,date2,lstrlen(date2)*2);	//date 2

				amount = cmd.executeint();
			}
		}CATCH(exception &ex){
			db_out(ex.what());
		}

		return amount;
	}

	int db_analysis::AccountBalanceById(int id,LPTRSDATE date, bool binit, LPTRSDATE datend){
		int in = 0;
		int out = 0;
		db_account account(this->conn);
		LPACCOUNT a = account.account(id);
		int balance = binit ? a->initval : 0;
		AccountInOutById(in,out,id,date,datend);
		return (balance + in - out);
	}
}



