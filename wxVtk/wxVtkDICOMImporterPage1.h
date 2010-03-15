/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxVtkDICOMImporterPage1.h 609 2008-01-14 09:21:24Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2008-01-14 10:21:24 +0100 (Mon, 14 Jan 2008) $
Version:   $Revision: 609 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _wx_VtkDICOMImporterPage1_h_
#define _wx_VtkDICOMImporterPage1_h_

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
#include  "wx/wx.h"
#endif

#include "wx/wizard.h"
#include <vtkINRIA3DConfigure.h>

/**
   \class wxVtkDICOMImporterPage1 wxVtkDICOMImporterPage1.h "wxVtkDICOMImporterPage1.h"
   \brief see wxVtkDICOMImporter
    
   \author Nicolas Toussaint
 */

class WX_VTK_EXPORT wxVtkDICOMImporterPage1 : public wxWizardPageSimple
{

 public:
  wxVtkDICOMImporterPage1 (wxWizard* parent)
    : wxWizardPageSimple (parent)
  {
    m_Text =
      new wxStaticText(this, wxID_ANY,
                       wxT("This wizard will guide you through the importation of a DICOM folder.\n")
                       wxT("\n\n")
                       wxT("You will be asked to input the DICOM root directory.\n")
                       wxT("This importer will reconstruct all possible 3D volumes.\n")
		       wxT("It is a fully automatic procedure, although you can manually split a ")
                       wxT("volume if it contains more than one image (interlaced acquisition), ")
                       wxT("and choose which images you would like to import.\n")
                       wxT("\n\n")
                       wxT("Please click Next to proceed."),
                       wxPoint(5,5));
  }

  virtual ~wxVtkDICOMImporterPage1(){};
  

  wxStaticText* m_Text;

 protected:

 private:
  
};


#endif
