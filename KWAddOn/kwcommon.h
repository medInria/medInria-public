/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: kwcommon.h 1296 2009-10-21 17:25:16Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2009-10-21 18:25:16 +0100 (Wed, 21 Oct 2009) $
Version:   $Revision: 1296 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __kwcommon_h
#define __kwcommon_h

#include <string>
#include <vtkKWMessageDialog.h>

static const std::string kwsupportedimageformats = "{{analyse} {.hdr}} {{inrimage} {.inr.gz .inr .gz}} {{gipl} {.gipl}} {{metafile} {.mha .mhd}} {{nrrd} {.nrrd}} {{NIfTI} {.nii .nii.gz}} {{jpeg} {.jpg .jpeg}} {{tiff} {.tiff .tif}} {{png} {.png .PNG}} {{PAR/REC} {.PAR .par}}";

static const std::string kwsupportedmeshformats = "{{vtkPolyData} {.vtp .vtk}} {{vtkUnstructuredGrid} {.vtu .vtk}} {{mips triangulation} {.tr3D .atr3D .mesh .tr .trian .obj .points .faces .s3d}} {{mips tetrahedrisation} {.tet3D .atet3D}} {{generic tetrahedrisation} {.noboite .mesh .msh}} {{mips simplex} {.sm}} ";

static const std::string kwsupportedimageformats_global = "{{image files} {.hdr .inr.gz .inr .gz .gipl .mha .mhd .nrrd .nii .nii.gz .jpg .jpeg .tif .tiff .png .PAR .par}} ";

static const std::string kwsupportedmeshformats_global = "{{mesh files} {.vtk .vtp .vtu .tr3D .atr3D .mesh .tr .trian .obj .points .faces .s3d .tet3D .atet3D .noboite .msh .sm}} ";

static const std::string kwallformat = "{{all} {.*}}";

static const std::string kwsupportedattributesformats = "{{all supported formats} {.cv* .*bb .*}} {{attribute file} {.cv*}} {{ bb files} {.*bb}} {{all} {.*}}";

static const std::string kwsupportedmanagerformats = "{{XML manager file} {.dmf .xml}} {{TCL script file} {.tcl}} ";

static const std::string kwallsupportedformats = "{{all supported formats} {.hdr .inr .gz .gipl .mha .mhd .nrrd .nii .jpg .jpeg .tif .tiff .png .PAR .par .vtk .vtp .vtu .tr3D .atr3D .mesh .tr .trian .obj .s3d .tet3D .atet3D .sm .dmf .xml .tcl .msh }} ";

static const std::string kwsupportedformats = kwallsupportedformats + kwsupportedimageformats_global + kwsupportedmeshformats_global + kwsupportedimageformats + kwsupportedmeshformats + kwsupportedmanagerformats + kwallformat;

#define vtkKWPopupErrorMessage(object, message)			\
  								\
  vtkKWMessageDialog::PopupMessage(object->GetApplication(),		\
				   object->GetParentTopLevel(),		\
				   "Error",				\
				   message,				\
				   vtkKWMessageDialog::ErrorIcon);

#define vtkKWPopupWarningMessage(object, message)			\
  									\
  vtkKWMessageDialog::PopupMessage(object->GetApplication(),		\
				   object->GetParentTopLevel(),		\
				   "Warning",				\
				   message,				\
				   vtkKWMessageDialog::WarningIcon);

#define vtkKWPopupInfoMessage(object, message)				\
  									\
  vtkKWMessageDialog::PopupMessage(object->GetApplication(),		\
				   object->GetParentTopLevel(),		\
				   "Info",				\
				   message,				\
				   vtkKWMessageDialog::QuestionIcon);


#endif
