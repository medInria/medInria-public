/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWMainWindow.h 877 2008-06-04 16:10:40Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-06-04 18:10:40 +0200 (Wed, 04 Jun 2008) $
Version:   $Revision: 877 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __vtkKWMainWindow_h
#define __vtkKWMainWindow_h

#include "vtkINRIA3DConfigure.h"

#include <vtkKWWindow.h>
#include <vector>



class vtkKWToolbar;
class vtkKWPageView;


class KW_ADDON_EXPORT vtkKWMainWindow : public vtkKWWindow
{
public:
  static vtkKWMainWindow* New();
  vtkTypeRevisionMacro(vtkKWMainWindow,vtkKWWindow);

  // Description:
  // Callbacks
  
  virtual void OnMenuNewTab();
  virtual void OnMenuDeleteCurrentTab();
  virtual void OnMenuFileOpen();
  virtual void OnMenuFileOpen4D();
  virtual void OnMenuFileOpenDICOM();
  virtual void OnMenuFileSaveDICOM();
  virtual void SaveManagerCallback (void);
  virtual void SaveDiffXMLCallback (void);
  
  virtual void OnDataSetSave();
  virtual void OnDataSetDelete();
  
  
  vtkKWPageView* GetCurrentPage (void);
  vtkKWPageView* GetPage (const char* title);
  void RaisePage(const char* title);
  
  //BTX
  std::vector<vtkKWPageView*> GetPageList ();
  //ETX

protected:
  vtkKWMainWindow();
  ~vtkKWMainWindow();

  // Description:
  // Create the widget.
  virtual void CreateWidget();
  virtual void CreateOpenToolbar();
  virtual void CreateViewToolbar();
  virtual void CreateMenu();
  virtual void PackSelf();
  
  vtkKWToolbar*         OpenToolbar;
  vtkKWToolbar*         ViewToolbar;

  unsigned int PageNumberIncrement;
  
private:
  
  vtkKWMainWindow(const vtkKWMainWindow&);   // Not implemented.
  void operator=(const vtkKWMainWindow&);  // Not implemented.

  

};

#endif
