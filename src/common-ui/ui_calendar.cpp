#include <cMzCommon.h>
using namespace cMzCommon;
#include "ui_calendar.h"

#include <commondef.h>

#include "resource.h"
extern CMzString getLngResString(int strid);
extern ImagingHelper *getResImage(int nID);
extern HINSTANCE hHandle;

class UiGrid : public UiWin
{
public:
	UiGrid(void);
	~UiGrid(void);
	virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate);
public:
	//if ret = false, no selection
	bool calcSelectedIndex(int x, int y,int &row, int &col);
	void setSelectedIndex(int row,int col);
	//if ret = false, no selection
	bool getSelectedIndex(int &row, int &col);
public:
	//if idx == -1, set all
	void setTextSize(int row,int col, int nSize);
	//if idx == -1 set all
	void setTextColor(int row,int col, COLORREF c);
	void setText(int row,int col, LPCTSTR text);
	int getTextSize(int row,int col);
	COLORREF getTextColor(int row,int col);
	LPCTSTR getText(int row,int col);
	void setSelectedBgColor(COLORREF c);
	void setSelectedTextColor(COLORREF c);
	void setGridAutoSize(bool);
	void setGridSize(int width, int height);
public:
	void setGrids(int nRow,int nCol);
	int getRowCount(void);
	int getColCount(void);
public:
	virtual void Update(){
		_reqUpdate = true;
	}
	virtual void SetPos(int x, int y, int w, int h, UINT flags=0);
private:
	typedef struct GridAttr{
		CMzString text;
		int textSize;
		COLORREF textColor;
		bool isSelected;
	}GridAttr_t,*GridAttr_ptr;
	GridAttr_ptr *_grids;
private:
	COLORREF _selbg, _seltxt;
	int _rows,_cols;
	int _gwidth, _gheight;
	bool _reqUpdate;
	int m_nMaxX;
	int m_nMaxY;
private:
	HDC pMemDC;             //定义内存DC指针
	HBITMAP pBitmap;        //定义内存位图指针
private:
	bool checkRange(int row, int col);
};

UiGrid::UiGrid()
	:UiWin()
{
	_grids = 0;
	_selbg = RGB(64,192,192);
	_seltxt = RGB(255,255,255);
	_rows = 1;
	_cols = 1;
	setGridSize(_rows,_cols);
    _gwidth = _gheight = 0;
	_reqUpdate = false;
}

UiGrid::~UiGrid(){
	setGridSize(0,0);	//release 
	if(pMemDC) ReleaseDC(GetParentWnd(),pMemDC);
	if(pBitmap) DeleteObject(pBitmap);
}

void UiGrid::setSelectedIndex(int row,int col){
	if(checkRange(row,col)){
		int oldr, oldc;
		if(getSelectedIndex(oldr,oldc)){
			_grids[oldr][oldc].isSelected = false;
		}
		_grids[row][col].isSelected = true;
	}
}

bool UiGrid::calcSelectedIndex(int x, int y, int &row, int &col){
	bool ret = true;
	int c = x / _gwidth;
	int r = y / _gheight;
	if(c > _cols || r > _rows){
		row = -1;
		col = -1;
		ret = false;
	}else{
		row = r;
		col = c;
	}
	return ret;
}

bool UiGrid::getSelectedIndex(int &row, int &col){
	bool ret = true;
		for(int i = 0; i < _rows; i++){
			for(int j = 0; j < _cols; j++){
				if(_grids[i][j].isSelected){
					row = i;
					col = j;
					return ret;
				}
			}
		}
	row = -1;
	col = -1;
	return false;
}

void UiGrid::setGridSize(int width, int height){
	_gwidth = width;
	_gheight = height;
}

void UiGrid::setGrids(int nRow, int nCol){
	if(_grids){
		for(int i = 0; i < _rows; i++){
			for(int j = 0; j < _cols; j++){
				_grids[i][j].text.SetBufferSize(0);
			}
			delete _grids[i];
		}
		delete _grids;
		_grids = 0;
	}
	_rows = nRow;
	_cols = nCol;
	if(_rows >= 0 && _cols >= 0){
		_grids = new GridAttr_ptr[_rows];
		for(int i = 0; i < _rows; i++){
			_grids[i] = new GridAttr_t[_cols];
			for(int j = 0; j < _cols; j++){
				_grids[i][j].isSelected = false;
				_grids[i][j].text = 0;
				_grids[i][j].textColor = RGB(0,0,0);
				_grids[i][j].textSize = 28;
			}
		}
	}
    if(_cols != 0 && _rows != 0){
	    _gwidth = GetWidth()/_cols;
	    _gheight = GetHeight()/_rows;
    }

}

