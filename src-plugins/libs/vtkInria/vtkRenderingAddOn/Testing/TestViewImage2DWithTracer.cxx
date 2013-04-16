/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include "vtkInteractorEventRecorder.h"

#include <vtkRenderingAddOn/vtkViewImage2DWithTracer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageEllipsoidSource.h>

char ImageTracerWidgetEventLog[] =
"# StreamVersion 1\n"
"MouseMoveEvent 322 145 0 0 0 0  b\n"
  //"LeftButtonPressEvent 322 145 0 0 0 0  b\n"
//"LeftButtonReleaseEvent 322 145 0 0 0 0  b\n"
"MouseMoveEvent 146 166 0 0 0 0  b\n"
"LeftButtonPressEvent 146 166 0 0 0 0  b\n"
"MouseMoveEvent 154 161 0 0 0 0  b\n"
"MouseMoveEvent 162 148 0 0 0 0  b\n"
"MouseMoveEvent 169 129 0 0 0 0  b\n"
"MouseMoveEvent 168 100 0 0 0 0  b\n"
"MouseMoveEvent 161 95 0 0 0 0  b\n"
"MouseMoveEvent 131 90 0 0 0 0  b\n"
"MouseMoveEvent 113 95 0 0 0 0  b\n"
"MouseMoveEvent 77 116 0 0 0 0  b\n"
"MouseMoveEvent 68 132 0 0 0 0  b\n"
"MouseMoveEvent 67 151 0 0 0 0  b\n"
"MouseMoveEvent 73 165 0 0 0 0  b\n"
"MouseMoveEvent 89 179 0 0 0 0  b\n"
"MouseMoveEvent 98 182 0 0 0 0  b\n"
"MouseMoveEvent 111 182 0 0 0 0  b\n"
"MouseMoveEvent 118 182 0 0 0 0  b\n"
"MouseMoveEvent 130 177 0 0 0 0  b\n"
"MouseMoveEvent 134 175 0 0 0 0  b\n"
"MouseMoveEvent 144 170 0 0 0 0  b\n"
"MouseMoveEvent 146 167 0 0 0 0  b\n"
//"LeftButtonReleaseEvent 146 167 0 0 0 0  b\n"
"MouseMoveEvent 132 164 0 0 0 0  b\n"
//"MiddleButtonPressEvent 132 164 0 0 0 0  b\n"
//"MiddleButtonReleaseEvent 132 164 0 0 0 0  b\n"
"MouseMoveEvent 131 163 0 0 0 0  b\n"
"MouseMoveEvent 127 161 0 0 0 0  b\n"
"MouseMoveEvent 120 153 0 0 0 0  b\n"
"MouseMoveEvent 110 146 0 0 0 0  b\n"
"MouseMoveEvent 104 140 0 0 0 0  b\n"
"MouseMoveEvent 101 132 0 0 0 0  b\n"
"MouseMoveEvent 99 128 0 0 0 0  b\n"
"MouseMoveEvent 95 123 0 0 0 0  b\n"
"MouseMoveEvent 91 116 0 0 0 0  b\n"
//"MiddleButtonPressEvent 91 116 0 0 0 0  b\n"
//"MiddleButtonReleaseEvent 91 116 0 0 0 0  b\n"
"MouseMoveEvent 95 116 0 0 0 0  b\n"
"MouseMoveEvent 105 118 0 0 0 0  b\n"
"MouseMoveEvent 115 121 0 0 0 0  b\n"
"MouseMoveEvent 124 124 0 0 0 0  b\n"
"MouseMoveEvent 136 127 0 0 0 0  b\n"
"MouseMoveEvent 144 128 0 0 0 0  b\n"
"MouseMoveEvent 150 130 0 0 0 0  b\n"
"MouseMoveEvent 154 132 0 0 0 0  b\n"
"MouseMoveEvent 157 133 0 0 0 0  b\n"
"MouseMoveEvent 161 133 0 0 0 0  b\n"
"MouseMoveEvent 164 134 0 0 0 0  b\n"
"MouseMoveEvent 167 135 0 0 0 0  b\n"
"MouseMoveEvent 169 136 0 0 0 0  b\n"
  //"KeyPressEvent 169 136 -128 0 0 1 Control_L\n"
//  "KeyPressEvent 169 136 0 0 0 1 v\n"
  //"KeyReleaseEvent 169 136 0 0 0 1 v\n"
//"MiddleButtonPressEvent 169 136 8 0 0 0 Control_L\n"
//"MiddleButtonReleaseEvent 169 136 8 0 0 0 Control_L\n"
  //"KeyReleaseEvent 169 136 0 0 0 1 Control_L\n"
