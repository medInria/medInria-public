/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxHelloWorldMainFrameInteractor.cxx 1282 2009-10-08 17:00:40Z acanale $
Language:  C++
Author:    $Author: acanale $
Date:      $Date: 2009-10-08 19:00:40 +0200 (Thu, 08 Oct 2009) $
Version:   $Revision: 1282 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "wxHelloWorldMainFrameInteractor.h"

#include <vtkViewImage2D.h>
#include <vtkViewImage3D.h>

#include <vtkMetaImageData.h>

wxHelloWorldMainFrameInteractor::wxHelloWorldMainFrameInteractor (wxWindow* parent, int id,
                                                                  const wxString& title,
                                                                  const wxPoint& pos,
                                                                  const wxSize& size,
                                                                  long style)
  : wxHelloWorldMainFrame (parent, id, title, pos, size, style)
{}


/**
   Overload of OnOpenButton method. We pop up a file dialog window asking
   for a file to open. It returns the filename. If it's empty, then we
   return (no file was selected). If not, we open it, using ITK. Last step:
   we convert the itkImage into a vtkImage that will feed our views.
 */

void wxHelloWorldMainFrameInteractor::OnOpenButton (wxCommandEvent &event)
{
  
  wxFileDialog* myFileDialog = new wxFileDialog(this, wxT ("Open an Image"),
                                                wxT(""), wxT(""),
                                                wxT("All (*.*)|*.*"),
                                                wxFD_OPEN|wxFD_CHANGE_DIR, wxDefaultPosition);
  wxString fileName;
  fileName.Empty();
  int OK = myFileDialog->ShowModal();
  
  if( OK==wxID_OK )
    fileName = myFileDialog->GetPath();
  
  myFileDialog->Destroy();


  if( fileName.IsEmpty() )
    return;

  

  vtkMetaImageData* metaimage = 0;
  
  if (vtkMetaImageData::CanReadFile(fileName.char_str()))
  {
    metaimage = vtkMetaImageData::New();
    try
    {
      metaimage->Read (fileName.char_str());
    }
    catch (vtkErrorCode::ErrorIds)
    {
      std::cerr << "Cannot read file " << fileName << std::endl;
      wxMessageDialog* myDialog = new wxMessageDialog(this, wxT("Error while reading file.\n(check the log window for more details)"),
						      wxT ("Error"),
						      wxOK|wxICON_ERROR);
      myDialog->ShowModal();
      myDialog->Destroy();
      metaimage->Delete();
      return;
    }
  }
  
  if( !metaimage )
  {
    return;
  }


  /**
     Reset the window/level and the current position.
  */
  this->m_View1->SetImage ( metaimage->GetImageData() );
  this->m_View2->SetImage ( metaimage->GetImageData() );
  this->m_View3->SetImage ( metaimage->GetImageData() );
  this->m_View4->SetImage ( metaimage->GetImageData() );
  
  this->m_View1->SyncReset();

  metaimage->Delete();

}
