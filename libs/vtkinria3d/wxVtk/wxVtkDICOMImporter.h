/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxVtkDICOMImporter.h 1193 2009-07-20 07:44:11Z filus $
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
#ifndef _wx_VtkDICOMImporter_h_
#define _wx_VtkDICOMImporter_h_

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
#endif

#include "wx/wizard.h"

#include "itkDICOMImageIO2Factory.h"
#include "itkDICOMImageIO2.h"
#include "itkGDCMImageIO.h"
#include "itkImageSeriesReader.h"
#include "itkDICOMSeriesFileNames.h"

#include <itkGDCMImporter2.h>
#include <vtkINRIA3DConfigure.h>

class vtkImageData;
class wxVtkDICOMImporterPage1;
class wxVtkDICOMImporterPage2;

/**
   \class wxVtkDICOMImporter wxVtkDICOMImporter.h "wxVtkDICOMImporter.h"
   \brief This Class is a wxwidgets based WIZARD that aims to import\n
   a DICOM exam (i.e. the root directory where DICOM files are)\n
   it takes a directory as input and uses 2D views to check the\n
   relevency of the ordered volumes.\n
   It can handle several rules of sorting the files.\n
   It can also sort specific acquisitions such as T2 / Proton density\n
   or DWI by setting the number of volumes and the way they are interlaced\n
   
   The output is a vector of 3D itk::images (unsigned short).\n
   Please use GetOutputs(void) or GetOutput(unsigned int i) to get the volumes\n
   You also can use the description of each volume with GetDescriptions(void)\n
   or GetDescription(unsigned int i)
   
   \author Nicolas Toussaint
   \date   Jan. 2007
*/


class WX_VTK_EXPORT wxVtkDICOMImporter : public wxWizard
{
  
 public:
  wxVtkDICOMImporter (wxWindow*, bool manual = 0);
  virtual ~wxVtkDICOMImporter();

  /** Type definition for ITK */
  typedef unsigned short ImageComponentType;
  typedef itk::Image<ImageComponentType,3> IntegerImageType;
  typedef IntegerImageType ImageType;
  typedef itk::GDCMImporter2<ImageType> GDCMImporterType;
  typedef GDCMImporterType::DICOMImageType DICOMImageType;
  
  typedef itk::ImageSeriesReader<IntegerImageType> ReaderType;
  
  /** Get a specific output volume from its id */
  ImageType::Pointer GetOutput (unsigned int i) const
  {
    if (i >= 0 && i < m_Outputs.size())
      return m_Outputs[i];
    return NULL;
  }
  /** Get a specific volume description from the volume id */
  std::string GetDescription (unsigned int i)
  {
    if (i >= 0 && i < m_Descriptions.size())
      return m_Descriptions[i];
    return NULL;
  }
  
  
  GDCMImporterType::Pointer GetImporter (void);
  
  /**
     Get the output vector containing all valid 3D images (itk)
     This function has to be called after the dicom importation is finished
     (after the user click finish)
  */
  std::vector<ImageType::Pointer> GetOutputs (void) const
  { return m_Outputs; }
  std::vector<std::string> GetDescriptions (void) const
  { return m_Descriptions; }
  unsigned int GetNumberOfOutputs (void) const
  { return m_Outputs.size(); }


  void LoadDirectory(const char* directoryname);
  void RunFromPage (unsigned int page = 1);
  
  
 protected:
  DECLARE_EVENT_TABLE()
 private:

  void OnPageChanged  (wxWizardEvent &event);
  void OnPageChanging (wxWizardEvent &event);
  void OnFinished     (wxWizardEvent &event);
  // get methods
  wxVtkDICOMImporterPage1* GetPage1 (void)
  { return m_Page1; }
  wxVtkDICOMImporterPage2* GetPage2 (void)
  { return m_Page2; }

  
  //list here all pages
  wxVtkDICOMImporterPage1* m_Page1;
  wxVtkDICOMImporterPage2* m_Page2;
  std::vector<ImageType::Pointer> m_Outputs;
  std::vector<std::string> m_Descriptions;
	
  
};


#endif