//"RightButtonPressEvent 169 136 0 0 0 0 Control_L\n"
"MouseMoveEvent 167 142 0 0 0 0 Control_L\n"
"MouseMoveEvent 164 146 0 0 0 0 Control_L\n"
"MouseMoveEvent 162 149 0 0 0 0 Control_L\n"
"MouseMoveEvent 159 152 0 0 0 0 Control_L\n"
"MouseMoveEvent 155 155 0 0 0 0 Control_L\n"
"MouseMoveEvent 152 157 0 0 0 0 Control_L\n"
"MouseMoveEvent 148 159 0 0 0 0 Control_L\n"
"MouseMoveEvent 143 163 0 0 0 0 Control_L\n"
"MouseMoveEvent 137 165 0 0 0 0 Control_L\n"
"MouseMoveEvent 133 166 0 0 0 0 Control_L\n"
"MouseMoveEvent 132 164 0 0 0 0 Control_L\n"
//"RightButtonReleaseEvent 132 164 0 0 0 0 Control_L\n"
"MouseMoveEvent 133 164 0 0 0 0 Control_L\n"
//"KeyPressEvent 133 164 -128 0 0 1 Control_L\n"
//"RightButtonPressEvent 133 164 8 0 0 0 Control_L\n"
//"RightButtonReleaseEvent 133 164 8 0 0 0 Control_L\n"
//"KeyReleaseEvent 133 164 0 0 0 1 Control_L\n"
"MouseMoveEvent 133 164 0 0 0 0 Control_L\n"
"MouseMoveEvent 129 162 0 0 0 0 Control_L\n"
"MouseMoveEvent 125 160 0 0 0 0 Control_L\n"
"MouseMoveEvent 125 156 0 0 0 0 Control_L\n"
"MouseMoveEvent 122 154 0 0 0 0 Control_L\n"
"MouseMoveEvent 121 152 0 0 0 0 Control_L\n"
//"KeyPressEvent 121 152 0 -128 0 1 Shift_L\n"
//"RightButtonPressEvent 121 152 0 4 0 0 Shift_L\n"
//"RightButtonReleaseEvent 121 152 0 4 0 0 Shift_L\n"
//"KeyReleaseEvent 121 152 0 0 0 1 Shift_L\n"
"MouseMoveEvent 108 137 0 0 0 0 Shift_L\n"
//"KeyPressEvent 108 137 0 -128 0 1 Shift_L\n"
//"RightButtonPressEvent 108 137 0 4 0 0 Shift_L\n"
//"RightButtonReleaseEvent 108 137 0 4 0 0 Shift_L\n"
//"KeyReleaseEvent 108 137 0 0 0 1 Shift_L\n"
//"RightButtonPressEvent 108 137 0 0 0 0 Shift_L\n"
"MouseMoveEvent 112 127 0 0 0 0 Shift_L\n"
"MouseMoveEvent 118 116 0 0 0 0 Shift_L\n"
"MouseMoveEvent 121 109 0 0 0 0 Shift_L\n"
"MouseMoveEvent 128 97 0 0 0 0 Shift_L\n"
"MouseMoveEvent 134 88 0 0 0 0 Shift_L\n"
"MouseMoveEvent 136 86 0 0 0 0 Shift_L\n"
//"RightButtonReleaseEvent 136 86 0 0 0 0 Shift_L\n"
//"MouseMoveEvent 122 152 0 0 0 0 Shift_L\n"
//"RightButtonPressEvent 122 152 0 0 0 0 Shift_L\n"
"MouseMoveEvent 125 149 0 0 0 0 Shift_L\n"
"MouseMoveEvent 156 143 0 0 0 0 Shift_L\n"
"MouseMoveEvent 164 141 0 0 0 0 Shift_L\n"
"MouseMoveEvent 168 140 0 0 0 0 Shift_L\n"
"MouseMoveEvent 170 140 0 0 0 0 Shift_L\n"
//"RightButtonReleaseEvent 170 140 0 0 0 0 Shift_L\n"
//"MouseMoveEvent 129 166 0 0 0 0 Shift_L\n"
//"RightButtonPressEvent 129 166 0 0 0 0 Shift_L\n"
"MouseMoveEvent 127 164 0 0 0 0 Shift_L\n"
"MouseMoveEvent 115 152 0 0 0 0 Shift_L\n"
"MouseMoveEvent 104 140 0 0 0 0 Shift_L\n"
"MouseMoveEvent 95 130 0 0 0 0 Shift_L\n"
"MouseMoveEvent 89 124 0 0 0 0 Shift_L\n"
"MouseMoveEvent 88 118 0 0 0 0 Shift_L\n"
//"RightButtonReleaseEvent 88 118 0 0 0 0 Shift_L\n"
//"MouseMoveEvent 168 140 0 0 0 0 Shift_L\n"
//"RightButtonPressEvent 168 140 0 0 0 0 Shift_L\n"
"MouseMoveEvent 165 140 0 0 0 0 Shift_L\n"
"MouseMoveEvent 162 142 0 0 0 0 Shift_L\n"
"MouseMoveEvent 159 145 0 0 0 0 Shift_L\n"
"MouseMoveEvent 156 146 0 0 0 0 Shift_L\n"
"MouseMoveEvent 153 148 0 0 0 0 Shift_L\n"
"MouseMoveEvent 150 150 0 0 0 0 Shift_L\n"
"MouseMoveEvent 147 153 0 0 0 0 Shift_L\n"
//"RightButtonReleaseEvent 147 153 0 0 0 0 Shift_L\n"
"MouseMoveEvent 137 84 0 0 0 0 Shift_L\n"
//"RightButtonPressEvent 137 84 0 0 0 0 Shift_L\n"
"MouseMoveEvent 133 94 0 0 0 0 Shift_L\n"
"MouseMoveEvent 130 107 0 0 0 0 Shift_L\n"
"MouseMoveEvent 123 124 0 0 0 0 Shift_L\n"
"MouseMoveEvent 110 147 0 0 0 0 Shift_L\n"
"MouseMoveEvent 99 160 0 0 0 0 Shift_L\n"
//"RightButtonReleaseEvent 99 160 0 0 0 0 Shift_L\n"
"MouseMoveEvent 337 163 0 0 0 0 Shift_L\n"
  //"RightButtonPressEvent 337 163 0 0 0 0 Shift_L\n"
