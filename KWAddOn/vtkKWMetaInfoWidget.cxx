/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWMetaInfoWidget.cxx 1294 2009-10-20 16:12:00Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2009-10-20 18:12:00 +0200 (Tue, 20 Oct 2009) $
Version:   $Revision: 1294 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkKWMetaInfoWidget.h"
#include "vtkObjectFactory.h"


#include "vtkKWIcon.h"
#include "vtkKWMessageWithLabel.h"
#include "vtkKWPiecewiseFunctionEditor.h"
#include "vtkKWPiecewiseFunctionEditorSet.h"
#include "vtkKWHistogram.h"
#include "vtkKWLabel.h"
#include "vtkKWMessage.h"
#include <vtksys/SystemTools.hxx>

#include "vtkKWRenderWidget.h"
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>
#include <vtkDataSet.h>

#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkCellData.h"

#include <pixmap/KWAddOnResources.h>
#include <vtkMetaDataSetSequence.h>
#include <vtkMetaDataSet.h>
#include <vtkMetaImageData.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkMetaVolumeMesh.h>

#include <vtkViewImage2D.h>
#include <vtkViewImage3D.h>

#include <vtkProperty.h>
#include <vtkMapper.h>


#include <sstream>

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWMetaInfoWidget );
vtkCxxRevisionMacro(vtkKWMetaInfoWidget, "$Revision: 1294 $");

//----------------------------------------------------------------------------
vtkKWMetaInfoWidget::vtkKWMetaInfoWidget()
{

  this->Sequence          = 0;
  this->MetaDataSet       = 0;
  this->Preview           = 0;
  this->RenderWidget      = vtkKWRenderWidget::New();
  
  this->AttributesMessage = vtkKWMessageWithLabel::New();
  this->HistogramsFrame   = vtkKWPiecewiseFunctionEditorSet::New();
  
  this->AttributesText = "No Sequence";
  
  
}

//----------------------------------------------------------------------------
vtkKWMetaInfoWidget::~vtkKWMetaInfoWidget()
{
  if (this->Preview)
    this->Preview->Delete();

  if (this->Sequence)
    this->Sequence->UnRegister (this);
  if (this->MetaDataSet)
    this->MetaDataSet->UnRegister (this);
  
  this->HistogramsFrame->Delete();
  
  this->RenderWidget->Delete();
  this->AttributesMessage->Delete();

}



//----------------------------------------------------------------------------
void vtkKWMetaInfoWidget::CreateWidget()
{
  // Check if already created

  if (this->IsCreated())
    {
    vtkErrorMacro("class already created");
    return;
    }

  // Call the superclass to create the whole widget
  this->Superclass::CreateWidget();

  this->CreateAttributes();
  this->CreateHistograms();
  this->CreateRenderWidget();
  

  this->Pack();
  


}

void vtkKWMetaInfoWidget::CreateAttributes()
{
  
  this->AttributesMessage->SetParent(this);
  this->AttributesMessage->Create();

  this->AttributesMessage->SetWidth(500);
  
  this->AttributesMessage->GetLabel()->SetImageToPredefinedIcon(vtkKWIcon::IconInfoMini);
  this->AttributesMessage->GetWidget()->SetText(this->AttributesText.c_str());
  this->AttributesMessage->SetBalloonHelpString("Information about the dataset");


}


void vtkKWMetaInfoWidget::CreateHistograms()
{
  this->HistogramsFrame->SetParent(this);
  this->HistogramsFrame->Create();
  this->HistogramsFrame->SetMaximumNumberOfWidgetsInPackingDirection (3);  
}



