/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWInfoToolBox.cxx 928 2008-09-04 16:43:57Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-09-04 17:43:57 +0100 (Thu, 04 Sep 2008) $
Version:   $Revision: 928 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <vtkKWInfoToolBox.h>
#include "vtkObjectFactory.h"

#include <vtkKWHSVColorSelector.h>

#include <vtkLookupTableManager.h>
#include <vtkMetaDataSet.h>
#include <vtkMetaImageData.h>

#include <vtkPointSet.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkDataArray.h>

#include <vtkKWPushButton.h>
// #include <vtkKWListBox.h>
#include <vtkKWComboBox.h>
#include <vtkKWLoadSaveDialog.h>
#include <vtkStringArray.h>
#include <vtkKWIcon.h>

#include <vtkKWMessageDialog.h>
#include <vtkDataArrayCollection.h>

#include <vtkVectorVisuManager.h>
#include <vtkFibersManager.h>
#include <vtkTensorVisuManager.h>
#include <vtksys/SystemTools.hxx>
#include <vtkErrorCode.h>

#include <vtkImageView3D.h>
#include <vtkImageView2D.h>
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

#include <vtkScalarBarActor.h>
#include <vtkKWMessageDialog.h>
#include <pixmap/KWAddOnResources.h>

#include <vtkKWSimpleEntryDialog.h>
#include <vtkKWEntryWithLabel.h>
#include <vtkKWEntry.h>

//#include <vtkKWColorPresetSelector.h>
#include <vtkKWMenuButton.h>
#include <vtkKWApplication.h>

#include <vtkKWComboBoxSet.h>
#include <vtkKWScaleSet.h>
#include <vtkKWComboBox.h>
#include <vtkKWScale.h>

#include <vtkMath.h>
#include <vtkKWSurfaceMaterialPropertyWidget.h>
#include <vtkKWPopupButtonWithLabel.h>

#include <vtkKWEntryWithLabel.h>
#include <vtkKWEntry.h>
#include <vtkKWPushButton.h>

#include <kwcommon.h>
#include <vtkKWDicomInfoWidget.h>
#include <vtkKWTkUtilities.h>
#include <vtkErrorCode.h>
#include <vtkKWLabel.h>
#include <vtkKWMenuButton.h>
#include <vtkKWMenu.h>
#include <vtkLookupTableManager.h>

#include "vtkKWHistogram.h"
#include "vtkKWLabel.h"
#include "vtkKWMessage.h"
#include <vtksys/SystemTools.hxx>
#include "vtkKWPiecewiseFunctionEditor.h"
#include "vtkKWPiecewiseFunctionEditorSet.h"

#include <vtkImageData.h>
#include <vtkDataSet.h>

#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkKWLabel.h"
#include "vtkKWInternationalization.h"
#include <vtkKWProgressGauge.h>

#include <vtkKWDICOMImporter2.h>

#include <vtkKWSequenceEditorWidget.h>
#include <vtkLookupTableManager.h>

#include "assert.h"
#include <algorithm>

int vtkkwrint(double a)
{  
#ifdef WIN32
  double test = fabs ( a - double(int(a)) );
  int res = 0;
  if(a>0)
    (test>0.5)?(res=a+1):res=a;  
  else
    (test>0.5)?(res=a-1):res=a;

  return res;   
#else
  return (int)rint(a);
#endif // WIN32
}

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWInfoToolBox );
vtkCxxRevisionMacro( vtkKWInfoToolBox, "$Revision: 928 $");

//----------------------------------------------------------------------------
vtkKWInfoToolBox::vtkKWInfoToolBox()
{
  this->ButtonRequestTags = vtkKWPushButton::New();
  this->ButtonSave = vtkKWPushButton::New();
  this->ButtonDelete = vtkKWPushButton::New();
  this->EntryName = vtkKWEntryWithLabel::New();

  this->LabelType = vtkKWLabel::New();
  this->LabelSize = vtkKWLabel::New();
  this->ListBoxTab = vtkKWComboBox::New();
  this->ButtonLUT = vtkKWMenuButton::New();
  this->ButtonLoad = vtkKWPushButton::New();
  this->ButtonSaveAttribute = vtkKWPushButton::New();
  this->ListBoxAttributes = vtkKWComboBox::New();
  
  
  this->Icon->SetImage(image_info, image_info_width, image_info_height, image_info_pixel_size, image_info_length);
  this->SetName("Data Information");
 
}