"MouseMoveEvent 337 162 0 0 0 0 Shift_L\n"
"MouseMoveEvent 337 160 0 0 0 0 Shift_L\n"
"MouseMoveEvent 338 158 0 0 0 0 Shift_L\n"
"MouseMoveEvent 342 153 0 0 0 0 Shift_L\n"
"MouseMoveEvent 346 149 0 0 0 0 Shift_L\n"
"MouseMoveEvent 349 147 0 0 0 0 Shift_L\n"
"MouseMoveEvent 352 144 0 0 0 0 Shift_L\n"
"MouseMoveEvent 354 141 0 0 0 0 Shift_L\n"
"MouseMoveEvent 356 139 0 0 0 0 Shift_L\n"
"MouseMoveEvent 358 136 0 0 0 0 Shift_L\n"
"MouseMoveEvent 359 135 0 0 0 0 Shift_L\n"
"MouseMoveEvent 360 133 0 0 0 0 Shift_L\n"
"MouseMoveEvent 360 131 0 0 0 0 Shift_L\n"
"MouseMoveEvent 361 130 0 0 0 0 Shift_L\n"
"MouseMoveEvent 362 128 0 0 0 0 Shift_L\n"
"MouseMoveEvent 364 124 0 0 0 0 Shift_L\n"
"MouseMoveEvent 365 122 0 0 0 0 Shift_L\n"
"MouseMoveEvent 367 119 0 0 0 0 Shift_L\n"
"MouseMoveEvent 368 117 0 0 0 0 Shift_L\n"
"MouseMoveEvent 369 114 0 0 0 0 Shift_L\n"
"MouseMoveEvent 370 113 0 0 0 0 Shift_L\n"
"MouseMoveEvent 370 112 0 0 0 0 Shift_L\n"
"MouseMoveEvent 370 113 0 0 0 0 Shift_L\n"
"MouseMoveEvent 368 114 0 0 0 0 Shift_L\n"
"MouseMoveEvent 367 115 0 0 0 0 Shift_L\n"
"MouseMoveEvent 366 116 0 0 0 0 Shift_L\n"
"MouseMoveEvent 366 118 0 0 0 0 Shift_L\n"
"MouseMoveEvent 365 118 0 0 0 0 Shift_L\n"
"MouseMoveEvent 365 120 0 0 0 0 Shift_L\n"
"MouseMoveEvent 364 121 0 0 0 0 Shift_L\n"
"MouseMoveEvent 363 123 0 0 0 0 Shift_L\n"
"MouseMoveEvent 362 125 0 0 0 0 Shift_L\n"
"MouseMoveEvent 362 127 0 0 0 0 Shift_L\n"
"MouseMoveEvent 361 128 0 0 0 0 Shift_L\n"
"MouseMoveEvent 360 130 0 0 0 0 Shift_L\n"
"MouseMoveEvent 360 131 0 0 0 0 Shift_L\n"
"MouseMoveEvent 359 133 0 0 0 0 Shift_L\n"
"MouseMoveEvent 358 134 0 0 0 0 Shift_L\n"
"MouseMoveEvent 357 136 0 0 0 0 Shift_L\n"
"MouseMoveEvent 356 139 0 0 0 0 Shift_L\n"
"MouseMoveEvent 355 141 0 0 0 0 Shift_L\n"
"MouseMoveEvent 354 143 0 0 0 0 Shift_L\n"
"MouseMoveEvent 353 145 0 0 0 0 Shift_L\n"
"MouseMoveEvent 352 147 0 0 0 0 Shift_L\n"
"MouseMoveEvent 352 148 0 0 0 0 Shift_L\n"
"MouseMoveEvent 352 150 0 0 0 0 Shift_L\n"
"MouseMoveEvent 351 152 0 0 0 0 Shift_L\n"
"MouseMoveEvent 350 156 0 0 0 0 Shift_L\n"
"MouseMoveEvent 349 158 0 0 0 0 Shift_L\n"
  //"RightButtonReleaseEvent 349 158 0 0 0 0 Shift_L\n"
