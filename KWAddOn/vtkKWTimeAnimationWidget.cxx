/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWTimeAnimationWidget.cxx 1059 2009-01-12 14:16:10Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2009-01-12 15:16:10 +0100 (Mon, 12 Jan 2009) $
Version:   $Revision: 1059 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkKWTimeAnimationWidget.h"
#include "vtkObjectFactory.h"


#include "vtkKWApplication.h"
#include "vtkCamera.h"
#include "vtkImageData.h"
#include "vtkKWComboBoxWithLabel.h"
#include "vtkKWComboBox.h"
#include "vtkKWIcon.h"
#include "vtkKWInternationalization.h"
#include "vtkKWLabel.h"
#include "vtkKWLabelWithLabel.h"
#include "vtkKWLoadSaveDialog.h"
#include "vtkKWMenu.h"
#include "vtkKWMessageDialog.h"
#include "vtkKWProgressGauge.h"
#include "vtkKWPushButton.h"
#include "vtkKWPushButtonSet.h"
#include "vtkRenderer.h"
#include "vtkKWScaleWithEntry.h"
#include "vtkKWScaleWithEntrySet.h"
#include "vtkRenderWindow.h"
#include "vtkKWRenderWidget.h"
#include "vtkWindowToImageFilter.h"
#include "vtkKWWindowBase.h"
#include "vtkToolkits.h"
#include "vtkImageWriter.h"
#include "vtkJPEGWriter.h"
#include "vtkTIFFWriter.h"
#include "vtkGenericMovieWriter.h"
#include <vtkKWScaleWithEntry.h>
#include <vtkViewImage.h>
#include <vtkKWCheckButtonWithLabel.h>
#include <vtkKWCheckButton.h>

#include <kwcommon.h>
#include <string>
#include <vtksys/SystemTools.hxx>
#include <fstream>
#include <sstream>
#include <vtkKWWidgetsVersion.h>



#ifdef VTK_USE_VIDEO_FOR_WINDOWS
#include "vtkKWWidgetsBuildConfigure.h"
#include "vtkAVIWriter.h"
#else
#ifdef VTK_USE_FFMPEG_ENCODER
#include "vtkFFMPEGWriter.h"
#endif
#endif

#ifdef VTK_USE_MPEG2_ENCODER
#include "vtkMPEG2Writer.h"
#endif

#include <vtksys/SystemTools.hxx>
#include <vtksys/stl/string>


//----------------------------------------------------------------------------

#define VTK_VV_ANIMATION_BUTTON_PREVIEW_ID 0
#define VTK_VV_ANIMATION_BUTTON_CREATE_ID  1
#define VTK_VV_ANIMATION_BUTTON_CANCEL_ID  2

#define VTK_VV_ANIMATION_SCALE_NB_OF_FRAMES_ID 0
#define VTK_VV_ANIMATION_SCALE_SLICE_START_ID  1
#define VTK_VV_ANIMATION_SCALE_SLICE_END_ID    2
#define VTK_VV_ANIMATION_SCALE_AZIMUTH_ID      3
#define VTK_VV_ANIMATION_SCALE_ELEVATION_ID    4
#define VTK_VV_ANIMATION_SCALE_ROLL_ID         5
#define VTK_VV_ANIMATION_SCALE_ZOOM_ID         6

#define VTK_VV_ANIMATION_SCALE_NB_FRAMES       500


//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWTimeAnimationWidget );
vtkCxxRevisionMacro(vtkKWTimeAnimationWidget, "$Revision: 1059 $");

//----------------------------------------------------------------------------
vtkKWTimeAnimationWidget::vtkKWTimeAnimationWidget()
{
  this->SequenceUpdateCommand = NULL;
  this->SequenceRange[0] = 0;
  this->SequenceRange[1] = 0;
  this->NumFrames        = 0;

  this->GoAndBackMode = 0;  
}

