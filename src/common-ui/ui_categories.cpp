#include <cMzCommon.h>
using namespace cMzCommon;
#include "ui_categories.h"
#include "resource.h"
#include <db-engine.h>
static db_connection* dbcon = 0;
extern ImagingHelper *getResImage(int nID);
extern CMzString getLngResString(int nID);

// Popup window derived from CMzWndEx
class CategoryList : public UiList {
public:
    // override the DrawItem member function to do your own drawing of the list
	CategoryList() { m_btn = 0; showtype = CT_OUTGOING; showChild = true;}

    void DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate);

    void SetupButton(UiButton *b){ m_btn = b; }
    void SetChildViewStatus(bool s) { showChild = s; }
    TRANSACT_TYPE GetShowType() { return showtype; }
    bool SetShowType(TRANSACT_TYPE t) { 
        if(showtype != t){
            showtype = t;
            return true;
        }
        return false;
    }
    bool ToggleViewStatus(int index){
        if(index == -1) return false;
        int cid = (int)GetItem(index)->Data;
        if(dbcon == 0) dbcon = ::createDatabaseOjbect();
        if(dbcon == 0) return false;
        db_category dbctg(*dbcon);
        LPCATEGORY cat = dbctg.category(cid);
        if(cat->level != 1) return false;
        dbctg.setstatus(cid,!cat->show);
        return true;
    }
    void UpdateCategories(){
        RemoveAll();
        SetSelectedIndex(-1);
        m_btn->SetVisible(false);

        if(dbcon == 0) dbcon = ::createDatabaseOjbect();
        if(dbcon == 0) return;
        db_category dbctg(*dbcon);
        dbctg.query_type(showtype);

        int sz = dbctg.query_size();
        if(sz > 0){
            for(int i = 0; i < sz; i++){
                LPCATEGORY c = dbctg.query_at(i);
                if(c->level == 1){
                    ListItem li;
                    li.Data = (void*)c->id;
                    AddItem(li);
                    if(showChild && c->show){
                        for(int j = i+1; j < sz; j++){
                            LPCATEGORY c2 = dbctg.query_at(j);
                            if(c2->level == 2 && c2->parentid == c->id){
                                ListItem li;
                                li.Data = (void*)c2->id;
                                AddItem(li);
                            }
                        }
                    }
                }
            }
        }
        ScrollTo(UI_SCROLLTO_TOP,0,false);
        //Invalidate();
    }
    int GetSelectedID(){
        int selidx = GetSelectedIndex();
        if(selidx == -1) return -1;
        return (int)GetItem(selidx)->Data;
    }
private:
	UiButton *m_btn;
    TRANSACT_TYPE showtype;
    bool showChild; //创建新子类的时候，最下层child不能显示出来
};

void CategoryList::DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate) {
    // draw the high-light background for the selected item
    if (nIndex == GetSelectedIndex()) {
        MzDrawSelectedBg(hdcDst, prcItem);
		if(m_btn){
			m_btn->SetPos(prcItem->right - m_btn->GetWidth() - 10,
				prcItem->top + 5 - prcWin->top,
				0,0,SP_NOSIZE);
			m_btn->SetVisible(true);
			AddChild(m_btn);
		}
    }

    // draw the text
    ListItem *pli = GetItem(nIndex);
    int cid = (int)pli->Data;

    if(dbcon == 0) dbcon = ::createDatabaseOjbect();
    if(dbcon == 0) return;
    db_category dbctg(*dbcon);
    LPCATEGORY cat = dbctg.category(cid);
    if(cat == NULL){
        RemoveItem(nIndex);
        return;
    }

	RECT rcText = *prcItem;
	int spacenum = cat->level - 1;
	rcText.left = rcText.left + 45 + 30 * spacenum;
	MzDrawText( hdcDst , cat->name , &rcText , DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );

	// setup button
    if(!showChild) return;
	if(cat->level == 1){
        int pngid = IDB_PNG_COLLAPSED;
        if(cat->show){
            pngid = IDB_PNG_EXPANED;
        }
    	RECT rcImg = rcText;
        rcImg.left = rcText.left - 10 - 16;
        rcImg.right = rcText.left - 10;
        rcImg.top = prcItem->top + 17;
        rcImg.bottom = prcItem->top + 17 + 16;
        ImagingHelper *img =getResImage(pngid);
        img->Draw(hdcDst,&rcImg);
    }
}

#define MZ_IDC_BUTTONBAR 101
#define MZ_IDC_CATEGORY_LIST 102
#define MZ_IDC_BTN_LIST_SELECT 103

