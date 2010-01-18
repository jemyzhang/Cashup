#pragma once
// include the MZFC library header file
#include <mzfc_inc.h>
#include <windows.h>

#include <vector>
using namespace std;


#include <sqlite3x.hpp>
using namespace sqlite3x;

#ifdef _DEBUG
#define db_out(s) printf("%s:%d: %s\n",__FUNCTION__, __LINE__, s)
#else
#define db_out(s)
#endif

#ifdef TRY
#undef TRY
#endif
#define TRY try

#ifdef CATCH
#undef CATCH
#endif
#define CATCH catch

//老版本定义
#define OLD_TABLE_ACCOUNT	L"ACCOUNTS_v1"
#define OLD_TABEL_CATEGORY	L"CATEGORIES_v1"
#define V1_TABLE_TRANSACTION	L"RECORDS_v1"
#define V2_TABLE_TRANSACTION	L"RECORDS_v2"
//新版本定义
#define TABLE_ACCOUNT	L"ACCOUNTS_v1"
#define TABLE_CATEGORY	L"CATEGORIES_v1"
#define TABLE_TRANSACTION	V2_TABLE_TRANSACTION
#define TABLE_PERSON	L"PERSONS_v1"

typedef union tagTDate{
    DWORD Value;
	struct {
		DWORD Day : 5;
		DWORD Month : 4;
		DWORD Year : 12;
	}Date;
}TRSDATE,*LPTRSDATE;

typedef struct _RECORDTIME {
    WORD Year;
    WORD Month;
    WORD Day;
    WORD Hour;
    WORD Minute;
    WORD Second;
} RECORDTIME_t, *RECORDTIME_ptr;

typedef struct tagCONST_ACCOUNT{
    unsigned int id;
    UINT nameID;	//multilanguage id
    int initval;
    UINT noteID;
    char isfix;
}CONST_ACCOUNT, *LPCONST_ACCOUNT;

typedef struct tagACCOUNT {
    unsigned int id;
    wchar_t* name;
    int initval;
    wchar_t* note;
    char isfix;
	tagACCOUNT(){
		id = 0; initval = 0;
		isfix = 0; name = 0;
		note = 0;
	}
	~tagACCOUNT(){
		if(name) { delete[] name; name = 0; }
		if(note) { delete[] note; note = 0; }
	}
} ACCOUNT, *LPACCOUNT;

typedef vector<LPACCOUNT> ACCOUNTLPS;


typedef enum tagTRANSACT_TYPE {
    CT_INCOME = 0,
    CT_OUTGOING = 1,
    CT_TRANSFER = 2,
} TRANSACT_TYPE;

typedef struct tagCONST_CATEGORY {
    unsigned int id;
    UINT nameID;	//multilanguage id
    TRANSACT_TYPE type;
    unsigned int parentid;
    char level; //0-1-2
} CONST_CATEGORY, *LPCONST_CATEGORY;

typedef struct tagCATEGORY {
    unsigned int id;
    wchar_t* name;
    TRANSACT_TYPE type;
    unsigned int parentid;
    char level; //0-1-2
	tagCATEGORY(){
		this->id = 0; this->type = CT_INCOME;
		this->parentid = 1; this->level = 2;
		this->name = 0; 
	}
	~tagCATEGORY(){
		if(name) { delete[] name; name = 0; }
	}
} CATEGORY, *LPCATEGORY;

typedef vector<LPCATEGORY> CATEGORYLPS;

typedef struct tagTRANSACT {
    unsigned int transid;
    wchar_t* date;
    int amount; //*100
    wchar_t* note;
    unsigned int accountid;
    unsigned int categoryid;
    unsigned int toaccountid;
    char isTransfer;
	unsigned int personid;
	tagTRANSACT(){
		this->transid = 0; this->date = 0;
		this->amount = 0; this->note = 0;
		this->accountid = 0; this->categoryid = 0;
		this->toaccountid = 0; this->isTransfer = 0;
		this->personid = 0;
	}
	~tagTRANSACT(){
		if(date) { delete[] date; date = 0; }
		if(note) { delete[] note; note = 0; }
	}
} TRANSACT, *LPTRANSACT;

typedef vector<LPTRANSACT> TRANSLPS;

typedef enum tagPERSON_TYPE {
	PRSN_ALL	=	-1,
    PRSN_FAMILY		=	0,
    PRSN_CONTACT	=	1,
    PRSN_INSTITUTION	=	2,
} PERSON_TYPE;

typedef struct tagPERSON {
	unsigned int id;
	wchar_t *name;
	PERSON_TYPE type;
	tagPERSON(){
		id = 0;
		name = 0;
		type = PRSN_FAMILY;
	}
	~tagPERSON(){
		if(name) { delete [] name; name = 0; }
	}
} PERSON, *LPPERSON;


namespace cashdatabase {
	class db_connection {
	public:
		db_connection(LPWSTR dbfile);
		~db_connection();
	public:
		bool connect();
		bool disconnect();
		bool checkpwd(wchar_t* pwd,int len);
		bool decrypt(wchar_t* pwd,int len);
		bool encrypt(wchar_t* pwd,int len);
	public:
		bool begin();
		bool commit();
		bool reorg();
	private:
		friend class db_transaction;
		friend class db_account;
		friend class db_category;
		friend class db_person;
		friend class db_analysis;
		friend class db_update;