int UiGrid::getColCount(){
	return _cols;
}

int UiGrid::getRowCount(){
	return _rows;
}

void UiGrid::setText(int row,int col, LPCTSTR text){
	if(checkRange(row,col)){
		_grids[row][col].text = text;
	}
}

void UiGrid::setTextColor(int row,int col, COLORREF c){
	if(checkRange(row,col)){
		_grids[row][col].textColor = c;
	}
}

void UiGrid::setTextSize(int row,int col, int nSize){
	if(checkRange(row,col)){
		_grids[row][col].textSize = nSize;
	}
}

LPCTSTR UiGrid::getText(int row,int col){
	if(checkRange(row,col)){
		return _grids[row][col].text;
	}
	return 0;
}

COLORREF UiGrid::getTextColor(int row,int col){
	if(checkRange(row,col)){
		return _grids[row][col].textColor;
	}
	return RGB(0,0,0);
}

int UiGrid::getTextSize(int row,int col){
	if(checkRange(row,col)){
		return _grids[row][col].textSize;
	}
	return 0;
}

bool UiGrid::checkRange(int row, int col){
	if(row >= _rows || col >= _cols ||
		row < 0 || col < 0){
			return false;
	}
	return true;
}

void UiGrid::SetPos(int x, int y, int w, int h, UINT flags){
	UiWin::SetPos(x,y,w,h,flags);
	m_nMaxX = w;
	m_nMaxY = h;
	pMemDC = CreateCompatibleDC(GetDC(GetParentWnd()));
	pBitmap = CreateCompatibleBitmap(GetDC(GetParentWnd()),m_nMaxX,m_nMaxY);
}

void UiGrid::PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
	UiWin::PaintWin(hdcDst,prcWin,prcUpdate);
	if(_reqUpdate){
		_reqUpdate = false;
		SelectObject(pMemDC, pBitmap);
		HBRUSH myBrush = CreateSolidBrush(RGB(255-16,255-16,255-16));
		RECT rect;
		rect.top = 0;
		rect.bottom = m_nMaxY;
		rect.left = 0;
		rect.right = m_nMaxX;

		FillRect(pMemDC,&rect,myBrush);//画之前先擦除.
		//SetTextColor(RGB(0,0,0));

		//无数据
		if(_rows <= 0 || _cols <= 0){
			BitBlt(hdcDst,0,0,m_nMaxX,m_nMaxY,pMemDC,0,0,SRCCOPY);
			return;
		}
		int _gridw = _gwidth;
		int _gridh = _gheight;
		int _x = 0;//prcWin->left;
		int _y = 0;//prcWin->top;

		HFONT font = FontHelper::GetFont(_grids[0][0].textSize);
		SelectObject(pMemDC,font);
		HPEN pen = CreatePen(PS_SOLID, 0,RGB(128,128,128));
		HPEN poldpen = (HPEN)SelectObject(pMemDC,pen);
		SetBkMode(pMemDC,TRANSPARENT);
		for(int i = 0; i < _rows; i++){
			int cx = _x;
			int cy = _y;
			for(int j = 0; j < _cols; j++){
				//格子
				RECT rect = {cx + (_gridw-1)*j,cy + (_gridh-1)*i,cx + _gridw*j + _gridw,cy + _gridh*i + _gridh};
				Rectangle(pMemDC,rect.left,rect.top,rect.right,rect.bottom);
				//格子背景
				RECT frect = {rect.left+1,rect.top + 1,rect.right - 2,rect.bottom - 2};
				RECT textrect = {rect.left+1,rect.top + 1,rect.right - 2,rect.bottom - 2};
				HBRUSH bqbrush;
				if(_grids[i][j].isSelected){	//selected
					//bqbrush = CreateSolidBrush(_selbg);	//选中格子背景颜色
					//pen = CreatePen(PS_SOLID, 1,_seltxt);
					::SetTextColor(pMemDC,_seltxt);
					MzDrawSelectedBg_NoLine(pMemDC,&frect);
				}else{
					bqbrush = CreateSolidBrush(RGB(255-16,255-16,255-16));
					//pen = CreatePen(PS_SOLID, 1,_grids[i][j].textColor);
					::SetTextColor(pMemDC,_grids[i][j].textColor);
					FillRect(pMemDC,&frect,bqbrush);
				}
				//选择字体颜色
				//HPEN oldpen = (HPEN)SelectObject(pMemDC,pen);

				MzDrawText( pMemDC,_grids[i][j].text.C_Str(), &textrect, DT_CENTER|DT_VCENTER );
				//DrawText(pMemDC,_grids[i][j].text.C_Str(),_grids[i][j].text.Length(),&textrect,DT_CENTER|DT_VCENTER);
				//SelectObject(pMemDC,oldpen);//恢复笔
				//DeleteObject(pen);
			}
		}
		SelectObject(pMemDC,poldpen);
	}
	BitBlt(hdcDst,prcWin->left,prcWin->top,m_nMaxX,m_nMaxY,pMemDC,0,0,SRCCOPY);

}

