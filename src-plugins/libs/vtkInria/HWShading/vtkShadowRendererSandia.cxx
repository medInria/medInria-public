/*
 * Copyright 2003 Sandia Corporation.
 * Under the terms of Contract DE-AC04-94AL85000, there is a non-exclusive
 * license for use of this work by or on behalf of the
 * U.S. Government. Redistribution and use in source and binary forms, with
 * or without modification, are permitted provided that this Notice and any
 * statement of authorship are reproduced on all copies.
 */


#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkLightCollection.h>
#include <vtkLight.h>
#include <vtkCamera.h>
#include <vtkTransform.h>
#include <vtkTimerLog.h>
#include "vtkShadowRendererSandia.h"
//#include "vtkOpenGLExtensionManager.h"
//#include "vtkgl.h"

//#ifndef VTK_IMPLEMENT_MESA_CXX
//#  if defined(__APPLE__) && (defined(VTK_USE_CARBON) || defined(VTK_USE_COCOA))
//#    include <OpenGL/gl.h>
//#  else
//#    include <GL/gl.h>
//#  endif
//#endif

#include <GL/glew.h>


vtkCxxRevisionMacro (vtkShadowRenderer, "$Revision: 540 $");
vtkStandardNewMacro (vtkShadowRenderer);


// Textures
GLuint  light_depth;

// Polygon Offset scale and bias
float po_scale, po_bias;


void vtkShadowRenderer::InitShadowStuff()
{
  // Initalize extensions.
//  vtkOpenGLExtensionManager *extensions = vtkOpenGLExtensionManager::New();
//  extensions->SetRenderWindow(this->RenderWindow);
//#define LOAD_EXTENSION(name)				\
//  if (!extensions->ExtensionSupported(name))		\
//    {							\
//    vtkErrorMacro(name " not supported.");		\
//    }							\
//  extensions->LoadExtension(name);
//  LOAD_EXTENSION("GL_NV_register_combiners");
//  LOAD_EXTENSION("GL_ARB_depth_texture");
//  LOAD_EXTENSION("GL_NV_texture_rectangle");
//  LOAD_EXTENSION("GL_ARB_shadow");
//  LOAD_EXTENSION("GL_ARB_multitexture");
//  LOAD_EXTENSION("GL_VERSION_1_2");
//#undef LOAD_EXTENSION
//  extensions->Delete();

  // Going heavy on the bias to avoid self-shadow
  if (this->RenderWindow->GetDepthBufferSize() == 16) {
    po_scale = 1.1;
    po_bias = 4.0;
  } else if (this->RenderWindow->GetDepthBufferSize() == 24) {
    po_scale = 2.2;
    po_bias = 4.0;
  }

  glGenTextures(1,&light_depth);
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, light_depth);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV,
		  GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV,
		  GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV,
		  GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV,
		  GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    // set up hardware shadow mapping
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_COMPARE_MODE_ARB, 
		  GL_COMPARE_R_TO_TEXTURE_ARB);
  glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_COMPARE_FUNC_ARB,
		  GL_LEQUAL);
  glCopyTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_DEPTH_COMPONENT, 0, 0,
		   win_size_x, win_size_y, 0);

  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  // Enable Texture Rectangles
  glEnable(GL_TEXTURE_RECTANGLE_NV);

  // Set up texture generation
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);
  glEnable(GL_TEXTURE_GEN_R);
  glEnable(GL_TEXTURE_GEN_Q);

  // bind the depth texture for the shadow mapping
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, light_depth);
}


//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkShadowRenderer);

//----------------------------------------------------------------------------
// Description:
vtkShadowRenderer::vtkShadowRenderer()
{
  win_size_x=win_size_y=0;
  SHADOW_RENDER = true;
}


//----------------------------------------------------------------------------
vtkShadowRenderer::~vtkShadowRenderer()
{
}