//----------------------------------------------------------------------------
vtkKWInfoToolBox::~vtkKWInfoToolBox()
{
  this->ButtonRequestTags->Delete();
  this->ButtonSave->Delete();
  this->ButtonDelete->Delete();
  this->EntryName->Delete();
  this->LabelType->Delete();
  this->LabelSize->Delete();
  this->ListBoxTab->Delete();
  this->ButtonLUT->Delete();
  this->ButtonLoad->Delete();
  this->ButtonSaveAttribute->Delete();
  this->ListBoxAttributes->Delete();
  
  
}


//----------------------------------------------------------------------------
void vtkKWInfoToolBox::CreateWidget()
{
  // Check if already created
  if (this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }
  
  // Call the superclass to create the whole widget
  this->Superclass::CreateWidget();


  this->EntryName->SetParent (this);
  this->EntryName->Create();
  this->EntryName->SetLabelText("Name :");
  this->EntryName->GetWidget()->SetWidth(20);
  this->EntryName->GetWidget()->SetValue("name...");
  this->EntryName->GetWidget()->SetCommand (this, "EntryNameCallback");
  this->EntryName->GetWidget()->SetBackgroundColor(1,1,1);
  this->EntryName->GetWidget()->SetCommandTrigger(vtkKWEntry::TriggerOnReturnKey);
  this->EntryName->GetWidget()->ReadOnlyOff();
  
  this->ButtonRequestTags->SetParent (this);
  this->ButtonRequestTags->Create();
  this->ButtonRequestTags->SetImageToPixels(image_cd, image_cd_width, image_cd_height, image_cd_pixel_size, image_cd_length); 
  this->ButtonRequestTags->SetCommand(this, "ButtonRequestTagsCallback");
  this->ButtonRequestTags->SetBalloonHelpString("Show available DICOM information");

  this->ButtonSave->SetParent (this);
  this->ButtonSave->Create();
  this->ButtonSave->SetImageToPixels(image_saveas, image_saveas_width, image_saveas_height, image_saveas_pixel_size, image_saveas_length); 
  this->ButtonSave->SetCommand(this, "ButtonSaveCallback");
  this->ButtonSave->SetBalloonHelpString("Save this dataset as...");

  this->ButtonDelete->SetParent (this);
  this->ButtonDelete->Create();
  this->ButtonDelete->SetImageToPixels(image_delete_small, image_delete_small_width, image_delete_small_height, image_delete_small_pixel_size, image_delete_small_length); 
  this->ButtonDelete->SetCommand(this, "ButtonDeleteCallback");
  this->ButtonDelete->SetBalloonHelpString("Remove this dataset from the manager...");

  this->LabelType->SetParent (this);
  this->LabelType->Create();
  this->LabelType->SetText("Type :");

  this->LabelSize->SetParent (this);
  this->LabelSize->Create();
  this->LabelSize->SetText("Size :");

  this->ButtonLoad->SetParent (this);
  this->ButtonLoad->Create();
  this->ButtonLoad->SetImageToPixels(image_attributesopen, image_attributesopen_width, image_attributesopen_height, image_attributesopen_pixel_size, image_attributesopen_length); 
  this->ButtonLoad->SetCommand(this, "ButtonLoadCallback");
  this->ButtonLoad->SetBalloonHelpString("Load attibute for this dataset ...");
  
  this->ButtonSaveAttribute->SetParent (this);
  this->ButtonSaveAttribute->Create();
  this->ButtonSaveAttribute->SetImageToPixels(image_saveas, image_saveas_width, image_saveas_height, image_saveas_pixel_size, image_saveas_length); 
  this->ButtonSaveAttribute->SetCommand(this, "ButtonSaveAttributeCallback");
  this->ButtonSaveAttribute->SetBalloonHelpString("Save this attribute to file ...");

  this->ListBoxTab->SetParent (this);
  this->ListBoxTab->Create();
  this->ListBoxTab->SetWidth (20);
  this->ListBoxTab->SetBackgroundColor (1,1,1);  
  this->ListBoxTab->ReadOnlyOn();
  this->ListBoxTab->SetValue("No Tab");
  this->ListBoxTab->SetCommand(this, "ListBoxTabCallback");
  this->ListBoxTab->SetBalloonHelpString("Select the view port you want the dataset to be displayed");

  
  this->ListBoxAttributes->SetParent (this);
  this->ListBoxAttributes->Create();
  this->ListBoxAttributes->SetWidth (20);
  this->ListBoxAttributes->SetBackgroundColor (1,1,1);
  this->ListBoxAttributes->ReadOnlyOn();
  this->ListBoxAttributes->SetValue("No Attribute");
  this->ListBoxAttributes->SetCommand(this, "ListBoxAttributesCallback");
  this->ListBoxAttributes->SetBalloonHelpString("List of the available dataset attributes ");


  std::vector<std::string> LUTList = vtkLookupTableManager::GetAvailableLookupTables();
  
  this->ButtonLUT->SetParent (this);
  this->ButtonLUT->Create();
  this->ButtonLUT->IndicatorVisibilityOff();
  this->ButtonLUT->SetImageToPixels(image_lookuptable, image_lookuptable_width, image_lookuptable_height, image_lookuptable_pixel_size, image_lookuptable_length);
  this->ButtonLUT->SetBalloonHelpString("Set the color map to use for this attribute ...");

  for (unsigned int i=0; i<LUTList.size(); i++)
  {
    this->ButtonLUT->GetMenu()->AddRadioButton(LUTList[i].c_str());
  }
  



    // Update according to the current metadataset
  this->Update();

  // Pack
  this->Pack();


}



