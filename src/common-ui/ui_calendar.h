#pragma once
#include <cMzCommonDll.h>
#include <ui_base.h>

class UiGrid;
class UiWeekBar;

class COMMON_API Ui_CalendarWnd : public Ui_BaseWnd 
{
	MZ_DECLARE_DYNAMIC(Ui_CalendarWnd);
public:
	Ui_CalendarWnd(void);
	~Ui_CalendarWnd(void);
public:
	UiCaption m_CaptionHeader;
	UiButton_Image m_BtnNext;
	UiButton_Image m_BtnPre;
	UiStatic m_YearMonth;
	UiWeekBar *m_pWeekBar;
	UiGrid *m_pCalendar;
public:
    void GetDate(int &y, int &m, int &d){
        y = _year; m = _month; d = _day;
    }
	void SetDate(int y, int m, int d){
		_year = y; _month = m; _day = d;
	}
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
    virtual void DelayShow() { updateGrid(); }	//—” ±œ‘ æ
	void updateGrid();
private:
	int _year;
	int _month;
	int _day;
};
