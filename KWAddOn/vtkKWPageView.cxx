/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWPageView.cxx 1350 2009-11-19 15:16:30Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2009-11-19 15:16:30 +0000 (Thu, 19 Nov 2009) $
Version:   $Revision: 1350 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkKWPageView.h"

#include "vtkKWApplication.h"
#include "vtkKWRenderWidget.h"
#include "vtkKWIcon.h"
#include "vtkObjectFactory.h"

#include <vtkWindowToImageFilter.h>
#include <vtkImageClip.h>
#include <vtkImagePermute.h>
#include <vtkImageResample.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkImageView/vtkImageView2D.h>
#include <vtkImageView/vtkImageView3D.h>
#include <vtkImageView/vtkImageViewCollection.h>

#include <vtkPlaneWidget.h>
#include <vtkImageActor.h>

#include <vtkDataSet.h>
#include <vtkDataSetCollection.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkLookupTable.h>

#include <vtkActor.h>

#include <vtkLandmarkManager.h>
#include <vtkRendererCollection.h>
#include <vtkImageViewCornerAnnotation.h>
#include <vtkTextProperty.h>


//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWPageView );
vtkCxxRevisionMacro( vtkKWPageView, "$Revision: 1350 $");


//----------------------------------------------------------------------------
vtkKWPageView::vtkKWPageView()
{
  this->View1         = vtkImageView2D::New();
  this->View2         = vtkImageView2D::New();
  this->View3         = vtkImageView2D::New();
  this->View4         = vtkImageView3D::New();

  this->RenderWidget1 = vtkKWRenderWidget::New();
  this->RenderWidget2 = vtkKWRenderWidget::New();
  this->RenderWidget3 = vtkKWRenderWidget::New();
  this->RenderWidget4 = vtkKWRenderWidget::New();

  m_OrientationMatrix = vtkMatrix4x4::New();
  
  m_Pool = 0;

  this->IsFullScreen = 0;

  this->ID = -1;

  this->GridType = vtkKWPageView::GRIDTYPE_3BOTTOM;
  
  this->LandmarkManager = vtkLandmarkManager::New();
  this->LandmarkManager->SetColorMode(vtkLandmarkManager::LANDMARK_COLOR_NONE);
  this->LandmarkManager->SetLandmarkType (vtkLandmark::LANDMARK_SPHERE);
  this->LandmarkManager->SetSelectedColor (1, 0.5, 0.5);
  
  this->LandmarkManager->AutoSelectOff();
  this->LandmarkManager->ChangeColorOnSelectedOff();
  this->LandmarkManager->ChangeColorOnSelectedOff();
  this->LandmarkManager->LinkerOff();

  this->FirstRender = true;
}

//----------------------------------------------------------------------------
vtkKWPageView::~vtkKWPageView()
{
  std::cout<<"deleting vtkKWPageView "<<this->GetTag()<<std::endl;

  this->LandmarkManager->Delete();

  m_OrientationMatrix->Delete();
  
  if( m_Pool )
    m_Pool->Delete();
  
  this->View1->Delete();
  this->View2->Delete();
  this->View3->Delete();
  this->View4->Delete();

  this->RenderWidget1->SetParent (NULL);
  this->RenderWidget1->Delete();
  this->RenderWidget2->SetParent (NULL);
  this->RenderWidget2->Delete();
  this->RenderWidget3->SetParent (NULL);
  this->RenderWidget3->Delete();
  this->RenderWidget4->SetParent (NULL);
  this->RenderWidget4->Delete();
  
}



//----------------------------------------------------------------------------
void vtkKWPageView::CreateRenderWidgets()
{
  this->RenderWidget1->SetParent(this);
  this->RenderWidget1->Create();
  this->RenderWidget2->SetParent(this);
  this->RenderWidget2->Create();
  this->RenderWidget3->SetParent(this);
  this->RenderWidget3->Create();
  this->RenderWidget4->SetParent(this);
  this->RenderWidget4->Create();  

}



