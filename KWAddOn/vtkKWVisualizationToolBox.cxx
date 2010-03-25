/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWVisualizationToolBox.cxx 1302 2009-10-27 21:57:16Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2009-10-27 22:57:16 +0100 (Tue, 27 Oct 2009) $
Version:   $Revision: 1302 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <vtkKWVisualizationToolBox.h>
#include "vtkObjectFactory.h"

#include <vtkKWHSVColorSelector.h>

#include <vtkLookupTableManager.h>
#include <vtkMetaDataSet.h>
#include <vtkPointSet.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkDataArray.h>
#include <kwcommon.h>
#include <vtkKWPushButton.h>
#include <vtkKWComboBox.h>
#include <vtkKWLoadSaveDialog.h>
#include <vtkStringArray.h>
#include <vtkKWIcon.h>
#include <vtkKWFrameWithLabel.h>
#include <vtkKWMessageDialog.h>
#include <vtkDataArrayCollection.h>
#include <vtksys/SystemTools.hxx>
#include <vtkErrorCode.h>
#include <vtkViewImage3D.h>
#include <vtkViewImage2D.h>
#include <vtkKWPageView.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkMetaDataSetSequence.h>
#include <vtkDataManager.h>
#include <vtkUnstructuredGrid.h>
#include <vtkKWMainWindowInteractor.h>
#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <sstream>
#include <vtkDiskSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkPolyDataNormals.h>
#include <vtkCellArray.h>
#include <vtkKWPageView.h>
#include <vtkScalarBarActor.h>
#include <vtkKWMessageDialog.h>
#include <pixmap/KWAddOnResources.h>
#include <vtkKWSimpleEntryDialog.h>
#include <vtkKWEntryWithLabel.h>
#include <vtkKWEntry.h>
#include <vtkKWMenuButton.h>
#include <vtkKWApplication.h>
#include <vtkKWComboBoxSet.h>
#include <vtkKWScaleSet.h>
#include <vtkKWComboBox.h>
#include <vtkKWScale.h>
#include <vtkMath.h>
#include <vtkKWSurfaceMaterialPropertyWidget.h>
#include <vtkKWPopupButtonWithLabel.h>
#include <vtkKWPopupButton.h>
#include <vtkKWSeparator.h>
#include <vtkKWMenuButton.h>
#include <vtkKWMenu.h>
#include <vtkKWLabel.h>
#include <vtkScalarBarActor.h>
#include <vtkKWCheckButtonWithLabel.h>
#include <vtkKWCheckButton.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkKWRange.h>
#include <vtkTextProperty.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWVisualizationToolBox );
vtkCxxRevisionMacro( vtkKWVisualizationToolBox, "$Revision: 1302 $");

//----------------------------------------------------------------------------
void vtkKWVisualizationCallback::Execute(vtkObject *   caller, 
					 unsigned long event, 
					 void *        callData)
{

    if (event == vtkViewImage::ViewImageWindowLevelChangeEvent)
    {
    if ( !this->WindowLevelLock )
    {
      vtkViewImage2D* view = vtkViewImage2D::SafeDownCast (caller);
      if (view)
      {
	if (this->Toolbox)
	{
	  double min = view->GetLevel() - 0.5*view->GetWindow();
	  double max = view->GetLevel() + 0.5*view->GetWindow();
	  this->Toolbox->GetImageWindowLevelRange()->DisableCommandsOn();
	  this->Toolbox->GetImageWindowLevelRange()->SetRange (min, max);
	  this->Toolbox->GetImageWindowLevelRange()->DisableCommandsOff();
	}
      }
    }
  }
}




//----------------------------------------------------------------------------
vtkKWVisualizationToolBox::vtkKWVisualizationToolBox()
{
  // mesh visualization

  this->MeshVisuFrame = vtkKWFrameWithLabel::New();
  this->ImageVisuFrame = vtkKWFrameWithLabel::New();
  
  this->HSVColorSelector = vtkKWHSVColorSelector::New();
  this->S_MaterialPropertyWidget = vtkKWSurfaceMaterialPropertyWidget::New();
  this->ScaleSet = vtkKWScaleSet::New();
  this->OpacityScale = NULL;
  this->LineWidthScale = NULL;
  this->ComboBoxSet = vtkKWComboBoxSet::New();
  this->SurfaceRepresentationComboBox = NULL;
  this->InterpolationModeComboBox = NULL;  

  this->DataListBox = vtkKWComboBox::New();
  this->ButtonSetColorMap = vtkKWMenuButton::New();
  this->ScalarBarCheckbox = vtkKWCheckButtonWithLabel::New();  
  this->ScalarBar1 = vtkScalarBarActor::New();
  this->ScalarBar2 = vtkScalarBarActor::New();
  this->ScalarBar3 = vtkScalarBarActor::New();
  this->ScalarBar4 = vtkScalarBarActor::New();

  this->ButtonSetLookupTable = vtkKWMenuButton::New();
  this->ShadingCheckbox = vtkKWCheckButtonWithLabel::New();

  this->ImageWindowLevelRange = vtkKWRange::New();
  
  this->MeshScalarRange = vtkKWRange::New();

  this->ImageRenderingModeComboBox = vtkKWComboBox::New();
  this->CroppingBoxCheckbox = vtkKWCheckButtonWithLabel::New();
  this->ImageScalarBarCheckbox = vtkKWCheckButtonWithLabel::New();
  
  this->Callback = vtkKWVisualizationCallback::New();
  this->Callback->SetToolbox (this);
  
  this->SetProperties();
  
}


//----------------------------------------------------------------------------
vtkKWVisualizationToolBox::~vtkKWVisualizationToolBox()
{

  this->S_MaterialPropertyWidget->Delete();
  this->HSVColorSelector->Delete();
  this->ScaleSet->Delete();
  this->ComboBoxSet->Delete();
  this->DataListBox->Delete();
  this->ButtonSetColorMap->Delete();
  this->ScalarBarCheckbox->Delete();
  this->ScalarBar1->Delete();
  this->ScalarBar2->Delete();
  this->ScalarBar3->Delete();
  this->ScalarBar4->Delete();
  this->MeshVisuFrame->Delete();
  
  this->ShadingCheckbox->Delete();
  this->ButtonSetLookupTable->Delete();
  this->ImageVisuFrame->Delete();

  this->ImageWindowLevelRange->Delete();
  this->MeshScalarRange->Delete();
  
  this->CroppingBoxCheckbox->Delete();
  this->ImageScalarBarCheckbox->Delete();
  
  this->ImageRenderingModeComboBox->Delete();
  
  this->Callback->Delete();
  
  
}



