#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include <Mzfc/MzProgressDialog.h> 
#include <db-engine.h>

//#define DEF_RECORD_TITLE_FORMAT_S L"����\t���\t�˻�\tת���˻�\t����\t��ע"
#define DEF_TRANSACTION_FORMAT_S L"%s\t%.2f\t%s\t%s\t%s\t%s\t%s"

//#define DEF_RECORD_TITLE_FORMAT_C L"\"����\",\"���\",\"�˻�\",\"ת���˻�\",\"����\",\"��ע\""
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
	//ȷ�ϵ�һ���Ƿ�Ϊ��Ч������
	//��������������˳�
	//type csv�ļ�����
	bool csv_import_validtitle(LPWSTR t,ImExType type,bool &bexcel);
	//����csv������
	bool csv_import_paraseline(LPWSTR l,LPCTSTR sep, bool bexcel);
	//�������һ����Ч��category id
	//bTransfer ��¼�Ƿ�Ϊת��
	int csv_import_parasecategory(LPWSTR c,  bool bTransfer);
	LPWSTR qif_date(LPCTSTR record_date);
private:
	void processOption(int t);
	//������ʱ�ַ����е�˫����
	wchar_t* processToken(wchar_t* &token);
private:
	bool _isImport;
	ImExType _filetype;
	int _process;
	bool _filenotsel;
	//�������ڷ�Χ
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
