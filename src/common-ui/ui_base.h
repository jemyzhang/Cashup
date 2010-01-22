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
	virtual void DelayShow() = 0;	//��ʱ��ʾ
public:
    //ͼƬ����ģʽ
    void EnableImageBackground(bool en);
    //��Сģʽ
    void SetSizeMode(bool fixed);
    //���ñ���ͼƬ��ƽ��ģʽ
    void SetBackgroundImage(ImagingHelper *img);
	virtual BOOL Create(int xPos, int yPos, int width, int height, HWND hwndParent=NULL, int uID = NULL, DWORD style=WS_CHILD, DWORD exstyle=0);
private:
   	HDC pMemDC;             //�����ڴ�DCָ��
	HBITMAP pBitmap;        //�����ڴ�λͼָ��
private:
    bool bOverideCreateSize;    //�Ƿ������С��Ĭ����
    bool bImageBackground;      //�Ƿ�ʹ��ͼƬ������Ĭ����
};


#endif /*_UI_BASE_H*/