//----------------------------------------------------------------------------
void vtkKWInfoToolBox::Pack()
{

  if (this->EntryName->IsCreated())
  {
    this->Script( "grid %s -sticky nw -column 0 -row 0 -columnspan 5 -padx 5 -pady 5", 
		  this->EntryName->GetWidgetName());
  }
  if (this->ButtonSave->IsCreated())
  {
    this->Script( "grid %s -sticky ne -column 5 -row 0 -padx 5 -pady 5", 
		  this->ButtonSave->GetWidgetName());
  }

  if (this->LabelType->IsCreated())
  {
    this->Script( "grid %s -sticky nw -column 0 -row 1 -columnspan 5 -padx 5 -pady 5", 
		  this->LabelType->GetWidgetName());
  }
  if (this->ButtonDelete->IsCreated())
  {
    this->Script( "grid %s -sticky ne -column 5 -row 1 -padx 5 -pady 5", 
		  this->ButtonDelete->GetWidgetName());
  }

  if (this->LabelSize->IsCreated())
  {
    this->Script( "grid %s -sticky nw -column 0 -row 2 -columnspan 5 -padx 5 -pady 5", 
		  this->LabelSize->GetWidgetName());
  }
  if (this->ButtonRequestTags->IsCreated())
  {
    this->Script( "grid %s -sticky ne -column 5 -row 2 -padx 5 -pady 5", 
		  this->ButtonRequestTags->GetWidgetName());
  }

  vtkKWLabel* l1 = vtkKWLabel::New();
  l1->SetParent(this);
  l1->Create();
  l1->SetText ("View Port : ");

  if (l1->IsCreated())
  {
    this->Script( "grid %s -sticky nw -column 0 -row 3 -padx 2 -pady 2", 
		  l1->GetWidgetName());
  }

  
  if (this->ListBoxTab->IsCreated())
  {
    this->Script( "grid %s -sticky nw -column 0 -row 4 -columnspan 4 -padx 5 -pady 5", 
		  this->ListBoxTab->GetWidgetName());
  }

  vtkKWLabel* l2 = vtkKWLabel::New();
  l2->SetParent(this);
  l2->Create();
  l2->SetText ("Data Attributes : ");

  if (l2->IsCreated())
  {
    this->Script( "grid %s -sticky nw -column 0 -row 5 -padx 2 -pady 2", 
		  l2->GetWidgetName());
  }

  l1->Delete();
  l2->Delete();
  
  
  if (this->ListBoxAttributes->IsCreated())
  {
    this->Script( "grid %s -sticky nw -column 0 -row 6 -columnspan 4 -padx 5 -pady 5", 
		  this->ListBoxAttributes->GetWidgetName());
  }
  if (this->ButtonLoad->IsCreated())
  {
    this->Script( "grid %s -sticky nw -column 4 -row 6 -padx 5 -pady 5", 
		  this->ButtonLoad->GetWidgetName());
  }
//   if (this->ButtonLUT->IsCreated())
//   {
//     this->Script( "grid %s -sticky nw -column 4 -row 4 -padx 5 -pady 5", 
// 		  this->ButtonLUT->GetWidgetName());
//   }
  if (this->ButtonSaveAttribute->IsCreated())
  {
    this->Script( "grid %s -sticky nw -column 5 -row 6 -padx 5 -pady 5", 
		  this->ButtonSaveAttribute->GetWidgetName());
  }
  
  this->Script("grid columnconfigure %s 0 -weight 1", this->GetWidgetName());
  this->Script("grid columnconfigure %s 1 -weight 1", this->GetWidgetName());
  this->Script("grid columnconfigure %s 2 -weight 1", this->GetWidgetName());
  this->Script("grid columnconfigure %s 3 -weight 1", this->GetWidgetName());
  this->Script("grid columnconfigure %s 4 -weight 1", this->GetWidgetName());
  this->Script("grid columnconfigure %s 5 -weight 1", this->GetWidgetName());
  


}


