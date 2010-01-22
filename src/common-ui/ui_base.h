#ifndef _UI_BASE_H
#define _UI_BASE_H

// include the MZFC library header file
#include <MzCommonDll.h>

// Main window derived from CMzWndEx

class COMMON_API Ui_BaseWnd : public CMzWndEx {
    MZ_DECLARE_DYNAMIC(Ui_BaseWnd);
public:
	Ui_BaseWnd();
	~Ui_BaseWnd();
protected:
	UiScrollWin m_ScrollWin;
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC window messages handler
    LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);

    // override the MZFC command handler
	virtual void OnTimer(UINT_PTR nIDEvent);
    virtual void PaintWin(HDC hdc, RECT* prcUpdate = NULL);
private:
	virtual void DelayShow() = 0;	//延时显示
public:
    //图片背景模式
    void EnableImageBackground(bool en);
    //大小模式
    void SetSizeMode(bool fixed);
    //设置背景图片，平铺模式
    void SetBackgroundImage(ImagingHelper *img);
	virtual BOOL Create(int xPos, int yPos, int width, int height, HWND hwndParent=NULL, int uID = NULL, DWORD style=WS_CHILD, DWORD exstyle=0);
private:
   	HDC pMemDC;             //定义内存DC指针
	HBITMAP pBitmap;        //定义内存位图指针
private:
    bool bOverideCreateSize;    //是否重设大小，默认是
    bool bImageBackground;      //是否使用图片背景，默认是
};


#endif /*_UI_BASE_H*/