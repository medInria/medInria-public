/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxItkProgressDialogCallback.h 973 2008-10-16 13:22:23Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2008-10-16 15:22:23 +0200 (Thu, 16 Oct 2008) $
Version:   $Revision: 973 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _wx_ItkProgressDialogCallback_h_
#define _wx_ItkProgressDialogCallback_h_

#include <itkObject.h>
#include <itkCommand.h>
#include <vtkINRIA3DConfigure.h>

class wxProgressDialog2;

      
class WX_VTK_EXPORT wxItkProgressDialogCallback : public itk::Command
{

 public:
  typedef wxItkProgressDialogCallback Self;
  typedef itk::Command                Superclass;

  typedef itk::SmartPointer<Self>          Pointer;
  typedef itk::SmartPointer<const Self>    ConstPointer;

  itkTypeMacro (wxItkProgressDialogCallback, itk::Command);
  itkNewMacro (Self);

  /** Standard Command virtual methods */
  void Execute(itk::Object *caller, const itk::EventObject &event);
  void Execute(const itk::Object *caller, const itk::EventObject &event);
  

  enum ProgressBarMode
  {
    MODE_TEXT,
    MODE_GUI
  };

  static int& GetGraphicalMode (void)
  {
    static int mode = MODE_GUI;
    return mode;
  }

  static void SetGraphicalModeToText (void)
  { wxItkProgressDialogCallback::GetGraphicalMode() = MODE_TEXT; }

  static void SetGraphicalModeToGUI (void)
  { wxItkProgressDialogCallback::GetGraphicalMode() = MODE_GUI; }
  

  void SetProgressDialog (wxProgressDialog2* dialog)
  { m_Dialog = dialog; }  
  wxProgressDialog2* GetProgressDialog (void) const
  { return m_Dialog; }


 protected:
  wxItkProgressDialogCallback();
  ~wxItkProgressDialogCallback(){};
    

 private:
  wxProgressDialog2* m_Dialog;

  int m_Count;
  bool m_StartingDash;
  bool m_EndingDash;
};



#endif