//----------------------------------------------------------------------------
void vtkKWInfoToolBox::Update()
{
  this->Superclass::Update();

  if (!this->MetaDataSet)
  {
    this->EntryName->GetWidget()->SetValue("name...");
    this->LabelType->SetText ("Type : ");
    this->LabelSize->SetText ("Size : ");

    this->ListBoxTab->DeleteAllValues ();
    this->ListBoxTab->SetValue("No Tab");
    this->ListBoxAttributes->DeleteAllValues();
    this->ListBoxAttributes->SetValue ("No attribute");
    
    return;
  }
  
  std::string name = this->MetaDataSet->GetName();    
  this->EntryName->GetWidget()->SetValue(name.c_str());
  
  std::string type = "Type : ";
  type+=this->MetaDataSet->GetDataSetType();
  this->LabelType->SetText (type.c_str());

  std::ostringstream size;
  size<<"Size : ";
  
  if (this->MetaDataSet->GetDataSet())
  {
    double bounds[6];
    this->MetaDataSet->GetDataSet()->GetBounds(bounds);
    double dim[3];
    int rdim[3];
    dim[0] = bounds[1] - bounds[0];
    dim[1] = bounds[3] - bounds[2];
    dim[2] = bounds[5] - bounds[4];
    rdim[0] = vtkkwrint (dim[0]);
    rdim[1] = vtkkwrint (dim[1]);
    rdim[2] = vtkkwrint (dim[2]);
    
    
    
    size<<rdim[0]<<" x "<<rdim[1]<<" x "<<rdim[2]<<" mm.";
  }
  this->LabelSize->SetText (size.str().c_str());


  this->ListBoxTab->DeleteAllValues ();

  if (this->MetaDataSet->GetType() != vtkMetaDataSet::VTK_META_IMAGE_DATA)
  {

    vtkDataManager* manager = this->GetParentObject()->GetDataManager();
    if (!manager)
      return;

    std::vector<vtkKWPageView*> pagelist = this->GetParentObject()->GetPageList();
    
    for (unsigned int i=0; i<pagelist.size(); i++)
    {
      this->ListBoxTab->AddValue (pagelist[i]->GetTag());
    }
  }
  
  if (this->GetParentObject()->GetPage(this->MetaDataSet->GetTag()))
    this->ListBoxTab->SetValue(this->MetaDataSet->GetTag());
  else
    this->ListBoxTab->SetValue("No Tab");
  

  this->ListBoxAttributes->DeleteAllValues ();

  if (this->MetaDataSet->GetType() == vtkMetaDataSet::VTK_META_IMAGE_DATA)
  {
    this->ListBoxAttributes->SetValue ("Scalars");
  }
  else
  {
    
    vtkDataArrayCollection* arrays = vtkDataArrayCollection::New();
    this->MetaDataSet->GetColorArrayCollection (arrays);
    int i=0;
    for (i=0; i<arrays->GetNumberOfItems(); i++)
    {
      if (!arrays->GetItem(i)->GetName() || !(*arrays->GetItem(i)->GetName()))
	continue;
      
      std::string name = arrays->GetItem(i)->GetName();
      
      std::ostringstream os;
      os<<name;
      
      this->ListBoxAttributes->AddValue (os.str().c_str());
      
      if (i == 0)
	this->ListBoxAttributes->SetValue (os.str().c_str());
    }
    arrays->Delete();

    
    vtkDataArrayCollection* arrays2 = this->MetaDataSet->GetArrayCollection ();
    
    i=0;
    for (i=0; i<arrays2->GetNumberOfItems(); i++)
    {
      if (!arrays2->GetItem(i)->GetName() || !(*arrays2->GetItem(i)->GetName()))
	continue;
      
      std::string name = arrays2->GetItem(i)->GetName();
      
      std::ostringstream os;
      os<<name;
      
      this->ListBoxAttributes->AddValue (os.str().c_str());
      
      if (i == 0)
	this->ListBoxAttributes->SetValue (os.str().c_str());
    }
  }
  
  if (!this->ListBoxAttributes->GetNumberOfValues())
    this->ListBoxAttributes->SetValue ("No attribute");

  
}



