/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxVtkImageFlip.cxx 1350 2009-11-19 15:16:30Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2009-11-19 16:16:30 +0100 (Thu, 19 Nov 2009) $
Version:   $Revision: 1350 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "wxVtkImageFlip.h"

#include "wxVTKRenderWindowInteractor.h"

#include "vtkViewImage2D.h"

#include "vtkImageData.h"
#include <vtkImageFlip.h>
#include <vtkRenderer.h>
#include <vtkImageReslice.h>
#include <vtkMatrix4x4.h>

#include "wxCommon.h"

wxVtkImageFlip::wxVtkImageFlip (wxWindow* parent, int id,
				const wxString& title,
				const wxPoint& pos,
				const wxSize& size,
				long style)
  : wxDialog(parent, id, title, pos, size, style)
{

  
  this->AcquisitionTxt   = new wxStaticText(this, -1, wxT("Acquisition Orientation:"));
  this->ComboAcquisition = new wxComboBox  (this, COMBOBOX_ACQ, wxT ("Change the acquisition flag..."));
  this->ComboAcquisition ->SetWindowStyle (wxCB_DROPDOWN);
  this->ComboAcquisition ->Append (wxT("AXIAL"));
  this->ComboAcquisition ->Append (wxT("SAGITTAL"));
  this->ComboAcquisition ->Append (wxT("CORONAL"));
  this->ComboAcquisition ->SetSelection (0);
  
  
  this->ButtonFlipRL = new wxButton (this, BUTTON_FLIPRL, wxT("Flip R-L"), wxDefaultPosition, wxSize (64,64));
  this->ButtonFlipFH = new wxButton (this, BUTTON_FLIPFH, wxT("Flip I-S"), wxDefaultPosition, wxSize (64,64));
  this->ButtonFlipAP = new wxButton (this, BUTTON_FLIPAP, wxT("Flip A-P"), wxDefaultPosition, wxSize (64,64));
  this->ButtonCANCEL = new wxButton (this, BUTTON_CANCEL, wxT("Cancel"),   wxDefaultPosition, wxSize (80,30));
  this->ButtonOK     = new wxButton (this, BUTTON_OK,     wxT("OK"),       wxDefaultPosition, wxSize (80,30));
  this->ButtonSave   = new wxButton (this, BUTTON_SAVE,   wxT("Save..."),  wxDefaultPosition, wxSize (80,30));
 
  long viewstyle = wxWANTS_CHARS | wxNO_FULL_REPAINT_ON_RESIZE | wxSUNKEN_BORDER;

  this->VtkView1 = new wxVTKRenderWindowInteractor (this, wxID_ANY, wxDefaultPosition, wxDefaultSize, viewstyle, wxT(""));
  this->VtkView2 = new wxVTKRenderWindowInteractor (this, wxID_ANY, wxDefaultPosition, wxDefaultSize, viewstyle, wxT(""));
  this->VtkView3 = new wxVTKRenderWindowInteractor (this, wxID_ANY, wxDefaultPosition, wxDefaultSize, viewstyle, wxT(""));
  
  this->View1 = vtkViewImage2D::New();
  this->View2 = vtkViewImage2D::New();
  this->View3 = vtkViewImage2D::New();

  
  vtkRenderer* rend1 = vtkRenderer::New();
  vtkRenderer* rend2 = vtkRenderer::New();
  vtkRenderer* rend3 = vtkRenderer::New();

  this->VtkView1->GetRenderWindow()->AddRenderer (rend1);
  this->VtkView2->GetRenderWindow()->AddRenderer (rend2);
  this->VtkView3->GetRenderWindow()->AddRenderer (rend3);

  this->View1->SetRenderWindow (this->VtkView1->GetRenderWindow());
  this->View2->SetRenderWindow (this->VtkView2->GetRenderWindow());
  this->View3->SetRenderWindow (this->VtkView3->GetRenderWindow());

  this->View1->SetRenderer (rend1);
  this->View2->SetRenderer (rend2);
  this->View3->SetRenderer (rend3);

  this->View1->SetShowAnnotations(0);
  this->View1->SetLeftButtonInteractionStyle(vtkViewImage2D::SELECT_INTERACTION);
  this->View1->SetRightButtonInteractionStyle(vtkViewImage2D::WINDOW_LEVEL_INTERACTION);
  this->View1->SetMiddleButtonInteractionStyle(vtkViewImage2D::ZOOM_INTERACTION);
  this->View2->SetShowAnnotations(0);
  this->View2->SetLeftButtonInteractionStyle(vtkViewImage2D::SELECT_INTERACTION);
  this->View2->SetRightButtonInteractionStyle(vtkViewImage2D::WINDOW_LEVEL_INTERACTION);
  this->View2->SetMiddleButtonInteractionStyle(vtkViewImage2D::ZOOM_INTERACTION);
  this->View3->SetShowAnnotations(0);
  this->View3->SetLeftButtonInteractionStyle(vtkViewImage2D::SELECT_INTERACTION);
  this->View3->SetRightButtonInteractionStyle(vtkViewImage2D::WINDOW_LEVEL_INTERACTION);
  this->View3->SetMiddleButtonInteractionStyle(vtkViewImage2D::ZOOM_INTERACTION);

  
  rend1->Delete();
  rend2->Delete();
  rend3->Delete();
  
  this->Image = 0;
  this->FirstRender = 0;
  
  this->ImageDim[0] = 0;
  this->ImageDim[1] = 0;
  this->ImageDim[2] = 0;

  this->ImageOrigin[0] = 0;
  this->ImageOrigin[1] = 0;
  this->ImageOrigin[2] = 0;

  this->ImageCenter[0] = 0;
  this->ImageCenter[1] = 0;
  this->ImageCenter[2] = 0;

  this->AcquisitionFlag = ACQ_AXIAL;

  this->AxesFlipped[0] = 0;
  this->AxesFlipped[1] = 0;
  this->AxesFlipped[2] = 0;

  this->Reslicer = vtkImageReslice::New();
  this->Reslicer->InterpolateOff();
  
  this->SetProperties();
  this->DoLayout();

  
}

