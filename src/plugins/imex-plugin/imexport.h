#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include <Mzfc/MzProgressDialog.h> 
#include <db-engine.h>

//#define DEF_RECORD_TITLE_FORMAT_S L"日期\t金额\t账户\t转至账户\t分类\t备注"
#define DEF_TRANSACTION_FORMAT_S L"%s\t%.2f\t%s\t%s\t%s\t%s\t%s"

//#define DEF_RECORD_TITLE_FORMAT_C L"\"日期\",\"金额\",\"账户\",\"转至账户\",\"分类\",\"备注\""
#define DEF_TRANSACTION_FORMAT_C L"\"%s\",\"%.2f\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\""

typedef enum tagImExType{
	IMEX_NONE		=	0x00,

	IMEX_EXPORT		=	0x0001,
	IMEX_IMPORT		=	0x0002,

	IMEX_CSV_S	=	0x0010,	//csv with space
	IMEX_CSV_C	=	0x0020,	//csv with comma
	IMEX_QIF	=	0x0040,
}ImExType;

extern CMzString getLngResString(int strid);

class ImExport
{
public:
	ImExport(HWND parenthWnd);
	~ImExport(void);
public:

	//process type
	bool excute(int t = (IMEX_EXPORT | IMEX_CSV_C));	//start process
	wchar_t* getLastErrMsg() { return _lastErrMsg; }
	bool isNoFileSelected() { return _filenotsel; }
	void setExportDateRange(bool isAll = true, int ds = 0, int de = 0){
		_dateAll = isAll;
		_sdate.Value = ds;
		_edate.Value = de;
	}
protected:
	bool csv_import(wchar_t* file,int &succed, int &omited, int &failed);
	bool csv_export(wchar_t* path,int &n);
	bool qif_import(wchar_t* file,int &succed, int &omited, int &failed);
	bool qif_export(wchar_t* path,int &n);
private:
	//确认第一行是否为有效标题行
	//是则继续，否则退出
	//type csv文件类型
	bool csv_import_validtitle(LPWSTR t,ImExType type,bool &bexcel);
	//分析csv行内容
	bool csv_import_paraseline(LPWSTR l,LPCTSTR sep, bool bexcel);
	//返回最后一个有效的category id
	//bTransfer 记录是否为转账
	int csv_import_parasecategory(LPWSTR c,  bool bTransfer);
	LPWSTR qif_date(LPCTSTR record_date);
private:
	void processOption(int t);
	//处理导入时字符串中的双引号
	wchar_t* processToken(wchar_t* &token);
private:
	bool _isImport;
	ImExType _filetype;
	int _process;
	bool _filenotsel;
	//导出日期范围
	bool _dateAll;
	TRSDATE _sdate;
	TRSDATE _edate;
private:
	MzProgressDialog m_Progressdlg;
	wchar_t _lastErrMsg[256];
	wchar_t _fname[128];
	HWND m_hWnd;
	LPWSTR qifdt;

};
