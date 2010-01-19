#include "ui_calculator.h"
#include <MzCommon.h>
using namespace MzCommon;

#include "calcore.h"

class UiNumericDisplay : public UiWin
{
public:
	UiNumericDisplay(){
		_dotPos = 0;
		_isEmpty = true;
	}
	void setText1(LPCTSTR text) { _l1str = text; }
	void setText2(LPCTSTR text) { _l2str = text; }
	void removeChar1();
	void addChar1(wchar_t ch);
	void addChar2(wchar_t ch){ addChar(ch,_l2str);	}
	void addString1(LPCTSTR str){ addString(str,_l1str);	}
	void addString2(LPCTSTR str){ addString(str,_l2str);	}
	CMzString getText1() { return _l1str; }
	CMzString getText2() { return _l2str; }
	bool isNonInput(){
		return _isEmpty;
	}
	void setNonInput(){
		_isEmpty = true;
		_dotPos = 0;
	}
	void initNum(wchar_t ch){
		_isEmpty = false;
		wchar_t s[2] = {ch,0};
		_l1str = s;
	}
	void initVal(double val){
		wchar_t strval[64];
		wsprintf(strval,L"%.2f",val);
		_isEmpty = false;
		_l1str = strval;
	}
	void clearNum();
	double getInputval();
private:
	void addChar(wchar_t ch,CMzString& s){ 
		wchar_t tch[2];
		tch[0] = ch;
		tch[1] = 0;
		s = s + tch;
	}
	void addString(LPCTSTR str,CMzString& s){
		s = s + str;
	}
protected:
	void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
		//draw background
		MzDrawSelectedBg_NoLine(hdcDst,prcWin);

		COLORREF cr;       
		HFONT hf;
		RECT rect;
		rect.left = prcWin->left + 5;
		rect.right = prcWin->right - 5;
		LONG height = prcWin->bottom - prcWin->top;

		SetBkMode(hdcDst,TRANSPARENT);

		if(!_l1str.IsEmpty()){	//draw text of line 1
			cr = RGB(0,0,0);
			hf = FontHelper::GetFont( 25 );
			SelectObject( hdcDst , hf );
			rect.top = prcWin->top + height / 3;
			rect.bottom = rect.top + height * 2 / 3;
			MzDrawText( hdcDst, _l1str.C_Str(), &rect, DT_RIGHT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );
			DeleteObject( hf );
		}
		if(!_l2str.IsEmpty()){  //draw text of line2
			cr = RGB(128,128,128);
			hf = FontHelper::GetFont( 17 );
			SelectObject( hdcDst , hf );
			rect.top = prcWin->top;
			rect.bottom = rect.top + height / 3;
			MzDrawText( hdcDst, _l2str.C_Str(), &rect, DT_RIGHT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );
			DeleteObject( hf );
		}
	}
private:
	CMzString _l1str;
	CMzString _l2str;
	int _dotPos;
	bool _isEmpty;	//never input
};

void UiNumericDisplay::addChar1(wchar_t chCharCode){
	//only accept 0-9,dot,-
	int inputlen = _l1str.Length();
	//排除连续输入0的问题
	if(inputlen == 1 && _l1str == L"0" && chCharCode == '0') return;
	//排除在输入运算符后输入dot的问题
	if(_isEmpty && chCharCode == '.'){
		_isEmpty = false;
		_l1str = L"0.";
		_dotPos = 2;
		return;
	}
	//only 2 digital after dot
	if(chCharCode != 0x08 && _dotPos != 0 && (inputlen - _dotPos > 1)) return;
	//only 8 digital before dot
	if(chCharCode != 0x08 &&  chCharCode != '.' && _dotPos == 0 && inputlen > 8) return;


	if(chCharCode < '0' || chCharCode > '9'){
		if(chCharCode != '.' && chCharCode != '-' && chCharCode != 0x08){
			return;
		}
		if(chCharCode == '-'){
			if(_isEmpty) return;
			if(_l1str.C_Str()[0] == '-'){
				_l1str = _l1str.SubStr(1,_l1str.Length() - 1);
			}else{
				CMzString Ssign = L"-";
				_l1str = Ssign + _l1str;
			}
			return;
		}
		if(chCharCode == '.'){
			if(_dotPos != 0){
				return;
			}else{
				_dotPos = inputlen + 1;
				chCharCode = '.';
			}
		}
		if(chCharCode == 0x08){ //backspace
			if(inputlen == _dotPos){
				_dotPos = 0;
			}
			removeChar1();
			return;
		}
	}
	addChar(chCharCode,_l1str);
	if(_isEmpty) _isEmpty = false;
}

void UiNumericDisplay::removeChar1(){
	if(_l1str.Length() == 1){
		setText1(L"0");
		_isEmpty = true;
		return;
	}
	_l1str = _l1str.SubStr(0,_l1str.Length() - 1);
}

void UiNumericDisplay::clearNum(){
	setText1(L"0");
	_isEmpty = true;
	_dotPos = 0;
}

double UiNumericDisplay::getInputval(){
	double retval = 0.0;
	if(_l1str.Length()){
		swscanf(_l1str.C_Str(),L"%lf",&retval);
	}
	return retval;
}

