#include "imexport.h"
#include "resource.h"
#include <MzCommon.h>
using namespace MzCommon;
#include <fstream>

//日期,金额,账户,转至账户,分类,人员,备注
enum tagCSVImportStep{
	CSV_IMP_DATE	=	0,
	CSV_IMP_AMOUNT	=	1,
	CSV_IMP_ACCOUNT	=	2,
	CSV_IMP_TOACCOUNT	=	3,
	CSV_IMP_CATEGORY	=	4,
	CSV_IMP_PERSON		=	5,
	CSV_IMP_NOTE		=	6,
	CSV_IMP_END			=	0xff,
};

int ImExport::csv_import_parasecategory(LPWSTR c, bool bTransfer){
	wchar_t* token;
	token = C::_wcstok(c,L" - ");

	CATEGORY cat;
	db_connection *dbcon = createDatabaseOjbect();
	db_category dbctg(*dbcon);
	int catid = -1;
	int level = bTransfer ? 2 : 0;
	TRANSACT_TYPE type = bTransfer ? CT_TRANSFER : CT_OUTGOING;

	while(token){
		C::newstrcpy(&cat.name,token);
		cat.parentid = catid;
		cat.level = level;
		cat.type = type;
		catid = dbctg.duplicated(&cat);
		if(catid == -1){
			dbctg.append(&cat);
			catid = cat.id;
			level = cat.level + 1;
			type = cat.type;
		}else{
			LPCATEGORY lpc = dbctg.category(catid);
			level = lpc->level + 1;
			type = lpc->type;
		}
		if(level >= 2) break;
		token = C::_wcstok(NULL,L" - ");
	}
	return catid;
}

bool ImExport::csv_import_paraseline(LPWSTR l,LPCTSTR seps, bool bexcel){
	TRANSACT record;
	wchar_t* token;
	int step = CSV_IMP_DATE;
	token = C::_wcstok(l,seps);

	db_connection *dbcon = createDatabaseOjbect();
	db_transaction dbtrans(*dbcon);
	db_account dbacnt(*dbcon);
	db_person dbpsn(*dbcon);

	while(token){
		processToken(token);	//omit \"\"
		switch(step){
				case CSV_IMP_DATE:
					{
						int y, d, m;
						if(bexcel){
							//兼容excel日期格式
							swscanf(token,L"%d/%d/%d",&y,&m,&d);
						}else{
							swscanf(token,L"%04d-%02d-%02d",&y,&m,&d);
						}
						//检测时间正确性
						if(y <= 1904 || m <= 0 || m > 12 ||
							d <= 0 || d > 31){ return false; }
						wchar_t *datestr = new wchar_t[64];
						wsprintf(datestr,L"%04d-%02d-%02d 00:00:00",y,m,d);
						record.date = datestr;
						step = CSV_IMP_AMOUNT;
						break;
					}
				case CSV_IMP_AMOUNT:
					{
						double d_amount = 0.0;
						swscanf(token,L"%lf",&d_amount);
						record.amount = (int)(d_amount * 100);
						step = CSV_IMP_ACCOUNT;
						break;
					}
				case CSV_IMP_ACCOUNT:
					{
						if(lstrlen(token) == 0){ return false; } //账户名称长度错误
						ACCOUNT a;
						C::newstrcpy(&a.name, token);
						int retid = dbacnt.duplicated(&a);
						if(retid != -1){	//账户存在
							record.accountid = retid;
						}else{	//账户不存在，添加
							a.initval = 0;
							a.isfix = 0;
							a.note = L"\0";
							dbacnt.append(&a);
							record.accountid = a.id;
						}
						step = CSV_IMP_TOACCOUNT;
						break;
					}
				case CSV_IMP_TOACCOUNT:
					{
						if(lstrlen(token) == 0){
							record.toaccountid = -1;
						}else{
							ACCOUNT a;
							C::newstrcpy(&a.name, token);
							int retid = dbacnt.duplicated(&a);
							if(retid != -1){	//账户存在
								record.toaccountid = retid;
							}else{	//账户不存在，添加
								a.initval = 0;
								a.isfix = 0;
								a.note = L"\0";
								dbacnt.append(&a);
								record.toaccountid = a.id;
							}
							record.isTransfer = 1;
						}
						step = CSV_IMP_CATEGORY;
						break;
					}
				case CSV_IMP_CATEGORY:
					{
						record.categoryid = 
							csv_import_parasecategory(token,record.isTransfer);
						step = CSV_IMP_PERSON;
						break;
					}
				case CSV_IMP_PERSON:
					{
						PERSON person;
						C::newstrcpy(&person.name,token);
						person.id = 0;
						person.type = PRSN_FAMILY;
						int retid = dbpsn.duplicated(&person);
						if(retid != -1){ //人员存在
							record.personid = retid;
						}else{
							dbpsn.append(&person);
							record.personid = person.id;
						}
						step = CSV_IMP_NOTE;
						break;
					}
				case CSV_IMP_NOTE:
					{
						if(lstrlen(token)){
							wchar_t *snote = new wchar_t[1024];
							C::newstrcpy(&record.note,token);
							C::restoreWrap(snote,record.note);
							delete[] record.note;
							record.note = snote;
						}else{
							record.note = L"\0";
						}
						step = CSV_IMP_END;
						break;
					}
				case CSV_IMP_END:
					{
						if(dbtrans.duplicated(&record) == -1){
							dbtrans.append(&record);
							return true;
						}else{
							return false;
						}
						break;
					}
				default:
					return false;
		}
		token = C::_wcstok(NULL,seps);
	}
	return false;
}

