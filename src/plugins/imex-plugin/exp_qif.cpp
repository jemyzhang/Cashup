#include "imexport.h"
#include "resource.h"
#include <cMzCommon.h>
using namespace cMzCommon;

LPWSTR ImExport::qif_date(LPCTSTR recdt){
	LPWSTR retval = NULL;
	if(recdt == NULL) return retval;

	DWORD y,m,d;
	swscanf(recdt,L"%04d-%02d-%02d",&y,&m,&d);
	y %= 100;	//取后两位
	if(qifdt == NULL) qifdt = new wchar_t[9];
	swprintf(qifdt,L"%02d/%02d/%02d",m,d,y);
	retval = qifdt;
	if(qifdt[0] == '0') retval ++;
	if(qifdt[3] == '0') qifdt[3] = ' ';
	if(qifdt[6] == '0') qifdt[6] = ' ';
	return retval;
}

bool ImExport::qif_export(wchar_t* file,int &sz){
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

	//载入账户
	db_account dbacnt(*dbcon);
	dbacnt.query_all();
	int acntsize = dbacnt.query_size();

	//明细记录
	db_transaction dbtrans(*dbcon);
	//分类
	db_category dbctg(*dbcon);
	db_person dbpsn(*dbcon);

	int nsuccess = 0;

	for(int i = 0;i < acntsize; i++){
		LPACCOUNT account = dbacnt.query_at(i);
		//导出账户信息
		fwprintf(fp,L"!Account\n");
		fwprintf(fp,L"N%s\n",account->name);
		if(lstrcmp(account->name,getLngResString(IDS_CASH).C_Str()) == 0){
			fwprintf(fp,L"TCash\n^\n!Type:Cash\n");
		}else{
			fwprintf(fp,L"TBank\n^\n!Type:Bank\n");
		}
		if(account->initval != 0){
			fwprintf(fp,L"D1/ 1/ 9\n");
			fwprintf(fp,L"T%.2f\n",(double)account->initval/100);
			fwprintf(fp,L"CX\n");
			fwprintf(fp,L"M账户初始金额\n");
			fwprintf(fp,L"L[%s]\n^\n",account->name);
		}

		//导出转入账户相关记录
		if(dbtrans.query_toaccount(account->id,&_sdate,&_edate)){
			for(int j = 0; j < dbtrans.query_size(); j++){
				LPTRANSACT record = dbtrans.query_at(j);

				fwprintf(fp,L"D%s\n",qif_date(record->date));

				fwprintf(fp,L"T%.2f\n",(double)record->amount/100);
				if(lstrlen(record->note)){
					wchar_t snote[1024];
					fwprintf(fp,L"M%s\n",C::removeWrap(snote,record->note));		//描述
				}
				fwprintf(fp,L"L[%s]\n^\n",dbacnt.name(record->accountid));
				nsuccess++;
			}
		}

		//导出转入账户相关记录
		if(dbtrans.query_account(account->id,&_sdate,&_edate)){	//找到帐号
			for(int j = 0; j < dbtrans.query_size(); j++){
				LPTRANSACT record = dbtrans.query_at(j);

				fwprintf(fp,L"D%s\n",qif_date(record->date));

				LPCATEGORY category = dbctg.category(record->categoryid);
				if(category->type == CT_INCOME){	//收入
					fwprintf(fp,L"T%.2f\n",(double)record->amount/100);
				}else{	//支出
					fwprintf(fp,L"T-%.2f\n",(double)record->amount/100);
				}
				if(lstrlen(record->note)){
					wchar_t snote[1024];
					fwprintf(fp,L"M%s\n",C::removeWrap(snote,record->note));		//描述
				}
				int level = category->level;
				if(record->isTransfer){	level = 3; }
				switch(level){
						case 0:
							fwprintf(fp,L"L%s\n",category->name);
							break;
						case 1:
							fwprintf(fp,L"L%s:%s\n",dbctg.name(category->parentid),category->name);
							break;
						case 2:
							fwprintf(fp,L"L%s:%s:%s\n",
								dbctg.name(dbctg.category(category->parentid)->parentid),
								dbctg.name(category->parentid),
								category->name);
							break;
						case 3:
							fwprintf(fp,L"L[%s]\n",dbacnt.name(record->toaccountid));
							break;
						default:
							break;
				}
				fwprintf(fp,L"P%s\n",dbpsn.name(record->personid));
				fwprintf(fp,L"^\n");
				nsuccess++;
			}
		}
		m_Progressdlg.SetCurValue(i*100/acntsize);
		m_Progressdlg.UpdateProgress();
	}
	fclose(fp);
	wsprintf(_lastErrMsg,getLngResString(IDS_EXPORT_RESULT).C_Str(),nsuccess);
	m_Progressdlg.EndProgress();

	return true;
}