#ifndef _wx_FFBitmapButton_h_
#define _wx_FFBitmapButton_h_

#include <wx/bmpbuttn.h>
#include "vtkINRIA3DConfigure.h"

class WX_ADDON_EXPORT wxFFBitmapButton : public wxBitmapButton
{
 public:
 wxFFBitmapButton(wxWindow* parent,
		  wxWindowID id,                                       
		  const wxBitmap& bitmap,
		  const wxPoint& pos = wxDefaultPosition,
		  const wxSize& size = wxDefaultSize,
		  long style = wxBU_AUTODRAW,
		  const wxValidator& validator = wxDefaultValidator, 
		  const wxString& name = wxT ("button")):
  wxBitmapButton(parent,id,bitmap,pos,size,style,validator,name){};
  void OnEraseBackGround(wxEraseEvent& event) {};
  DECLARE_EVENT_TABLE()
    };


#endif
