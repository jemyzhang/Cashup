#include "imexport.h"

#include "ui_imexport.h"
#include <UiFileDialogWnd.h>
#include <InitGuid.h>
#include <IMzUnknown.h>
#include <IMzUnknown_IID.h>
#include <IFileBrowser.h>
#include <IFileBrowser_GUID.h>

#include <MzCommon.h>
using namespace MzCommon;

extern HINSTANCE AfxGetInstanceHandle();
static HINSTANCE lngres = 0;

CMzString getLngResString(int strid){
	if(lngres == 0){
		lngres = AfxGetInstanceHandle();
	}
	return MzLoadString(strid,lngres);
}


ImExport::ImExport(HWND parent)
: m_hWnd(parent)
{
	wsprintf(_lastErrMsg,L"Not Started Yet~");
	m_Progressdlg.SetRoundRect(true);
//	m_Progressdlg.SetShowInfo(true);
	m_Progressdlg.SetShowTitle(true);
	_filenotsel = false;
	qifdt = NULL;
}

ImExport::~ImExport(void)
{
}

bool ImExport::excute(int t){
	if(pldb == 0){
		wsprintf(_lastErrMsg,L"数据库句柄错误。");
		return false;
	}

	bool ret;

	processOption(t);

	IFileBrowser *pFile = NULL;                      
	CoInitializeEx(NULL, COINIT_MULTITHREADED );
	if(_isImport){
		IMzSelect *pSelect = NULL; 
		if ( SUCCEEDED( CoCreateInstance( CLSID_FileBrowser, NULL,CLSCTX_INPROC_SERVER ,IID_MZ_FileBrowser,(void **)&pFile ) ) )
		{     
			if( SUCCEEDED( pFile->QueryInterface( IID_MZ_Select, (void**)&pSelect ) ) )
			{
				TCHAR file[ MAX_PATH ] = { 0 };
				pFile->SetParentWnd( m_hWnd );
				pFile->SetOpenDirectoryPath( L"\\Disk" );
				pFile->SetExtFilter( (_filetype == PROCESS_FILE_CSV_S || (_filetype == PROCESS_FILE_CSV_C)) ? L"*.csv" : L"*.qif" );                                      
				pFile->SetOpenDocumentType(DOCUMENT_SELECT_SINGLE_FILE);
				std::wstring fileDlgTitle = L"请选择需要导入的";
				fileDlgTitle += ((_filetype == PROCESS_FILE_CSV_S || (_filetype == PROCESS_FILE_CSV_C)) ? L"CSV文件" : L"QIF文件");
				pFile->SetTitle((TCHAR*)fileDlgTitle.c_str());
				fileDlgTitle.clear();

				if( pSelect->Invoke() ) 
				{						
					_tcscpy( file, pFile->GetSelectedFileName() );
					//process import action
					int n,o,f;
					if(_filetype == PROCESS_FILE_CSV_S || (_filetype == PROCESS_FILE_CSV_C)){
						ret = csv_import(file,n,o,f);
					}else{
						ret = qif_import(file,n,o,f);
					}
				}else{
					_filenotsel = true;
					return false;
				}
				pSelect->Release();
			}     
			pFile->Release();
		}	
		CoUninitialize();
		
	}else{
		UiFileDialogWnd dlg;
		dlg.SetTitle(L"请设置导出文件名");
		dlg.SetInitFileName(_fname);
		if(_filetype == PROCESS_FILE_CSV_S || (_filetype == PROCESS_FILE_CSV_C)){
			dlg.SetInitFileSuffix(L".csv");
		}else{
			dlg.SetInitFileSuffix(L".qif");
		}
		dlg.SetInitFolder(L"\\Disk");
        RECT rcWork = MzGetWorkArea();
        dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
                m_hWnd, 0, WS_POPUP);
		if( dlg.DoModal() == ID_OK ) 
		{
			DateTime::waitms(500);
			//process file export
			int n;
			if(_filetype == PROCESS_FILE_CSV_S || (_filetype == PROCESS_FILE_CSV_C)){
				ret = csv_export(dlg.GetFullFileName(),n);
			}else{
				ret = qif_export(dlg.GetFullFileName(),n);
			}
		}else{
			_filenotsel = true;
			return false;
		}
	}
	return ret;
}

void ImExport::processOption(int t){
	_isImport = ((t&0x000f) == PROCESS_IMPORT);
	_filetype = (t&0x00f0);
	_process = (t&0x0f00);
	if(!_isImport){
		switch(_process){
			case PROCESS_ACCOUNT:
				wsprintf(_fname,LOADSTRING(IDS_STR_EX_ACCOUNT_FILENAME).C_Str(),L"导出",DateTime::NowtoStr());
				break;
			case PROCESS_CATEGORY:
				wsprintf(_fname,LOADSTRING(IDS_STR_EX_CATEGORY_FILENAME).C_Str(),L"导出",DateTime::NowtoStr());
				break;
			case PROCESS_RECORD:
				if(_dateAll){
					wsprintf(_fname,LOADSTRING(IDS_STR_EX_TRANS_ALL_FILENAME).C_Str(),L"导出",DateTime::NowtoStr());
				}else{
					ExportDate_t d1, d2;
					d1.Value = _sdate;
					d2.Value = _edate;
					if(_sdate == _edate){
						wsprintf(_fname,LOADSTRING(IDS_STR_EX_TRANS_D1_FILENAME).C_Str(),
							d1.Date.Year,d1.Date.Month,d1.Date.Day,L"导出");
					}else{
						wsprintf(_fname,LOADSTRING(IDS_STR_EX_TRANS_D2_FILENAME).C_Str(),
							d1.Date.Year,d1.Date.Month,d1.Date.Day,
							d2.Date.Year,d2.Date.Month,d2.Date.Day,L"导出");
					}
				}
				break;
			default:
				wsprintf(_fname,LOADSTRING(IDS_STR_EX_UNK_FILENAME).C_Str(),DateTime::NowtoStr(),L"导出");
				break;
		}
	}
}

wchar_t* ImExport::processToken(wchar_t* &token){
	if(token[0] == '\"'){
		token++;
	}
	int endpos = lstrlen(token);
	if(endpos > 0){
		endpos -= 1;
		if(token[endpos] == '\"'){
			token[endpos] = '\0';
		}
	}
	return token;
}


