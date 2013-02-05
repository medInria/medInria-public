#ifndef _wx_FFSlider_h_
#define _wx_FFSlider_h_

#include <wx/slider.h>
#include "vtkINRIA3DConfigure.h"

class WX_ADDON_EXPORT wxFFSlider : public wxSlider
{
 public:
 wxFFSlider(wxWindow* parent,
	    wxWindowID id,                                       
	    int value , int minValue, int maxValue,
	    const wxPoint& pos = wxDefaultPosition,
	    const wxSize& size = wxDefaultSize,
	    long style = wxSL_HORIZONTAL,
	    const wxValidator& validator = wxDefaultValidator, 
	    const wxString& name = wxT ("slider")):
  wxSlider(parent,id,value,minValue,maxValue,pos,size,style,validator,name){};
  void OnEraseBackGround(wxEraseEvent& event) {};
  DECLARE_EVENT_TABLE()

    };

#endif