//----------------------------------------------------------------------------
void vtkKWPageView::ConfigureView(vtkImageView* view, vtkKWRenderWidget* widget)
{
  
  view->SetupInteractor (widget->GetRenderWindow()->GetInteractor());
  vtkRenderer* renderer = vtkRenderer::New();
  view->SetRenderer(renderer);

  vtkRendererCollection* collection = widget->GetRenderWindow()->GetRenderers();
  collection->RemoveAllItems();
  widget->GetRenderWindow()->AddRenderer (renderer);
  widget->RemoveAllRenderers();
  widget->AddRenderer (renderer);
  widget->SetRenderModeToInteractive();
  view->SetRenderWindow (widget->GetRenderWindow());
}

//----------------------------------------------------------------------------
void vtkKWPageView::SetOrientationMatrix (vtkMatrix4x4* matrix)
{
  if (matrix)
    m_OrientationMatrix->DeepCopy (matrix);
}


//----------------------------------------------------------------------------
void vtkKWPageView::Create4Views()
{

  this->ConfigureView (this->View1, this->RenderWidget1);
  this->ConfigureView (this->View2, this->RenderWidget2);
  this->ConfigureView (this->View3, this->RenderWidget3);
  this->ConfigureView (this->View4, this->RenderWidget4);  

  m_Pool = vtkImageViewCollection::New();
  
  m_Pool->AddItem (this->View1);
  m_Pool->AddItem (this->View2);
  m_Pool->AddItem (this->View3);
  m_Pool->AddItem (this->View4);
}

//----------------------------------------------------------------------------
void vtkKWPageView::SetProperties()
{
  
  this->View1->SetViewOrientation (vtkImageView2D::VIEW_ORIENTATION_AXIAL);
  this->View2->SetViewOrientation (vtkImageView2D::VIEW_ORIENTATION_CORONAL);
  this->View3->SetViewOrientation (vtkImageView2D::VIEW_ORIENTATION_SAGITTAL);

  this->m_Pool->SyncSetAnnotationStyle( vtkImageView2D::AnnotationStyle2);
  this->m_Pool->SyncSetShowRulerWidget (0);
  
  this->GetPool()->SyncSetLeftButtonInteractionStyle(vtkInteractorStyleImageView2D::InteractionTypeWindowLevel);
  this->GetPool()->SyncSetMiddleButtonInteractionStyle(vtkInteractorStyleImageView2D::InteractionTypePan);
  this->GetPool()->SyncSetRightButtonInteractionStyle(vtkInteractorStyleImageView2D::InteractionTypeZoom);
  this->GetPool()->SyncSetKeyboardInteractionStyle(vtkInteractorStyleImageView2D::InteractionTypeSlice);

  this->View1->GetCornerAnnotation()->SetText (1, "INRIA 2011 - CardioViz3D");
  this->View2->GetCornerAnnotation()->SetText (1, "INRIA 2011 - CardioViz3D");
  this->View3->GetCornerAnnotation()->SetText (1, "INRIA 2011 - CardioViz3D");
  this->View4->GetCornerAnnotation()->SetText (1, "INRIA 2011 - CardioViz3D");
  
  m_Pool->SyncSetShowScalarBar (1);
  double zeros[3] = {0,0,0};
  m_Pool->SyncSetBackground (zeros);
  
  this->View4->SetBackground (0.9,0.9,0.9);
  this->View4->GetTextProperty()->SetColor (0,0,0);

  this->View4->SetVolumeMapperTo3DTexture();
  
  this->LandmarkManager->AddView(this->GetView1()); 
  this->LandmarkManager->AddView(this->GetView2());
  this->LandmarkManager->AddView(this->GetView3());
  this->LandmarkManager->AddView(this->GetView4());
}