class UiWeekBar : public UiStatic{
public:
    virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
        int width = RECT_WIDTH(*prcWin)/7;
        RECT rect;
        rect.top = prcWin->top;
        rect.bottom = prcWin->bottom;

        HFONT font = FontHelper::GetFont(GetTextSize());
        SelectObject(hdcDst,font);	
        SetBkMode(hdcDst,TRANSPARENT);

        ::SetTextColor(hdcDst,GetTextColor());
        for(int i = 0; i < 7; i++){
            rect.left = width * i;
            rect.right = width * (i + 1);
            MzDrawText( hdcDst,getLngResString(IDS_STR_WEEKDAY_NAME0 + i).C_Str(), &rect, DT_CENTER|DT_VCENTER );
        }
        DeleteObject(font);
        UiStatic::PaintWin(hdcDst,prcWin,prcUpdate);
    }
};

MZ_IMPLEMENT_DYNAMIC(Ui_CalendarWnd)

#define MZ_IDC_Edit_YEAR 102
#define MZ_IDC_Edit_MONTH 103
#define MZ_IDC_Edit_DAY 104
#define MZ_IDC_CALENDAR_GRID 105
#define MZ_IDC_CALENDAR_NEXT 106
#define MZ_IDC_CALENDAR_PRE 107

//////

Ui_CalendarWnd::Ui_CalendarWnd(void)
{
	_year = 0; _month = 0; _day = 0;
    m_pCalendar = 0;
    m_pWeekBar = 0;
}

Ui_CalendarWnd::~Ui_CalendarWnd(void)
{
    delete m_pWeekBar;
    delete m_pCalendar;
}

BOOL Ui_CalendarWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!Ui_BaseWnd::OnInitDialog()) {
        ui_out("initial error!\n");
        return FALSE;
    }

    // Then init the controls & other things in the window
    int y = 0;

	m_CaptionHeader.SetPos(0,y,GetWidth(),MZM_HEIGHT_CAPTION * 2);
	AddUiWin(&m_CaptionHeader);

	m_YearMonth.SetPos(60,10,GetWidth()*3/4,MZM_HEIGHT_CAPTION);
	m_YearMonth.SetTextSize(30);
	m_CaptionHeader.AddChild(&m_YearMonth);


    ImagingHelper *rrleaseimg = getResImage(IDB_PNG_L_RRELEASE);
    ImagingHelper *rpressimg = getResImage(IDB_PNG_L_RPRESS);
	int bw = rrleaseimg->GetImageWidth();
	int bh = rrleaseimg->GetImageHeight();

	m_BtnNext.SetPos(m_CaptionHeader.GetWidth() - bw - 5, (m_CaptionHeader.GetHeight() - bh)/2 - 10, bw, bh);
	m_BtnNext.SetButtonType(MZC_BUTTON_NONE);
	m_BtnNext.SetImage_Normal(rrleaseimg);
	m_BtnNext.SetImage_Pressed(rpressimg);
	m_BtnNext.SetID(MZ_IDC_CALENDAR_NEXT);
	m_CaptionHeader.AddChild(&m_BtnNext);

    ImagingHelper *lrleaseimg = getResImage(IDB_PNG_L_LRELEASE);
    ImagingHelper *lpressimg = getResImage(IDB_PNG_L_LPRESS);
	m_BtnPre.SetPos(5, (m_CaptionHeader.GetHeight() - bh)/2 - 10, bw, bh);
	m_BtnPre.SetButtonType(MZC_BUTTON_NONE);
	m_BtnPre.SetImage_Normal(lrleaseimg);
	m_BtnPre.SetImage_Pressed(lpressimg);
	m_BtnPre.SetID(MZ_IDC_CALENDAR_PRE);
	m_CaptionHeader.AddChild(&m_BtnPre);

    m_pWeekBar = new UiWeekBar;
	m_pWeekBar->SetPos(0,MZM_HEIGHT_CAPTION*3/2,GetWidth(),MZM_HEIGHT_CAPTION/2);
	m_pWeekBar->SetTextSize(17);
	m_pWeekBar->SetTextColor(RGB(128,128,128));
	m_CaptionHeader.AddChild(m_pWeekBar);

	y += MZM_HEIGHT_CAPTION * 2;
    m_pCalendar = new UiGrid;
    m_pCalendar->SetPos(0, y, GetWidth(), GetHeight() - y);
	m_pCalendar->SetID(MZ_IDC_CALENDAR_GRID);
	m_pCalendar->EnableNotifyMessage(true);
    AddUiWin(m_pCalendar);

    ::PostMessage(GetParent(),MZ_MW_CHANGE_TITLE,IDS_TITLE_CALENDAR,(LPARAM)hHandle);
	DateTime::waitms(1);

    return TRUE;
}