wxVtkImageFlip::~wxVtkImageFlip()
{  
   this->View1->Detach();
   this->View2->Detach();
   this->View3->Detach();
  
  this->VtkView1->Delete();
  this->VtkView2->Delete();
  this->VtkView3->Delete();
  
  this->View1->Delete();
  this->View2->Delete();
  this->View3->Delete();

  
  this->Reslicer->Delete(); 
}

void wxVtkImageFlip::SetProperties()
{

  this->View1->SetBackgroundColor (0.0, 0.0, 0.0);
  this->View2->SetBackgroundColor (0.0, 0.0, 0.0);
  this->View3->SetBackgroundColor (0.0, 0.0, 0.0);

  this->View1->SetAboutData ("AXIAL");
  this->View2->SetAboutData ("CORONAL");
  this->View3->SetAboutData ("SAGITTAL");
  
  this->View1->SetOrientation( vtkViewImage2D::AXIAL_ID);
  this->View2->SetOrientation( vtkViewImage2D::CORONAL_ID);
  this->View3->SetOrientation( vtkViewImage2D::SAGITTAL_ID);
  
  this->View1->AddChild (this->View2);
  this->View2->AddChild (this->View3);
  this->View3->AddChild (this->View1);
 
}


void wxVtkImageFlip::DoLayout()
{
  wxBoxSizer* sizermain        = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer* sizerviews       = new wxBoxSizer (wxHORIZONTAL);
  wxBoxSizer* sizerbuttons     = new wxBoxSizer(wxHORIZONTAL);
  wxBoxSizer* sizerOK          = new wxBoxSizer(wxHORIZONTAL);
  
  sizerviews      ->Add (this->VtkView1, 1, wxALL|wxEXPAND, 5);
  sizerviews      ->Add (this->VtkView2, 1, wxALL|wxEXPAND, 5);
  sizerviews      ->Add (this->VtkView3, 1, wxALL|wxEXPAND, 5);

  sizerbuttons   ->Add (this->AcquisitionTxt,   0, wxALL|wxFIXED_MINSIZE, 5);
  sizerbuttons   ->Add (this->ComboAcquisition, 0, wxALL|wxFIXED_MINSIZE, 5);
  sizerbuttons   ->Add (this->ButtonFlipFH, 0, wxALL|wxEXPAND, 5);
  sizerbuttons   ->Add (this->ButtonFlipRL, 0, wxALL|wxEXPAND, 5);
  sizerbuttons   ->Add (this->ButtonFlipAP, 0, wxALL|wxEXPAND, 5);

  sizerOK        ->Add (this->ButtonSave,   0, wxALL|wxEXPAND, 5);
  sizerOK        ->Add (this->ButtonCANCEL, 0, wxALL|wxEXPAND, 5);
  sizerOK        ->Add (this->ButtonOK,     0, wxALL|wxEXPAND, 5);
  
  sizermain       ->Add (sizerviews,    1, wxALL|wxEXPAND, 0);
  sizermain       ->Add (sizerbuttons,  0, wxCENTER, 0);
  sizermain       ->Add (sizerOK,       0, wxALIGN_RIGHT, 0);
  

  this->SetAutoLayout(true);
  this->SetSizerAndFit (sizermain);

  this->SetSize (wxSize (700,400));
  
  this->Layout();
  
}


