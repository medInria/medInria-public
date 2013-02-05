/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxSnapshotTaker.cxx 1276 2009-09-26 16:46:34Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2009-09-26 18:46:34 +0200 (Sat, 26 Sep 2009) $
Version:   $Revision: 1276 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "wxSnapshotTaker.h"


#include <vtkWindowToImageFilter.h>

#include <vtkImageWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkBMPWriter.h>
#include <vtkTIFFWriter.h>
#include <vtkPNGWriter.h>

#include <vtkRendererCollection.h>

#include <vector>

wxSnapshotTaker::wxSnapshotTaker (wxWindow* parent)
{
  m_Parent        = parent;
  m_Magnification = 1;
  m_RenderWindow  = 0;
}


wxSnapshotTaker::~wxSnapshotTaker()
{}


void wxSnapshotTaker::Snap()
{
  if( !m_RenderWindow )
  {
    std::cerr << "Error: RenderWindow is not set." << std::endl;
    return;
  }
  
  
  wxFileDialog* myFileDialog = new wxFileDialog(m_Parent, wxT("Save snapshot as"),
                                                wxT(""), wxT(""),
                                                wxT("Image file (*.jpg;*.jpeg;*.bmp;*.png;*.tiff)|*.jpg;*.jpeg;*.bmp;*.png;*.tiff|")
                                                wxT("jpeg (*.jpg;*.jpeg)|*.jpg;*.jpeg|")
                                                wxT("bmp (*.bmp)|*.bmp|")
                                                wxT("png (*.png)|*.png|")
                                                wxT("tiff (*.tiff)|*.tiff|")
                                                wxT("all (*.*)|*.*"),
                                                wxFD_SAVE|wxFD_CHANGE_DIR|wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
  wxString fileName;
  fileName.Empty();
  int OK = myFileDialog->ShowModal();
  
  if( OK==wxID_OK )
  {
    fileName = myFileDialog->GetPath();
  }
  
  myFileDialog->Destroy();


  if( fileName.IsEmpty() )
  {
    return;
  }
  
  std::string s_filename (fileName.char_str());
  //vtksys_stl::string ext = vtksys::SystemTools::GetFilenameLastExtension (s_filename);
  std::string ext = "";
  std::string::size_type dot_pos = s_filename.rfind(".");
  if(dot_pos != std::string::npos)
  {
    ext = s_filename.substr(dot_pos);
  }

  if( ext=="" )
  {
    fileName += wxT (".jpg");
    //ext = vtksys::SystemTools::GetFilenameLastExtension (s_filename);
	ext = ".jpg";
  }

  vtkImageWriter* writer = 0;

  if( ext==".jpg" || ext==".jpeg" )
  {
    writer = vtkJPEGWriter::New();
  }
  else if ( ext==".bmp" )
  {
    writer = vtkBMPWriter::New();
  }
  else if ( ext==".tiff" )
  {
    writer = vtkTIFFWriter::New();
  }
  else if ( ext==".png" )
  {
    writer = vtkPNGWriter::New();
  }
  else
  {
    //ERROR message here
    wxString message = wxT ("Error: Format is not supported."); 
    wxMessageDialog* myDialog = new wxMessageDialog(m_Parent, message,
                                                    wxT ("Error"),
                                                    wxOK|wxICON_ERROR);
    myDialog->ShowModal();
    myDialog->Destroy();
    return;
  }


  std::vector<double> red;
  std::vector<double> green;
  std::vector<double> blue;
  
  vtkRendererCollection *collection = m_RenderWindow->GetRenderers();
  collection->InitTraversal();
  while (vtkRenderer *ren = collection->GetNextItem() )
  {
    // save renderer background color
    red.push_back   (ren->GetBackground()[0]);
    green.push_back (ren->GetBackground()[1]);
    blue.push_back  (ren->GetBackground()[2]);

    // set background color to white (better for screenshots)
    ren->SetBackground (1.0, 1.0, 1.0);
  }
  
  vtkWindowToImageFilter* snapper = vtkWindowToImageFilter::New();
  snapper->SetMagnification( m_Magnification );
  snapper->SetInput( m_RenderWindow );

  
  writer->SetInput( snapper->GetOutput() );
  writer->SetFileName( fileName.char_str() );
  writer->Write();

  int index = 0;
  collection->InitTraversal();
  while (vtkRenderer *ren = collection->GetNextItem() )
  {
    // restore renderer background color
    ren->SetBackground ( red[index], green[index], blue[index]);
    index++;
  }

  snapper->Delete();
  writer->Delete();
  
}
