/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id:  1182 2009-06-30 09:19:05Z  $
Language:  C++
Author:    $Author:  $
Date:      $Date: 2009-06-30 11:19:05 +0200 (mar, 30 jun 2009) $
Version:   $Revision: 1182 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _wx_VtkImageFlip_txx_
#define _wx_VtkImageFlip_txx_

#include "wxVtkImageFlip.h"

#include <itkVTKImageToImageFilter.h>
#include <itkImageFileWriter.h>
#include <itkMatrix.h>


template <class TImage>
void
wxVtkImageFlip::SaveImage (vtkImageData* image, const char* filename)
{
 
  typedef TImage ImageType;

  typename itk::VTKImageToImageFilter<ImageType>::Pointer MyConverter =
    itk::VTKImageToImageFilter<ImageType>::New();
  MyConverter->SetInput ( image );
  
  try
  {
    MyConverter->Update();
  }
  catch(itk::ExceptionObject &e)
  {
    std::cerr << e << std::endl;
    wxMessageDialog* myDialog = new wxMessageDialog(this, wxT("An error occured when writing file."),
                                                    wxT ("Error"),
                                                    wxOK|wxICON_ERROR);
    myDialog->ShowModal();
    myDialog->Destroy();
    
    return;
  }

  itk::Matrix<double,3,3> cosines;
  cosines[0][0]= 1;
  cosines[0][1]= 0;
  cosines[0][2]= 0;
  cosines[1][0]= 0;
  cosines[1][1]=-1;
  cosines[1][2]= 0;
  cosines[2][0]= 0;
  cosines[2][1]= 0;
  cosines[2][2]= 1;
  
  typename ImageType::Pointer itkimage = const_cast<ImageType*>( MyConverter->GetOutput() );
  itkimage->SetDirection(cosines);
  
  
  typename itk::ImageFileWriter<ImageType>::Pointer MyWriter = itk::ImageFileWriter<ImageType>::New();
  MyWriter->SetFileName ( filename );
  MyWriter->SetInput ( MyConverter->GetOutput() );

  std::cout << "Writing: " << filename << " of type " << image->GetScalarTypeAsString() << std::endl;
  try
  {
    MyWriter->Write();
  }
  catch(itk::ExceptionObject &e)
  {
    std::cerr << e << std::endl;
    wxMessageDialog* myDialog = new wxMessageDialog(this, wxT("An error occured when writing file."),
                                                    wxT ("Error"),
                                                    wxOK|wxICON_ERROR);
    myDialog->ShowModal();
    myDialog->Destroy();
    
    return;
  }
}


#endif