//----------------------------------------------------------------------------
void vtkKWVisualizationToolBox::SetProperties()
{

  this->Icon->SetImage(image_visualization, image_visualization_width, image_visualization_height, image_visualization_pixel_size, image_visualization_length);
  this->SetName("Visualization tools");
  this->ColorMapId = vtkLookupTableManager::LUT_LONI;
  this->LUTId = vtkLookupTableManager::LUT_LONI;

  this->ScalarBar1->GetLabelTextProperty()->SetColor (1.0,1.0,1.0);
  this->ScalarBar1->GetTitleTextProperty()->SetColor (1.0,1.0,1.0);
  this->ScalarBar1->GetLabelTextProperty()->BoldOff();
  this->ScalarBar1->GetLabelTextProperty()->ShadowOff();
  this->ScalarBar1->GetLabelTextProperty()->ItalicOff();
  this->ScalarBar1->SetNumberOfLabels (3);
  this->ScalarBar1->GetLabelTextProperty()->SetFontSize (1);
  this->ScalarBar1->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  this->ScalarBar1->SetWidth (0.1);
  this->ScalarBar1->SetHeight (0.5);
  this->ScalarBar1->SetPosition (0.9,0.3);
  this->ScalarBar1->VisibilityOff();

  this->ScalarBar2->GetLabelTextProperty()->SetColor (1.0,1.0,1.0);
  this->ScalarBar2->GetTitleTextProperty()->SetColor (1.0,1.0,1.0);
  this->ScalarBar2->GetLabelTextProperty()->BoldOff();
  this->ScalarBar2->GetLabelTextProperty()->ShadowOff();
  this->ScalarBar2->GetLabelTextProperty()->ItalicOff();
  this->ScalarBar2->SetNumberOfLabels (3);
  this->ScalarBar2->GetLabelTextProperty()->SetFontSize (1);
  this->ScalarBar2->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  this->ScalarBar2->SetWidth (0.1);
  this->ScalarBar2->SetHeight (0.5);
  this->ScalarBar2->SetPosition (0.9,0.3);
  this->ScalarBar2->VisibilityOff();

  this->ScalarBar3->GetLabelTextProperty()->SetColor (1.0,1.0,1.0);
  this->ScalarBar3->GetTitleTextProperty()->SetColor (1.0,1.0,1.0);
  this->ScalarBar3->GetLabelTextProperty()->BoldOff();
  this->ScalarBar3->GetLabelTextProperty()->ShadowOff();
  this->ScalarBar3->GetLabelTextProperty()->ItalicOff();
  this->ScalarBar3->SetNumberOfLabels (3);
  this->ScalarBar3->GetLabelTextProperty()->SetFontSize (1);
  this->ScalarBar3->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  this->ScalarBar3->SetWidth (0.1);
  this->ScalarBar3->SetHeight (0.5);
  this->ScalarBar3->SetPosition (0.9,0.3);
  this->ScalarBar3->VisibilityOff();

  this->ScalarBar4->GetLabelTextProperty()->SetColor (1.0,1.0,1.0);
  this->ScalarBar4->GetTitleTextProperty()->SetColor (1.0,1.0,1.0);
  this->ScalarBar4->GetLabelTextProperty()->BoldOff();
  this->ScalarBar4->GetLabelTextProperty()->ShadowOff();
  this->ScalarBar4->GetLabelTextProperty()->ItalicOff();
  this->ScalarBar4->SetNumberOfLabels (3);
  this->ScalarBar4->GetLabelTextProperty()->SetFontSize (1);
  this->ScalarBar4->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  this->ScalarBar4->SetWidth (0.1);
  this->ScalarBar4->SetHeight (0.5);
  this->ScalarBar4->SetPosition (0.9,0.3);
  this->ScalarBar4->VisibilityOff();
  this->ScalarBar4->GetLabelTextProperty()->SetColor (0.0,0.0,0.0);
  this->ScalarBar4->GetTitleTextProperty()->SetColor (0.0,0.0,0.0);
 
}