typedef enum CALCBTNTYPE{
	CALBTN_NONE,
	CALBTN_NUM,
	CALBTN_OP,
	CALBTN_FUNC,
}CALCBTNTYPE_t;

typedef struct CALCBTNINFO{
	int ID;
	LPCTSTR text;
	CALCBTNTYPE_t atype;	//action type
	int hsize;	//blocks
	int vsize;	//blocks
}CALCBTNINFO_t;

#define MZ_MW_BTN_NUMERIC MZFC_WM_MESSAGE+0x80
#define MZ_MW_BTN_OPERATOR MZFC_WM_MESSAGE+0x81
#define MZ_MW_BTN_FUNCTION MZFC_WM_MESSAGE+0x82

class UiCalButton : public UiButton
{
public:
	UiCalButton(int ID = -1, LPCTSTR text = 0, CALCBTNTYPE_t t = CALBTN_NONE){
		if(ID > 0){
			SetID(ID);
		}
		if(text){
			SetText(text);
		}
		SetTextColor(RGB(0,0,0));
		_t = t;
	}
	void setActType(CALCBTNTYPE_t t){
		_t = t;
	}
	CALCBTNTYPE_t getActType(){
		return _t;
	}

	virtual int OnLButtonUp(UINT fwKeys, int xPos, int yPos){
		switch(_t){
			case CALBTN_NUM:
				PostMessageW(MZ_MW_BTN_NUMERIC,GetID(),0);
				break;
			case CALBTN_OP:
				PostMessageW(MZ_MW_BTN_OPERATOR,GetID(),0);
				break;
			case CALBTN_FUNC:
				PostMessageW(MZ_MW_BTN_FUNCTION,GetID(),0);
				break;
			default:
				break;
		}
		return UiButton::OnLButtonUp(fwKeys,xPos,yPos);
	}
private:
	CALCBTNTYPE_t _t;
};

class UiControlBG : public UiWin
{
public:
	UiControlBG(void){}
	~UiControlBG(void) {}
protected:
	virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
		UiWin::PaintWin(hdcDst, prcWin, prcUpdate);
		MzDrawGridPopupMenuBG_480(hdcDst,prcWin);
	}
};

MZ_IMPLEMENT_DYNAMIC(Ui_CalculatorWnd)

#define MZ_IDC_BTN_NUM_BASE 101	//0-9 101-110
#define MZ_IDC_BTN_DOT		111
#define MZ_IDC_BTN_SIGN		112
#define MZ_IDC_BTN_ADD		113
#define MZ_IDC_BTN_DEC		114
#define MZ_IDC_BTN_PLUS		115
#define MZ_IDC_BTN_DIV		116
#define MZ_IDC_BTN_CLEAR	117
#define MZ_IDC_BTN_BACK		118
#define MZ_IDC_BTN_EQU		119
#define MZ_IDC_BTN_EXIT		120

CALCBTNINFO_t calbtns[][5] = {
	{
		{ MZ_IDC_BTN_NUM_BASE + 7,	L"7",	CALBTN_NUM,	1,	1},
		{ MZ_IDC_BTN_NUM_BASE + 8,	L"8",	CALBTN_NUM,	1,	1},
		{ MZ_IDC_BTN_NUM_BASE + 9,	L"9",	CALBTN_NUM,	1,	1},
		{ MZ_IDC_BTN_ADD,			L"+",	CALBTN_OP,	1,	1},
		{ MZ_IDC_BTN_EXIT,			L"⊙",CALBTN_FUNC,1,	1},
	},
	{
		{ MZ_IDC_BTN_NUM_BASE + 4,	L"4",	CALBTN_NUM,	1,	1},
		{ MZ_IDC_BTN_NUM_BASE + 5,	L"5",	CALBTN_NUM,	1,	1},
		{ MZ_IDC_BTN_NUM_BASE + 6,	L"6",	CALBTN_NUM,	1,	1},
		{ MZ_IDC_BTN_DEC,			L"-",	CALBTN_OP,	1,	1},
		{ MZ_IDC_BTN_CLEAR,			L"C",	CALBTN_FUNC,1,	1},
	},
	{
		{ MZ_IDC_BTN_NUM_BASE + 1,	L"1",	CALBTN_NUM,	1,	1},
		{ MZ_IDC_BTN_NUM_BASE + 2,	L"2",	CALBTN_NUM,	1,	1},
		{ MZ_IDC_BTN_NUM_BASE + 3,	L"3",	CALBTN_NUM,	1,	1},
		{ MZ_IDC_BTN_PLUS,			L"×",	CALBTN_OP,	1,	1},
		{ MZ_IDC_BTN_BACK,			L"←",	CALBTN_FUNC,1,	1},
	},
	{
		{ MZ_IDC_BTN_NUM_BASE + 0,	L"0",	CALBTN_NUM,	1,	1},
		{ MZ_IDC_BTN_DOT,			L".",	CALBTN_NUM,	1,	1},
		{ MZ_IDC_BTN_SIGN,			L"±",	CALBTN_NUM,1,	1},
		{ MZ_IDC_BTN_DIV,			L"÷",	CALBTN_OP,	1,	1},
		{ MZ_IDC_BTN_EQU,			L"=",	CALBTN_FUNC,1,	1},
	},
};