		sqlite3_connection sqlconn;
	    wchar_t db_path[MAX_PATH];
	};

	class db_transaction {
	public:
		db_transaction(db_connection &conn) : conn(conn) {}
		~db_transaction() { clear_result(); }
	public:
		bool load();
		bool create();
		bool append(LPTRANSACT);
		bool update(LPTRANSACT);
		bool delete_id(int);
		int delete_idx(int);
		int size();
		int maxid();
		TRSDATE mindate();
		TRSDATE maxdate();
		//检测是否有重复记录，如果有则返回记录id，无则返回-1
		int duplicated(LPTRANSACT);

		bool transaction(int id,LPTRANSACT pr);

		int query_size(){	return v_trans.size(); }
		LPTRANSACT query_at(int n) { return v_trans.at(n);	}
	private:
		bool search(sqlite3_command& cmd);
		void clear_result();
		TRANSLPS v_trans;
	private:
		db_connection &conn;
	public:
		//orderby: 0: none			
		//		1: date
		//		2: category
		//		4: account
		bool query_date(LPTRSDATE date = 0, LPTRSDATE datend = 0, UINT orderby = 1);
		bool query_category(int id, LPTRSDATE date = 0, LPTRSDATE datend = 0);
		bool query_account(int id, LPTRSDATE date = 0, LPTRSDATE datend = 0);
		bool query_toaccount(int id, LPTRSDATE date = 0, LPTRSDATE datend = 0);
	};

	class db_account {
	public:
		db_account(db_connection &conn) : conn(conn) {}
		~db_account() { clear_result(); }
	public:
		bool load();
		bool create();	//创建默认账户
		bool append(LPACCOUNT);
		bool update(LPACCOUNT);
		bool remove(int);	//delete by id
		int size();
		int maxid();
		//检测是否有重复帐号，如果有则返回账号id，无则返回-1
		int duplicated(LPACCOUNT,bool *bconflict = 0);

		LPACCOUNT account(int id);
		wchar_t* name(int id);
	public:
		bool query_all();
		int query_size(){	return v_accounts.size(); }
		LPACCOUNT query_at(int n) { return v_accounts.at(n);}
	private:
		bool search(sqlite3_command& cmd);
		void clear_result();
		ACCOUNTLPS v_accounts;
	private:
		db_connection &conn;
	};

	class db_category {
	public:
		db_category(db_connection &conn) : conn(conn) {}
		~db_category() { clear_result(); }
	public:
		bool load();
		bool create();	//创建默认分类
		bool restore();
		bool append(LPCATEGORY);
		bool update(LPCATEGORY);
		bool remove(int);
		LPCATEGORY category(int id);
		bool category_list(int level);	//stored in search list
		wchar_t* name(int id);
		wchar_t* fullname(int id);
		int size();
		int maxid();
		bool ischild(int cid, int pid);	//child id / parent id
	//(根据名称)检测是否有重复类别，如果有则返回类别id，无则返回-1
	//返回值：冲突情况， true表示冲突
		int duplicated(LPCATEGORY,bool *bconflict = 0);
	private:
		bool search(sqlite3_command& cmd);
		void clear_result();
	public:
		vector<LPCATEGORY> v_categories;
	private:
		db_connection &conn;
	};

	class db_person {
	public:
		db_person(db_connection &conn) : conn(conn) {}
		~db_person() { clear_result(); }
	public:
		bool load();
		bool create();
		int typenameid(PERSON_TYPE tid);
		const int* typenameids(int *sz);
		wchar_t* name(int id);
		LPPERSON person(int id);
		bool append(LPPERSON);
		bool update(LPPERSON);
		bool remove(int id);
		//检测是否有重复记录，如果有则返回记录id，无则返回-1
		int	duplicated(LPPERSON);
		bool person_list(PERSON_TYPE); //result stored in search list
		int maxid();
	private:
		bool search(sqlite3_command& cmd);
		void clear_result();
	public:
		vector<LPPERSON> v_persons;
	private:
		db_connection &conn;
	};

	class db_analysis {
	public:
		db_analysis(db_connection &conn) : conn(conn) {}
	public:
		int AccountBalanceById(int id,LPTRSDATE date = 0, bool binit = true, LPTRSDATE datend = 0);
		int AccountInById(int,LPTRSDATE date = 0, LPTRSDATE datend = 0);
		int AccountOutById(int,LPTRSDATE date = 0, LPTRSDATE datend = 0);
		void AccountInOutById(int &, int &,int,LPTRSDATE date = 0, LPTRSDATE datend = 0);
	private:
		db_connection &conn;
	};
#if 0
	class db_update {
	public:
		db_update(db_connection &conn) : conn(conn) {}
	private:
		bool TableExists(wchar_t* tablename);
		//检查数据库版本
		//false:需要更新数据库
		bool checkDatabaseVersion_v1();
		//更新数据库版本
		void updateDatabaseVersion_v1();
		//增加人员分类,需要在RECORDS_v2中增加PERSON_ID字段 2009-9-20
		bool addFieldPerson();
	public:
		void versionUpdate(HWND hWnd);
	private:
		db_connection &conn;
	};
#endif

	class db_common {
	public:
		static void regular_date(LPTRSDATE date, LPTRSDATE datend,
			TRSDATE &d1, TRSDATE &d2);
	};
}