//----------------------------------------------------------------------------
void vtkShadowRenderer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
static void combinercode()
{
  //vtkgl::CombinerParameteriNV(vtkgl::NUM_GENERAL_COMBINERS_NV, 1);
  glCombinerParameteriNV(GL_NUM_GENERAL_COMBINERS_NV, 1);

  // First stage input
  //vtkgl::CombinerInputNV(vtkgl::COMBINER0_NV, GL_RGB, vtkgl::VARIABLE_A_NV, 
  //			 vtkgl::PRIMARY_COLOR_NV, vtkgl::UNSIGNED_IDENTITY_NV,
  //			 GL_RGB);
  glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_A_NV,
			GL_PRIMARY_COLOR_NV, GL_UNSIGNED_IDENTITY_NV,
			GL_RGB);

  //vtkgl::CombinerInputNV(vtkgl::COMBINER0_NV, GL_RGB, vtkgl::VARIABLE_B_NV, 
  //			 GL_ZERO, vtkgl::HALF_BIAS_NEGATE_NV, GL_RGB);
  glCombinerInputNV(GL_COMBINER0_NV, GL_RGB, GL_VARIABLE_B_NV,
			GL_ZERO, GL_HALF_BIAS_NEGATE_NV, GL_RGB);

  // First stage output
  //vtkgl::CombinerOutputNV(vtkgl::COMBINER0_NV, GL_RGB, vtkgl::SPARE0_NV, 
  //			  vtkgl::DISCARD_NV, vtkgl::DISCARD_NV,
  //			  GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);
  glCombinerOutputNV(GL_COMBINER0_NV, GL_RGB, GL_SPARE0_NV,
			GL_DISCARD_NV, GL_DISCARD_NV,
			GL_NONE, GL_NONE, GL_FALSE, GL_FALSE, GL_FALSE);

  // Final stage input
  //vtkgl::FinalCombinerInputNV(vtkgl::VARIABLE_A_NV, vtkgl::TEXTURE0_ARB, 
  //			      vtkgl::UNSIGNED_IDENTITY_NV, GL_RGB);
  glFinalCombinerInputNV(GL_VARIABLE_A_NV, GL_TEXTURE0_ARB,
				GL_UNSIGNED_IDENTITY_NV, GL_RGB);
  //vtkgl::FinalCombinerInputNV(vtkgl::VARIABLE_B_NV,
  //			      vtkgl::SPARE0_PLUS_SECONDARY_COLOR_NV, 
  //			      vtkgl::UNSIGNED_IDENTITY_NV, GL_RGB);
  glFinalCombinerInputNV(GL_VARIABLE_B_NV,
				GL_SPARE0_PLUS_SECONDARY_COLOR_NV,
				GL_UNSIGNED_IDENTITY_NV, GL_RGB);
  //vtkgl::FinalCombinerInputNV(vtkgl::VARIABLE_C_NV, GL_ZERO, 
  //			      vtkgl::UNSIGNED_IDENTITY_NV, GL_RGB);
  glFinalCombinerInputNV(GL_VARIABLE_C_NV, GL_ZERO,
				GL_UNSIGNED_IDENTITY_NV, GL_RGB);

  //vtkgl::FinalCombinerInputNV(vtkgl::VARIABLE_D_NV, vtkgl::SPARE0_NV, 
  //			      vtkgl::UNSIGNED_IDENTITY_NV, GL_RGB);
  glFinalCombinerInputNV(GL_VARIABLE_D_NV, GL_SPARE0_NV,
				GL_UNSIGNED_IDENTITY_NV, GL_RGB);
}