void vtkKWPageView::PackSelf()
{
  this->UnpackChildren();

  switch(this->GetGridType())
  {
      case vtkKWPageView::GRIDTYPE_3LEFT :
	
	this->Script ("grid %s -column 0 -row 0 -sticky news",
		      this->RenderWidget1->GetWidgetName());
	this->Script ("grid %s -column 0 -row 1 -sticky news",
		      this->RenderWidget2->GetWidgetName());
	this->Script ("grid %s -column 0 -row 2 -sticky news",
		      this->RenderWidget3->GetWidgetName());
	this->Script ("grid %s -column 1 -row 0 -rowspan 3 -sticky news",
		      this->RenderWidget4->GetWidgetName());
	
	this->Script ("grid columnconfigure %s 0 -weight 2",
		      this->GetWidgetName());
	this->Script ("grid columnconfigure %s 1 -weight 5",
		      this->GetWidgetName());
	this->Script ("grid rowconfigure %s 0 -weight 1",
		      this->GetWidgetName());
	this->Script ("grid rowconfigure %s 1 -weight 1",
		      this->GetWidgetName());
	this->Script ("grid rowconfigure %s 2 -weight 1",
		      this->GetWidgetName());

	break;
	
      case vtkKWPageView::GRIDTYPE_3BOTTOM :
	
	this->Script ("grid %s -column 0 -row 0 -columnspan 3 -sticky news",
		      this->RenderWidget4->GetWidgetName());
	this->Script ("grid %s -column 0 -row 1 -sticky news",
		      this->RenderWidget1->GetWidgetName());
	this->Script ("grid %s -column 1 -row 1 -sticky news",
		      this->RenderWidget2->GetWidgetName());
	this->Script ("grid %s -column 2 -row 1 -sticky news",
		      this->RenderWidget3->GetWidgetName());
	
	
	this->Script ("grid columnconfigure %s 0 -weight 1",
		      this->GetWidgetName());
	this->Script ("grid columnconfigure %s 1 -weight 1",
		      this->GetWidgetName());
	this->Script ("grid columnconfigure %s 2 -weight 1",
		      this->GetWidgetName());
	this->Script ("grid rowconfigure %s 0 -weight 5",
		      this->GetWidgetName());
	this->Script ("grid rowconfigure %s 1 -weight 2",
		      this->GetWidgetName());

	break;
	
      case vtkKWPageView::GRIDTYPE_EQUAL_SIZE :
      default :
	this->Script ("grid %s -column 0 -row 0 -sticky news",
		      this->RenderWidget1->GetWidgetName());
	this->Script ("grid %s -column 1 -row 0 -sticky news",
		      this->RenderWidget2->GetWidgetName());
	this->Script ("grid %s -column 0 -row 1 -sticky news",
		      this->RenderWidget3->GetWidgetName());
	this->Script ("grid %s -column 1 -row 1 -sticky news",
		      this->RenderWidget4->GetWidgetName());
	
	
	this->Script ("grid columnconfigure %s 0 -weight 1",
		      this->GetWidgetName());
	this->Script ("grid columnconfigure %s 1 -weight 1",
		      this->GetWidgetName());
	this->Script ("grid rowconfigure %s 0 -weight 1",
		      this->GetWidgetName());
	this->Script ("grid rowconfigure %s 1 -weight 1",
		      this->GetWidgetName());

	break;
  }
  
	
}


//----------------------------------------------------------------------------
void vtkKWPageView::CreateWidget()
{
  // Check if already created

  if (this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }
  
  // Call the superclass to create the whole widget
  this->Superclass::CreateWidget();
  // attach the RenderWidgets to the view frame, and pack

  this->CreateRenderWidgets();
  // attach the Views to the RenderWidgets

  this->Create4Views();
  // pack everything

  char buffer[1024];
  bool valid = this->GetApplication()->GetRegistryValue(1, "RunTime", "PageGridType", buffer);
  if (!valid)
    this->SetGridType (GRIDTYPE_EQUAL_SIZE);
  else
  {
    if (*buffer)
    {
      std::istringstream is;
      is.str (buffer);
      int val = 0;
      is >> val;
      this->SetGridType (val);
    }
  }

  

  this->PackSelf();
  
  
  // and Set the properties !
  this->SetProperties();
  
}