//----------------------------------------------------------------------------
vtkKWTimeAnimationWidget::~vtkKWTimeAnimationWidget()
{
  if (this->SequenceUpdateCommand)
    {
    delete [] this->SequenceUpdateCommand;
    this->SequenceUpdateCommand = NULL;
    }
}



//----------------------------------------------------------------------------
void vtkKWTimeAnimationWidget::CreateWidget()
{
  if (this->IsCreated())
    {
    vtkErrorMacro("vtkKWTimeAnimationWidget already created.");
    return;
    }
  
  // Call the superclass to create the whole widget

  this->Superclass::CreateWidget();

  this->HelpLabel->GetWidget()->SetText(
					k_(""));

  vtkKWScaleWithEntry* scale = this->Parameters->GetWidget(VTK_VV_ANIMATION_SCALE_NB_OF_FRAMES_ID);
  if (scale)
    this->Script("pack forget %s", scale->GetWidgetName());

  vtkKWCheckButtonWithLabel* cb = vtkKWCheckButtonWithLabel::New();
  cb->SetParent (this);
  cb->Create();
  
  cb->SetLabelText ("Go & back mode : ");
  cb->SetLabelPositionToLeft();
  cb->GetWidget()->SetCommand (this, "GoAndBackCallback");

  this->Script("pack %s -side top -anchor nw -expand false -fill none", 
	       cb->GetWidgetName());

  
}


//----------------------------------------------------------------------------
void vtkKWTimeAnimationWidget::GoAndBackCallback(int val)
{
  this->GoAndBackMode = val;
}

//----------------------------------------------------------------------------
void vtkKWTimeAnimationWidget::SetAnimationType(int val)
{
  if (val < vtkKWTimeAnimationWidget::AnimationTypeCamera)
    {
    val = vtkKWTimeAnimationWidget::AnimationTypeCamera;
    }
  if (val > vtkKWTimeAnimationWidget::AnimationTypeSequence)
    {
    val = vtkKWTimeAnimationWidget::AnimationTypeSequence;
    }

  if (this->AnimationType == val)
    {
    return;
    }

  this->AnimationType = val;
  this->Modified();

  this->Update();
}



//----------------------------------------------------------------------------
void vtkKWTimeAnimationWidget::Update()
{
  this->Superclass::Update();

  if (this->NumFrames)
  {  
    vtkKWScaleWithEntry* scale = this->Parameters->GetWidget(VTK_VV_ANIMATION_SCALE_NB_OF_FRAMES_ID);
    scale->SetValue (this->NumFrames);
  }
  
  if (this->AnimationButtonSet && vtkKWTimeAnimationWidget::AnimationTypeSequence)
    {
    this->AnimationButtonSet->SetEnabled(1);
    }
}


//----------------------------------------------------------------------------
void vtkKWTimeAnimationWidget::SetSequenceUpdateCommand(
  vtkObject *object, const char *method)
{
  this->SetObjectMethodCommand(
    &this->SequenceUpdateCommand, object, method);
  this->Update();
}


//----------------------------------------------------------------------------
void vtkKWTimeAnimationWidget::InvokeSequenceUpdateCommand(double t)
{
  if (this->SequenceUpdateCommand && *this->SequenceUpdateCommand && 
      this->GetApplication())
    {
    this->Script("%s %f", this->SequenceUpdateCommand, t);
    }
}


//----------------------------------------------------------------------------
void vtkKWTimeAnimationWidget::PreviewAnimationCallback()
{
  
  if (!this->IsCreated() || !this->RenderWidget)
    {
    return;
    }
  
  if (!this->IsCreated())
    {
    return;
    }

  // Disable buttons but preview

  this->DisableButtonsButCancel();

  // Run preview

  if (this->AnimationType == 
      vtkKWTimeAnimationWidget::AnimationTypeCamera)
    {
    this->PreviewCameraAnimation();
    }
  else if (this->AnimationType == 
           vtkKWTimeAnimationWidget::AnimationTypeSlice)
    {
    this->PreviewSliceAnimation();
    }
  else if (this->AnimationType == 
           vtkKWTimeAnimationWidget::AnimationTypeSequence)
    {
    this->PreviewSequenceAnimation();
    }

  // Reenable buttons

  this->EnableButtonsButCancel();
}