//----------------------------------------------------------------------------
void vtkKWVisualizationToolBox::CreateMeshVisuFrame()
{


  this->MeshVisuFrame->SetParent (this);
  this->MeshVisuFrame->Create();
  this->MeshVisuFrame->SetLabelText ("mesh");

  vtkKWFrame* tmp1 = vtkKWFrame::New();
  tmp1->SetParent(this->MeshVisuFrame->GetFrame());
  tmp1->Create();
  
  this->HSVColorSelector->SetParent(tmp1);
  this->HSVColorSelector->Create();  
  this->HSVColorSelector->SetHueSatWheelRadius (30);
  this->HSVColorSelector->SetBalloonHelpString("Set the solid color of the dataset");
  this->HSVColorSelector->SetSelectionChangingCommand (this, "ColorChangingCallback");
  this->HSVColorSelector->SetSelectionChangedCommand (this, "ColorChangedCallback");
  this->HSVColorSelector->InvokeCommandsWithRGBOn();

  this->Script("pack %s -side left -anchor nw -expand yes  -padx 0 -pady 0", 
	       this->HSVColorSelector->GetWidgetName());
  
  this->S_MaterialPropertyWidget->SetParent(tmp1);
  this->S_MaterialPropertyWidget->PopupModeOn();
  this->S_MaterialPropertyWidget->Create();
  this->S_MaterialPropertyWidget->GetPopupButton()->SetLabelPositionToTop();
  this->S_MaterialPropertyWidget->SetPresetSize (25);
  this->S_MaterialPropertyWidget->SetPreviewSize(25);  
  this->S_MaterialPropertyWidget->SetPropertyChangedCommand (this, "InvokeMetaDataSetModifiedCommand");
  this->S_MaterialPropertyWidget->SetPropertyChangingCommand (this, "InvokeMetaDataSetModifiedCommand");

  this->Script("pack %s -side left -anchor sw  -expand no -padx 10 -pady 10",
	       this->S_MaterialPropertyWidget->GetWidgetName());

  
  this->Script("pack %s -side top -anchor nw -expand no  -padx 0 -pady 0",
	       tmp1->GetWidgetName());
  tmp1->Delete();

  this->ScaleSet->SetParent(this->MeshVisuFrame->GetFrame());
  this->ScaleSet->PackHorizontallyOn();
  this->ScaleSet->Create();
  this->ScaleSet->SetBorderWidth(2);
  this->ScaleSet->SetReliefToGroove();
  this->ScaleSet->SetWidgetsPadX(1);
  this->ScaleSet->SetWidgetsPadY(1);
  this->ScaleSet->SetPadX(1);
  this->ScaleSet->SetPadY(1);
  this->ScaleSet->SetMaximumNumberOfWidgetsInPackingDirection(2);

  this->Script("pack %s -side top -anchor nw -expand no  -padx 0 -pady 0",
	       this->ScaleSet->GetWidgetName());

  this->OpacityScale = this->ScaleSet->AddWidget(0);
  this->OpacityScale->SetOrientationToHorizontal();
  this->OpacityScale->SetLabelText("Opacity");
  this->OpacityScale->SetRange (0,100);
  this->OpacityScale->SetResolution (1);
  this->OpacityScale->SetLength (95);
  this->OpacityScale->SetSliderLength (10);
  this->OpacityScale->SetValue (100);
  this->OpacityScale->SetCommand (this, "OpacityChangingCallback");
  this->OpacityScale->SetBalloonHelpString ("Set the opacity of the dataset");
  
  this->LineWidthScale = this->ScaleSet->AddWidget(1);
  this->LineWidthScale->SetOrientationToHorizontal();
  this->LineWidthScale->SetLabelText("Line Width");
  this->LineWidthScale->SetRange (1,10);
  this->LineWidthScale->SetResolution (1);
  this->LineWidthScale->SetLength (95);
  this->LineWidthScale->SetSliderLength (8);
  this->LineWidthScale->SetValue (1);
  this->LineWidthScale->SetCommand (this, "LineWidthChangingCallback");
  this->LineWidthScale->SetBalloonHelpString ("Set the line width and point size of the dataset");

  this->ComboBoxSet->SetParent(this->MeshVisuFrame->GetFrame());
  this->ComboBoxSet->PackHorizontallyOn();
  this->ComboBoxSet->Create();
  this->ComboBoxSet->SetBorderWidth(2);
  this->ComboBoxSet->SetReliefToGroove();
  this->ComboBoxSet->SetWidgetsPadX(1);
  this->ComboBoxSet->SetWidgetsPadY(1);
  this->ComboBoxSet->SetPadX(1);
  this->ComboBoxSet->SetPadY(1);
  this->ComboBoxSet->SetMaximumNumberOfWidgetsInPackingDirection(2);

  this->Script("pack %s -side top -anchor nw -expand no  -padx 0 -pady 0",
	       this->ComboBoxSet->GetWidgetName());

  this->SurfaceRepresentationComboBox = this->ComboBoxSet->AddWidget (0);
  this->SurfaceRepresentationComboBox->SetBackgroundColor (1,1,1);  
  this->SurfaceRepresentationComboBox->SetWidth (12);
  this->SurfaceRepresentationComboBox->ReadOnlyOn();
  this->SurfaceRepresentationComboBox->SetValue("Representation...");
  this->SurfaceRepresentationComboBox->AddValue("Solid");
  this->SurfaceRepresentationComboBox->AddValue("Wireframe");
  this->SurfaceRepresentationComboBox->AddValue("Points");
  this->SurfaceRepresentationComboBox->AddValue("Hidden lines");
  this->SurfaceRepresentationComboBox->SetCommand(this, "SurfaceRepresentationChangedCallBack");
  this->SurfaceRepresentationComboBox->SetBalloonHelpString ("Choose the surface representation mode of the dataset");

  this->InterpolationModeComboBox = this->ComboBoxSet->AddWidget (1);
  this->InterpolationModeComboBox->SetWidth (12);
  this->InterpolationModeComboBox->ReadOnlyOn();
  this->InterpolationModeComboBox->SetValue("Interpolation...");
  this->InterpolationModeComboBox->AddValue("Flat");
  this->InterpolationModeComboBox->AddValue("Gouraud");
  this->InterpolationModeComboBox->SetCommand(this, "InterpolationModeChangedCallBack");
  this->InterpolationModeComboBox->SetBalloonHelpString ("Choose the surface interpolation mode of the dataset");
  this->InterpolationModeComboBox->SetBackgroundColor (1,1,1);



  vtkKWSeparator* separator1 = vtkKWSeparator::New();
  separator1->SetParent(this->MeshVisuFrame->GetFrame());
  separator1->Create();
  separator1->SetWidth (300);
  separator1->SetOrientationToHorizontal();
  separator1->SetThickness(2);
  this->Script( "pack %s -side top -anchor nw -expand yes -fill x -padx 0 -pady 2",
		separator1->GetWidgetName());
  separator1->Delete();

  vtkKWLabel* label = vtkKWLabel::New();
  label->SetParent(this->MeshVisuFrame->GetFrame());
  label->Create();
  label->SetText("Attributes mapping");
  this->Script( "pack %s -side top -anchor nw -expand no -padx 0 -pady 2",
		label->GetWidgetName());
  label->Delete();

  vtkKWFrame* tmp2 = vtkKWFrame::New();
  tmp2->SetParent(this->MeshVisuFrame->GetFrame());
  tmp2->Create();
  
  this->DataListBox->SetParent(tmp2);
  this->DataListBox->Create();
  this->DataListBox->SetWidth (20);
  this->DataListBox->SetListboxWidth (200);
  
  this->DataListBox->ReadOnlyOn();
  this->DataListBox->SetValue("Select Attributes");
  this->DataListBox->AddValue("Solid");
  this->DataListBox->SetCommand(this, "ColorMapChangedCallback");
  this->DataListBox->SetCommandTrigger(vtkKWEntry::TriggerOnReturnKey);
  this->DataListBox->SetBalloonHelpString ("Choose the scalar attribute you want to display for this dataset");
  this->DataListBox->SetBackgroundColor (1,1,1);

  this->Script("pack %s -side left -anchor nw -expand no  -padx 0 -pady 0",
	       this->DataListBox->GetWidgetName());

  this->ButtonSetColorMap->SetParent (tmp2);
  this->ButtonSetColorMap->Create();
  this->ButtonSetColorMap->IndicatorVisibilityOff();
  this->ButtonSetColorMap->SetImageToPixels(image_lookuptable, image_lookuptable_width, image_lookuptable_height, image_lookuptable_pixel_size, image_lookuptable_length);
  this->ButtonSetColorMap->SetBalloonHelpString("Set the color map to use for this attribute ...");
  this->ButtonSetColorMap->SetBalloonHelpString ("Change the scalar color map of this dataset");
  std::vector<std::string> LUTList = vtkLookupTableManager::GetAvailableLookupTables();  
  for (unsigned int i=0; i<LUTList.size(); i++)
  {
    this->ButtonSetColorMap->GetMenu()->AddRadioButton(LUTList[i].c_str(), this, "ButtonSetColorMapCallBack");
    this->ButtonSetColorMap->GetMenu()->SetItemGroupName (i, "LUT");
  }
  
  this->Script("pack %s -side left -anchor nw -expand no  -padx 0 -pady 0",
	       this->ButtonSetColorMap->GetWidgetName());

  this->ScalarBarCheckbox->SetParent (tmp2);
  this->ScalarBarCheckbox->Create();
  this->ScalarBarCheckbox->SetLabelText ("Bar :");
  this->ScalarBarCheckbox->GetWidget()->SetCommand(this, "ScalarbarsVisibilityCallback");

  this->Script("pack %s -side left -anchor nw -expand no  -padx 0 -pady 0",
	       this->ScalarBarCheckbox->GetWidgetName());

  this->Script("pack %s -side top -anchor nw -expand no  -padx 0 -pady 0",
	       tmp2->GetWidgetName());
  tmp2->Delete();

  this->MeshScalarRange->SetParent (this->MeshVisuFrame->GetFrame());
  this->MeshScalarRange->Create();
  this->MeshScalarRange->SetLabelText ("scalar range");
  this->MeshScalarRange->SetWholeRange (0.0, 255.0);
  this->MeshScalarRange->SetRange (0.0, 255.0);
  this->MeshScalarRange->SetResolution(0.5);
  this->MeshScalarRange->SetReliefToGroove();
  this->MeshScalarRange->SliderCanPushOn();
  this->MeshScalarRange->SetBorderWidth (0);
  this->MeshScalarRange->SetCommand (this, "MeshScalarRangeCallback");
  this->MeshScalarRange->SetBalloonHelpString("Set the scalar range for the scalars");
  this->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
	       this->MeshScalarRange->GetWidgetName());

  this->MeshVisuFrame->CollapseFrame();
  
}