void vtkKWPageView::SetImage (vtkImageData* image, vtkMatrix4x4* orientationmatrix)
{

  if (!image)
    return;
  
  m_Pool->SyncSetInput(image);
  if (orientationmatrix)
    m_Pool->SyncSetOrientationMatrix (orientationmatrix);
  this->SetOrientationMatrix (orientationmatrix);
  this->View1->SetViewOrientation( vtkImageView2D::VIEW_ORIENTATION_AXIAL);
  this->View2->SetViewOrientation( vtkImageView2D::VIEW_ORIENTATION_SAGITTAL);
  this->View3->SetViewOrientation( vtkImageView2D::VIEW_ORIENTATION_CORONAL);
  
  m_Pool->SyncReset();
  m_Pool->SyncRender();
  
  this->LandmarkManager->InteractionOn();
}

void vtkKWPageView::SetImage (itk::Image<float, 3>::Pointer image)
{

  if (image.IsNull())
    return;
  
//   this->View1->SetITKImage(image);
//   this->View2->SetITKImage(image);
//   this->View3->SetITKImage(image);
//   this->View4->SetITKImage(image);
  
//   this->View1->SyncReset();

}


vtkImageData* vtkKWPageView::GetImage ()
{
  return this->View1->GetInput();  
}

//---------------------------------------------------------------------------
int vtkKWPageView::BuildScreenshotFromImage(vtkImageData *image, vtkKWIcon* screenshot, int size)
{
  if (!image || !screenshot)
  {
    return 0;
  }

  // Empty image, remove thumbnail/screenshot

  int image_dims[3];
  image->GetDimensions(image_dims);
  if (image_dims[0] == 0 || 
      image_dims[1] == 0 || 
      image_dims[2] == 0)
  {
    return 0;
  }

  double factor;
  vtkImageData *resample_input, *resample_output;

  // First, let's make sure we are processing the image as it
  // is by clipping its UpdateExtent. By doing so, we prevent our resample
  // and permute filter the process the image's *whole* extent.

  vtkImageClip *clip = vtkImageClip::New();
  clip->SetInput(image);
  clip->SetOutputWholeExtent(image->GetUpdateExtent());
  clip->Update();

  // Permute, as a convenience

  int clip_dims[3];
  clip->GetOutput()->GetDimensions(clip_dims);

  vtkImagePermute *permute = NULL;
  if (clip_dims[2] != 1)
  {
    permute = vtkImagePermute::New();
    permute->SetInput(clip->GetOutput());
    if (clip_dims[0] == 1)
    {
      permute->SetFilteredAxes(1, 2, 0);
    }
    else
    {
      permute->SetFilteredAxes(0, 2, 1);
    }
    resample_input = permute->GetOutput();
  }
  else
  {
    resample_input = clip->GetOutput();
  }
  
  resample_input->Update();
  int resample_input_dims[3], resample_output_dims[3];

  resample_input->GetDimensions(resample_input_dims);
  double *resample_input_spacing = resample_input->GetSpacing();

  int large_dim = 0, small_dim = 1;
  if (resample_input_dims[0] < resample_input_dims[1])
  {
    large_dim = 1; small_dim = 0;
  }

  vtkImageResample *resample = vtkImageResample::New();
  resample->SetInput(resample_input);
  resample->SetInterpolationModeToCubic();
  resample->SetDimensionality(2);

  // Create the screenshot
    
  factor = 
    (double)size / (double)resample_input_dims[large_dim];
  resample->SetAxisMagnificationFactor(large_dim, factor);
  resample->SetAxisMagnificationFactor(
    small_dim, factor * (resample_input_spacing[small_dim] / 
                         resample_input_spacing[large_dim]));
  resample->Update();
  resample_output = resample->GetOutput();
  resample_output->GetDimensions(resample_output_dims);


  screenshot->SetImage(
    (const unsigned char*)resample_output->GetScalarPointer(),
    resample_output_dims[0],
    resample_output_dims[1],
    3,
    0,
    vtkKWIcon::ImageOptionFlipVertical);
  
  // Deallocate

  clip->Delete();
  resample->Delete();
  if (permute)
  {
    permute->Delete();
  }

  
  
  return 1;
}


//---------------------------------------------------------------------------
int vtkKWPageView::BuildScreenshotFromRenderWindow(vtkRenderWindow *win, vtkKWIcon* screenshot, int size)
{
  if (win && screenshot)
  {
    vtkWindowToImageFilter *filter = vtkWindowToImageFilter::New();
    filter->ShouldRerenderOff();
    filter->SetInput(win);
    filter->Update();
    int res = this->BuildScreenshotFromImage(filter->GetOutput(), screenshot, size);
    filter->Delete();
    return res;
  }
  return 0;
}