BEGIN_EVENT_TABLE (wxVtkImageFlip, wxDialog)
  EVT_COMBOBOX (COMBOBOX_ACQ,         wxVtkImageFlip::OnComboAcquisition)
  EVT_BUTTON (BUTTON_FLIPRL,          wxVtkImageFlip::OnButtonFlipRL)
  EVT_BUTTON (BUTTON_FLIPAP,          wxVtkImageFlip::OnButtonFlipAP)
  EVT_BUTTON (BUTTON_FLIPFH,          wxVtkImageFlip::OnButtonFlipFH)
  EVT_BUTTON (BUTTON_CANCEL,          wxVtkImageFlip::OnButtonCANCEL)
  EVT_BUTTON (BUTTON_OK,              wxVtkImageFlip::OnButtonOK)
  EVT_BUTTON (BUTTON_SAVE,            wxVtkImageFlip::OnButtonSave)
END_EVENT_TABLE()


void wxVtkImageFlip::OnButtonFlipFH(wxCommandEvent &event)
{
  if (!this->Image)
    return;
  
  unsigned int direction = this->View1->GetOrthogonalAxis (vtkViewImage::AXIAL_ID);
 
  this->AxesFlipped[direction] = !this->AxesFlipped[direction];

  this->UpdateReslicer();
  this->Synchronize();

}


void wxVtkImageFlip::OnButtonFlipRL(wxCommandEvent &event)
{
  if (!this->Image)
    return;
  
  unsigned int direction = this->View1->GetOrthogonalAxis (vtkViewImage::SAGITTAL_ID);

  this->AxesFlipped[direction] = !this->AxesFlipped[direction];

  this->UpdateReslicer();
  this->Synchronize();
 
}

void wxVtkImageFlip::OnButtonFlipAP(wxCommandEvent &event)
{
  if (!this->Image)
    return;

  unsigned int direction = this->View1->GetOrthogonalAxis (vtkViewImage::CORONAL_ID);  

  this->AxesFlipped[direction] = !this->AxesFlipped[direction];

  this->UpdateReslicer();
  this->Synchronize();
}


void wxVtkImageFlip::SetImage (vtkImageData* image,  vtkMatrix4x4* mat)
{
  if (!image)
    return;
  
  this->Image = image;
  
  image->GetDimensions (this->ImageDim);
  image->GetOrigin (this->ImageOrigin);
  image->GetCenter (this->ImageCenter);
  image->GetWholeExtent (this->ImageExtent);

  
  image->GetSpacing (this->ImageSpacing);
  
  this->Reslicer->SetInput (image);

  this->Reslicer->SetOutputExtentToDefault();
  this->Reslicer->SetOutputSpacingToDefault();
  this->Reslicer->SetOutputOriginToDefault();

  this->Reslicer->Update();
  
  this->View1->SetImage (this->Reslicer->GetOutput());
  this->View2->SetImage (this->Reslicer->GetOutput());
  this->View3->SetImage (this->Reslicer->GetOutput());

  if (mat)
  {
    this->View1->SetDirectionMatrix (mat);
    this->View2->SetDirectionMatrix (mat);
    this->View3->SetDirectionMatrix (mat);
  }

  this->UpdateReslicer();
  this->Synchronize();
  

}