//----------------------------------------------------------------------------
void vtkKWVisualizationToolBox::CreateImageVisuFrame()
{


  this->ImageVisuFrame->SetParent (this);
  this->ImageVisuFrame->Create();
  this->ImageVisuFrame->SetLabelText ("image");

  vtkKWFrame* tmp1 = vtkKWFrame::New();
  vtkKWFrame* tmp2 = vtkKWFrame::New();
  vtkKWFrame* tmp3 = vtkKWFrame::New();
  tmp1->SetParent (this->ImageVisuFrame->GetFrame());
  tmp2->SetParent (this->ImageVisuFrame->GetFrame());
  tmp3->SetParent (this->ImageVisuFrame->GetFrame());
  tmp1->Create();
  tmp2->Create();
  tmp3->Create();
  
  this->Script("pack %s -side top -anchor nw -expand t  -padx 0 -pady 0",
	       tmp1->GetWidgetName());
  this->Script("pack %s -side top -anchor nw -expand t  -padx 0 -pady 0",
	       tmp2->GetWidgetName());
  this->Script("pack %s -side top -anchor nw -expand t  -padx 0 -pady 0",
	       tmp3->GetWidgetName());
  
  
  this->ButtonSetLookupTable->SetParent (tmp1);
  this->ButtonSetLookupTable->Create();
  this->ButtonSetLookupTable->IndicatorVisibilityOff();
  this->ButtonSetLookupTable->SetImageToPixels(image_lookuptable, image_lookuptable_width, image_lookuptable_height, image_lookuptable_pixel_size, image_lookuptable_length);
  this->ButtonSetLookupTable->SetBalloonHelpString("Set the color map to use for this image...");
  std::vector<std::string> LUTList = vtkLookupTableManager::GetAvailableLookupTables();  
  for (unsigned int i=0; i<LUTList.size(); i++)
  {
    this->ButtonSetLookupTable->GetMenu()->AddRadioButton(LUTList[i].c_str(), this, "ButtonSetLookupTableCallBack");
    this->ButtonSetLookupTable->GetMenu()->SetItemGroupName (i, "LUT");
  }
  
  this->Script("pack %s -side left -anchor nw -expand no  -padx 5 -pady 2",
	       this->ButtonSetLookupTable->GetWidgetName());

  this->ImageRenderingModeComboBox->SetParent (tmp1);
  this->ImageRenderingModeComboBox->Create();
  this->ImageRenderingModeComboBox->SetBackgroundColor (1,1,1);  
  this->ImageRenderingModeComboBox->SetWidth (16);
  this->ImageRenderingModeComboBox->ReadOnlyOn();
  this->ImageRenderingModeComboBox->SetValue("Rendering Mode");
  this->ImageRenderingModeComboBox->AddValue("Multi Planar");
  this->ImageRenderingModeComboBox->AddValue("Volume Rendering");
//   this->ImageRenderingModeComboBox->AddValue("MIP");
  this->ImageRenderingModeComboBox->SetCommand(this, "ImageRenderingModeCallback");
  this->ImageRenderingModeComboBox->SetBalloonHelpString ("Choose the 3D rendering mode of this image");
  this->Script("pack %s -side left -anchor nw -expand no  -padx 8 -pady 2",
	       this->ImageRenderingModeComboBox->GetWidgetName());

  
  this->CroppingBoxCheckbox->SetParent (tmp2);
  this->CroppingBoxCheckbox->Create();
  this->CroppingBoxCheckbox->SetLabelText ("Cropping box :");
  this->CroppingBoxCheckbox->GetWidget()->SetCommand(this, "CroppingBoxCallback");
  this->CroppingBoxCheckbox->SetBalloonHelpString("Toggle the cropping box visibility");
  this->Script("pack %s -side left -anchor nw -expand no  -padx 5 -pady 2",
	       this->CroppingBoxCheckbox->GetWidgetName());

  this->ImageScalarBarCheckbox->SetParent (tmp2);
  this->ImageScalarBarCheckbox->Create();
  this->ImageScalarBarCheckbox->SetLabelText ("Scalar bar :");
  this->ImageScalarBarCheckbox->GetWidget()->SetCommand(this, "ImageScalarBarCallback");
  this->ImageScalarBarCheckbox->SetBalloonHelpString("Toggle the image scalar bar visibility");
  this->Script("pack %s -side left -anchor nw -expand no  -padx 5 -pady 2",
	       this->ImageScalarBarCheckbox->GetWidgetName());

  
  this->ImageWindowLevelRange->SetParent (tmp3);
  this->ImageWindowLevelRange->Create();
  this->ImageWindowLevelRange->SetLabelText ("windowing");
  this->ImageWindowLevelRange->SetWholeRange (0.0, 255.0);
  this->ImageWindowLevelRange->SetRange (20.0, 155.0);
  this->ImageWindowLevelRange->SetResolution(0.5);
  this->ImageWindowLevelRange->SetReliefToGroove();
  this->ImageWindowLevelRange->SliderCanPushOn();
  this->ImageWindowLevelRange->SetBorderWidth (0);
  this->ImageWindowLevelRange->SetCommand (this, "ImageWindowLevelRangeCallback");
  this->ImageWindowLevelRange->SetBalloonHelpString("Set the windowing of the image");
  this->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
	       this->ImageWindowLevelRange->GetWidgetName());
  
  this->ShadingCheckbox->SetParent (tmp3);
  this->ShadingCheckbox->Create();
  this->ShadingCheckbox->SetLabelText ("Shading :");
  this->ShadingCheckbox->GetWidget()->SetCommand(this, "ShadingCallback");
  this->ShadingCheckbox->SetBalloonHelpString("Toggle shading effects");
  this->Script("pack %s -side left -anchor nw -expand no  -padx 5 -pady 2",
	       this->ShadingCheckbox->GetWidgetName());

  
  this->ImageVisuFrame->CollapseFrame();

  tmp1->Delete();
  tmp2->Delete();
  tmp3->Delete();
  
}



