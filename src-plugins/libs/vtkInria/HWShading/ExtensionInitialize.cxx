/*============================================================================

The Hardware Shading (HWShading) module is protected by the
following copyright:

Copyright (c) 2007 Biomedical Image Analysis (BMIA) - Department of
Biomedical Engineering - Eindhoven University of Technology.
All rights reserved. See Copyright.txt for details.

The HWShading implementation was originally written by Tim Peeters (BMIA - TUe)
and published at the "11th International Fall Workshop on Vision, Modeling,
and Visualization 2006" (VMV'06):
"Visualization of the Fibrous Structure of the Heart", by T. Peeters et al.
See http://timpeeters.com/research/vmv2006 for more information.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

============================================================================*/
/**
 * ExtensionInitialize.cxx
 * by Tim Peeters
 *
 * 2006-01-30	Tim Peeters
 * - First version
 */


#include "ExtensionInitialize.h"

#include <vtkOpenGLExtensionManager.h>
#include <vtkgl.h>
#include <vtkRenderWindow.h>

bool InitializeExtensions(vtkRenderWindow* rw)
{
  cout<<"InitializeExtensions("<<rw<<")"<<endl;
  bool success = false;

  vtkOpenGLExtensionManager *extensions = vtkOpenGLExtensionManager::New();
//  extensions->DebugOn();
  extensions->SetRenderWindow(rw);

//  cout<<"Read OpenGL extensions."<<endl;
//  extensions->ReadOpenGLExtensions();

  cout<<"Updating extension manager.."<<endl;
  extensions->Update();

//  cout<<"Getting extensions string.."<<endl;
  cout<<"Extensions: "<< extensions->GetExtensionsString()<<endl;

  cout<<"Checking for OpenGL 2.0"<<endl;
  extensions->ExtensionSupported("GL_VERSION_2_0");

   if ( !extensions->ExtensionSupported("GL_VERSION_2_0") 
       || !extensions->ExtensionSupported("GL_EXT_framebuffer_object") )
//     || !extensions->ExtensionSupported("GL_ARB_multitexture") ) {
    {
//    vtkErrorMacro("Required extensions not supported!");
    return success;
    }

  extensions->LoadExtension("GL_VERSION_2_0");
  extensions->LoadExtension("GL_EXT_framebuffer_object");
  success = true;

  extensions->Delete(); extensions = NULL;

  return success;
}