int vtkKWPageView::GetAxialScreenshot(vtkKWIcon* screenshot, int size)
{
  return this->BuildScreenshotFromRenderWindow (this->RenderWidget1->GetRenderWindow(), screenshot, size);
}

void vtkKWPageView::Render (void)
{
  if (this->RenderWidget1->IsMapped())
   this->View1->Render();
  if (this->RenderWidget2->IsMapped())
   this->View2->Render();
  if (this->RenderWidget3->IsMapped())
   this->View3->Render();
  if (this->RenderWidget4->IsMapped())
   this->View4->Render();
}

void vtkKWPageView::SetFullScreenView (int id)
{
  if (!id)
  {
    this->PackSelf();
    this->IsFullScreen = 0;
    return;
  }

  this->UnpackChildren();
  
  this->Script ("grid remove %s",
		this->RenderWidget1->GetWidgetName());
  this->Script ("grid remove %s",
		this->RenderWidget2->GetWidgetName());
  this->Script ("grid remove %s",
		this->RenderWidget3->GetWidgetName());
  this->Script ("grid remove %s",
		this->RenderWidget4->GetWidgetName());
  
  vtkImageView* view = 0;
  vtkKWRenderWidget* widget = 0;
  
  switch (id)
  {
      case 1 :
	view = this->View1;
	widget = this->RenderWidget1;
	break;
      case 2 :
	view = this->View2;
	widget = this->RenderWidget2;
	break;
      case 3 :
	view = this->View3;
	widget = this->RenderWidget3;
	break;
      case 4 :
	view = this->View4;	
	widget = this->RenderWidget4;
	break;
      default :
	vtkErrorMacro (<<"full screen mode not recognized !"<<endl);
	return;
	break;
  }

  this->Script ("pack %s -side top -expand yes -fill both -padx 2 -pady 2",
		widget->GetWidgetName());
	
  view->Render();
  this->IsFullScreen = id;
}

void vtkKWPageView::ToggleFullScreenView4 (void)
{

  if (this->IsFullScreen != 4)
  {
    this->SetFullScreenView (4);
  }
  else
  {
    this->SetFullScreenView (0);    
  }
}

void vtkKWPageView::ToggleFullScreenAxial (void)
{
  if (this->IsFullScreen != 1)
  {
    this->SetFullScreenView (1);
  }
  else
  {
    this->SetFullScreenView (0);    
  }
}

void vtkKWPageView::ToggleFullScreenSagittal (void)
{
  if (this->IsFullScreen != 2)
  {
    this->SetFullScreenView (2);
  }
  else
  {
    this->SetFullScreenView (0);    
  }
}

void vtkKWPageView::ToggleFullScreenCoronal (void)
{
  if (this->IsFullScreen != 3)
  {
    this->SetFullScreenView (3);
  }
  else
  {
    this->SetFullScreenView (0);    
  }
}

int vtkKWPageView::GetVisibility (vtkDataSet* dataset)
{
//   return false;
  
  if (!dataset)
    return false;

  if (this->GetImage() && vtkImageData::SafeDownCast(dataset))
  {
    return this->View1->GetImageActor()->GetVisibility();
  }
  if (this->View1->GetDataSetCollection()->IsItemPresent (dataset))
    return this->View1->FindDataSetActor (dataset)->GetVisibility ();
  if (this->View2->GetDataSetCollection()->IsItemPresent (dataset))
    return this->View2->FindDataSetActor (dataset)->GetVisibility ();
  if (this->View3->GetDataSetCollection()->IsItemPresent (dataset))
    return this->View3->FindDataSetActor (dataset)->GetVisibility ();
  if (this->View4->GetDataSetCollection()->IsItemPresent (dataset))
    return this->View4->FindDataSetActor (dataset)->GetVisibility ();
  return 0;
}