void wxVtkImageFlip::OnButtonSave (wxCommandEvent &event)
{

  if( !this->GetImage() )
  {
    return;
  }
  
  wxFileDialog* myFileDialog = new wxFileDialog(this, wxT("Save image as"),
						wxT(""), wxT(""),
						wxSupportedImageExtensionsFilter,
						wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
  wxString filename;
  filename.Empty();
  int OK = myFileDialog->ShowModal();
  
  if( OK==wxID_OK )
  {
    filename = myFileDialog->GetPath();
  }
  myFileDialog->Destroy();

  if( filename.IsEmpty() )
  {
    return;
  }

  
  switch( this->GetImage()->GetScalarType() )
  {
    
      case VTK_CHAR:
	this->SaveImage<itk::Image<char,3> > ( this->Reslicer->GetOutput(), filename.char_str() );
        break;
  
    case VTK_SIGNED_CHAR :
      this->SaveImage< itk::Image<unsigned char, 3> >( this->Reslicer->GetOutput(), filename.char_str());
      break;

    case VTK_SHORT :	
      this->SaveImage< itk::Image<short, 3> >( this->Reslicer->GetOutput(), filename.char_str() );
      break;

    case VTK_UNSIGNED_SHORT :	
      this->SaveImage< itk::Image<unsigned short, 3> >( this->Reslicer->GetOutput(), filename.char_str() );
      break;

    case VTK_INT :	
      this->SaveImage< itk::Image<int, 3> >( this->Reslicer->GetOutput(), filename.char_str() );
      break;
	
    case VTK_UNSIGNED_INT :	
      this->SaveImage< itk::Image<unsigned int, 3> >( this->Reslicer->GetOutput(), filename.char_str() );
      break;
	
    case VTK_LONG :	
      this->SaveImage< itk::Image<long, 3> >( this->Reslicer->GetOutput(), filename.char_str() );
      break;
	
    case VTK_UNSIGNED_LONG :	
      this->SaveImage< itk::Image<unsigned long, 3> >( this->Reslicer->GetOutput(), filename.char_str() );
      break;
	
    case VTK_FLOAT :	
      this->SaveImage< itk::Image<float, 3> >( this->Reslicer->GetOutput(), filename.char_str() );
      break;
	
    case VTK_DOUBLE :	
      this->SaveImage< itk::Image<double, 3> >( this->Reslicer->GetOutput(), filename.char_str() );
      break;

    default:
      wxMessageDialog* myDialog = new wxMessageDialog(this, wxT("Error: pixel type is not supported."),
						wxT ("Error"),
						wxOK|wxICON_ERROR);
    myDialog->ShowModal();
    myDialog->Destroy();
  }
    

}

  
void wxVtkImageFlip::OnButtonCANCEL (wxCommandEvent &event)
{
  EndModal(wxID_CANCEL); 
}

void wxVtkImageFlip::OnButtonOK (wxCommandEvent &event)
{
    EndModal(wxID_OK);
}

void wxVtkImageFlip::OnComboAcquisition(wxCommandEvent &event)
{
  unsigned int flag    = this->ComboAcquisition->GetSelection();
  this->AcquisitionFlag = flag;

  this->UpdateReslicer();
  this->Synchronize();
}


void wxVtkImageFlip::UpdateReslicer (void)
{
  if (!this->Image)
  {
    return;
  }
  
  int    newaxes[3] = {  0, 1, 2  };
  double I[3][3]    = { {1, 0, 0},
			{0, 1, 0},
			{0, 0, 1} };
  
  double resliceorigin[3] = { 0, 0, 0 };
  
  

  
  switch (this->AcquisitionFlag)
  {
      case ACQ_AXIAL:
	break;
      case ACQ_CORONAL:
	newaxes[0] = 2;
	newaxes[1] = 0;
	newaxes[2] = 1;
	break;
      case ACQ_SAGITTAL:
	newaxes[0] = 0;
	newaxes[1] = 2;
	newaxes[2] = 1;
	break;
  }

  for (unsigned int i=0; i<3; i++)
  {
    if (AxesFlipped[i])
    {
      I[newaxes[i]][newaxes[i]]=-1;
      resliceorigin[newaxes[i]] = 2*this->ImageOrigin[newaxes[i]] +
	this->ImageSpacing[newaxes[i]]*
	(this->ImageExtent[2*newaxes[i]] + this->ImageExtent[2*newaxes[i]+1]);
    }
  }

  this->Reslicer->SetResliceAxesDirectionCosines( I[newaxes[0]],
						  I[newaxes[1]],
						  I[newaxes[2]] );

  this->Reslicer->SetResliceAxesOrigin (resliceorigin);

  this->Reslicer->Modified();
  this->Reslicer->UpdateWholeExtent();

  // force a reset of image bounds for the views
  this->View1->SetImage (this->Reslicer->GetOutput());
  this->View2->SetImage (this->Reslicer->GetOutput());
  this->View3->SetImage (this->Reslicer->GetOutput());

}


void wxVtkImageFlip::Synchronize (void)
{

  if (!this->Image)
    return;
  
//   this->View1->Update();
//   this->View2->Update();
//   this->View3->Update();
  
  this->View1->SyncReset();

  if( !this->View1->GetRenderWindow()->GetNeverRendered() )
    this->View1->Render();
  if( !this->View2->GetRenderWindow()->GetNeverRendered() )
    this->View2->Render();
  if( !this->View1->GetRenderWindow()->GetNeverRendered() )
    this->View2->Render();}