void vtkKWMetaInfoWidget::CreatePreviewTo2D()
{

  if (this->Preview)
  {
    this->Preview->Delete();
    this->Preview = NULL;
  }

  this->Preview = vtkViewImage2D::New();
  vtkViewImage2D* view2d = vtkViewImage2D::SafeDownCast(this->Preview);

  if (!view2d)
  {
    vtkErrorMacro("failed to initialize view !");  
    return;
  }  
  
  view2d->SetRenderWindow(this->RenderWidget->GetRenderWindow());
  view2d->SetRenderer(this->RenderWidget->GetRenderer());
  view2d->SetRenderWindowInteractor(this->RenderWidget->GetRenderWindow()->GetInteractor());
  view2d->SetAboutData ("");
  view2d->SetOrientation(vtkViewImage::AXIAL_ID);
  view2d->SetBackgroundColor (0,0,0);
  view2d->SetShowAnnotations (false);
  view2d->Show2DAxisOff();
  view2d->SetShowDirections(false);
  view2d->SetInteractionStyle (vtkViewImage2D::WINDOW_LEVEL_INTERACTION);
}

void vtkKWMetaInfoWidget::CreatePreviewTo3D()
{

  if (this->Preview)
  {
    this->Preview->Delete();
    this->Preview = NULL;
  }

  this->Preview = vtkViewImage3D::New();
  vtkViewImage3D* view3d = vtkViewImage3D::SafeDownCast(this->Preview);

  if (!view3d)
  {
    vtkErrorMacro("failed to initialize view !");  
    return;
  }

  view3d->SetRenderWindow(this->RenderWidget->GetRenderWindow());
  view3d->SetRenderer(this->RenderWidget->GetRenderer());
  view3d->SetRenderWindowInteractor(this->RenderWidget->GetRenderWindow()->GetInteractor());
  view3d->SetAboutData ("");
  view3d->SetShowAnnotations (false);
  
}


void vtkKWMetaInfoWidget::CreateRenderWidget()
{
  if (this->RenderWidget)
  {
    
    this->RenderWidget->SetParent(this);
    this->RenderWidget->Create();
    
    this->RenderWidget->SetWidth(150);
    this->RenderWidget->SetHeight(150);
  }
  
}



void vtkKWMetaInfoWidget::Pack()
{
  
  if (this->AttributesMessage->IsCreated())
  {
    this->Script( "grid %s -sticky nw -column 0 -row 0 -padx 10 -pady 5", 
		  this->AttributesMessage->GetWidgetName());
  }
  if (this->RenderWidget && this->RenderWidget->IsCreated())
  {
    this->Script( "grid %s -sticky nw -column 1 -row 0 -padx 10 -pady 5", 
		  this->RenderWidget->GetWidgetName());
  }
  
  if (this->HistogramsFrame->IsCreated())
  {
    this->Script( "grid %s -sticky nw -column 0 -row 1 -padx 10 -pady 5", 
		  this->HistogramsFrame->GetWidgetName());
  }

  this->Script("grid columnconfigure %s 0 -weight 3", this->GetWidgetName());
  this->Script("grid columnconfigure %s 1 -weight 1", this->GetWidgetName());
}


void vtkKWMetaInfoWidget::SetSequence(vtkMetaDataSetSequence* sequence)
{
  if (!this->IsCreated())
  {
    vtkWarningMacro("This method has to be called \nafter the widget has been created !");
    return;
  }

  if (!sequence)
  {
    vtkWarningMacro("Attempt to set sequence with NULL pointer !");
    return;
  }

  if (this->Sequence)
  {
    this->Sequence->UnRegister(this);
    this->Sequence = NULL;
  }

  this->Sequence = sequence;
  this->Sequence->Register(this);

  switch(this->Sequence->GetType())
  {
      case vtkMetaDataSet::VTK_META_IMAGE_DATA :
	this->CreatePreviewTo2D();
	break;
      case vtkMetaDataSet::VTK_META_SURFACE_MESH :
      case vtkMetaDataSet::VTK_META_VOLUME_MESH :
	this->CreatePreviewTo3D();
	break;
      default :
	break;
  }
  
  this->Update();

  this->Pack();
}



void vtkKWMetaInfoWidget::Update()
{
  this->UpdateAttributes();
  this->UpdateHistograms();
  this->UpdatePreview();
}