void vtkKWPageView::SetVisibility (vtkDataSet* dataset, bool state)
{
  if (!dataset)
    return;

  if (this->GetImage() && vtkImageData::SafeDownCast(dataset))
  {
    this->View1->GetImageActor()->SetVisibility (state);
    this->View2->GetImageActor()->SetVisibility (state);
    this->View3->GetImageActor()->SetVisibility (state);
    this->View4->SetVisibility (state, 0);
  }

  if (this->View1->GetDataSetCollection()->IsItemPresent (dataset))
    this->View1->FindDataSetActor (dataset)->SetVisibility (state);
  if (this->View2->GetDataSetCollection()->IsItemPresent (dataset))
    this->View2->FindDataSetActor (dataset)->SetVisibility (state);
  if (this->View3->GetDataSetCollection()->IsItemPresent (dataset))
    this->View3->FindDataSetActor (dataset)->SetVisibility (state);
  if (this->View4->GetDataSetCollection()->IsItemPresent (dataset))
    this->View4->FindDataSetActor (dataset)->SetVisibility (state);
}


//----------------------------------------------------------------------------
const char* vtkKWPageView::GetTag (void) const
{ return this->Tag.c_str(); }
//----------------------------------------------------------------------------
void vtkKWPageView::SetTag (const char* tag)
{
  this->Tag = tag;
}


void vtkKWPageView::SetScalarBarVisibility (bool state)
{

  this->m_Pool->SyncSetShowScalarBar(state);
  
}

int vtkKWPageView::GetScalarBarVisibility (void) const
{
  return this->View1->GetShowScalarBar();
}

void  vtkKWPageView::SetLookupTable (vtkLookupTable* lut)
{
  if( !lut )
    return;
  this->View1->SetLookupTable (lut,0);
  this->View2->SetLookupTable (lut,0);
  this->View3->SetLookupTable (lut,0);
  this->View4->SetLookupTable (lut);
  this->m_Pool->SyncRender();
  
}

void vtkKWPageView::RemoveDataSet(vtkDataSet* dataset)
{
  this->m_Pool->SyncRemoveDataSet(vtkPointSet::SafeDownCast (dataset));
}

std::vector<vtkActor*> vtkKWPageView::AddDataSet(vtkDataSet* dataset, vtkProperty* property)
{
  std::vector<vtkActor*> list;

  // vtkActor* actor1 = NULL;
  // vtkActor* actor2 = NULL;
  // vtkActor* actor3 = NULL;
  // vtkActor* actor4 = NULL;
  // actor1 = this->View1->AddDataSet(dataset, property);
  // actor2 = this->View2->AddDataSet(dataset, property);
  // actor3 = this->View3->AddDataSet(dataset, property);
  // actor4 = this->View4->AddDataSet(dataset, property);
  // if (actor1) list.push_back(actor1);
  // if (actor2) list.push_back(actor2);
  // if (actor3) list.push_back(actor3);
  // if (actor4) list.push_back(actor4);
  if (vtkPointSet::SafeDownCast (dataset))
    this->m_Pool->SyncAddDataSet(vtkPointSet::SafeDownCast (dataset), property);
  else
    this->m_Pool->SyncSetInput (vtkImageData::SafeDownCast (dataset));
  
  return list;
}


std::vector<vtkActor*> vtkKWPageView::AddPolyData(vtkPolyData* dataset, vtkProperty* property, double thickness)
{
  std::vector<vtkActor*> list;
  return list;
}


vtkKWRenderWidget* vtkKWPageView::GetActiveRenderWidget ()
{

  switch(this->IsFullScreen)
  {
      case 1 :
	return this->GetRenderWidget1();
	break;
      case 2 :
	return this->GetRenderWidget2();
	break;
      case 3 :
	return this->GetRenderWidget3();
	break;
      default :
	return this->GetRenderWidget4();
	break;
  }
}


vtkImageView* vtkKWPageView::GetActiveView ()
{

  switch(this->IsFullScreen)
  {
      case 1 :
	return this->GetView1();
	break;
      case 2 :
	return this->GetView2();
	break;
      case 3 :
	return this->GetView3();
	break;
      default :
	return this->GetView4();
	break;
  }
}


