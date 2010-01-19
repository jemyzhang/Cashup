#include <MzCommon.h>
using namespace MzCommon;

#include "UiIconButton.h"

UiIconButton::UiIconButton(){
	m_button.SetPos(0,0,90,90);
	m_button.SetButtonType(MZC_BUTTON_NONE);
	m_button.EnableNotifyMessage(true);

	m_text.SetPos(0,90,90,25);
	m_text.SetDrawTextFormat(DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS);
    m_text.SetTextColor(RGB(255,255,255));
	m_text.SetTextSize(20);

	AddChild(&m_button);
	AddChild(&m_text);
}

void UiIconButton::SetImage(ImagingHelper *img){
	m_button.SetImage_Normal(img);
	m_button.SetImage_Pressed(img);
}

void UiIconButton::SetText(LPCTSTR text){
	m_text.SetText(text);
}

void UiIconButton::SetID(int nID){
	m_button.SetID(nID);
}