#ifndef _UI_ICONBUTTON_H
#define _UI_ICONBUTTON_H

#include <MzCommonDll.h>


class COMMON_API UiIconButton : public UiWin
{
public:
	UiIconButton(void);
	~UiIconButton(void) {	}
public:
	void SetImage(ImagingHelper *img);
	void SetText(LPCTSTR t);
	void SetID(int nID);
private:
	UiButton_Image m_button;
	UiStatic m_text;
};

#endif /*_UI_ICONBUTTON_H*/