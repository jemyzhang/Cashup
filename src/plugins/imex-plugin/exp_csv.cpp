#include "imexport.h"
#include "resource.h"
#include <cMzCommon.h>
using namespace cMzCommon;

bool ImExport::csv_export(wchar_t* file,int &sz){
	sz = 0;
	//打开数据库
	db_connection *dbcon = createDatabaseOjbect();
	if(dbcon == NULL){
		wsprintf(_lastErrMsg,getLngResString(IDS_ERR_DB_CONNECTION).C_Str());
		return false;
	}

	//打开文件
	FILE* fp;
	fp = _wfopen(file,L"wt");
	if(fp == NULL){
		wsprintf(_lastErrMsg,getLngResString(IDS_ERR_FILE_OPEN).C_Str());
		return false;
	}

	m_Progressdlg.SetRange(0,100);
	m_Progressdlg.SetTitle(getLngResString(IDS_EXPORT_WAIT).C_Str());
	m_Progressdlg.BeginProgress(m_hWnd);
	
	//打印标题
	fwprintf(fp,
		getLngResString(IMEX_CSV_S ? IDS_TRANS_TITLE_S:IDS_TRANS_TITLE_C).C_Str()
			);

	fwprintf(fp,L"\n");

	//获取记录
	db_transaction dbtrans(*dbcon);
	dbtrans.query_date(&_sdate,&_edate);

	sz = dbtrans.query_size();

	//设定导出结构
	wchar_t *exformat = _filetype == 
		IMEX_CSV_S ? DEF_TRANSACTION_FORMAT_S : DEF_TRANSACTION_FORMAT_C;

	db_account dbacnt(*dbcon);
	db_category dbctg(*dbcon);
	db_person dbpsn(*dbcon);
	//开始导出数据
	for(int i = 0; i < sz; i++){
		LPTRANSACT c = dbtrans.query_at(i);

		wchar_t note[512];
		CMzString dateonly = c->date;
		fwprintf(fp,exformat,
			dateonly.SubStr(0,10).C_Str(),
			(double)c->amount/100,
			dbacnt.name(c->accountid),
			c->isTransfer ? dbacnt.name(c->toaccountid) : L"\0",
			dbctg.fullname(c->categoryid),dbpsn.name(c->personid),
			C::removeWrap(note,c->note)
			);
		fwprintf(fp,L"\n");
		m_Progressdlg.SetCurValue(i*100/sz);
		m_Progressdlg.UpdateProgress();
	}
	//导出完成，关闭文件
	fclose(fp);

	wsprintf(_lastErrMsg,getLngResString(IDS_EXPORT_RESULT).C_Str(),sz);
	m_Progressdlg.EndProgress();

	return true;
}