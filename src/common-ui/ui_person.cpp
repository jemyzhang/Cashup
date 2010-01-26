#include <cMzCommon.h>
using namespace cMzCommon;
#include "ui_person.h"
#include "resource.h"

extern CMzString getLngResString(int nID);
static db_connection* dbcon = 0;

// Popup window derived from CMzWndEx
class PersonList : public UiList {
public:
    // override the DrawItem member function to do your own drawing of the list
	PersonList() { 
        showtype = PRSN_FAMILY; 
        SetItemHeight(50); 
        SetTextSize(25);
    }
	void DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate);
    void UpdatePersons(){
        RemoveAll();
        SetSelectedIndex(-1);
        if(dbcon == 0) dbcon = ::createDatabaseOjbect();
        if(dbcon == 0) return;
        db_person dbpsn(*dbcon);
        dbpsn.query_type(showtype);

        int sz = dbpsn.query_size();
        for(int i = 0; i < sz; i++){
            LPPERSON p = dbpsn.query_at(i);
            ListItem li;
            li.Data = (void*)p->id;
            li.Text = p->name;
            AddItem(li);
        }
        ScrollTo(UI_SCROLLTO_TOP,0,false);
    }
    PERSON_TYPE GetShowType() { return showtype; }
    bool SetShowType(PERSON_TYPE t) { 
        if(showtype != t){
            showtype = t;
            return true;
        }
        return false;
    }
    int GetSelectedID(){
        int selidx = GetSelectedIndex();
        if(selidx == -1) return -1;
        return (int)GetItem(selidx)->Data;
    }
private:
    PERSON_TYPE showtype;
};
void PersonList::DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate) {
    // draw the high-light background for the selected item
    if (nIndex == GetSelectedIndex()) {
        MzDrawSelectedBg(hdcDst, prcItem);
    }

    COLORREF cr = RGB(0,0,0);       
    HFONT hf;
	RECT rcText = *prcItem;
	RECT Rect01;
	RECT Rect02;

	//第一块区域 显示图标 暂时不使用
	Rect01.left = rcText.left;
	Rect01.top = rcText.top;
	Rect01.right = rcText.left + 32;
	Rect01.bottom = rcText.bottom;
	//第二块区域 显示名称
	Rect02.left = Rect01.right + 10;
	Rect02.top = Rect01.top;
	Rect02.right = rcText.right - 20;
	Rect02.bottom = rcText.bottom;

	//图标
	//RECT rectImage = {prcItem->right - (imgArrow->GetImageWidth()<<1),prcItem->top,prcItem->right,prcItem->bottom};
	//imgArrow->Draw(hdcDst,&rectImage);
	//名称
	ListItem *pl = GetItem(nIndex);
	cr = RGB(0,0,0);
	::SetTextColor( hdcDst , cr );
	hf = FontHelper::GetFont( 25 );
	SelectObject( hdcDst , hf );
	MzDrawText( hdcDst , pl->Text , &Rect02 , DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );
	DeleteObject( hf );

}

#define MZ_IDC_PERSON_LIST 101
#define MZ_IDC_BUTTONBAR 102

MZ_IMPLEMENT_DYNAMIC(Ui_PersonsWnd)

Ui_PersonsWnd::~Ui_PersonsWnd(){
    delete m_pList;
}

BOOL Ui_PersonsWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!Ui_BaseWnd::OnInitDialog()) {
        return FALSE;
    }
	int y = 0;
    m_PersonTypeBar.SetID(MZ_IDC_BUTTONBAR);
    m_PersonTypeBar.SetPos(0, 0, GetWidth(), MZM_HEIGHT_BUTTONBAR);
    AddUiWin(&m_PersonTypeBar);

    y+=MZM_HEIGHT_BUTTONBAR;
    m_pList = new PersonList;
    m_pList->SetPos(0, y, GetWidth(), GetHeight() - MZM_HEIGHT_BUTTONBAR);
    m_pList->SetID(MZ_IDC_PERSON_LIST);
    m_pList->EnableScrollBarV(true);
    m_pList->EnableNotifyMessage(true);
    AddUiWin(m_pList);
	return TRUE;
}

void Ui_PersonsWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
        case MZ_IDC_BUTTONBAR:
        {
            int index = lParam;

            if(m_pList->SetShowType((PERSON_TYPE)index)){
                m_pList->UpdatePersons();
            }
        }
    }
}

LRESULT Ui_PersonsWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case MZ_WM_MOUSE_NOTIFY:
        {
            int nID = LOWORD(wParam);
            int nNotify = HIWORD(wParam);
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
			if (nID == MZ_IDC_PERSON_LIST && nNotify == MZ_MN_LBUTTONUP) {
				if (!m_pList->IsMouseDownAtScrolling() && !m_pList->IsMouseMoved()) {
					int nIndex = m_pList->CalcIndexOfPos(x, y);
					if(nIndex != -1 && nIndex == _sel_idx){
						_selection = m_pList->GetSelectedID();
						EndModal(ID_OK);
						return 0;
					}
					_sel_idx = nIndex;
					m_pList->SetSelectedIndex(nIndex);
					m_pList->Invalidate();
				}
				return 0;
			}
            if (nID == MZ_IDC_PERSON_LIST && nNotify == MZ_MN_MOUSEMOVE) {
				_sel_idx = -1;
                m_pList->SetSelectedIndex(-1);
                m_pList->Invalidate();
               return 0;
            }
       }
    }
    return Ui_BaseWnd::MzDefWndProc(message, wParam, lParam);
}

void Ui_PersonsWnd::DelayShow() {
    SetupPersonTypeBar();  
    m_pList->UpdatePersons();
}

void Ui_PersonsWnd::SetupPersonTypeBar(){
    if(dbcon == 0) dbcon = ::createDatabaseOjbect();
    if(dbcon == 0) return;
    db_person dbpsn(*dbcon);
    int sz = 0;
    wchar_t* name;
    while((name = dbpsn.type_name(sz))){
        m_PersonTypeBar.SetButton(sz,true,true,name);
        sz++;
    }
    m_PersonTypeBar.Invalidate();
}