//----------------------------------------------------------------------------
void vtkKWInfoToolBox::ButtonRequestTagsCallback()
{

  if (!this->GetMetaDataSet() || (this->GetMetaDataSet()->GetType() != vtkMetaDataSet::VTK_META_IMAGE_DATA))
    return;

  vtkMetaImageData* imagedata = vtkMetaImageData::SafeDownCast (this->GetMetaDataSet());
  if (!imagedata)
  {
    vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::SafeDownCast (this->GetMetaDataSet());
    if (sequence)
      imagedata = vtkMetaImageData::SafeDownCast (sequence->GetMetaDataSet (0));
  }
  if (!imagedata)
    return;
  
  vtkKWTopLevel* toplevel = vtkKWTopLevel::New();
  
  toplevel->SetMasterWindow(this);
  toplevel->SetApplication(this->GetApplication());
  toplevel->Create();
  toplevel->SetTitle(this->MetaDataSet->GetName());
  toplevel->SetDeleteWindowProtocolCommand(toplevel, "Withdraw");

  vtkKWDicomInfoWidget* widget = vtkKWDicomInfoWidget::New();
  
  widget->SetParent(toplevel);
  widget->Create();
  
  this->Script("pack %s -fill both -side top -expand t", 
  	       widget->GetWidgetName());

  widget->SetDicomEntryList(imagedata->GetDicomEntryList());
  
  // Get the position of the mouse, the size of the top level window.

  int px, py, tw, th, sw, sh;

  vtkKWTkUtilities::GetMousePointerCoordinates(this, &px, &py);
  vtkKWTkUtilities::GetWidgetRequestedSize(toplevel, &tw, &th);
  vtkKWTkUtilities::GetScreenSize(toplevel, &sw, &sh);

  px -= tw / 2;
  if (px + tw > sw)
    {
    px -= (px + tw - sw);
    }
  if (px < 0)
    {
    px = 0;
    }

  py -= th / 2;
  if (py + th > sh)
    {
    py -= (py + th - sh);
    }
  if (py < 0)
    {
    py = 0;
    }

  toplevel->SetPosition(px, py);
  toplevel->DeIconify();
  toplevel->Raise();

  widget->Delete();
 
}

//----------------------------------------------------------------------------
void vtkKWInfoToolBox::ButtonSaveCallback()
{

  

  if (!this->MetaDataSet)
    return;

  this->ParentObject->OnDataSetSave();
  
  
//   vtkKWLoadSaveDialog *dialog = vtkKWLoadSaveDialog::New() ;
  
//   dialog->SetParent(this);
//   dialog->Create();
//   dialog->MultipleSelectionOff();
//   if ( (vtkMetaDataSetSequence::SafeDownCast(this->MetaDataSet) ) && (this->MetaDataSet->GetType() != vtkMetaDataSet::VTK_META_IMAGE_DATA))
//     dialog->ChooseDirectoryOn();
//   dialog->SaveDialogOn ();
//   dialog->RetrieveLastPathFromRegistry("DataPath");
//   std::ostringstream os;
//   os << "Save dataset "<<this->MetaDataSet->GetName()<<" to file...";
  
//   dialog->SetTitle (os.str().c_str());

//   int val = this->MetaDataSet->GetType();
//   std::string ext;
  
//   switch (val)
//   {
//       case vtkMetaDataSet::VTK_META_IMAGE_DATA :
// 	ext = kwsupportedimageformats_global + kwsupportedimageformats;
// 	break;
	
//       case vtkMetaDataSet::VTK_META_SURFACE_MESH :
//       case vtkMetaDataSet::VTK_META_VOLUME_MESH :
// 	ext = kwsupportedmeshformats_global + kwsupportedmeshformats;
// 	break;
//       default :
// 	ext = kwsupportedformats;
// 	break;
//   }

//   dialog->SetFileTypes (ext.c_str());
  
//   if ( dialog->Invoke () == 0 )
//   {
//     dialog->Delete();
//     return;
//   }
  
//   const char* filename = dialog->GetFileName();
//   try
//   {
//     this->MetaDataSet->Write (filename);
//   }
//   catch (vtkErrorCode::ErrorIds)
//   {
//     std::cout<<"cannot save file "<<dialog->GetFileName()<<std::endl;
//   }

//   dialog->SaveLastPathToRegistry("DataPath");
//   dialog->Delete();
 
}


