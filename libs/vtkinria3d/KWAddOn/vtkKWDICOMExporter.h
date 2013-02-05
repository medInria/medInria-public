/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWDICOMExporter.h 1 2007-11-20 17:46:10Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2007-11-20 18:46:10 +0100 (Tue, 20 Nov 2007) $
Version:   $Revision: 1 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtkKWDICOMExporter_h_
#define _vtkKWDICOMExporter_h_

#include "vtkINRIA3DConfigure.h"


#include <vtkKWMessageDialog.h>
#include <itkGDCMExporter.h>


class vtkDataManager;
class vtkKWDataSelectorWidget;
class vtkKWPushButton;
class vtkKWLabel;
class vtkKWEntry;


class KW_ADDON_EXPORT vtkKWDICOMExporter: public vtkKWMessageDialog
{
 public:

  static vtkKWDICOMExporter* New();
  vtkTypeRevisionMacro(vtkKWDICOMExporter,vtkKWMessageDialog);
  
  void EditDICOMInfoCallback();
  void PropagateDICOMInfoCallback();
  void SetOutputDirectoryCallback();

  void SetDataManager (vtkDataManager* manager);
  vtkDataManager* GetDataManager (void);

  virtual int  PreInvoke();
  virtual void PostInvoke();
  
  
 protected:
  vtkKWDICOMExporter();
  ~vtkKWDICOMExporter();
  
  // Description:
  // Create the widget.
  virtual void CreateWidget();
  virtual void PackSelf();


 private:
  
  vtkKWDICOMExporter(const vtkKWDICOMExporter&);      // Not implemented.
  void operator=(const vtkKWDICOMExporter&);        // Not implemented.

  vtkKWDataSelectorWidget* SelectionWidget;
  vtkKWPushButton*         ButtonEditDICOMInfo;
  vtkKWPushButton*         ButtonPropagateDICOMInfo;
  vtkKWPushButton*         ButtonSetOutputDirectory;
  vtkKWLabel*              OutputDirectoryLabel;
  vtkKWEntry*              OutputPrefixEntry;

  //BTX
  itk::GDCMExporter::Pointer GDCMExporter;
  //ETX
  
};

#endif