MZ_IMPLEMENT_DYNAMIC(Ui_CategoriesWnd)

Ui_CategoriesWnd::~Ui_CategoriesWnd(){
    if(m_pList) delete m_pList;
}

void Ui_CategoriesWnd::SetupCategoryTypeBar(){
    if(dbcon == 0) dbcon = ::createDatabaseOjbect();
    if(dbcon == 0) return;
    db_category dbctg(*dbcon);
    dbctg.query_main();
    for(int i = 0; i < dbctg.query_size(); i++){
        m_CategoryTypeBar.SetButton(i,true,true,dbctg.query_at(i)->name);
    }
    m_CategoryTypeBar.Invalidate();
}

void Ui_CategoriesWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
		case MZ_IDC_BTN_LIST_SELECT:
		{
			int nID = m_pList->GetSelectedID();
			if(nID == -1) return;
			_selection = nID;
			EndModal(ID_OK);
			return;
		}
        case MZ_IDC_BUTTONBAR:
        {
            int index = lParam;

            if(dbcon == 0) dbcon = ::createDatabaseOjbect();
            if(dbcon == 0) return;
            db_category dbctg(*dbcon);
            dbctg.query_main();
            TRANSACT_TYPE t = dbctg.query_at(index)->type;
            if(m_pList->SetShowType(t)){
                m_pList->UpdateCategories();
            }
        }
    }
}

BOOL Ui_CategoriesWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!Ui_BaseWnd::OnInitDialog()) {
        return FALSE;
    }
	int y = 0;
    m_CategoryTypeBar.SetID(MZ_IDC_BUTTONBAR);
    m_CategoryTypeBar.SetPos(0, 0, GetWidth(), MZM_HEIGHT_BUTTONBAR);
    AddUiWin(&m_CategoryTypeBar);

    y+=MZM_HEIGHT_BUTTONBAR;
    m_pList = new CategoryList;
    m_pList->SetPos(0, y, GetWidth(), GetHeight() - MZM_HEIGHT_BUTTONBAR);
	m_pList->SetItemHeight(50);
	m_pList->SetTextSize(25);
    m_pList->SetID(MZ_IDC_CATEGORY_LIST);
    m_pList->EnableScrollBarV(true);
    m_pList->EnableNotifyMessage(true);
    AddUiWin(m_pList);

	m_BtnSelect.SetPos(0,0,60,45);
	m_BtnSelect.SetButtonType(MZC_BUTTON_DOWNLOAD);
	m_BtnSelect.SetText(getLngResString(IDS_SELECT).C_Str());
	m_BtnSelect.SetVisible(false);
	m_BtnSelect.SetTextColor(RGB(64,64,128));
	m_BtnSelect.SetID(MZ_IDC_BTN_LIST_SELECT);
	//AddUiWin(&m_BtnSelect);
	m_pList->SetupButton(&m_BtnSelect);
    
    return TRUE;
}

LRESULT Ui_CategoriesWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case MZ_WM_MOUSE_NOTIFY:
        {
            int nID = LOWORD(wParam);
            int nNotify = HIWORD(wParam);
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            if (nID == MZ_IDC_CATEGORY_LIST && nNotify == MZ_MN_LBUTTONUP) {
                if (!m_pList->IsMouseDownAtScrolling() && !m_pList->IsMouseMoved()) {
                    int nIndex = m_pList->CalcIndexOfPos(x, y);

					if(nIndex != -1 && x < GetWidth()/5){
                        if(m_pList->ToggleViewStatus(nIndex)){
                            m_pList->UpdateCategories();
                        }
					}else{
						if(nIndex != -1 && nIndex == _sel_idx){
							_selection = m_pList->GetSelectedID();
							EndModal(ID_OK);
							return 0;
						}						
					}

					_sel_idx = nIndex;
					m_pList->SetSelectedIndex(nIndex);
                    m_pList->Invalidate();
                }
                return 0;
            }
            if (nID == MZ_IDC_CATEGORY_LIST && nNotify == MZ_MN_MOUSEMOVE) {
				_sel_idx = -1;
                m_pList->SetSelectedIndex(-1);
                m_pList->Invalidate();
				m_BtnSelect.SetVisible(false);
				m_BtnSelect.Invalidate();
                return 0;
            }
		}
    }
    return Ui_BaseWnd::MzDefWndProc(message, wParam, lParam);
}

void Ui_CategoriesWnd::DelayShow() {
    SetupCategoryTypeBar();  
    m_pList->UpdateCategories();
}