//----------------------------------------------------------------------------
void vtkKWInfoToolBox::EntryNameCallback(const char* name)
{
  if (!this->IsMapped())
    return;
  
  if (this->MetaDataSet)
    this->MetaDataSet->SetName(name);
  this->ParentObject->Update();
  

}



//----------------------------------------------------------------------------
void vtkKWInfoToolBox::ButtonDeleteCallback()
{
  
  if (!this->MetaDataSet)
    return;
  
  vtkKWMessageDialog* dialog = vtkKWMessageDialog::New();
  dialog->SetApplication (this->GetApplication());
  dialog->SetStyleToYesNo ();
  dialog->Create();
  dialog->SetTitle("Warning !");
  
  std::ostringstream os;
  os << "Are you sure you want to remove ";
  os<<this->MetaDataSet->GetName()<<std::endl;

  os<<"from the manager ?";
  
  dialog->SetText(os.str().c_str());
  
  if ( dialog->Invoke () != 1)
  {
    dialog->Delete();
    return;
  }

  dialog->Delete();

  this->GetParentObject()->RemoveMetaDataSet (this->MetaDataSet);
  this->GetParentObject()->Update();
  this->GetParentObject()->Render();  
  
}




//----------------------------------------------------------------------------
void vtkKWInfoToolBox::ListBoxTabCallback(const char* tab)
{


  
  
  if (!this->MetaDataSet || this->MetaDataSet->GetType() == vtkMetaDataSet::VTK_META_IMAGE_DATA)
    return;


  if ( (strcmp(this->MetaDataSet->GetTag(), tab) == 0) || !this->GetParentObject()->GetPage(tab) )
    return;

  vtkKWPageView* oldpage = this->GetParentObject()->GetPage(this->MetaDataSet->GetTag());
  vtkKWPageView* newpage = this->GetParentObject()->GetPage(tab);
    
  if (oldpage)
  {
    oldpage->RemoveMetaDataSet(this->MetaDataSet);
  }
  

  newpage->AddMetaDataSet (this->MetaDataSet);
  
  this->MetaDataSet->SetTag (tab); 
  this->GetParentObject()->RaisePage(tab);
  vtkDataArray* array = this->MetaDataSet->GetCurrentScalarArray();
  this->MetaDataSet->ColorByArray (array);
  newpage->Render();
  
}




//----------------------------------------------------------------------------
void vtkKWInfoToolBox::ListBoxAttributesCallback(const char* attribute)
{

}

  

//----------------------------------------------------------------------------
void vtkKWInfoToolBox::ButtonLoadCallback()
{


  if (!this->MetaDataSet)
    return;

//   if (this->MetaDataSet->GetType() == vtkMetaDataSet::VTK_META_IMAGE_DATA)
//   {
//     vtkKWPopupWarningMessage (this, "Images cannot have other attributes \nthan their scalars !");
//     return;
//   }
  
  vtkKWLoadSaveDialog *dialog = vtkKWLoadSaveDialog::New() ;

  dialog->SetParent(this);
  dialog->Create();
  dialog->MultipleSelectionOn();
  dialog->RetrieveLastPathFromRegistry("DataPath");
  dialog->SetTitle ("Open attributes file(s)...");
  dialog->SetFileTypes (kwsupportedattributesformats.c_str());
  
  if ( (dialog->Invoke () == 0) || (!dialog->GetNumberOfFileNames()))
  {
    dialog->Delete();
    return ;
  }

  vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::SafeDownCast (this->MetaDataSet);
  const char* firstfile = dialog->GetFileNames ()->GetValue (0);
  std::string ext = vtksys::SystemTools::GetFilenameLastExtension (firstfile);


  

  vtkKWWindowBase *win = this->GetApplication()->GetNthWindow(0);

  if (!win)
    return;
  
  win->SetStatusText(ks_("Progress|Opening files..."));
  win->GetProgressGauge()->SetValue(0);
  

  bool all_ok = true;
  
  
  for (int i=0; i<dialog->GetNumberOfFileNames(); i++)
  {
    win->GetProgressGauge()->SetValue((int)(100.0 * i / dialog->GetNumberOfFileNames()));
    
    
    const char* filename = dialog->GetFileNames ()->GetValue (i);

    try
    {
      if (sequence && (strcmp (ext.c_str(), ".scl") == 0) )
      {
	if (i == 0)
	  sequence->ReadData (filename);
	sequence->ReadFrameData (filename, i);
      }
      else
      {
	this->MetaDataSet->ReadData (filename);
      }
      
    }
    catch (vtkErrorCode::ErrorIds)
    {
      // we don't throw exception as we want to continue scanning the directory
      all_ok = false;
      
      vtkKWMessageDialog::PopupMessage(this->GetApplication(),
				       this->GetParentTopLevel(),
				       "Error",
				       "Cannot open file !\nPlease check the log window for details.",
				       vtkKWMessageDialog::ErrorIcon);
      continue;  
    }
  }

  
  vtksys_stl::string end_msg(win->GetStatusText());
  end_msg += " -- ";
  end_msg += ks_("Progress|Done");  
  win->SetStatusText(end_msg.c_str());
  win->GetProgressGauge()->SetValue(0);
  
  if (!all_ok)
  {
    vtkKWMessageDialog::PopupMessage(this->GetApplication(),
				     this->GetParentTopLevel(),
				     "Error",
				     "One or more file(s) could not be opened !\nPlease check the log window for details.",
				     vtkKWMessageDialog::ErrorIcon);
  }

  dialog->SaveLastPathToRegistry("DataPath");
  dialog->Delete();

  vtkDataArrayCollection* collection = vtkDataArrayCollection::New();
  this->MetaDataSet->GetColorArrayCollection (collection);
  unsigned int N_arrays = collection->GetNumberOfItems();
  if (N_arrays)
  {
    vtkDataArray* array = vtkDataArray::SafeDownCast (collection->GetItemAsObject (N_arrays-1));
    if (array && (array->GetNumberOfComponents() == 1))
    {
      vtkLookupTable* lut = vtkLookupTableManager::GetLookupTable(vtkLookupTableManager::LUT_LONI);
      array->SetLookupTable (lut);
      this->MetaDataSet->ColorByArray (array);
      this->MetaDataSet->ScalarVisibilityOn();
      lut->Delete();    
    }
  }
  
  collection->Delete();
  
  this->Update();

  vtkKWPageView* page = this->ParentObject->GetCurrentPage();
  if (page)
    page->Render();
  

}




