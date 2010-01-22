#define IDC_MODULE_BEGIN 200
#define IDC_PLUGIN_BEGIN 400

#define SZ_LOGO_HEIGHT 129

#define MZ_MW_CUSTOM_MSG  0xFFF20000
#define MZ_MW_CHANGE_TITLE  MZ_MW_CUSTOM_MSG	/*wParam: Language String ID*/
#define MZ_MW_CHANGE_TOPWND  MZ_MW_CUSTOM_MSG + 1	/*wParam: HWND*/
#define MZ_MW_CHILDWND_QUIT  MZ_MW_CUSTOM_MSG + 2	/*wParam: HWND*/
#define MZ_MW_CHILDWND_SHOW MZ_MW_CUSTOM_MSG + 3

#ifdef _DEBUG
#define ui_out(s,...) printf("%s:%s:%d: "s"\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);
#else
#define ui_out(s,...)
#endif