//----------------------------------------------------------------------------
// Okay this is the real stuff
void vtkShadowRenderer::DeviceRender()
{
  static bool INIT=true;
  static vtkCamera *light_cam = vtkCamera::New();
//   static vtkLight *light = vtkLight::New();
  static float x_plane[]={1,0,0,0}, y_plane[]={0,1,0,0};
  static float z_plane[]={0,0,1,0}, w_plane[]={0,0,0,1};
  vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
  static vtkTimerLog *timer = vtkTimerLog::New();
  timer->StartTimer();

  // Do not remove this MakeCurrent! Due to Start / End methods on
  // some objects which get executed during a pipeline update, 
  // other windows might get rendered since the last time
  // a MakeCurrent was called.
  this->RenderWindow->MakeCurrent();

  // Default stuff done by VTK
  // Not doing this because it messes everything up
#if 0
  this->ClearLights();
  this->UpdateCamera();
  this->UpdateLightGeometry();
  this->UpdateLights();
#endif

  
  // Get the window size
  int *win_size = GetRenderWindow()->GetSize();
  win_size_x = win_size[0];
  win_size_y = win_size[1]; 

    // INIT stuff
  if (INIT) {
    // Shadow stuff
    InitShadowStuff();
    glEnable(GL_LIGHT0);
    INIT = false;
  }


  // Get the camera
  vtkCamera *view_cam = this->GetActiveCamera();

  // Light camera follows the regular camera
  double vec[3];
  view_cam->GetPosition(vec);
  light_cam->SetPosition(vec);
  view_cam->GetFocalPoint(vec);
  light_cam->SetFocalPoint(vec);
  view_cam->GetViewUp(vec);
  light_cam->SetViewUp(vec);
  light_cam->SetViewAngle(view_cam->GetViewAngle());
  light_cam->SetClippingRange(view_cam->GetClippingRange());

  // Off to one side
  light_cam->Roll(-10);
  // Elevate
  light_cam->Elevation(20);
  // Zoom out
  light_cam->Zoom(.8);

  // Light follows light camera
  light_cam->GetPosition(vec);
  matrix->DeepCopy(light_cam->GetViewTransformMatrix());
  matrix->Invert();
  float light_pos[4];
  light_pos[0] = matrix->Element[0][3];
  light_pos[1] = matrix->Element[1][3];
  light_pos[2] = matrix->Element[2][3];
  light_pos[3] = 1;

  // Not shadow render
  if (!SHADOW_RENDER) {

    // Disable Texture Rectangles
    glDisable(GL_TEXTURE_RECTANGLE_NV);

    // Misc buffer/lighting
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable(GL_LIGHTING);
    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SINGLE_COLOR);
    
    // Set projection matrix
    glMatrixMode(GL_PROJECTION);
    matrix->DeepCopy(
      view_cam->GetPerspectiveTransformMatrix(win_size_x/(float)win_size_y,-1,1));
    matrix->Transpose();
    glLoadMatrixd(matrix->Element[0]);
    glViewport(0, 0, win_size_x, win_size_y);
        
    // Set modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    matrix->DeepCopy(view_cam->GetViewTransformMatrix());
    matrix->Transpose();
    glLoadMatrixd(matrix->Element[0]);

    // Position the light
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos );

    // Draw geometry
    this->UpdateGeometry();
    glPopMatrix();

    // Enable Texture Rectangles
    glEnable(GL_TEXTURE_RECTANGLE_NV);

    return;
  }


  ///////////////////////////////////////////////////////////
  // Render the scene from the lights point of view
  ///////////////////////////////////////////////////////////

  // Clear buffer
  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);

  // Set up projection parameters
  glMatrixMode(GL_PROJECTION);
  matrix->DeepCopy(light_cam->GetPerspectiveTransformMatrix(win_size_x/(float)win_size_y,-1,1));
  matrix->Transpose();
  glLoadMatrixd(matrix->Element[0]);
  glViewport(0, 0, win_size_x, win_size_y);

  // Set up modelview parameters
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  matrix->DeepCopy(light_cam->GetViewTransformMatrix());
  matrix->Transpose();
  glLoadMatrixd(matrix->Element[0]);

  // Push back polygons for shadow bias
  glPolygonOffset(po_scale, po_bias);
  glEnable(GL_POLYGON_OFFSET_FILL);
  
  // Draw geometry
  this->UpdateGeometry();
  glPopMatrix();

  // bind the depth texture for the shadow mapping
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, light_depth);

  // This is slower but safer. With CopyTexSubImage you have to know how
  // big the window is going to be ahead of time
  glCopyTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_DEPTH_COMPONENT, 0, 0,
		   win_size_x, win_size_y, 0);
  //glCopyTexSubImage2D(vtkgl::TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0,win_size_x, win_size_y); 


  // Undo Push back polygons for shadow bias
  glDisable(GL_POLYGON_OFFSET_FILL);

  ///////////////////////////////////////////////////////////
  // END: Render the scene from the lights point of view
  ///////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////
  // Render the scene from the camera's point of view
  ///////////////////////////////////////////////////////////
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glEnable(GL_LIGHTING);
  glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL,
		GL_SEPARATE_SPECULAR_COLOR);
    

    // Set up the texture matrix (tex coords go through texture matrix)
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();

  // Needed for rectangular texture
  glScalef(win_size_x,win_size_y,1);
  // Offset
  glTranslatef(.5f, .5f, .5f);
  // Bias
  glScalef(.5f, .5f, .5f);
  matrix->DeepCopy(light_cam->GetCompositePerspectiveTransformMatrix(win_size_x/(float)win_size_y,-1,1));
  matrix->Transpose();
  glMultMatrixd(matrix->Element[0]);

  // bind the depth texture for the shadow mapping
  glBindTexture(GL_TEXTURE_RECTANGLE_NV, light_depth);

  // Set up projection matrix
  glMatrixMode(GL_PROJECTION);
  matrix->DeepCopy(
    view_cam->GetPerspectiveTransformMatrix(win_size_x/(float)win_size_y,-1,1));
  matrix->Transpose();
  glLoadMatrixd(matrix->Element[0]);
  glViewport(0, 0, win_size_x, win_size_y);
        

      
  // Set up modelview matrix
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  matrix->DeepCopy(view_cam->GetViewTransformMatrix());
  matrix->Transpose();
  glLoadMatrixd(matrix->Element[0]);

  // Position the light
  glLightfv(GL_LIGHT0, GL_POSITION, light_pos );

  // Set up the 'eye' planes (current modelview is applied)
  glTexGenfv(GL_S, GL_EYE_PLANE, x_plane);
  glTexGenfv(GL_T, GL_EYE_PLANE, y_plane);
  glTexGenfv(GL_R, GL_EYE_PLANE, z_plane);
  glTexGenfv(GL_Q, GL_EYE_PLANE, w_plane);

  // Draw geometry
  combinercode();
  glEnable(GL_REGISTER_COMBINERS_NV);
  this->UpdateGeometry();
  glDisable(GL_REGISTER_COMBINERS_NV);
  glPopMatrix();
  
}