//----------------------------------------------------------------------------
void vtkKWInfoToolBox::ButtonLUTCallback(const char* LUT)
{


  if (!this->MetaDataSet)
    return;
}




// //----------------------------------------------------------------------------
// void vtkKWInfoToolBox::ButtonStatsCallback()
// {


//   if (!this->MetaDataSet)
//     return;

//   vtkKWHistogram* histogram = vtkKWHistogram::New();
  
//   std::string name = this->ListBoxAttributes->GetValue();
  
//   histogram->SetMaximumNumberOfBins (100);
      
//   if (this->MetaDataSet->GetType() == vtkMetaDataSet::VTK_META_IMAGE_DATA)
//   {
//     name = "Scalars";
    
//     if (this->MetaDataSet->GetDataSet()->GetPointData()->GetScalars())
//     {
//       histogram->BuildHistogram(this->MetaDataSet->GetDataSet()->GetPointData()->GetScalars(), 0);
//     }
//     else
//     {
//       histogram->Delete();
//       return;
//     }
    
      
//   }
//   else
//   {
//     vtkDataArray* array = this->MetaDataSet->GetArray (name.c_str());
//     if (array)
//     { 
//       histogram->BuildHistogram(array, 0);
//     }
//     else
//     {
//       histogram->Delete();
//       return;
//     }
//   }


  
  
//   vtkKWTopLevel* toplevel = vtkKWTopLevel::New();
  
//   toplevel->SetMasterWindow(this);
//   toplevel->SetApplication(this->GetApplication());
//   toplevel->Create();
//   toplevel->SetTitle(this->MetaDataSet->GetName());
//   toplevel->SetDeleteWindowProtocolCommand(toplevel, "Withdraw");

  
      
//   vtkKWPiecewiseFunctionEditor* histo = vtkKWPiecewiseFunctionEditor::New();
//   histo->SetParent(toplevel);
//   histo->Create();

//   this->Script("pack %s -fill both -side top -padx 10 -pady 10", 
// 	       histo->GetWidgetName());
  
  
//   histo->SetHistogram(histogram);
  