void Ui_CalendarWnd::updateGrid(){
	if(_year == 0 || _month == 0 || _day == 0){
		DateTime::getDate(&_year,&_month,&_day);
	}
	wchar_t datestr[16];
	wsprintf(datestr,getLngResString(IDS_STR_YYYYMM).C_Str(),_year,_month);
	m_YearMonth.SetText(datestr);
	m_YearMonth.Invalidate();
	int week = (DateTime::getWeekDay(_year,_month,1)+1)%7;	//获取1号的星期
	int days = DateTime::getDays(_year,_month);
	int rows = (week+days)/7 + ((week+days)%7 ? 1 : 0);
	m_pCalendar->setGrids(rows,7);
	int t_year,t_month,t_day;
	DateTime::getDate(&t_year,&t_month,&t_day);
	for(int i = 0; i < days; i++){
		int r = (week + i) / 7;
		int c = (week + i) % 7;
		wsprintf(datestr,L"%d",i+1);
		m_pCalendar->setText(r,c,datestr);
		if(t_year == _year && t_month == _month && t_day == i+1){
			m_pCalendar->setTextColor(r,c,RGB(192,64,64));
		}
		if(_day == i+1){
			m_pCalendar->setSelectedIndex(r,c);
		}
	}
	m_pCalendar->Invalidate();
	m_pCalendar->Update();
}

void Ui_CalendarWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
		case MZ_IDC_CALENDAR_NEXT:
			DateTime::getNextDate(_year,_month);
			_day = 1;
			updateGrid();
			break;
		case MZ_IDC_CALENDAR_PRE:
			DateTime::getPreDate(_year,_month);
			_day = 1;
			updateGrid();
			break;
	}
}

LRESULT Ui_CalendarWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
		case MZ_WM_MOUSE_NOTIFY:
		{
			int nID = LOWORD(wParam);
			int nNotify = HIWORD(wParam);
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            if (nID == MZ_IDC_CALENDAR_GRID && nNotify == MZ_MN_LBUTTONDOWN) {
                if (!m_pCalendar->IsMouseDownAtScrolling() && !m_pCalendar->IsMouseMoved()) {
					int r = 0;
					int c = 0;
					if( m_pCalendar->calcSelectedIndex(x,y,r,c)){
						//check if is invalid selection
						CMzString s = m_pCalendar->getText(r,c);
						if(s.Length() == 0){
							return 0;
						}
						m_pCalendar->setSelectedIndex(r,c);
						m_pCalendar->Invalidate();
						m_pCalendar->Update();
					}
                }
                return 0;
            }
            if (nID == MZ_IDC_CALENDAR_GRID && nNotify == MZ_MN_LBUTTONDBLCLK) {
                if (!m_pCalendar->IsMouseDownAtScrolling() && !m_pCalendar->IsMouseMoved()) {
					int r = 0;
					int c = 0;
					if( m_pCalendar->calcSelectedIndex(x,y,r,c)){
						//check if is invalid selection
						CMzString s = m_pCalendar->getText(r,c);
						if(s.Length() == 0){
							return 0;
						}
						_day = _wtoi(s.C_Str());
						EndModal(ID_OK);
					}
                }
                return 0;
            }
			break;
		}
    }
    return Ui_BaseWnd::MzDefWndProc(message, wParam, lParam);
}
