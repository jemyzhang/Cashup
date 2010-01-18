#include <MzCommon.h>
using namespace MzCommon;

#include "ui_base.h"
#include "..\resource.h"
#include "..\commondef.h"

#define	MZ_IDC_SCROLLWIN			802

MZ_IMPLEMENT_DYNAMIC(Ui_BaseWnd)

Ui_BaseWnd::Ui_BaseWnd(){
    pMemDC = 0;
    pBitmap = 0;
}

Ui_BaseWnd::~Ui_BaseWnd(){
	if(pMemDC) ReleaseDC(m_hWnd,pMemDC);
	if(pBitmap) DeleteObject(pBitmap);
}

BOOL Ui_BaseWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window

	int y = 0;
	m_ScrollWin.SetPos(0,y,GetWidth(),GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR);
	m_ScrollWin.EnableScrollBarV(true);
	m_ScrollWin.SetID(MZ_IDC_SCROLLWIN);
	AddUiWin(&m_ScrollWin);

    SetBackgroundImage(ImagingHelper::GetImageObject(MzGetInstanceHandle(),IDB_PNG_BG));
	//ÉèÖÃ¶¥²ãHWND
	::PostMessage(GetParent(),MZ_MW_CHANGE_TOPWND,(WPARAM)m_hWnd,0);
	DateTime::waitms(1);

	::SetTimer(m_hWnd,0x8001,100,NULL);
	return TRUE;
}

void Ui_BaseWnd::OnTimer(UINT_PTR nIDEvent){
	if(nIDEvent == 0x8001){
		::KillTimer(m_hWnd,0x8001);
		DelayShow();
	}
}

LRESULT Ui_BaseWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    switch(message){
        case MZ_MW_CHILDWND_QUIT:
        {
		    EndModal(ID_OK);
		    return 0;
	    }
        case MZ_MW_CHILDWND_SHOW:
        {
            ShowWindow(SW_SHOW);
            return 0;
        }
    }
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

void Ui_BaseWnd::PaintWin(HDC hdc, RECT* prcUpdate){
    ::BitBlt(hdc,0,0,GetWidth(),GetHeight(),
        pMemDC,0,0,SRCCOPY);
    CMzWndEx::PaintWin(hdc,prcUpdate);
}

void Ui_BaseWnd::SetBackgroundImage(ImagingHelper *img){
    if(img == NULL) return;

	if(pMemDC) ReleaseDC(m_hWnd,pMemDC);
	if(pBitmap) DeleteObject(pBitmap);
	pMemDC = CreateCompatibleDC(GetDC(m_hWnd));
	pBitmap = CreateCompatibleBitmap(GetDC(m_hWnd),GetWidth(),GetHeight());
    ::SelectObject(pMemDC,pBitmap);

    //starting draw background
    int imgw = img->GetImageWidth();
    int imgh = img->GetImageHeight();
    int rows = GetWidth() / imgw + 1;
    int cols = GetHeight() / imgh + 1;
    RECT rcDraw = {0,0,imgw,imgh};
    for(int i = 0; i < cols; i ++){
        for(int j = 0; j < rows; j++){
            rcDraw.top = i * imgh; rcDraw.left = j * imgw;
            rcDraw.bottom = rcDraw.top + imgh;
            rcDraw.right = rcDraw.left + imgw;
            img->Draw(pMemDC,&rcDraw);
        }
    }

}

BOOL Ui_BaseWnd::Create(int xPos, int yPos, int width, int height, HWND hwndParent, int uID, DWORD style, DWORD exstyle){
    RECT rcWork = MzGetWorkArea();
    return CMzWndEx::Create(rcWork.left, rcWork.top + SZ_LOGO_HEIGHT, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork) - SZ_LOGO_HEIGHT,
        hwndParent,uID,style,exstyle);
}