void vtkKWMetaInfoWidget::SetMetaDataSet(vtkMetaDataSet* metadataset)
{
  if (!this->IsCreated())
  {
    vtkWarningMacro("This method has to be called \nafter the widget has been created !");
    return;
  }

  if (!metadataset)
  {
    vtkWarningMacro("Attempt to set metadataset with NULL pointer !");
    return;
  }

  if (this->MetaDataSet)
  {
    this->MetaDataSet->UnRegister(this);
    this->MetaDataSet = NULL;
  }

  this->MetaDataSet = metadataset;
  this->MetaDataSet->Register(this);

  switch(this->MetaDataSet->GetType())
  {
      case vtkMetaDataSet::VTK_META_IMAGE_DATA :
	this->CreatePreviewTo2D();
	break;
      case vtkMetaDataSet::VTK_META_SURFACE_MESH :
      case vtkMetaDataSet::VTK_META_VOLUME_MESH :
	this->CreatePreviewTo3D();
	break;
      default :
	break;
  }
    
  this->Update();

  this->Pack();
}


void vtkKWMetaInfoWidget::UpdateAttributes()
{
  if (!this->Sequence && !this->MetaDataSet)
  {
    vtkWarningMacro("cannot create attributes,\nno data to process !");
  }

  if (this->Sequence && this->MetaDataSet)
  {
    vtkWarningMacro("cannot create attributes for both sequence & dataset,\nprocessing only the sequence information.");
  }

  std::ostringstream os;
  vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::SafeDownCast (this->MetaDataSet);
  
  os << "name : \t\t"<< this->MetaDataSet->GetName()<< std::endl
     << "dataset type : \t"<< this->MetaDataSet->GetDataSetType()<< std::endl;

  vtkMetaImageData* metaimage = vtkMetaImageData::SafeDownCast(this->MetaDataSet);
  
  if (metaimage)
  {
//     for (unsigned int i=0; i<metaimage->GetDicomTagList().size(); i++)
//     {
//       vtkMetaImageData::DicomTagType tag = metaimage->GetDicomTagList()[i];
//       std::string value = tag.value;
//       if (!value.size())
// 	continue;
//       std::cout<<"adding : "<<tag.name <<": \t"<<tag.value<<std::endl;
      
//       os << tag.name <<": \t"<<tag.value<<std::endl;
//     }
  }
  
    
  if (sequence)
  {
    os << "duration : \t"<< sequence->GetSequenceDuration()<< std::endl
       << "n. of files : \t"<< sequence->GetNumberOfMetaDataSets()<< std::endl;
  }
  
  
  this->AttributesText = os.str();
  this->AttributesMessage->GetWidget()->SetText(this->AttributesText.c_str());
  
}


void vtkKWMetaInfoWidget::AddHistogram(vtkKWHistogram* histogram, const char* name)
{
  
  vtkKWPiecewiseFunctionEditor* histo = this->HistogramsFrame->AddWidget(this->HistogramsFrame->GetNumberOfWidgets());
  histo->SetHistogram(histogram);

  histo->DisplayHistogramOnly();
  histo->ParameterTicksVisibilityOn();
  histo->ValueTicksVisibilityOn();
  histo->ComputeValueTicksFromHistogramOn();
  histo->SetParameterTicksFormat("%-#6.3f");
  histo->SetParameterEntryFormat("%-#6.3f");
  histo->SetMidPointGuidelineValueFormat("%-#6.3f");
  histo->SetValueTicksFormat("%-#6.0f");
  histo->GetLabel()->SetImageToPredefinedIcon(vtkKWIcon::IconInfoMini);
  histo->GetLabel()->SetText(name);
  histo->SetBalloonHelpString(name);
  histo->ParameterRangeVisibilityOn();
  histo->SetCanvasWidth(450);
  histo->SetCanvasHeight(150);
  histo->LabelVisibilityOn();
  histo->SetHistogramColor(0.18, 0.24, 0.49);
  histo->SetHistogramStyleToBars();
  histo->ParameterRangeLabelVisibilityOn();
  histo->ParameterRangeVisibilityOn();
  histo->SetFrameBackgroundColor(0.92, 1.0, 0.92);

}

