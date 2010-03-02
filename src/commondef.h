#define IDC_MODULE_BEGIN 200
#define IDC_PLUGIN_BEGIN 400

#define SZ_LOGO_HEIGHT 129

#define MZ_MW_CUSTOM_MSG  0xFFF20000
//parent -> child
#define MZ_MW_REQ_FROM_PARENT    MZ_MW_CUSTOM_MSG

#define MZ_MW_REQ_CHILDWND_QUIT  MZ_MW_REQ_FROM_PARENT + 0	/*wParam: HWND*/
#define MZ_MW_REQ_CHILDWND_SHOW MZ_MW_REQ_FROM_PARENT + 1

//child -> parent
#define MZ_MW_REQ_FROM_CHILD    MZ_MW_CUSTOM_MSG + 10

#define MZ_MW_REQ_CHANGE_TITLE  MZ_MW_REQ_FROM_CHILD + 0	/*wParam: Language String ID*/
#define MZ_MW_REQ_CHANGE_TOPWND  MZ_MW_REQ_FROM_CHILD + 1	/*wParam: HWND*/
#define MZ_MW_ACK_WND_ISQUIT  MZ_MW_REQ_FROM_CHILD + 2	/*window is quit, wParam: HWND*/

#ifdef _DEBUG
#define dbg_printf printf
#else
#define dbg_printf(s,...)
#endif

#define ui_out(s,...) dbg_printf("%s:%s:%d: "s"\n",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);
