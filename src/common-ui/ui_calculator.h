#pragma once

#include <MzCommonDll.h>
#include <ui_base.h>

class UiControlBG;
class UiNumericDisplay;
class UiCalButton;
class CalcCore;

class COMMON_API Ui_CalculatorWnd : public Ui_BaseWnd 
{
	MZ_DECLARE_DYNAMIC(Ui_CalculatorWnd);
public:
	Ui_CalculatorWnd(void);
	~Ui_CalculatorWnd(void);
public:
	UiControlBG *m_pbg;
	UiNumericDisplay *m_pDisplay;
	UiCalButton *pm_Buttons[20];
public:
	double GetResult() { return _result; };
	void SetInitOprand(double val);
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
private:
    virtual void DelayShow() { }	//—” ±œ‘ æ
private:
	double _result;
	CalcCore *pcore;
	bool _opPressed;
	bool _opndInput;
};