void vtkKWMetaInfoWidget::UpdateHistograms()
{
  if (!this->MetaDataSet)
  {
    vtkWarningMacro("cannot create histogram,\nno data to process !");
    return;
  }

  if (this->MetaDataSet->GetType() == vtkMetaDataSet::VTK_META_IMAGE_DATA)
  {

    if (this->MetaDataSet->GetDataSet()->GetPointData()->GetScalars())
    {  
      this->MetaDataSet->GetDataSet()->GetPointData()->GetScalars()->SetName ("scalars");
      vtkKWHistogram* histogram = vtkKWHistogram::New();
      histogram->SetMaximumNumberOfBins (100);
      
      histogram->BuildHistogram(this->MetaDataSet->GetDataSet()->GetPointData()->GetScalars(), 0);
      this->AddHistogram (histogram, this->MetaDataSet->GetDataSet()->GetPointData()->GetScalars()->GetName());
      histogram->Delete();
    }
  }
  else
  {
    unsigned int N_CellArrays = 0;
    if (this->MetaDataSet->GetDataSet()->GetCellData())
      N_CellArrays = this->MetaDataSet->GetDataSet()->GetCellData()->GetNumberOfArrays();
    
    unsigned int N_PointArrays = 0;
    if (this->MetaDataSet->GetDataSet()->GetPointData())
      N_PointArrays = this->MetaDataSet->GetDataSet()->GetPointData()->GetNumberOfArrays();

    for (unsigned int i=0; i<N_CellArrays; i++)
    {
      vtkDataArray* array = this->MetaDataSet->GetDataSet()->GetCellData()->GetArray (i);
      vtkKWHistogram* histogram = vtkKWHistogram::New();
      histogram->SetMaximumNumberOfBins (100);
      histogram->BuildHistogram(array, 0);
      this->AddHistogram (histogram, array->GetName());
      histogram->Delete();
    }
    for (unsigned int i=0; i<N_PointArrays; i++)
    {
      vtkDataArray* array = this->MetaDataSet->GetDataSet()->GetPointData()->GetArray (i);
      vtkKWHistogram* histogram = vtkKWHistogram::New();
      histogram->SetMaximumNumberOfBins (100);
      histogram->BuildHistogram(array, 0);

//       vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::SafeDownCast (this->MetaDataSet);
  
//       if (sequence)
//       {
// 	std::vector<vtkMetaDataSet*> list = sequence->GetMetaDataSetList();
	
// 	for (unsigned int j=0; i<list.size(); j++)
// 	{
// 	  vtkMetaDataSet* it = list[j];
// 	  if (!it ||
// 	      !it->GetDataSet() ||
// 	      !it->GetDataSet()->GetPointData())
// 	    continue;
	  
// 	  vtkDataArray* array_j = sequence->GetMetaDataSet (j)->GetDataSet()->GetPointData()->GetArray (array->GetName());
// 	  if (!array_j)
// 	    continue;
// // 	  double* hist_range = histogram->GetRange();
// // 	  double* array_range = array_j->GetRange();
// // 	  double min = hist_range[0];
// // 	  double max = hist_range[1];
// // 	  if (min > array_range[0])
// // 	    min = array_range[0];
// // 	  if (max < array_range[1])
// // 	    max = array_range[1];
// // 	  histogram->SetRange (min, max);
// // 	  histogram->EmptyHistogram();
// 	  histogram->AccumulateHistogram(array_j, 0);
// 	}
//       }
      
      this->AddHistogram (histogram, array->GetName());
      histogram->Delete();
    }
  }
}


void vtkKWMetaInfoWidget::UpdatePreview()
{
  
  if (!this->Preview)
  {
    vtkWarningMacro("cannot create preview,\npreview is NULL");
    return;
  }

  if (!this->MetaDataSet)
  {
    vtkWarningMacro("cannot create preview,\nno dataset to process");
    return;
  }

  vtkDataSet*   dataset  = NULL;
//   vtkProperty*  property = NULL;

  dataset = this->MetaDataSet->GetDataSet();

  vtkProperty* prop   = vtkProperty::SafeDownCast(this->MetaDataSet->GetProperty());
  
  this->Preview->AddDataSet(this->MetaDataSet->GetDataSet(), prop);
  this->Preview->ResetCurrentPoint();
  this->Preview->ResetWindowLevel();
  
  
  
}