bool ImExport::csv_import_validtitle(LPWSTR t,ImExType type,bool &bexcel){
	bexcel = false;
	int lngid;
	if(type == IMEX_CSV_S){
		lngid = IDS_TRANS_TITLE_S;
	}else{
		lngid = IDS_TRANS_TITLE_C;
	}
	if(lstrcmp(getLngResString(lngid),t) != 0){
		if(lstrcmp(getLngResString(IDS_TRANS_TITLE_EXCEL_C),t) != 0){
			wsprintf(_lastErrMsg,getLngResString(IDS_ERR_CSV_FORMAT).C_Str());
			return false;
		}else{
			bexcel = true;
		}
	}
	return true;
}

bool ImExport::csv_import(wchar_t* filename,int &succed, int &omited, int &failed){
	//打开数据库
	db_connection *dbcon = createDatabaseOjbect();
	if(dbcon == NULL){
		wsprintf(_lastErrMsg,getLngResString(IDS_ERR_DB_CONNECTION).C_Str());
		return false;
	}

	TEXTENCODE_t enc = File::getTextCode(filename);
	if(enc != ttcAnsi && 
		enc != ttcUnicode &&
		enc != ttcUnicodeBigEndian){
			wsprintf(_lastErrMsg,getLngResString(IDS_ERR_UNK_FORMAT).C_Str());
			return false;
	}

	m_Progressdlg.SetShowTitle(true);
	m_Progressdlg.SetShowInfo(true);
	m_Progressdlg.SetRange(0,100);
	m_Progressdlg.SetTitle(getLngResString(IDS_IMPORT_WAIT).C_Str());
	m_Progressdlg.SetInfo(getLngResString(IDS_IMPORT_ANALYSIS).C_Str());
	m_Progressdlg.BeginProgress(m_hWnd);


	wchar_t* seps = _filetype == IMEX_CSV_S ? L"\t" : L",";
	bool bline0 = true;	//if is the first line
	bool bexcel;
	int nitems = 0;
	int nbytes = 0;
	int nPercent = 0;
	int _nPercent = 0;

	if(enc == ttcAnsi){
        ifstream file;
        file.open(filename,  ios::in | ios::binary);
        if (file.is_open())
        {
            file.seekg(0, ios::end);
            unsigned int nLen = file.tellg();
            char *sbuf = new char[nLen+1];
            file.seekg(0, ios::beg);
			while(!file.eof()){
                if(file.getline(sbuf,nLen).good()){
                    nbytes += strlen(sbuf);
                    wchar_t *wss;
                    File::chr2wch(sbuf,&wss);
					if(bline0){
						bline0 = false;
						if(!csv_import_validtitle(wss,_filetype,bexcel)){
							wsprintf(_lastErrMsg,getLngResString(IDS_ERR_IMPORT_FORMAT).C_Str());
							m_Progressdlg.EndProgress();
							delete wss;
							return false;
						}
					}
                    if(csv_import_paraseline(wss,seps,bexcel)){
                        nitems++;
                        nPercent = 100 * nbytes / nLen;
                        if(nPercent != _nPercent){
                            _nPercent = nPercent;
                            wchar_t info[32];
                            wsprintf(info,L"读入%d条记录...%d%%",nitems,nPercent);
                            m_Progressdlg.SetInfo(info);
                            m_Progressdlg.SetCurValue(100 * nbytes / nLen);
                            m_Progressdlg.UpdateProgress();
                            DateTime::waitms(1);
                        }
                    }
					delete wss;
                }else{
                    //错误发生
                }
			}
			delete[] sbuf;
        }
        file.close();
	}else if(enc == ttcUnicode ||
		enc == ttcUnicodeBigEndian){
		wifstream ofile;
        ofile.open(filename, ios::in | ios ::binary);
        if (ofile.is_open())
        {
            ofile.seekg(0, ios::end);
            unsigned int nLen = ofile.tellg();
            wchar_t *sbuf = new wchar_t[nLen+1];
            ofile.seekg(2, ios::beg);
            while(!ofile.eof()){
                if(ofile.getline(sbuf,nLen).good()){
                    nbytes += lstrlen(sbuf);
 					if(bline0){
						bline0 = false;
						if(!csv_import_validtitle(sbuf,_filetype,bexcel)){
							wsprintf(_lastErrMsg,getLngResString(IDS_ERR_IMPORT_FORMAT).C_Str());
							m_Progressdlg.EndProgress();
							return false;
						}
					}
                   if(csv_import_paraseline(sbuf,seps,bexcel)){
                        nitems++;
                        wchar_t info[32];
                        wsprintf(info,L"读入记录中...%d",nitems);
                        m_Progressdlg.SetInfo(info);
                        m_Progressdlg.SetCurValue(80 * nbytes / nLen);
                        m_Progressdlg.UpdateProgress();
                        DateTime::waitms(1);
                    }
                }else{
                    //错误发生
                }
            }
            delete [] sbuf;
		}
        ofile.close();
	}

	wsprintf(_lastErrMsg,
		getLngResString(IDS_IMPORT_RESULT).C_Str(),nitems);
	m_Progressdlg.EndProgress();
	return true;
}