//----------------------------------------------------------------------------
void vtkKWTimeAnimationWidget::CreateAnimationCallback()
{
  
  if (!this->IsCreated() || !this->RenderWidget || !this->NumFrames)
    {
    return;
    }

  int res;
  std::string supportedformats = "{{all supported formats} {.avi .mpg .mpeg .mp2 .jpg .jpeg .tif}} {{all } {.*}}";
  
  vtksys_stl::string filename;
//   vtksys_stl::string filetypes("{{JPEG} {.jpg}} {{TIFF} {.tif}}");
  vtksys_stl::string filetypes = supportedformats;
#ifdef VTK_USE_MPEG2_ENCODER
  vtksys_stl::string defaultExtension = ".mpg";
#else //VTK_USE_MPEG2_ENCODER
  vtksys_stl::string defaultExtension = ".avi";
#endif

  vtkKWLoadSaveDialog *save_dialog = vtkKWLoadSaveDialog::New();
  save_dialog->SetParent(this->GetParentTopLevel());
  save_dialog->RetrieveLastPathFromRegistry("DataPath");
  save_dialog->Create();
  save_dialog->SetTitle(
    ks_("Animation|Save Animation Dialog|Title|Save Animation"));
  save_dialog->SaveDialogOn();
  
  save_dialog->SetFileTypes(filetypes.c_str());
  save_dialog->SetDefaultExtension(defaultExtension.c_str());

  res = save_dialog->Invoke();
  if (res)
    {
    filename = save_dialog->GetFileName();
    save_dialog->SaveLastPathToRegistry("DataPath");
    }

  std::string extension = vtksys::SystemTools::GetFilenameLastExtension(filename);

  if ( strcmp (extension.c_str(), ".avi") &&
       strcmp (extension.c_str(), ".mpg") &&
       strcmp (extension.c_str(), ".mpeg") &&
       strcmp (extension.c_str(), ".mp2") &&
       strcmp (extension.c_str(), ".jpg") &&
       strcmp (extension.c_str(), ".jpeg") &&
       strcmp (extension.c_str(), ".tif") )
  {
    filename+=defaultExtension;
  }
  save_dialog->SaveLastPathToRegistry("DataPath");
  save_dialog->Delete();
  if (!res)
    {
    return;
    }

  // Split into root and extension.

  vtksys_stl::string ext = 
    vtksys::SystemTools::GetFilenameLastExtension(filename.c_str());

  vtksys_stl::string filename_path = 
    vtksys::SystemTools::GetFilenamePath(filename.c_str());

  vtksys_stl::string file_root(filename_path);
  file_root += '/';
  file_root +=
    vtksys::SystemTools::GetFilenameWithoutLastExtension(filename.c_str());

  if (!ext.size())
    {
    vtkErrorMacro(<< "Could not find extension in " << filename.c_str());
    return;
    }

  int orig_width = this->RenderWidget->GetRenderWindow()->GetSize()[0];
  int orig_height = this->RenderWidget->GetRenderWindow()->GetSize()[1];

  int width, height;

  // Is this a video format

  int is_mpeg = 
    (!strcmp(ext.c_str(), ".mpg") || !strcmp(ext.c_str(), ".mpeg") ||
     !strcmp(ext.c_str(), ".MPG") || !strcmp(ext.c_str(), ".MPEG") ||
     !strcmp(ext.c_str(), ".MP2") || !strcmp(ext.c_str(), ".mp2"));
  int is_avi = 
    (!strcmp(ext.c_str(), ".avi") || !strcmp(ext.c_str(), ".AVI"));

  // Prompt for the size of the movie
    
  vtkKWMessageDialog *msg_dialog = vtkKWMessageDialog::New();
  msg_dialog->SetMasterWindow(this->GetParentTopLevel());
  msg_dialog->SetTitle(
    ks_("Animation|Create Animation Dialog|Title|Frame Size"));
  msg_dialog->SetStyleToOkCancel();
  msg_dialog->Create();

  vtksys_stl::string msg(
      k_("Specify the width and height of each frame to be saved from this "
         "animation."));
  
  if (is_mpeg)
    {
    msg += " ";
    msg += 
      k_("The width must be a multiple of 32 and the height a "
         "multiple of 8. Each will be resized to the next smallest multiple "
         "if it does not meet this criterion. The maximum size allowed is "
         "1920 by 1080.");
    }
  else if (is_avi)
    { 
    msg += " ";
    msg += 
      k_("Each dimension must be a multiple of 4. Each will be "
         "resized to the next smallest multiple of 4 if it does not meet this "
         "criterion.");
    }

  msg_dialog->SetText(msg.c_str());
  
  vtkKWFrame *frame = vtkKWFrame::New();
  frame->SetParent(msg_dialog->GetTopFrame());
  frame->Create();

  char buffer[1024];
  int nb_scanned = 0, key_w, key_h;

  vtkKWApplication *app = this->GetApplication();

  if (app->HasRegistryValue(2, "RunTime", "AnimationSize") &&
      app->GetRegistryValue(2, "RunTime", "AnimationSize", buffer))
    {
    nb_scanned = sscanf(buffer, "%dx%d", &key_w, &key_h);
    }

  vtkKWComboBox *combobox;

  vtkKWComboBoxWithLabel *width_combobox = vtkKWComboBoxWithLabel::New();
  width_combobox->SetParent(frame);
  width_combobox->Create();
  width_combobox->SetLabelText(
    ks_("Animation|Create Animation Dialog|Frame Size|Width:"));

  combobox = width_combobox->GetWidget();
  combobox->SetValueAsInt(orig_width);
  if (nb_scanned == 2)
    {
    combobox->AddValueAsInt(key_w);
    }
  combobox->AddValueAsInt(640);
  combobox->AddValueAsInt(800);
  combobox->AddValueAsInt(1024);
  combobox->AddValueAsInt(1280);
  combobox->AddValueAsInt(1680);
    
  vtkKWComboBoxWithLabel *height_combobox = vtkKWComboBoxWithLabel::New();
  height_combobox->SetParent(frame);
  height_combobox->Create();
  height_combobox->SetLabelText(
    ks_("Animation|Create Animation Dialog|Frame Size|Height:"));

  combobox = height_combobox->GetWidget();
  combobox->SetValueAsInt(orig_height);
  if (nb_scanned == 2)
    {
    combobox->AddValueAsInt(key_h);
    }
  combobox->AddValueAsInt(600);
  combobox->AddValueAsInt(768);
  combobox->AddValueAsInt(800);
  combobox->AddValueAsInt(1024);
  combobox->AddValueAsInt(1050);

  this->Script("pack %s %s -side left -fill both -expand t",
               width_combobox->GetWidgetName(), 
               height_combobox->GetWidgetName());
    
  this->Script("pack %s -side top -pady 5", frame->GetWidgetName());
    
  //res = msg_dialog->Invoke();
  res = 1;
  
  width = width_combobox->GetWidget()->GetValueAsInt();
  height = height_combobox->GetWidget()->GetValueAsInt();

  width_combobox->Delete();
  height_combobox->Delete();
  frame->Delete();
  msg_dialog->Delete();

  if (!res)
    {
    return;
    }

  app->SetRegistryValue(
    2, "RunTime", "AnimationSize", "%dx%d", width, height);

  // Fix the size

  if (is_mpeg)
    {
    if ((width % 32) > 0)
      {
      width -= width % 32;
      }
    if ((height % 8) > 0)
      {
      height -= height % 8;
      }
    if (width > 1920)
      {
      width = 1920;
      }
    if (height > 1080)
      {
      height = 1080;
      }      
    }
  else if (is_avi)
    {
    if ((width % 4) > 0)
      {
      width -= width % 4;
      }
    if ((height % 4) > 0)
      {
      height -= height % 4;
      }
    }
  
  // Disable buttons but preview

  this->DisableButtonsButCancel();

  // Create the animation

#ifdef vtkINRIA3D_USE_KWWIDGETS_OLD_API
    if (this->AnimationType == 
	vtkKWTimeAnimationWidget::AnimationTypeCamera)
    {
      this->CreateCameraAnimation(file_root.c_str(), ext.c_str(), width, height);
    }
    else if (this->AnimationType == 
	     vtkKWTimeAnimationWidget::AnimationTypeSlice)
    {
      this->CreateSliceAnimation(file_root.c_str(), ext.c_str(), width, height);
    }
    else if (this->AnimationType == 
	     vtkKWTimeAnimationWidget::AnimationTypeSequence)
    {
      this->CreateSequenceAnimation(file_root.c_str(), ext.c_str(), width, height);
    }
#else
        if (this->AnimationType == 
	vtkKWTimeAnimationWidget::AnimationTypeCamera)
    {
      this->CreateCameraAnimation(file_root.c_str(), width, height, 25, ext.c_str());
    }
    else if (this->AnimationType == 
	     vtkKWTimeAnimationWidget::AnimationTypeSlice)
    {
      this->CreateSliceAnimation(file_root.c_str(), width, height, 25, ext.c_str());
    }
    else if (this->AnimationType == 
	     vtkKWTimeAnimationWidget::AnimationTypeSequence)
    {
      this->CreateSequenceAnimation(file_root.c_str(), ext.c_str(), width, height);
    }
#endif  
    
  // Reenable buttons

  this->EnableButtonsButCancel();
}