Ui_CalculatorWnd::Ui_CalculatorWnd(void)
{
	_opPressed = false;
	_opndInput = false;
	_result = 0.00;
}

Ui_CalculatorWnd::~Ui_CalculatorWnd(void)
{
	for(int i =0; i < 20; i++){
		delete pm_Buttons[i];
	}
    delete m_pDisplay;
    delete m_pbg;
}
#define HSIZE 85
#define VSIZE 50
BOOL Ui_CalculatorWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
    m_pbg = new UiControlBG;
	m_pbg->SetPos(0,0,GetWidth(),GetHeight());
	AddUiWin(m_pbg);

    int y = 8;
    m_pDisplay = new UiNumericDisplay;
	m_pDisplay->SetPos(10,y,GetWidth() - 20,70);
	m_pDisplay->setText1(L"0");
	m_pDisplay->clearNum();
	m_pbg->AddChild(m_pDisplay);

	y += 80;
	int cnt = 0;
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 5; j++){
			if(calbtns[i][j].ID > 0){
				pm_Buttons[cnt] = new UiCalButton(calbtns[i][j].ID,
					calbtns[i][j].text,calbtns[i][j].atype);
				pm_Buttons[cnt]->SetButtonType(MZC_BUTTON_DOWNLOAD);
				pm_Buttons[cnt]->SetPos(10 + j*(HSIZE + 7), 
					y + i *(VSIZE + 5),
					HSIZE * calbtns[i][j].hsize, 
					VSIZE * calbtns[i][j].vsize );
				m_pbg->AddChild(pm_Buttons[cnt]);
				cnt++;
			}
		}
	}

    pcore = new CalcCore;
	pcore->InitStack();
    return TRUE;
}

void Ui_CalculatorWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
//    switch (id) {

//	}
	return;
}

extern CalcOperator optrs[];

LRESULT Ui_CalculatorWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
		case MZ_MW_BTN_NUMERIC:
		{
			if(_opPressed){
				_opPressed = false;
				_opndInput = true;
			}
			int nID = LOWORD(wParam);
			if(nID <= MZ_IDC_BTN_NUM_BASE + 9){  
				int num = nID - MZ_IDC_BTN_NUM_BASE;
				wchar_t numstr = num + '0';
				if(m_pDisplay->isNonInput()){
					m_pDisplay->initNum(numstr);
				}else{
					m_pDisplay->addChar1(numstr);
				}
			}else if(nID == MZ_IDC_BTN_DOT){
				m_pDisplay->addChar1('.');
			}else if(nID == MZ_IDC_BTN_SIGN){
				m_pDisplay->addChar1('-');
			}
			m_pDisplay->Invalidate();
			break;
		}
		case MZ_MW_BTN_OPERATOR:
		{
			if(_opPressed) break;
			_opPressed = true;
			pcore->PushOpnd(m_pDisplay->getInputval());
			int nID = LOWORD(wParam);
			wchar_t* s;
			switch(nID){
				case MZ_IDC_BTN_ADD:
					s = L" + ";
					pcore->PushOptr(optrs[0]);
					break;
				case MZ_IDC_BTN_DEC:
					s = L" - ";
					pcore->PushOptr(optrs[1]);
					break;
				case MZ_IDC_BTN_PLUS:
					s = L" × ";
					pcore->PushOptr(optrs[2]);
					break;
				case MZ_IDC_BTN_DIV:
					s = L" ÷ ";
					pcore->PushOptr(optrs[3]);
					break;
			}
			m_pDisplay->addString2(m_pDisplay->getText1().C_Str());
			m_pDisplay->addString2(s);
			if(_opndInput){
				m_pDisplay->setText1(pcore->GetResult());
			}
			m_pDisplay->Invalidate();
			m_pDisplay->setNonInput();
			break;
		}
		case MZ_MW_BTN_FUNCTION:
		{
			int nID = LOWORD(wParam);
			switch(nID){
				case MZ_IDC_BTN_EXIT:
					EndModal(ID_CANCEL);
					break;
				case MZ_IDC_BTN_EQU:
					pcore->PushOpnd(m_pDisplay->getInputval());
					_result = pcore->CalculateAll();
					EndModal(ID_OK);
					break;
				case MZ_IDC_BTN_CLEAR:
					_opPressed = false;
					_opndInput = false;
					m_pDisplay->setText2(L"\0");
					m_pDisplay->clearNum();
					m_pDisplay->Invalidate();
					pcore->InitStack();
					break;
				case MZ_IDC_BTN_BACK:
					if(_opPressed && _opndInput){	//结果不可以用退格键删除
						return 0;
					}
					m_pDisplay->addChar1(0x08);
					m_pDisplay->Invalidate();
					break;
			}
		}
	}

    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

void Ui_CalculatorWnd::SetInitOprand(double val){
    _opndInput = false;
    m_pDisplay->initVal(val);
    m_pDisplay->setNonInput();
}