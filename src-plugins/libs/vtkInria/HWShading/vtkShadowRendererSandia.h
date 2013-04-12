/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: CodeCopyright.txt 1182 2009-06-30 09:19:05Z acanale $
Language:  C++
Author:    $Author: $
Date:      $Date: 2009-06-30 11:19:05 +0200 (mar, 30 jun 2009) $
Version:   $Revision: 1182 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

/*
 * Copyright 2003 Sandia Corporation.
 * Under the terms of Contract DE-AC04-94AL85000, there is a non-exclusive
 * license for use of this work by or on behalf of the
 * U.S. Government. Redistribution and use in source and binary forms, with
 * or without modification, are permitted provided that this Notice and any
 * statement of authorship are reproduced on all copies.
 */

#pragma once

//#include "vtksnlRenderingWin32Header.h"
#include <vtkOpenGLRenderer.h>


//class VTK_SNL_RENDERING_EXPORT vtkShadowRenderer : public vtkOpenGLRenderer {
class VTK_HWSHADING_EXPORT vtkShadowRenderer : public vtkOpenGLRenderer {

public:
  static vtkShadowRenderer *New();
  vtkTypeRevisionMacro(vtkShadowRenderer,vtkOpenGLRenderer);
  
  void PrintSelf(ostream& os, vtkIndent indent);
  void SetShadows(bool b) {SHADOW_RENDER = b;}
  bool GetShadows() {return SHADOW_RENDER;}
  
protected:
  virtual void DeviceRender();
  vtkShadowRenderer();
  ~vtkShadowRenderer();

private:
  vtkShadowRenderer(const vtkShadowRenderer&);  // Not implemented
  void operator=(const vtkShadowRenderer&);     // Not implemented

  void InitShadowStuff();

  // Attributes
  int win_size_x;
  int win_size_y;
  bool SHADOW_RENDER;
 
};