//----------------------------------------------------------------------------
void vtkKWTimeAnimationWidget::PreviewSequenceAnimation()
{
  this->PerformSequenceAnimation(NULL, NULL, -1, -1);
}


//----------------------------------------------------------------------------
void vtkKWTimeAnimationWidget::CreateSequenceAnimation(const char *file_root,
                                              const char *ext,
                                              int width, int height)
{
  this->PerformSequenceAnimation(file_root, ext, width, height);
}



//----------------------------------------------------------------------------
void vtkKWTimeAnimationWidget::PerformSequenceAnimation(const char *file_root,
                                                        const char *ext,
                                                        int width, int height)
{

  if (!this->IsCreated() || !this->RenderWidget)
    {
    return;
    }

  int previewing = !file_root;

  vtkKWWindowBase *win = this->GetApplication()->GetNthWindow(0);

  int old_render_mode = 0, old_size[2], status;

  vtkWindowToImageFilter *w2i = NULL;
  vtkGenericMovieWriter *movie_writer = NULL;
  vtkImageWriter *image_writer = 0;
  char *image_filename = 0;

  if (previewing)
    {
    old_render_mode = this->RenderWidget->GetRenderMode();
    this->RenderWidget->SetRenderModeToInteractive();
    if (win)
      {
      win->SetStatusText(ks_("Progress|Previewing animation"));
      }
    status = vtkKWTimeAnimationWidget::AnimationPreviewing;
    }
  else
  {
    
    if (ext)
    {
      if (!strcmp(ext, ".jpg") ||
		  !strcmp(ext, ".JPG") ||
		  !strcmp(ext, ".jpeg") ||
		  !strcmp(ext, ".JPEG"))
      {
        image_writer = vtkJPEGWriter::New();
      }
#ifdef VTK_USE_MPEG2_ENCODER
      else if (!strcmp(ext, ".mpg") ||
				!strcmp(ext, ".MPG") ||
				!strcmp(ext, ".mpeg") ||
				!strcmp(ext, ".MPEG") ||
				!strcmp(ext, ".mp2") ||
				!strcmp(ext, ".MP2")
				)
      {
        movie_writer = vtkMPEG2Writer::New();
      }
#endif
#ifdef VTK_USE_VIDEO_FOR_WINDOWS
      else if (!strcmp(ext, ".avi") ||
				!strcmp(ext, ".AVI")
				)
      {
        movie_writer = vtkAVIWriter::New();
		vtkAVIWriter* aviwriter = vtkAVIWriter::SafeDownCast(movie_writer);
		double res = (this->SequenceRange[1]-this->SequenceRange[0])/(double)this->NumFrames;
		if (res > 0.0)
		{
			int rate = (int)(1.0/res);
#if VTK_MAJOR_VERSION==5 && VTK_MINOR_VERSION>=1	  
			// only in VTK-CVS version :
			aviwriter->SetRate(rate);
#endif
		}
      }
#else
#ifdef VTK_USE_FFMPEG_ENCODER
      else if (!strcmp(ext, ".avi") ||
				!strcmp(ext, ".AVI")
				)
      {
        movie_writer = vtkFFMPEGWriter::New();
		vtkFFMPEGWriter* aviwriter = vtkFFMPEGWriter::SafeDownCast(movie_writer);
		double res = (this->SequenceRange[1]-this->SequenceRange[0])/(double)this->NumFrames;
		if (res > 0.0)
		{
			int rate = (int)(1.0/res);
#if VTK_MAJOR_VERSION==5 && VTK_MINOR_VERSION>=1	  
			// only in VTK-CVS version :
			aviwriter->SetRate(rate);
#endif
		}
      }
#endif
#endif
      else if (!strcmp(ext, ".tif"))
      {
        image_writer = vtkTIFFWriter::New();
      }
	  else
	  {
		vtkErrorMacro(<<""<<endl);
		vtkKWPopupErrorMessage(this, "movie type not recognized !");
		return;
	  }
    }

#ifdef WIN32
    this->RenderWidget->OffScreenRenderingOn();
#endif
    
    old_size[0] = this->RenderWidget->GetRenderWindow()->GetSize()[0];
    old_size[1] = this->RenderWidget->GetRenderWindow()->GetSize()[1];
    if (width > 0)
    {
      this->RenderWidget->GetRenderWindow()->SetSize(width, height);
    }
    if (win)
    {
      win->SetStatusText(
			 ks_("Progress|Generating animation (rendering to memory; please wait)"));
    }
    status = vtkKWTimeAnimationWidget::AnimationCreating;
    
    w2i = vtkWindowToImageFilter::New();
    w2i->SetInput(this->RenderWidget->GetRenderWindow());
    if(movie_writer)
    {
      movie_writer->SetInput(w2i->GetOutput());
      vtksys_stl::string filename(file_root);
      filename += ext;
      movie_writer->SetFileName(filename.c_str());
      movie_writer->Start();
    }
    else if(image_writer)
    {
      image_writer->SetInput(w2i->GetOutput());
      image_filename = new char[strlen(file_root) + strlen(ext) + 25];
    }
  }
  
  this->AnimationStatus = status;
  
  // Save the camera state
  
  double pos[3], view_up[3], angle, parallel_scale;
  
  vtkCamera *cam = this->RenderWidget->GetRenderer()->GetActiveCamera();
  cam->GetPosition(pos);
  cam->GetViewUp(view_up);
  angle = cam->GetViewAngle();
  parallel_scale = cam->GetParallelScale();

  double firsttime = this->SequenceRange[0];
  double lasttime = this->SequenceRange[1];
  
  // Get the animation parameters
  
  vtkKWScaleWithEntry *scale;
  
  scale = this->Parameters->GetWidget(VTK_VV_ANIMATION_SCALE_NB_OF_FRAMES_ID);
  int num_frames = (int)scale->GetValue();
  //   int num_frames = (int)this->NumFrames;

  double resolution = (this->SequenceRange[1]-this->SequenceRange[0])/(double)num_frames;
  
  
  scale = this->Parameters->GetWidget(VTK_VV_ANIMATION_SCALE_AZIMUTH_ID);
//   double azimuth = scale->GetValue() / (double)num_frames;
  
  scale = this->Parameters->GetWidget(VTK_VV_ANIMATION_SCALE_ELEVATION_ID);
//   double elev = scale->GetValue() / (double)num_frames;
  
  scale = this->Parameters->GetWidget(VTK_VV_ANIMATION_SCALE_ROLL_ID);
//   double roll = scale->GetValue() / (double)num_frames;
  
  scale = this->Parameters->GetWidget(VTK_VV_ANIMATION_SCALE_ZOOM_ID);
//   double zoom = pow(scale->GetValue(), (double)1.0 / (double)num_frames);
  
  
  // Perform the animation
  double total_percentage = 100.0;
  if (this->GoAndBackMode)
    total_percentage = 50.0;
  
  
  if (!movie_writer || movie_writer->GetError() == 0)
  {
    for (int i = 0; 
         i < num_frames && 
           this->AnimationStatus != 
           vtkKWTimeAnimationWidget::AnimationCanceled; i++)
    {
      if (win)
      {
        win->GetProgressGauge()->SetValue((int)(total_percentage * i / num_frames));
      }
      // process pending events... necessary for being able to interrupt
      this->GetApplication()->ProcessPendingEvents();
      
      double t_time = firsttime + (double)(i)*resolution;
      this->InvokeSequenceUpdateCommand (t_time);

      this->RenderWidget->Render();
      if (w2i)
      {
        w2i->Modified();
        if (movie_writer)
	{
          movie_writer->Write();
	}
        else if(image_writer)
	{
	  sprintf(image_filename, "%s.%04d%s", file_root, i, ext);
          image_writer->SetFileName(image_filename);
          image_writer->Write();
	}
      }
    }
    
    if (this->GoAndBackMode)
    {
      
      for (int i = 0; 
	   i < num_frames && 
	     this->AnimationStatus != 
	     vtkKWTimeAnimationWidget::AnimationCanceled; i++)
      {
	if (win)
	{
	  win->GetProgressGauge()->SetValue((int)(total_percentage + (total_percentage * i / num_frames)));
	}
	// process pending events... necessary for being able to interrupt
	this->GetApplication()->ProcessPendingEvents();
	
	double t_time = lasttime - (double)(i)*resolution;
	this->InvokeSequenceUpdateCommand (t_time);
	
	this->RenderWidget->Render();
	if (w2i)
	{
	  w2i->Modified();
	  if (movie_writer)
	  {
	    movie_writer->Write();
	  }
	  else if(image_writer)
	  {
	    sprintf(image_filename, "%s.%04d%s", file_root, i, ext);
	    image_writer->SetFileName(image_filename);
	    image_writer->Write();
	  }
	}
      }
    }
    
    if (movie_writer)
      {
      movie_writer->End();
      movie_writer->SetInput(0);
      }
    }

  // Update status

  if (win)
    {
    vtksys_stl::string end_msg(win->GetStatusText());
    end_msg += " -- ";
    if (this->AnimationStatus != status)
      {
      end_msg += ks_("Progress|Canceled");
      }
    else
      {
      end_msg += ks_("Progress|Done");
      }
    win->SetStatusText(end_msg.c_str());
    win->GetProgressGauge()->SetValue(0);
    }
  
  this->AnimationStatus = vtkKWTimeAnimationWidget::AnimationDone;

  // Restore camera state

  cam->SetPosition(pos);
  cam->SetViewUp(view_up);
  cam->SetViewAngle(angle);
  cam->SetParallelScale(parallel_scale);

  // Switch back to the previous render mode / widget state

  if (previewing)
    {
    this->RenderWidget->SetRenderMode(old_render_mode);
    }
  else
    {
      this->RenderWidget->GetRenderWindow()->SetSize(old_size);
#ifdef WIN32
      this->RenderWidget->OffScreenRenderingOff();
#endif
    }

  this->InvokeCameraPostAnimationCommand();

  this->RenderWidget->Render();

  // Cleanup

  if (w2i)
    {
    w2i->Delete();
    }
  if (movie_writer)
    {
    movie_writer->Delete();
    }
  if (image_writer)
    {
    delete [] image_filename;
    image_writer->Delete();
    }


}