//----------------------------------------------------------------------------
void vtkKWVisualizationToolBox::CreateWidget()
{
  // Check if already created
  if (this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }
  
  // Call the superclass to create the whole widget
  this->Superclass::CreateWidget();

  this->CreateMeshVisuFrame();
  this->CreateImageVisuFrame();
  
  // Update according to the current metadataset
  this->Update();

  // Pack
  this->Pack();


}


//----------------------------------------------------------------------------
void vtkKWVisualizationToolBox::Pack()
{

  if (this->MeshVisuFrame->IsCreated())
  {
    this->Script( "pack %s -side top -anchor nw -expand yes -fill x -padx 0 -pady 0",
		  this->MeshVisuFrame->GetWidgetName());
  }
  if (this->ImageVisuFrame->IsCreated())
  {
    this->Script( "pack %s -side top -anchor nw -expand yes -fill x -padx 0 -pady 0",
		  this->ImageVisuFrame->GetWidgetName());
  }

}


//----------------------------------------------------------------------------
void vtkKWVisualizationToolBox::Update()
{
  this->Superclass::Update();
  
  this->PopulateDataList();

  if (!this->MetaDataSet)
    return;

  switch (this->MetaDataSet->GetType())
  {
      case vtkMetaDataSet::VTK_META_IMAGE_DATA :
	this->MeshVisuFrame->CollapseFrame();
	this->ImageVisuFrame->ExpandFrame();
	break;
      case vtkMetaDataSet::VTK_META_SURFACE_MESH :
      case vtkMetaDataSet::VTK_META_VOLUME_MESH :
	this->MeshVisuFrame->ExpandFrame();
	this->ImageVisuFrame->CollapseFrame();
	break;
      default :
	this->MeshVisuFrame->CollapseFrame();
	this->ImageVisuFrame->CollapseFrame();
	break;
  }

//   if (!this->IsMapped())
//     return;
  
  if (this->MetaDataSet->GetType() == vtkMetaDataSet::VTK_META_IMAGE_DATA)
  {
    vtkImageData* image = vtkImageData::SafeDownCast (this->MetaDataSet->GetDataSet());
    if (image)
    {
      double* range = image->GetScalarRange();
      double resolution = (range[1] - range[0])/255.0;
      this->ImageWindowLevelRange->SetWholeRange (range[0], range[1]);
      this->ImageWindowLevelRange->SetResolution (resolution);

      vtkKWPageView* page = this->ParentObject->GetPage (this->MetaDataSet->GetTag());
      if (page)
      {
	bool watchviews = true;
	
	char buffer[1024];
	bool valid = this->GetApplication()->GetRegistryValue(1, "RunTime", "LinkWindowLevelSliders", buffer);
	if (valid)
	{
	  if (*buffer)
	  {
	    std::istringstream is;
	    is.str (buffer);
	    int val = 0;
	    is >> val;
	    watchviews = (val > 0);
	  }
	}
	if (watchviews)
	{
	  double min = page->GetView1()->GetLevel() - 0.5*page->GetView1()->GetWindow();
	  double max = page->GetView1()->GetLevel() + 0.5*page->GetView1()->GetWindow();
	  this->ImageWindowLevelRange->SetRange (min, max);
	  page->GetView1()->AddObserver (vtkViewImage::ViewImageWindowLevelChangeEvent, this->Callback);
	  page->GetView2()->AddObserver (vtkViewImage::ViewImageWindowLevelChangeEvent, this->Callback);
	  page->GetView3()->AddObserver (vtkViewImage::ViewImageWindowLevelChangeEvent, this->Callback);
	}

	this->CroppingBoxCheckbox->GetWidget()->SetSelectedState (page->GetView4()->GetBoxWidgetVisibility());
	this->ImageScalarBarCheckbox->GetWidget()->SetSelectedState (page->GetScalarBarVisibility());
	this->ShadingCheckbox->GetWidget()->SetSelectedState (page->GetView4()->GetShade());
	
	switch(page->GetView4()->GetRenderingMode())
	{
	    case vtkViewImage3D::VOLUME_RENDERING :
	      this->ImageRenderingModeComboBox->SetValue ("Volume Rendering");
	      this->CroppingBoxCheckbox->GetWidget()->SetStateToNormal();
	      this->ShadingCheckbox->GetWidget()->SetStateToNormal();
	      break;
	    case vtkViewImage3D::PLANAR_RENDERING :
	      this->ImageRenderingModeComboBox->SetValue ("Multi Planar");
	      this->CroppingBoxCheckbox->GetWidget()->SetStateToDisabled();
	      this->ShadingCheckbox->GetWidget()->SetStateToDisabled();
	      break;
	    default :
	      break;
	}
	
	
      }
      
    }
  }
  
    
  
  if (!vtkProperty::SafeDownCast(this->MetaDataSet->GetProperty()))
    return;
  
  this->HSVColorSelector->SetSelectedColor (vtkMath::RGBToHSV (vtkProperty::SafeDownCast(this->MetaDataSet->GetProperty())->GetColor()));
  this->S_MaterialPropertyWidget->SetProperty (vtkProperty::SafeDownCast(this->MetaDataSet->GetProperty()));
  this->OpacityScale->SetValue ((double)(vtkProperty::SafeDownCast(this->MetaDataSet->GetProperty())->GetOpacity()*100));
  this->LineWidthScale->SetValue ((double)(vtkProperty::SafeDownCast(this->MetaDataSet->GetProperty())->GetLineWidth()));

  switch(vtkProperty::SafeDownCast(this->MetaDataSet->GetProperty())->GetRepresentation())
  {
      case VTK_POINTS :
	this->SurfaceRepresentationComboBox->SetValue("Points");
	break;
      case VTK_WIREFRAME :
	this->SurfaceRepresentationComboBox->SetValue("Wireframe");
	break;
      default :
	if (this->MetaDataSet->GetWirePolyData())
	  this->SurfaceRepresentationComboBox->SetValue("Hidden lines");
	else
	  this->SurfaceRepresentationComboBox->SetValue("Solid");
	break;
  }
  
  switch(vtkProperty::SafeDownCast(this->MetaDataSet->GetProperty())->GetInterpolation())
  {
      case VTK_FLAT :
	this->InterpolationModeComboBox->SetValue("Flat");
	break;
      default :
	this->InterpolationModeComboBox->SetValue("Gouraud");
	break;
  }

  
}


