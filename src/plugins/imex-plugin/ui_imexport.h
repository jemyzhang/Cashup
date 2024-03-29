#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include "imexport.h"

class UiDateEditBG : public UiWin
{
public:
	UiDateEditBG(void){}
	~UiDateEditBG(void) {}
protected:
	virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
		UiWin::PaintWin(hdcDst, prcWin, prcUpdate);
		MzDrawGridPopupMenuBG_480(hdcDst,prcWin);
	}
};

class UiSelectionStatus : public UiWin
{
public:
	UiSelectionStatus(void){}
	~UiSelectionStatus(void) {}
	void SetStatus(bool b) { bSelected = b; }
	bool GetStatus() { return bSelected; }
protected:
	virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
		UiWin::PaintWin(hdcDst, prcWin, prcUpdate);
		MzDrawControl(hdcDst, prcWin,  bSelected ? MZC_SELECTED : MZC_UNSELECTED, 0);
	}
private:
	bool bSelected;
};

#define MZ_IDC_BTN_S1	101
#define MZ_IDC_BTN_S2	102
#define MZ_IDC_BTN_OK	103
#define MZ_IDC_BTN_CANCEL	104
#define MZ_IDC_BTN_EFROM	105
#define MZ_IDC_BTN_ETO		106
#define MZ_IDC_BTN_ALL		107

class Ui_ExDateRangeWnd : public CMzWndEx 
{
	MZ_DECLARE_DYNAMIC(Ui_ExDateRangeWnd);
public:
	Ui_ExDateRangeWnd(void){
		_selIndex = 0;
		_sdate.Value = 0;
		_edate.Value = 0;
	}
//	~Ui_ExDateRangeWnd(void);
public:
	UiDateEditBG m_bg;
	UiStatic m_LblAll;

	UiButton m_EdtFromDate;
	UiStatic m_LblTo;
	UiButton m_EdtToDate;

	UiButton m_BtnOK;
	UiButton m_BtnCancel;

	UiSelectionStatus m_s1;
	UiSelectionStatus m_s2;
public:
	void setFromDate(TRSDATE date){
		_sdate.Value = date.Value;
	}
	void setToDate(TRSDATE date){
		_edate.Value = date.Value;
	}
	void setSelection(bool sel){
		_selIndex = sel;
	}
	bool getSelection(){
		return _selIndex;
	}
	TRSDATE getDateFrom() { return _sdate; }
	TRSDATE getDateTo()	{ return _edate; }
	void updateUi(){
		m_s1.SetStatus(_selIndex);
		m_s1.Invalidate();
		
		m_s2.SetStatus(!_selIndex);
		m_s2.Invalidate();
		if(_sdate.Value == 0){
			m_EdtFromDate.SetText(L"起始日期");
		}else{
			wchar_t strDate[16];
			wsprintf(strDate,L"%d.%d.%d",
				_sdate.Date.Year,_sdate.Date.Month,_sdate.Date.Day);
			m_EdtFromDate.SetText(strDate);
		}
		m_EdtFromDate.Invalidate();

		if(_edate.Value == 0){
			m_EdtToDate.SetText(L"结束日期");
		}else{
			wchar_t strDate[16];
			wsprintf(strDate,L"%d.%d.%d",
				_edate.Date.Year,_edate.Date.Month,_edate.Date.Day);
			m_EdtToDate.SetText(strDate);
		}
		m_EdtToDate.Invalidate();
	}
private:
	bool _selIndex;
	TRSDATE _sdate,_edate;
protected:
    // Initialization of the window (dialog)
	virtual BOOL OnInitDialog();

    // override the MZFC window messages handler
	virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam){
		switch (message) {
			case MZ_WM_MOUSE_NOTIFY:
			{
				int nID = LOWORD(wParam);
				int nNotify = HIWORD(wParam);
				if(nNotify == MZ_MN_LBUTTONUP){	//双击 显示全部
					switch(nID){
						case MZ_IDC_BTN_S1:
							_selIndex = true;
							updateUi();
							break;
						case MZ_IDC_BTN_S2:
							_selIndex = false;
							updateUi();
							break;
						case MZ_IDC_BTN_ALL:
							_selIndex = true;
							updateUi();
							break;
					}
				}
			}
		}
		return CMzWndEx::MzDefWndProc(message, wParam, lParam);
	}
    // override the MZFC command handler
	virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
};


class Ui_ImExportWnd : public CMzWndEx
{
	MZ_DECLARE_DYNAMIC(Ui_ImExportWnd);
public:
	Ui_ImExportWnd(void);
	~Ui_ImExportWnd(void);
	void updateButtonRange();
protected:
    UiToolbar_Text m_Toolbar;
    UiCaption m_CaptionTitle;

    UiButtonEx m_BtnImport;
    UiButtonEx m_BtnExport;
	UiButtonEx m_BtnExRange;

protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
private:
	bool _isImport;
	//导出范围相关
	bool _bExportAll;
	TRSDATE _StartExportDate;
	TRSDATE _EndExportDate;
};
