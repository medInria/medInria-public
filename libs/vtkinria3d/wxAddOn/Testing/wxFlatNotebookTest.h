/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxFlatNotebookTest.h 477 2007-11-20 17:46:10Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2007-11-20 18:46:10 +0100 (mar, 20 nov 2007) $
Version:   $Revision: 477 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _wx_FlatNotebookTest_h_
#define _wx_FlatNotebookTest_h_

#include <wx/wx.h>

class App :
	public wxApp
{
public:
	App(void);
	~App(void);
	virtual bool OnInit(void);
	virtual int OnExit(void);
};
DECLARE_APP(App)

#endif
