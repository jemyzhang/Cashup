#include "imexport.h"
#include "resource.h"

bool ImExport::qif_import(wchar_t* file,int &succed, int &omited, int &failed){
	wsprintf(_lastErrMsg,getLngResString(IDS_ERR_FUNC_INCOMPLETE).C_Str());
	return false;
}