void vtkKWVisualizationToolBox::PopulateDataList ()
{
  
//   if (!this->IsMapped())
//     return;
  
  this->DataListBox->DeleteAllValues ();
  
  if (!this->MetaDataSet)
    return;
    
  
  vtkDataArray* array = this->MetaDataSet->GetCurrentScalarArray();

  this->DataListBox->AddValue ("Solid");
  if (!array)
    this->DataListBox->SetValue("Solid");
  
  vtkDataArrayCollection* arrays = vtkDataArrayCollection::New();
  this->MetaDataSet->GetColorArrayCollection (arrays);
  
  for (int i=0; i<arrays->GetNumberOfItems(); i++)
  {
    if (!arrays->GetItem(i)->GetName() || !(*arrays->GetItem(i)->GetName()))
      continue;
    std::string name = arrays->GetItem(i)->GetName();
    if (!name.size())
      continue;
    
    std::ostringstream os;
    os<<name;
    
    this->DataListBox->AddValue (os.str().c_str());
  }
  arrays->Delete();
  
  if (!this->DataListBox->GetNumberOfValues())
    this->DataListBox->SetValue("Not available");

  if (array)
  {
    this->DataListBox->SetValue(array->GetName());

    double* wholerange = this->MetaDataSet->GetCurrentScalarRange();
    double resolution = (wholerange[1] - wholerange[0])/255.0;
    if (resolution <= 0)
      resolution = 0.5;
    
    this->MeshScalarRange->SetWholeRange (wholerange[0], wholerange[1]);
    this->MeshScalarRange->SetResolution (resolution);
    
    if (array->GetLookupTable())
    {
      double* range = array->GetLookupTable()->GetRange();
      this->MeshScalarRange->SetRange (range[0], range[1]);
    }
    else
      this->MeshScalarRange->SetRange (wholerange[0], wholerange[1]);
    
  }
  else
  {
    this->MeshScalarRange->SetWholeRange (0.0, 255.0);
    this->MeshScalarRange->SetResolution (0.5);
    this->MeshScalarRange->SetRange (0.0, 255.0);
  }
  
}

//----------------------------------------------------------------------------
void vtkKWVisualizationToolBox::ColorMapChangedCallback(const char* value)
{

  // Actually you can easily choose the color array of a metadataset :
  // metadataset->ColorByArray(array);
  // metadataset->ScalarVisibilityOn();
  // and this line is quite important : if it is not here, then
  // each time we switch toolbox, this current function is called and
  // do a mess...

  // so as I am doing a release I uncomment it
  // 
  // I comment these lines cause I need to set the array in view even if the menu doesn't appear...
  // --- for any complain, mail to damien.lepiller@sophia.inria.fr 
  // 
  if (!this->IsMapped())
    return;
  
  std::string name = value;

  if (!this->MetaDataSets.size())
    return;
  
  for (unsigned int i=0; i<this->MetaDataSets.size(); i++)
  {
    vtkMetaDataSet* metadataset = this->MetaDataSets[i];
    if ((metadataset->GetType() != vtkMetaDataSet::VTK_META_SURFACE_MESH) &&
	(metadataset->GetType() != vtkMetaDataSet::VTK_META_VOLUME_MESH))
    continue;
    
    if (!name.size() ||
	(strcmp(name.c_str(), "Solid") == 0) ||
	(strcmp(name.c_str(), "Not available") == 0) )
    {
      metadataset->ScalarVisibilityOff();
      metadataset->ColorByArray(NULL);
      continue;
    }
    
    std::string arrayname = name;
    
    vtkDataArray* array = metadataset->GetArray(arrayname.c_str());
    if (!array)
    {
      std::ostringstream os;
      os <<"Warning : Array "<<value<<" not found in dataset !";
      vtkWarningMacro(<< os.str().c_str() << endl);
      continue;
    }
    
    if (array->GetNumberOfComponents() > 1)
    {
      vtkWarningMacro("Warning : These attributes have more than 1 component !\nNo mapping can be done.");
      continue;
    }
    
    vtkLookupTable* lut = vtkLookupTableManager::GetLookupTable(this->ColorMapId);  
    this->ScalarBar1->SetLookupTable (lut);
    this->ScalarBar2->SetLookupTable (lut);
    this->ScalarBar3->SetLookupTable (lut);
    this->ScalarBar4->SetLookupTable (lut);
    array->SetLookupTable(lut);
    lut->Delete();
    
    metadataset->ColorByArray(array);
    metadataset->ScalarVisibilityOn();
    
    vtkKWPageView* page = this->ParentObject->GetPage(metadataset->GetTag());
    if (page)
    {
      page->GetView1()->AddActor (this->ScalarBar1);
      page->GetView2()->AddActor (this->ScalarBar2);
      page->GetView3()->AddActor (this->ScalarBar3);
      page->GetView4()->AddActor (this->ScalarBar4);
    }
  }

  vtkKWPageView* page = this->ParentObject->GetCurrentPage();
  if (page)
    page->Render();

  vtkDataArray* array = this->MetaDataSet->GetCurrentScalarArray();
  
  if (array)
  {
    this->DataListBox->SetValue(array->GetName());
    
    double* wholerange = this->MetaDataSet->GetCurrentScalarRange();
    double resolution = (wholerange[1] - wholerange[0])/255.0;
    if (resolution <= 0)
      resolution = 0.5;
    
    this->MeshScalarRange->SetWholeRange (wholerange[0], wholerange[1]);
    this->MeshScalarRange->SetResolution (resolution);
    
    if (array->GetLookupTable())
    {
      double* range = array->GetLookupTable()->GetRange();
      this->MeshScalarRange->SetRange (range[0], range[1]);
    }
    else
      this->MeshScalarRange->SetRange (wholerange[0], wholerange[1]);
    
  }
  else
  {
    this->MeshScalarRange->SetWholeRange (0.0, 255.0);
    this->MeshScalarRange->SetResolution (0.5);
    this->MeshScalarRange->SetRange (0.0, 255.0);
  }
  
  

}


void vtkKWVisualizationToolBox::ButtonSetColorMapCallBack()
{

  if (!this->IsMapped())
    return;
  
  int index = this->ButtonSetColorMap->GetMenu()->GetIndexOfSelectedItemInGroup ("LUT");
  if (index == -1)
    return;

  this->ColorMapId = index;
  
  std::string tmp = this->DataListBox->GetValue();
  this->ColorMapChangedCallback(tmp.c_str());
  
}


void vtkKWVisualizationToolBox::ButtonSetLookupTableCallBack()
{

  if (!this->IsMapped())
    return;
  
  if (!this->MetaDataSets.size())
    return;
  
  int index = this->ButtonSetLookupTable->GetMenu()->GetIndexOfSelectedItemInGroup ("LUT");
  if (index == -1)
    return;

  for (unsigned int i=0; i<this->MetaDataSets.size(); i++)
  {
    vtkMetaDataSet* metadataset = this->MetaDataSets[i];
    
    if (metadataset->GetType() != vtkMetaDataSet::VTK_META_IMAGE_DATA)
      continue;
    
    this->LUTId = index;
    
    vtkKWPageView* page = this->ParentObject->GetPage(metadataset->GetTag());
    if (!page)
      continue;
    vtkLookupTable* lut = vtkLookupTableManager::GetLookupTable(this->LUTId);
    page->SetLookupTable(lut);
    lut->Delete();  
  }
}




