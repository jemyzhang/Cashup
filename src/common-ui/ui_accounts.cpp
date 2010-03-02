#include <cMzCommon.h>
using namespace cMzCommon;
#include "ui_accounts.h"
#include "resource.h"
#include <commondef.h>

static db_connection* dbcon = 0;
extern CMzString getLngResString(int nID);
extern HINSTANCE hHandle;
#pragma comment(lib,"db-engine.lib")

class AccountList : public UiList {
public:
    // override the DrawItem member function to do your own drawing of the list
	AccountList() { idlist = 0; }
	void DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate);
	void setupList(int* i) { idlist = i; }
private:
	int* idlist;
};

void AccountList::DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate) {
    // draw the high-light background for the selected item
    if (nIndex == GetSelectedIndex()) {
        MzDrawSelectedBg(hdcDst, prcItem);
    }

    // draw the text
	if(idlist == 0) return;
    if(dbcon == 0) dbcon = ::createDatabaseOjbect();
    if(dbcon == 0) return;
    db_account dbacnt(*dbcon);

    LPACCOUNT a = dbacnt.account(idlist[nIndex]);

    COLORREF cr = RGB(0,0,0);       
    HFONT hf;
	RECT rcText = *prcItem;
	RECT Rect01;
	RECT Rect02;
	RECT Rect03;

	int lineHeight = rcText.bottom - rcText.top;
	lineHeight /= 2;
	//第一块区域 第一行 显示账户名称
	Rect01.left = rcText.left + 10;
	Rect01.top = rcText.top;
	Rect01.right = rcText.right - 200;
	Rect01.bottom = rcText.top + lineHeight;
	//第二块区域 第一行 显示盈余
	Rect02.left = rcText.right - 200;
	Rect02.top = rcText.top;
	Rect02.right = rcText.right - 20;
	Rect02.bottom = rcText.top + lineHeight;
	//第三块区域 第二行 显示备注
	Rect03.left = rcText.left + 10;
	Rect03.top = rcText.top + lineHeight;
	Rect03.right = rcText.right - 10;
	Rect03.bottom = rcText.bottom;

	//账户名称
	cr = RGB(0,0,0);
	::SetTextColor( hdcDst , cr );
	hf = FontHelper::GetFont( 25 );
	SelectObject( hdcDst , hf );
	MzDrawText( hdcDst , a->name , &Rect01 , DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );
	DeleteObject( hf );
	//盈余
	hf = FontHelper::GetFont( 20 );

	SelectObject( hdcDst , hf );
	ListItem *item = GetItem(nIndex);
	if(item->Data != 0){
		cr = RGB(128,64,64);
	}else{
		cr = RGB(64,128,64);
	}
	::SetTextColor( hdcDst , cr );
	MzDrawText( hdcDst , item->Text.C_Str() , &Rect02 , DT_RIGHT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );

	//备注
	cr = RGB(128,128,128);
	::SetTextColor( hdcDst , cr );
	SelectObject( hdcDst , hf );
	MzDrawText( hdcDst , a->note , &Rect03 , DT_RIGHT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );

	DeleteObject( hf );
}

#define MZ_IDC_ACCOUNT_LIST 101

MZ_IMPLEMENT_DYNAMIC(Ui_AccountsWnd)

Ui_AccountsWnd::Ui_AccountsWnd() {
    _mode = 0;
    idarray = 0;
    _sel_idx = -1;
}

Ui_AccountsWnd::~Ui_AccountsWnd() {
    if(idarray) delete[] idarray;
    delete m_pList;
}

BOOL Ui_AccountsWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!Ui_BaseWnd::OnInitDialog()) {
        return FALSE;
    }
	int y = 0;

    m_pList = new AccountList;
    m_pList->SetPos(0, y, GetWidth(), GetHeight());
    m_pList->SetID(MZ_IDC_ACCOUNT_LIST);
    m_pList->EnableScrollBarV(true);
    m_pList->EnableNotifyMessage(true);
    AddUiWin(m_pList);

    ::PostMessage(GetParent(),MZ_MW_REQ_CHANGE_TITLE,IDS_TITLE_ACCOUNTS,(LPARAM)hHandle);
	DateTime::waitms(1);
	return TRUE;
}

void Ui_AccountsWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
}

LRESULT Ui_AccountsWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case MZ_WM_MOUSE_NOTIFY:
        {
            int nID = LOWORD(wParam);
            int nNotify = HIWORD(wParam);
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            if (nID == MZ_IDC_ACCOUNT_LIST && nNotify == MZ_MN_LBUTTONUP) {
                if (!m_pList->IsMouseDownAtScrolling() && !m_pList->IsMouseMoved()) {
                    int nIndex = m_pList->CalcIndexOfPos(x, y);
                    if(nIndex != -1 && nIndex == _sel_idx){
                        _selection = idarray[m_pList->GetSelectedIndex()];
                        EndModal(ID_OK);
                        return 0;
                    }
                    _sel_idx = nIndex;
                    m_pList->SetSelectedIndex(nIndex);
                    m_pList->Invalidate();
                }
                return 0;
            }
            if (nID == MZ_IDC_ACCOUNT_LIST && nNotify == MZ_MN_MOUSEMOVE) {
				_sel_idx = -1;
                m_pList->SetSelectedIndex(-1);
                m_pList->Invalidate();
                return 0;
            }
       }
    }
    return Ui_BaseWnd::MzDefWndProc(message, wParam, lParam);
}

void Ui_AccountsWnd::UpdateUi(){
	m_pList->RemoveAll();

    if(dbcon == 0) dbcon = ::createDatabaseOjbect();
    if(dbcon == 0) return;

    ListItem li;
    CMzString str;
    db_account dbacnt(*dbcon);
    dbacnt.query_all();

    int tz = dbacnt.query_size();
    if(tz <= 0) return;

	if(idarray) delete[] idarray;
	idarray = new int[tz];

	int *p = idarray;
    db_analysis dbanalysis(*dbcon);

	for (int i = 0;i < tz; i++) {
        LPACCOUNT acc = dbacnt.query_at(i);

		wchar_t str[128];
        double balance = dbanalysis.AccountBalanceById(acc->id);
		if(balance < 0){
			li.Data = (void*)1;
		}else{
			li.Data = 0;
		}
		wsprintf(str,getLngResString(IDS_ACCOUNT_BALANCE).C_Str(), balance/100);
		li.Text = str;
		
		m_pList->AddItem(li);
		*(p++) = acc->id;
    }
	m_pList->setupList(idarray);
    m_pList->Invalidate();
}
