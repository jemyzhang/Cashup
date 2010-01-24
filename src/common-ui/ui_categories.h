#ifndef _UI_CATEGORIES_H
#define _UI_CATEGORIES_H
#include <cMzCommonDll.h>
#include <ui_base.h>

class CategoryList;

class Ui_CategoriesWnd : public Ui_BaseWnd {
    MZ_DECLARE_DYNAMIC(Ui_CategoriesWnd);
public:
    Ui_CategoriesWnd() {
		_mode = 0;
		_maxdepth = 2;
		_hideCategoryId = -1;
		_sel_idx = -1;
    }
    ~Ui_CategoriesWnd();
    //set display mode
    //0: edit mode, 1: selection mode
    //use before show
	void setMode(int m) { _mode = m; }

    int getSelectionIndex() {
        return _selection;
    }

	void setNodeDepth(int d){
		if(d < 0) d = 0;
		if(d > 2) d = 2;
		_maxdepth = d;
	}

	void setHideCategory(int h){
		_hideCategoryId = h;
	}
private:
    //收入支出转账等导航按钮
    UiButtonBar m_CategoryTypeBar;
    void SetupCategoryTypeBar();
protected:
	UiButton m_BtnSelect;
    CategoryList *m_pList;

    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
    virtual void DelayShow();
private:
	int _mode;
    int _selection;
	int _maxdepth;	//max = 2
	int _hideCategoryId;
	int _sel_idx;
};

#endif /*_UI_CATEGORIES_H*/