void vtkKWVisualizationToolBox::ImageRenderingModeCallback (const char* value)
{
  
  if (!this->IsMapped())
    return;
  
  if (!this->MetaDataSets.size())
    return;
  
  for (unsigned int i=0; i<this->MetaDataSets.size(); i++)
  {
    vtkMetaDataSet* metadataset = this->MetaDataSets[i];
    if (metadataset->GetType() != vtkMetaDataSet::VTK_META_IMAGE_DATA)
      continue;
    
    vtkKWPageView* page = this->ParentObject->GetPage (metadataset->GetTag());
    if (!page)
      continue;
    
    if (strcmp (value, "Multi Planar") == 0)
    {
      page->GetView4()->SetRenderingModeToPlanar();
      this->CroppingBoxCheckbox->GetWidget()->SetStateToDisabled();
      this->ShadingCheckbox->GetWidget()->SetStateToDisabled();

    }
    if (strcmp (value, "Volume Rendering") == 0)
    {
//       page->GetView4()->SetVolumeMapperToTexture();
      page->GetView4()->SetRenderingModeToVR();
      page->GetView4()->SetBoxWidgetVisibility (this->CroppingBoxCheckbox->GetWidget()->GetSelectedState());
      this->CroppingBoxCheckbox->GetWidget()->SetStateToNormal();
      this->ShadingCheckbox->GetWidget()->SetStateToNormal();
	      
    }
    if (strcmp (value, "MIP") == 0)
    {
//       page->GetView4()->SetVolumeMapperToRayCast();
      page->GetView4()->SetVolumeRayCastFunctionToMIP();
      page->GetView4()->SetRenderingModeToVR();
    }
  }

  vtkKWPageView* page = this->ParentObject->GetCurrentPage();
  if (page)
    page->Render();

  
}


void vtkKWVisualizationToolBox::CroppingBoxCallback (int val)
{
  
  if (!this->IsMapped())
    return;
  
  if (!this->MetaDataSets.size())
    return;
  
  for (unsigned int i=0; i<this->MetaDataSets.size(); i++)
  {
    vtkMetaDataSet* metadataset = this->MetaDataSets[i];
    if (metadataset->GetType() != vtkMetaDataSet::VTK_META_IMAGE_DATA)
      continue;
    
    vtkKWPageView* page = this->ParentObject->GetPage (metadataset->GetTag());
    if (!page)
      continue;
    
    page->GetView4()->SetBoxWidgetVisibility (val);
  }
  
}


void vtkKWVisualizationToolBox::ImageScalarBarCallback (int val)
{
  
  if (!this->MetaDataSets.size())
    return;
  
  for (unsigned int i=0; i<this->MetaDataSets.size(); i++)
  {
    vtkMetaDataSet* metadataset = this->MetaDataSets[i];
    if (metadataset->GetType() != vtkMetaDataSet::VTK_META_IMAGE_DATA)
      continue;
    
    vtkKWPageView* page = this->ParentObject->GetPage (metadataset->GetTag());
    if (!page)
      continue;
    
    page->SetScalarBarVisibility(val);
  }
  
  vtkKWPageView* cpage = this->ParentObject->GetCurrentPage();
  if (cpage)
    cpage->Render();

}



void vtkKWVisualizationToolBox::ImageWindowLevelRangeCallback (double min, double max)
{
  if (!this->IsMapped())
    return;
  
  if (!this->MetaDataSets.size())
    return;
  
  for (unsigned int i=0; i<this->MetaDataSets.size(); i++)
  {
    vtkMetaDataSet* metadataset = this->MetaDataSets[i];
    if (metadataset->GetType() != vtkMetaDataSet::VTK_META_IMAGE_DATA)
      continue;
    
    vtkKWPageView* page = this->ParentObject->GetPage (metadataset->GetTag());
    if (!page)
      continue;
    
    double window = max-min;
    double level = min + 0.5*window;
    page->GetView1()->SyncSetWindow (window);
    page->GetView1()->SyncSetLevel (level);

  }
  
  vtkKWPageView* cpage = this->ParentObject->GetCurrentPage();
  if (cpage)
    cpage->Render();

  
}

void vtkKWVisualizationToolBox::MeshScalarRangeCallback (double min, double max)
{
  if (!this->IsMapped())
    return;
  
  if (!this->IsMapped())
    return;
  
  if (!this->MetaDataSets.size())
    return;
  
  for (unsigned int i=0; i<this->MetaDataSets.size(); i++)
  {
    vtkMetaDataSet* metadataset = this->MetaDataSets[i];
    if (!metadataset)
      return;
    
    if (metadataset->GetType() == vtkMetaDataSet::VTK_META_IMAGE_DATA)
      continue;
    
    vtkKWPageView* page = this->ParentObject->GetPage (metadataset->GetTag());
    if (!page)
      continue;

    vtkDataArray* array = metadataset->GetCurrentScalarArray();
    if (!array)
      continue;

    vtkLookupTable* lut = array->GetLookupTable();
    if (lut)
      lut->SetRange (min, max);

    metadataset->Modified();
  }
  
  vtkKWPageView* cpage = this->ParentObject->GetCurrentPage();
  if (cpage)
    cpage->Render();
}


void vtkKWVisualizationToolBox::OpacityChangingCallback (double op)
{
  if (!this->IsMapped())
  {
    return;
  }
  if (!this->MetaDataSets.size())
  {
    return;
  }
  
  for (unsigned int i=0; i<this->MetaDataSets.size(); i++)
  {
    vtkMetaDataSet* metadataset = this->MetaDataSets[i];
  
    if (vtkProperty::SafeDownCast(metadataset->GetProperty()))
    {
      vtkProperty::SafeDownCast(metadataset->GetProperty())->SetOpacity (op/100.0);
    }
  }
  

  vtkKWPageView* page = this->ParentObject->GetCurrentPage();
  if (page)
  {
    page->Render();
  }
  
  
  
}


void vtkKWVisualizationToolBox::ColorChangingCallback (double r, double g, double b)
{
  
  if (!this->IsMapped())
    return;
  
  if (!this->MetaDataSets.size())
    return;
  
  for (unsigned int i=0; i<this->MetaDataSets.size(); i++)
  {
    vtkMetaDataSet* metadataset = this->MetaDataSets[i];
    if (vtkProperty::SafeDownCast(metadataset->GetProperty()))
      vtkProperty::SafeDownCast(metadataset->GetProperty())->SetColor (r,g,b);
  }
  
  vtkKWPageView* page = this->ParentObject->GetCurrentPage();
  if (page)
    page->Render();
  

}

void vtkKWVisualizationToolBox::ColorChangedCallback (double r, double g, double b)
{
  if (!this->IsMapped())
    return;
  
  if (!this->MetaDataSet)
    return;
  
  if (vtkProperty::SafeDownCast(this->MetaDataSet->GetProperty()))
    this->S_MaterialPropertyWidget->SetMaterialColor (r,g,b);

  vtkKWPageView* page = this->ParentObject->GetCurrentPage();
  if (page)
    page->Render();

}

