/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxCommon.h 1296 2009-10-21 17:25:16Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2009-10-21 19:25:16 +0200 (Wed, 21 Oct 2009) $
Version:   $Revision: 1296 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _wx_Common_
#define _wx_Common_

// For compilers that supports precompilation , includes  wx/wx.h
#include  "wx/wxprec.h"

#ifndef WX_PRECOMP
#include  "wx/wx.h"
#endif

#include <vtkINRIA3DConfigure.h>

// define some global variables for all wxVtk programs

static const wxString wxSupportedImageExtensionsFilter=
  wxT ("Image File (*.hdr;*.inr;*.inr.gz;*.gipl;*.gipl.gz;*.dim;*.ima;*.mha;*.mhd;*.vtk;*.nrrd;*.nhdr;*.nii;*.nii.gz;*.jpg;*.tif)|*.hdr;*.inr;*.inr.gz;*.gipl;*.gipl.gz;*.dim;*.ima;*.mha;*.mhd;*.vtk;*.nrrd;*.nhdr;*.nii;*.nii.gz;*.jpg;*.tif;*.par;*.PAR|")
  wxT ("Analyze (*.hdr)|*.hdr|")
  wxT ("Inrimage (*.inr;*.inr.gz)|*.inr;*.inr.gz|")
  wxT ("Gipl (*.gipl;*.gipl.gz)|*.gipl;*.gipl.gz|")
  wxT ("GIS (*.dim;*.ima)|*.dim;*.ima|")
  wxT ("Metafile (*.mha;*.mhd)|*.mha;*.mhd|")
  wxT ("VTK (*.vtk)|*.vtk|")
  wxT ("Nrrd (*.nrrd;*.nhdr)|*.nrrd;*.nhdr|")
  wxT ("NIfTI (*.nii;*.nii.gz)|*.nii;*.nii.gz|")
  wxT ("Jpeg (*.jpg;*.jpeg)|*.jpg;*.jpeg|")
  wxT ("Tiff (*.tif;*.tiff)|*.tif;*.tiff|")
  wxT ("PAR/REC (*.PAR;*.PAR)|*.par;*.PAR|")
  wxT ("All (*)|*");
  
static const wxString wxSupportedTensorImageExtensionsFilter=
  wxT ("Tensor File (*.mha;*.mhd;*.inr.gz;*.vtk;*.nrrd;*.nhdr;*.nii;*.nii.gz)|*.inr.gz;*.vtk;*.nrrd;*.nhdr;*.nii;*.nii.gz|")
  wxT ("ITK MetaImage (*.mha)|*.mhd|")
  wxT ("Inrimage (*.inr.gz)|*.inr.gz|")
  wxT ("VTK (*.vtk)|*.vtk|")
  wxT ("Nrrd (*.nrrd;*.nhdr)|*.nrrd;*.nhdr|")
  wxT ("NIfTI (*.nii;*.nii.gz)|*.nii;*.nii.gz|")
  wxT ("All (*.*)|*.*");

static const wxString wxSupportedQBallImageExtensionsFilter=
  wxT ("Q-Ball File (*.mhd)|*.mhd|")
  wxT ("All (*.*)|*.*");

#endif