//   histo->DisplayHistogramOnly();
//   histo->ParameterTicksVisibilityOn();
//   histo->ValueTicksVisibilityOn();
//   histo->ComputeValueTicksFromHistogramOn();
//   histo->SetParameterTicksFormat("%-#6.3f");
//   histo->SetParameterEntryFormat("%-#6.3f");
//   histo->SetMidPointGuidelineValueFormat("%-#6.3f");
//   histo->SetValueTicksFormat("%-#6.0f");
//   histo->GetLabel()->SetImageToPredefinedIcon(vtkKWIcon::IconInfoMini);
//   histo->GetLabel()->SetText(name.c_str());
//   histo->SetBalloonHelpString(name.c_str());
//   histo->ParameterRangeVisibilityOn();
//   histo->SetCanvasWidth(450);
//   histo->SetCanvasHeight(150);
//   histo->LabelVisibilityOn();
//   histo->SetHistogramColor(0.18, 0.24, 0.49);
//   histo->SetHistogramStyleToBars();
//   histo->ParameterRangeLabelVisibilityOn();
//   histo->ParameterRangeVisibilityOn();
//   histo->SetFrameBackgroundColor(0.92, 1.0, 0.92);

  
//   // Get the position of the mouse, the size of the top level window.

//   int px, py, tw, th, sw, sh;

//   vtkKWTkUtilities::GetMousePointerCoordinates(this, &px, &py);
//   vtkKWTkUtilities::GetWidgetRequestedSize(toplevel, &tw, &th);
//   vtkKWTkUtilities::GetScreenSize(toplevel, &sw, &sh);

//   px -= tw / 2;
//   if (px + tw > sw)
//     {
//     px -= (px + tw - sw);
//     }
//   if (px < 0)
//     {
//     px = 0;
//     }

//   py -= th / 2;
//   if (py + th > sh)
//     {
//     py -= (py + th - sh);
//     }
//   if (py < 0)
//     {
//     py = 0;
//     }

//   toplevel->SetPosition(px, py);
//   toplevel->DeIconify();
//   toplevel->Raise();

  
//   histogram->Delete();
//   histo->Delete();
  
// }




//----------------------------------------------------------------------------
void vtkKWInfoToolBox::ButtonSaveAttributeCallback()
{
  

  if (!this->IsMapped() || !this->MetaDataSet)
    return;

  std::string name = this->ListBoxAttributes->GetValue();
  vtkDataArray* array = this->MetaDataSet->GetArray (name.c_str());
  if (!array)
    return;

  vtkKWLoadSaveDialog *dialog = vtkKWLoadSaveDialog::New() ;
  
  dialog->SetParent(this);
  dialog->Create();
  dialog->MultipleSelectionOff();

  bool do_directory = true;
  
  if (vtkMetaDataSetSequence::SafeDownCast(this->MetaDataSet))
  {
    vtkKWMessageDialog* dialog2 = vtkKWMessageDialog::New();
    dialog2->SetApplication (this->GetApplication());
    dialog2->SetStyleToYesNo ();
    dialog2->Create();
    dialog2->SetTitle("Info !");
    
    std::ostringstream os;
    os << "Dataset ";
    os<<this->MetaDataSet->GetName()<<std::endl;
    
    os<<"is sequence."<<std::endl;
    os<<"Do you want to save attribute "<<name.c_str()<<"in several files ?"<<std::endl;
      
    dialog2->SetText(os.str().c_str());
      
    if ( dialog2->Invoke () == 1)
    {
      do_directory = true;
    }
    else
      do_directory = false;
    
    dialog2->Delete();
  }
  else
    do_directory = false;
  
  if (do_directory)
    dialog->ChooseDirectoryOn();
  else
    dialog->ChooseDirectoryOff();

  if (do_directory)
  {
    vtkKWPopupWarningMessage(this, "not supported yet !");
    dialog->ChooseDirectoryOff();
  }
  dialog->SaveDialogOn ();
  dialog->RetrieveLastPathFromRegistry("DataPath");
  std::ostringstream os2;
  os2 << "Save ";
  os2<<name.c_str();
  
  os2<<" to file...";
  
  dialog->SetTitle (os2.str().c_str());

  std::string ext = kwsupportedattributesformats;
  
  dialog->SetFileTypes (ext.c_str());
  
  if ( dialog->Invoke () == 0 )
  {
    dialog->Delete();
    return ;
  }
  
  try
  {
    this->MetaDataSet->WriteData (dialog->GetFileName(), name.c_str());
  }
  catch (vtkErrorCode::ErrorIds)
  {
    std::cerr<<"cannot save file "<<dialog->GetFileName()<<std::endl;
    vtkErrorMacro(<<"Error when saving file "<<dialog->GetFileName());
    vtkKWPopupErrorMessage (this, "Cannot save file");
  }

  dialog->SaveLastPathToRegistry("DataPath");
  dialog->Delete();
}

