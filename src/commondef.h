#define IDC_MODULE_BEGIN 200
#define IDC_PLUGIN_BEGIN 400

#ifndef IDB_PNG_BG
#define IDB_PNG_BG 104
#endif

#define SZ_LOGO_HEIGHT 129

#define MZ_MW_CUSTOM_MSG  0xFFF20000
#define MZ_MW_CHANGE_TITLE  MZ_MW_CUSTOM_MSG	/*wParam: Language String ID*/
#define MZ_MW_CHANGE_TOPWND  MZ_MW_CUSTOM_MSG + 1	/*wParam: HWND*/
#define MZ_MW_CHILDWND_QUIT  MZ_MW_CUSTOM_MSG + 2	/*wParam: HWND*/
#define MZ_MW_CHILDWND_SHOW MZ_MW_CUSTOM_MSG + 3