void vtkKWVisualizationToolBox::SurfaceRepresentationChangedCallBack( const char* value)
{
  if (!this->IsMapped())
    return;
  
  if (!this->MetaDataSets.size())
    return;
  
  for (unsigned int i=0; i<this->MetaDataSets.size(); i++)
  {
    vtkMetaDataSet* metadataset = this->MetaDataSets[i];
    
    if (!vtkProperty::SafeDownCast(metadataset->GetProperty()))
      continue;
    
    vtkKWPageView* page = this->ParentObject->GetPage (metadataset->GetTag());
    
    if (strcmp (value, "Hidden lines")==0)
    {
      
      metadataset->CreateWirePolyData();
      vtkProperty::SafeDownCast(metadataset->GetProperty())->SetRepresentationToSurface();
      
      if (page)
      {
	page->RemoveMetaDataSet (metadataset);
	page->AddMetaDataSet (metadataset);
      }
    }
    else if (metadataset->GetWirePolyData())
    {
      page->RemoveDataSet (metadataset->GetWirePolyData());
    }
    
    
    if (strcmp (value, "Solid")==0)
      vtkProperty::SafeDownCast(metadataset->GetProperty())->SetRepresentationToSurface();
    else if (strcmp (value, "Wireframe")==0)
      vtkProperty::SafeDownCast(metadataset->GetProperty())->SetRepresentationToWireframe();
    else if (strcmp (value, "Points")==0)
      vtkProperty::SafeDownCast(metadataset->GetProperty())->SetRepresentationToPoints();
  }
  
  vtkKWPageView* page = this->ParentObject->GetCurrentPage();
  if (page)
    page->Render();
}

void vtkKWVisualizationToolBox::InterpolationModeChangedCallBack( const char* value)
{
  if (!this->IsMapped())
    return;
  
  if (!this->MetaDataSets.size())
    return;
  
  for (unsigned int i=0; i<this->MetaDataSets.size(); i++)
  {
    vtkMetaDataSet* metadataset = this->MetaDataSets[i];
    
    if (!vtkProperty::SafeDownCast(metadataset->GetProperty()))
      continue;
    
    if (strcmp (value, "Flat")==0)
      vtkProperty::SafeDownCast(metadataset->GetProperty())->SetInterpolationToFlat();
    else if (strcmp (value, "Gouraud")==0)
      vtkProperty::SafeDownCast(metadataset->GetProperty())->SetInterpolationToGouraud();
  }
  
  vtkKWPageView* page = this->ParentObject->GetCurrentPage();
  if (page)
    page->Render();
}

void vtkKWVisualizationToolBox::LineWidthChangingCallback (double width)
{
  if (!this->IsMapped())
    return;
  
  if (!this->MetaDataSets.size())
    return;
  
  for (unsigned int i=0; i<this->MetaDataSets.size(); i++)
  {
    vtkMetaDataSet* metadataset = this->MetaDataSets[i];
    
    if (vtkProperty::SafeDownCast(metadataset->GetProperty()))
    {
      vtkProperty::SafeDownCast(metadataset->GetProperty())->SetLineWidth (width);
      vtkProperty::SafeDownCast(metadataset->GetProperty())->SetPointSize (width);
    }
  }
  
  
  vtkKWPageView* page = this->ParentObject->GetCurrentPage();
  if (page)
    page->Render();
}



void vtkKWVisualizationToolBox::ScalarbarsVisibilityCallback (int val)
{
  if (!this->IsMapped())
    return;
  
  if (val)
  {
    this->ScalarBar1->VisibilityOn();
    this->ScalarBar2->VisibilityOn();
    this->ScalarBar3->VisibilityOn();
    this->ScalarBar4->VisibilityOn();
  }
  else
  {
    this->ScalarBar1->VisibilityOff();
    this->ScalarBar2->VisibilityOff();
    this->ScalarBar3->VisibilityOff();
    this->ScalarBar4->VisibilityOff();
  }

  vtkKWPageView* page = this->ParentObject->GetCurrentPage();
  if (page)
    page->Render();  
  
}



void vtkKWVisualizationToolBox::ShadingCallback(int val)
{
  if (!this->IsMapped())
    return;
  

  if (!this->MetaDataSet)
    return;
  if (this->MetaDataSet->GetType() != vtkMetaDataSet::VTK_META_IMAGE_DATA)
    return;

  vtkKWPageView* page = this->ParentObject->GetPage(this->MetaDataSet->GetTag());
  if (!page)
    return;

  if (val)
    page->GetView4()->ShadeOn();
  else
    page->GetView4()->ShadeOff();

  page->GetView4()->Render();
  
  

}



void vtkKWVisualizationToolBox::StanleyFunction (void)
{

  if (!this->IsMapped())
    return;
  
  if (!this->MetaDataSet)
    return;

  vtkPolyData* polydata = vtkPolyData::SafeDownCast (this->MetaDataSet->GetDataSet());
  
  if (!polydata)
    return;

  vtkKWPageView* page = this->ParentObject->GetPage (this->MetaDataSet->GetTag());
  if (!page)
    return;
  vtkViewImage3D* view = page->GetView4();

  for (unsigned int i = 0; i < this->MetaDataSet->GetNumberOfActors(); i++)
  {
    view->RemoveActor (this->MetaDataSet->GetActor (i));
  }
  this->MetaDataSet->RemoveAllActors();
  
  vtkPoints* points = polydata->GetPoints();
  vtkDataArray* normals = polydata->GetPointData()->GetNormals();
  vtkPolyDataNormals* normalextractor = vtkPolyDataNormals::New();
  if (!normals)
  {
    normalextractor->SetInput (polydata);
    normalextractor->ComputePointNormalsOn();
    normalextractor->SplittingOff();
    normalextractor->ConsistencyOff();
    normalextractor->Update();
    normals = normalextractor->GetOutput()->GetPointData()->GetNormals();
  }
  
  polydata->GetPolys()->Reset();


  for (int i=0; i<points->GetNumberOfPoints(); i++)
  {
    double* position = points->GetPoint (i);
    vtkDiskSource* disk = vtkDiskSource::New();
    double* normal = normals->GetTuple3 (i);
    double radius = std::sqrt (normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
    disk->SetInnerRadius (0.0);
    disk->SetOuterRadius (radius);
    vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
    mapper->SetInput (disk->GetOutput());
    
    vtkActor* actor = vtkActor::New();
    actor->SetMapper (mapper);
    actor->SetPosition (position);
    view->AddActor (actor);

    this->MetaDataSet->AddActor (actor);

    actor->Delete();
    mapper->Delete();
    disk->Delete();
  }

  normalextractor->Delete();
  
}
