/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWDicomInfoWidget.h 612 2008-01-14 10:59:37Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-01-14 10:59:37 +0000 (Mon, 14 Jan 2008) $
Version:   $Revision: 612 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtkKWDicomInfoWidget_h_
#define _vtkKWDicomInfoWidget_h_

#include "vtkINRIA3DConfigure.h"

#include <vtkKWFrame.h>
#include <string>
#include <vector>
#include <itkDicomTagManager.h>

class vtkKWMultiColumnList;
class vtkKWMainWindowInteractor;
class vtkMetaDataSet;
class vtkKWSimpleEntryDialog;
class vtkKWEntry;



class KW_ADDON_EXPORT vtkKWDicomInfoWidget: public vtkKWFrame
{
 public:

  static vtkKWDicomInfoWidget* New();
  vtkTypeRevisionMacro(vtkKWDicomInfoWidget,vtkKWFrame);

  //BTX
  typedef itk::DicomTag DicomTagType;
  typedef std::vector<DicomTagType> DicomTagListType;
  //ETX
  
  vtkSetMacro(Editable, int);
  vtkGetMacro(Editable, int);
  //
  vtkBooleanMacro (Editable, int);
  
  
  virtual void Update();
  void SelectionChangedCallback();
  void DoubleClickCallback();
  void SearchCallback();
  void ValidateSearchCallback(const char*);
  void SearchEntryCallback(const char*);
  void CellUpdateCallback(int row, int col, const char* text);
  


  //BTX
  void SetDicomTagListAsDictionary (itk::MetaDataDictionary dictionary);
  void SetDicomTagList(DicomTagListType list);
  DicomTagListType GetDicomTagList(void)
  { return this->DicomTagList; }
  itk::MetaDataDictionary GetDicomTagListAsDictionary (void);
  //ETX

  
 protected:
  vtkKWDicomInfoWidget();
  ~vtkKWDicomInfoWidget();

  // Description:
  // Create the widget.
  virtual void CreateWidget();
  virtual void Pack();


 private:
  
  vtkKWDicomInfoWidget(const vtkKWDicomInfoWidget&);      // Not implemented.
  void operator=(const vtkKWDicomInfoWidget&);        // Not implemented.

  vtkKWMultiColumnList* DicomInfoList;
  vtkKWSimpleEntryDialog* SearchDialog;
  vtkKWEntry*             SearchEntry;
  
  
  int                   Editable;

  //BTX
  DicomTagListType      DicomTagList;
  //ETX
  
};

#endif