bool vtkKWPageView::HasDataSet (vtkDataSet* dataset)
{
  if (!dataset)
    return false;

  if (this->View1->GetDataSetCollection()->IsItemPresent (dataset) ||
      this->View2->GetDataSetCollection()->IsItemPresent (dataset) ||
      this->View3->GetDataSetCollection()->IsItemPresent (dataset) ||
      this->View4->GetDataSetCollection()->IsItemPresent (dataset) ||
      this->View1->GetInput() == dataset
      )
    return true;

  return false;
}


std::vector<vtkActor*> vtkKWPageView::AddMetaDataSet(vtkMetaDataSet* metadataset)
{
  std::vector<vtkActor*> list;
  
  if ( !metadataset || !metadataset->GetDataSet() )
    return list;
  
  vtkDataSet* dataset = metadataset->GetDataSet();
  vtkProperty* property = vtkProperty::SafeDownCast (metadataset->GetProperty());

  // vtkActor* actor1 = NULL;
  // vtkActor* actor2 = NULL;
  // vtkActor* actor3 = NULL;
  // vtkActor* actor4 = NULL;
  // actor1 = this->View1->AddDataSet(dataset, property);
  // actor2 = this->View2->AddDataSet(dataset, property);
  // actor3 = this->View3->AddDataSet(dataset, property);
  // actor4 = this->View4->AddDataSet(dataset, property);
  
  // if (actor1) list.push_back(actor1);
  // if (actor2) list.push_back(actor2);
  // if (actor3) list.push_back(actor3);
  // if (actor4) list.push_back(actor4);

  if (vtkImageData::SafeDownCast (dataset))
    this->m_Pool->SyncSetInput (vtkImageData::SafeDownCast (dataset));
  else
    this->m_Pool->SyncAddDataSet (vtkPointSet::SafeDownCast (dataset), property);
  
  if (metadataset->GetWirePolyData())
  {
    vtkPointSet* wire = metadataset->GetWirePolyData();
    
    vtkActor* actor5 = NULL;
    vtkActor* actor6 = NULL;
    vtkActor* actor7 = NULL;
    vtkActor* actor8 = NULL;
    actor5 = this->View1->AddDataSet(wire, property);
    actor6 = this->View2->AddDataSet(wire, property);
    actor7 = this->View3->AddDataSet(wire, property);
    actor8 = this->View4->AddDataSet(wire, property);
    if (actor5) list.push_back(actor5);
    if (actor6) list.push_back(actor6);
    if (actor7) list.push_back(actor7);
    if (actor8) list.push_back(actor8);
  }

  for (unsigned int j=0; j<list.size(); j++)
    metadataset->AddActor (list[j]);

  if (this->FirstRender)
  {
    this->GetView4()->ResetCamera();
    this->FirstRender = false;
  }

  return list;

}


void vtkKWPageView::RemoveMetaDataSet(vtkMetaDataSet* metadataset)
{
  vtkPointSet* dataset = vtkPointSet::SafeDownCast (metadataset->GetDataSet());
  if (!dataset)
  {
    this->m_Pool->SyncSetInput (NULL);
    return;
  }
  
  this->View1->RemoveDataSet(dataset);
  this->View2->RemoveDataSet(dataset);
  this->View3->RemoveDataSet(dataset);
  this->View4->RemoveDataSet(dataset);

  for (unsigned int i=0; i<metadataset->GetNumberOfActors(); i++)
  {
    this->View1->GetRenderer()->RemoveViewProp (metadataset->GetActor (i));
    this->View2->GetRenderer()->RemoveViewProp (metadataset->GetActor (i));
    this->View3->GetRenderer()->RemoveViewProp (metadataset->GetActor (i));
    this->View4->GetRenderer()->RemoveViewProp (metadataset->GetActor (i));
  }
  
  if (metadataset->GetWirePolyData())
  {
    vtkPointSet* wire = metadataset->GetWirePolyData();

    this->View1->RemoveDataSet(wire);
    this->View2->RemoveDataSet(wire);
    this->View3->RemoveDataSet(wire);
    this->View4->RemoveDataSet(wire);
  }

  metadataset->RemoveAllActors();

  
}

