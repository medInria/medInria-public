/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxVtkDICOMImporter.cxx 1193 2009-07-20 07:44:11Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2009-07-20 09:44:11 +0200 (Mon, 20 Jul 2009) $
Version:   $Revision: 1193 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "wxVtkDICOMImporter.h"
#include <wxItkProgressDialog.h>

#include "wxVtkDICOMImporterPage1.h"
#include "wxVtkDICOMImporterPage2.h"

#include <string.h>

wxVtkDICOMImporter::wxVtkDICOMImporter (wxWindow* parent, bool manual)
  : wxWizard (parent,wxID_ANY,wxT("DICOM Importer Wizard"),
              wxNullBitmap, wxDefaultPosition,
              wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{

  m_Page1 = new wxVtkDICOMImporterPage1 (this);
  m_Page2 = new wxVtkDICOMImporterPage2 (this);
  
  wxWizardPageSimple::Chain(m_Page1, m_Page2);

  wxSize size = m_Page2->GetBestSize();
  size.SetHeight ( (int)(size.GetWidth()*0.75) );
  
  this->SetPageSize ( size );

  m_Page1->m_Text->Wrap ( size.GetWidth()-10 );

  if (manual)
    return;
  
  if( this->RunWizard (m_Page1) )
  {}
  
}


wxVtkDICOMImporter::~wxVtkDICOMImporter()
{
  m_Page1->Destroy();
  m_Page2->Destroy();
}


BEGIN_EVENT_TABLE (wxVtkDICOMImporter, wxWizard)
  EVT_WIZARD_PAGE_CHANGED (wxID_ANY, wxVtkDICOMImporter::OnPageChanged)
  EVT_WIZARD_FINISHED (wxID_ANY, wxVtkDICOMImporter::OnFinished)
  EVT_WIZARD_PAGE_CHANGING (wxID_ANY, wxVtkDICOMImporter::OnPageChanging)
END_EVENT_TABLE()


wxVtkDICOMImporter::GDCMImporterType::Pointer wxVtkDICOMImporter::GetImporter (void)
{
  return this->m_Page2->GetImporter();
}
  

void wxVtkDICOMImporter::OnPageChanged(wxWizardEvent &event)
{

}


void wxVtkDICOMImporter::OnFinished(wxWizardEvent &event)
{

  //m_Page2->DeleteCurrentNavigationThread();
  
  wxItkProgressDialog* progressDialog = new wxItkProgressDialog(wxT ("Please wait"),
                                                                wxT("Building volumes..."),100, this,wxPD_AUTO_HIDE);
  
  this->GetImporter()->AddObserver (itk::ProgressEvent(), progressDialog->GetCallback());  

  try
  {
    this->GetImporter()->Update();
  }
  catch (itk::ExceptionObject & e)
  {
    std::cerr << e;
  }

  this->GetImporter()->RemoveAllObservers();
  progressDialog->Destroy();
  
  for (unsigned int i=0; i<this->GetImporter()->GetNumberOfOutputs(); i++)
  {
    DICOMImageType::Pointer dicomimage = static_cast<DICOMImageType*>(this->GetImporter()->GetOutput (i));
    if (dicomimage.IsNull())
      continue;
    m_Outputs.push_back( static_cast<ImageType*>(dicomimage) );
    m_Descriptions.push_back( dicomimage->GetName() );
  }

  this->GetImporter()->Initialize();  
  
}

void wxVtkDICOMImporter::OnPageChanging(wxWizardEvent &event)
{
  
}



void wxVtkDICOMImporter::LoadDirectory(const char* directoryname)
{
  m_Page2->OpenDirectory (directoryname);
}

void wxVtkDICOMImporter::RunFromPage (unsigned int page)
{
  if (page == 1)
    this->RunWizard (m_Page1);
  if (page == 2)
    this->RunWizard (m_Page2);
  
}