"MouseMoveEvent 381 179 0 0 0 0 Shift_L\n"
  //"LeftButtonPressEvent 381 179 0 0 0 0 Shift_L\n"
"MouseMoveEvent 382 179 0 0 0 0 Shift_L\n"
"MouseMoveEvent 379 179 0 0 0 0 Shift_L\n"
"MouseMoveEvent 376 177 0 0 0 0 Shift_L\n"
"MouseMoveEvent 371 174 0 0 0 0 Shift_L\n"
"MouseMoveEvent 364 167 0 0 0 0 Shift_L\n"
"MouseMoveEvent 353 156 0 0 0 0 Shift_L\n"
"MouseMoveEvent 348 146 0 0 0 0 Shift_L\n"
"MouseMoveEvent 345 139 0 0 0 0 Shift_L\n"
"MouseMoveEvent 342 129 0 0 0 0 Shift_L\n"
"MouseMoveEvent 340 121 0 0 0 0 Shift_L\n"
"MouseMoveEvent 337 111 0 0 0 0 Shift_L\n"
"MouseMoveEvent 336 101 0 0 0 0 Shift_L\n"
"MouseMoveEvent 336 98 0 0 0 0 Shift_L\n"
"MouseMoveEvent 335 95 0 0 0 0 Shift_L\n"
"MouseMoveEvent 335 93 0 0 0 0 Shift_L\n"
"MouseMoveEvent 333 91 0 0 0 0 Shift_L\n"
"MouseMoveEvent 331 87 0 0 0 0 Shift_L\n"
"MouseMoveEvent 329 85 0 0 0 0 Shift_L\n"
"MouseMoveEvent 329 84 0 0 0 0 Shift_L\n"
"MouseMoveEvent 328 84 0 0 0 0 Shift_L\n"
"LeftButtonReleaseEvent 328 84 0 0 0 0 Shift_L\n"
"KeyPressEvent 328 84 0 0 0 0 v\n"
"KeyReleaseEvent 328 84 0 0 0 0 v\n"
;


int TestViewImage2DWithTracer( int argc, char *argv[] )
{

  vtkViewImage2DWithTracer* view = vtkViewImage2DWithTracer::New();
  vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();

  vtkRenderWindow* rwin = vtkRenderWindow::New();
  
  vtkRenderer* renderer = vtkRenderer::New();

  iren->SetRenderWindow (rwin);

  rwin->AddRenderer (renderer);

  view->SetRenderWindow ( rwin );

  view->SetRenderer ( renderer );

  view->SetInteractionStyle (vtkViewImage2D::SELECT_INTERACTION);

  view->SetOrientation (vtkViewImage2D::AXIAL_ID);
  
  view->SetAboutData ("Powered by vtkINRIA3D");
  

  vtkImageEllipsoidSource* imageSource = vtkImageEllipsoidSource::New();
  imageSource->SetWholeExtent (0, 63, 0, 63, 0, 63);
  imageSource->SetCenter (32,32,32);
  imageSource->SetRadius (32,16,16);
  imageSource->Update();

  vtkImageData* image = imageSource->GetOutput();
  //image->SetSpacing (3.2, 3.2, 1.5);


  view->SetImage (image);
  view->SetManualTracingVisibility(1);

  view->SyncResetCurrentPoint();
  view->SyncResetWindowLevel();

  iren->Initialize();
  
  view->Render();

  
  vtkInteractorEventRecorder *recorder = vtkInteractorEventRecorder::New();
  recorder->SetInteractor(iren);
  recorder->ReadFromInputStringOn();
  recorder->SetInputString(ImageTracerWidgetEventLog);
    
  recorder->Play();

  // Remove the observers so we can go interactive. Without this the "-I"
  // testing option fails.
  recorder->Off();


  view->ValidateTracing();

  vtkImageData* roi = view->GetMaskImage();
  if ( !roi )
  {
    return EXIT_FAILURE;
  }

  recorder->SetInteractor (0);
  
  view->Delete();
  rwin->Delete();
  iren->Delete();
  renderer->Delete();
  imageSource->Delete();
  recorder->Delete();
  
  return 0;
}
