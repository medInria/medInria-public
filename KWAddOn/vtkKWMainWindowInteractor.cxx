/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWMainWindowInteractor.cxx 1366 2009-11-30 08:10:27Z tmansi $
Language:  C++
Author:    $Author: tmansi $
Date:      $Date: 2009-11-30 09:10:27 +0100 (Mon, 30 Nov 2009) $
Version:   $Revision: 1366 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <itkGDCMImporter3.h>
#include <itkImage.h>
#include <itkImageToVTKImageFilter.h>
#include <kwcommon.h>
#include <pixmap/KWAddOnResources.h>
#include <sstream>
#include <vtkActor.h>
#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkColorTransferFunction.h>
#include <vtkCommand.h>
#include <vtkDataArray.h>
#include <vtkDataManager.h>
#include <vtkDataManagerCallback.h>
#include <vtkDataSetMapper.h>
#include <vtkErrorCode.h>
#include <vtkFibersManager.h>
#include <vtkImageClamp.h>
#include <vtkImageData.h>
#include <vtkKWApplication.h>
#include <vtkKWApplication.h>
// #include <vtkKWDICOMExporter.h>
#include <vtkKWDICOMImporter2.h>
#include <vtkKWDataManagerWidget.h>
#include <vtkKWDragAndDropTargetSet.h>
#include <vtkKWEntry.h>
#include <vtkKWEvent.h>
#include <vtkKWEntryWithLabel.h>
#include <vtkKWFrameWithLabel.h>
#include <vtkKWInfoToolBox.h>
#include <vtkKWInternationalization.h>
#include <vtkKWLabel.h>
#include <vtkKWLandmarkManagerWidget.h>
#include <vtkKWLoadSaveDialog.h>
#include <vtkKWMainCallback.h>
#include <vtkKWMainWindowInteractor.h>
#include <vtkKWMenu.h>
#include <vtkKWMessageDialog.h>
#include <vtkKWMultiColumnList.h>
#include <vtkKWNotebook.h>
#include <vtkKWPageView.h>
#include <vtkKWPreviewPage.h>
#include <vtkKWProgressGauge.h>
#include <vtkKWPushButton.h>
#include <vtkKWRenderWidget.h>
#include <vtkKWSequenceImporter.h>
#include <vtkKWSequencer.h>
#include <vtkKWSimpleEntryDialog.h>
#include <vtkKWSnapshotHandler.h>
#include <vtkKWSplashScreen.h>
#include <vtkKWSplitFrame.h>
#include <vtkKWSurfaceMaterialPropertyWidget.h>
#include <vtkKWTkUtilities.h>
#include <vtkKWToolBox.h>
#include <vtkKWToolbar.h>
#include <vtkKWToolbarSet.h>
#include <vtkKWToolboxManager.h>
#include <vtkKWVisualizationToolBox.h>
#include <vtkKWWidgetsPaths.h>
#include <vtkKWWindowBase.h>
#include <vtkLandmarkManager.h>
#include <vtkLookupTableManager.h>
#include <vtkMetaDataSetSequence.h>
#include <vtkMetaImageData.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkMetaVolumeMesh.h>
#include <vtkObjectFactory.h>
#include <vtkPlaneWidget.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkStringArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkVolumeProperty.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkIdList.h>
#include <vtkSurfaceReconstructionFilter.h>
#include <vtkContourFilter.h>
#include <vtkDelaunay3D.h>
#include <vtkSelectPolyData.h>

// #include <vtkKWDicomInfoWidget.h>

#include <vtksys/SystemTools.hxx>
#include <vtksys/stl/string>
#include <vtksys/stl/vector>
// #include <vtkKWMetaDataSetControlWidget.h>
// #include <vtkMetaDataSetVisuManager.h>

#if VTK_MAJOR_VERSION>=5 && VTK_MINOR_VERSION>=1
#include <vtkDataManagerWriter.h>
#include <vtkDataManagerReader.h>
#endif

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWMainWindowInteractor );
vtkCxxRevisionMacro( vtkKWMainWindowInteractor, "$Revision: 1366 $");

//----------------------------------------------------------------------------
vtkKWMainWindowInteractor::vtkKWMainWindowInteractor()
{
  this->DataManager         = vtkDataManager::New();
  this->DataManagerCallback = vtkDataManagerCallback::New();
  this->MainCallback        = vtkKWMainCallback::New();
  this->ManagerWidget       = vtkKWDataManagerWidget::New();
  this->ManagerDialog       = vtkKWTopLevel::New();
  this->LandmarkManagerDialog = vtkKWTopLevel::New();
  
  
  this->ToolboxNotebook     = vtkKWNotebook::New();

//   this->DataManagerCallback->SetDataManager(this->DataManager);
//   this->DataManager->AddObserver(vtkDataManager::MetaDataSetPickEvent, this->MainCallback);

  this->GetViewNotebook()->AddObserver(vtkKWEvent::NotebookRaisePageEvent, this->MainCallback);

  this->PanelSplitFrame = vtkKWSplitFrame::New();
  this->ToolboxManager = vtkKWToolboxManager::New();
  this->ToolboxManager->SetParentObject(this);
  
  this->PreviewPage = vtkKWPreviewPage::New();
  this->SnapshotHandler = vtkKWSnapshotHandler::New();

  IllustrationMode = false;
  ModifiedPage = NULL;

//   itk::AnalyzeImageIOFactory::RegisterOneFactory();
//   itk::GDCMImageIOFactory::RegisterOneFactory();

}

//----------------------------------------------------------------------------
vtkKWMainWindowInteractor::~vtkKWMainWindowInteractor()
{

  std::cout<<"deleting mainframeinteractor"<<std::endl;
  this->DataManager->Delete();
  this->DataManagerCallback->Delete();
  this->MainCallback->Delete();
  this->ManagerWidget->Delete();
  this->ToolboxNotebook->Delete();
  this->PanelSplitFrame->Delete();
  this->ManagerDialog->Delete();
  this->ToolboxManager->Delete();
  this->LandmarkManagerDialog->Delete();
  
  this->PreviewPage->Delete();
  this->SnapshotHandler->Delete();
  
  if (this->GetViewNotebook()->IsCreated())
  {
    // to avoid some VTK warnings,
    // we have to manually destroy the pages from the view notebook
    this->GetViewNotebook()->RemovePagesMatchingTag(0);
    this->GetViewNotebook()->RemovePage (0);
  }
}

vtkKWPageView* vtkKWMainWindowInteractor::CreateNewPage (const char* name, vtkImageData* image, vtkMatrix4x4* matrix)
{
  if (!name)
    return NULL;

  this->PageNumberIncrement++;
  int id = this->GetViewNotebook()->AddPage (name);
  this->GetViewNotebook()->SetPageTag (id, 0);
  
  vtkKWPageView* viewframe = vtkKWPageView::New();
  viewframe->SetParent(this->GetViewNotebook()->GetFrame (id));
  viewframe->Create();
  viewframe->SetTag (name);
  this->Script("pack %s -fill both -side top -expand t", 
	       viewframe->GetWidgetName());
  viewframe->SetImage (image, matrix);
  this->GetViewNotebook()->RaisePage (id);
  this->GetViewNotebook()->RaiseCallback (id);
  
  viewframe->GetView4()->GetInteractor()->AddObserver(vtkCommand::LeftButtonPressEvent, this->DataManagerCallback);
  viewframe->Delete();

  return this->GetPage (name);
  
}

vtkKWPageView* vtkKWMainWindowInteractor::CreateNewPage (const char* name, itk::Image<float, 3>::Pointer image)
{
  if (!name)
    return NULL;

  this->PageNumberIncrement++;
  int id = this->GetViewNotebook()->AddPage (name);
  this->GetViewNotebook()->SetPageTag (id, 0);

  vtkKWPageView* viewframe = vtkKWPageView::New();
  viewframe->SetParent(this->GetViewNotebook()->GetFrame (id));
  viewframe->Create();
  viewframe->SetTag (name);
  this->Script("pack %s -fill both -side top -expand t", 
	       viewframe->GetWidgetName());
  viewframe->SetImage (image);
  this->GetViewNotebook()->RaisePage (id);
  this->GetViewNotebook()->RaiseCallback(id);
  
  viewframe->GetView4()->GetInteractor()->AddObserver(vtkCommand::LeftButtonPressEvent, this->DataManagerCallback);
  viewframe->Delete();

  return this->GetPage (name);
  
}


void vtkKWMainWindowInteractor::RemovePage (const char* title)
{
  vtkKWFrame* frame = this->GetViewNotebook()->GetFrame(title);
  
  if (frame)
    frame->RemoveAllChildren();

  unsigned int id = this->GetViewNotebook()->GetPageId (title, 0);
  
  this->GetViewNotebook()->RemovePage (title);
  unsigned int newid = 0;
  if (this->GetViewNotebook()->HasPage (id+1))
    newid = id+1;
  else if (this->GetViewNotebook()->HasPage (id-1))
    newid = id-1;
  this->GetViewNotebook()->RaisePage (newid);
  this->GetViewNotebook()->RaiseCallback(newid);
  
  
}




//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::SetDataManager (vtkDataManager* manager)
{
  if (this->DataManager == manager)
  {
    return;
  }
  if (this->DataManager)
  {
    this->DataManager->UnRegister(this);
  }
  this->DataManager = manager;
  
  if (this->DataManager)
  {
    this->DataManager->Register(this);
  }

  if (this->DataManager)
  {
//     this->DataManagerCallback->SetDataManager(this->DataManager);
//     this->DataManager->AddObserver(vtkDataManager::MetaDataSetPickEvent, this->MainCallback);
  }

//   this->Update();
}


//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::SetDataManagerWidget (vtkKWDataManagerWidget* widget)
{
  if (this->ManagerWidget == widget)
  {
    return;
  }
  if (this->ManagerWidget)
  {
    this->ManagerWidget->UnRegister(this);
  }
  this->ManagerWidget = widget;
  
  if (this->ManagerWidget)
  {
    this->ManagerWidget->Register(this);
  }
}

//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::CreatePreviewPage (void)
{

  if (this->PreviewPage->IsCreated())
  {
    vtkErrorMacro("preview already created");
    return;
  }
  
  this->PageNumberIncrement++;
  int id = this->GetViewNotebook()->AddPage ("Preview");
  this->GetViewNotebook()->SetPageTag (id, 0);
  
  this->PreviewPage->SetParent(this->GetViewNotebook()->GetFrame (id));
  this->PreviewPage->Create();
  this->Script("pack %s -side top -expand t -fill both", 
	       this->PreviewPage->GetWidgetName());

  this->PreviewPage->Update();
  

}


//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::CreateWidget()
{

  if (this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }
  
  this->Superclass::CreateWidget();
  
  vtkKWApplication *app = this->GetApplication();
  
  if (app->GetSplashScreenVisibility()) 
  {
    app->GetSplashScreen()->SetProgressMessage("Creating 2D and 3D views...");
  }

  
  // first creation :
  if (this->GetViewNotebook()->GetNumberOfPages() == 0 )
  {
    bool showpreview = false;
    
    char buffer[1024];
    bool valid = this->GetApplication()->GetRegistryValue(1, "RunTime", "ShowPreview", buffer);
    if (valid)
    {
      if (*buffer)
      {
    	std::istringstream is;
    	is.str (buffer);
    	int val = 0;
    	is >> val;
    	showpreview = val;
      }
    }
    
    if (showpreview)
      this->CreatePreviewPage();
  }
  

  if (app->GetSplashScreenVisibility()) 
  {
    app->GetSplashScreen()->SetProgressMessage("Setting layout properties...");
  }
  
  this->MainCallback->SetApplication (this->GetApplication());

  this->PanelSplitFrame->SetParent(this->GetMainPanelFrame());
  this->PanelSplitFrame->Create();
  this->PanelSplitFrame->SetOrientationToVertical();
  this->PanelSplitFrame->SetReliefToFlat();
  this->PanelSplitFrame->SetBorderWidth(2);
  this->PanelSplitFrame->SetExpandableFrameToBothFrames();
  this->PanelSplitFrame->SetFrame1MinimumSize(250);
  this->PanelSplitFrame->SetFrame2MinimumSize(120);
  this->PanelSplitFrame->SetSeparatorPosition (1000);
  
  
  this->Script("pack %s -fill both -side top -anchor nw -expand yes", 
	       this->PanelSplitFrame->GetWidgetName());

  vtkKWFrame* temp = vtkKWFrame::New();
  temp->SetParent (this->PanelSplitFrame->GetFrame2());
  temp->Create();
  vtkKWLabel* label = vtkKWLabel::New();
  label->SetParent (temp);
  label->Create();
  label->SetText ("Data Manager");
  vtkKWPushButton* button = vtkKWPushButton::New();
  button->SetParent (temp);
  button->Create();
  button->SetImageToPixels(image_popup, image_popup_width, image_popup_height, image_popup_pixel_size, image_popup_length); 
  button->SetCommand (this, "PopupManager");
  button->SetBalloonHelpString("Popup the Data Manager");

  this->Script("pack %s -side left -anchor nw -expand n", 
	       label->GetWidgetName());
  this->Script("pack %s -side right -anchor ne -expand n", 
	       button->GetWidgetName());
  this->Script("pack %s -side top -fill x -anchor nw -expand n", 
	       temp->GetWidgetName());
  label->Delete();
  temp->Delete();
  button->Delete();
  
  
  
  if (app->GetSplashScreenVisibility()) 
  {
    app->GetSplashScreen()->SetProgressMessage("Creating data manager widget...");
  }

  this->ManagerWidget->SetParent(this->PanelSplitFrame->GetFrame2());
  this->ManagerWidget->SetParentObject (this);
  this->ManagerWidget->Create();
  this->ManagerWidget->SetHeight (60);

  this->ToolboxNotebook->SetParent(this->PanelSplitFrame->GetFrame1());
  this->ToolboxNotebook->Create();

  if (app->GetSplashScreenVisibility()) 
  {
    app->GetSplashScreen()->SetProgressMessage("Creating toolbox manager...");
  }

  this->ToolboxManager->SetParent(this->PanelSplitFrame->GetFrame1());
  this->ToolboxManager->Create();

  this->PackSelf();

  
  if (app->GetSplashScreenVisibility()) 
  {
    app->GetSplashScreen()->SetProgressMessage("loading toolboxes...");
  }

  this->MainPanelVisibilityOn();
  this->SecondaryPanelVisibilityOff();
  
  
  vtkKWInfoToolBox* information = vtkKWInfoToolBox::New();
  this->LoadToolBox(information, "Information");
  this->ToolboxManager->SelectToolboxCallback(0);
  information->Delete();
  
  vtkKWSequencer* Sequencer = vtkKWSequencer::New();
  this->LoadToolBox(Sequencer, "Sequencer");
  Sequencer->Delete();

  vtkKWVisualizationToolBox* visutools = vtkKWVisualizationToolBox::New();
  this->LoadToolBox(visutools, "Visualization");
  visutools->Delete();

  this->Update();
  
  
}

void vtkKWMainWindowInteractor::PackSelf()
{
  this->Superclass::PackSelf();


  if (this->ManagerWidget->IsCreated())
  {
    this->Script("pack %s -fill both -side top -anchor nw -expand yes", 
		 this->ManagerWidget->GetWidgetName());
  }
  
  if (this->ToolboxManager->IsCreated())
  {
    this->Script(
		 "pack %s -side top -anchor nw -fill x -padx 2 -pady 2", 
		 this->ToolboxManager->GetWidgetName());
  }

}



//-----------------------------------------------------------------------------
std::vector<vtkKWToolBox*> vtkKWMainWindowInteractor::GetToolBoxList()
{
  return this->ToolboxManager->GetToolBoxList();
}



//-----------------------------------------------------------------------------
vtkKWToolBox* vtkKWMainWindowInteractor::GetToolboxByID(unsigned int id)
{
  return this->GetToolbox (id);
}

//-----------------------------------------------------------------------------
vtkKWToolBox* vtkKWMainWindowInteractor::GetToolbox(unsigned int id)
{
  return this->GetToolboxManager()->GetToolbox (id);
}

//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::CreateAliases ()
{
  if (!this->IsCreated())
    return;
  
  vtkKWApplication *app = this->GetApplication();

  app->Script("set m_window %s", this->GetTclName());
  app->Script("set m_app %s", this->GetApplication()->GetTclName());

  if (this->GetDataManager())
    app->Script("set m_manager [$m_window GetDataManager]");
  
  if (this->GetCurrentPage())
  {
    
    app->Script("set m_multiviews [$m_window GetCurrentPage]");
    app->Script("set m_v1 [$m_multiviews GetView1]");
    app->Script("set m_v2 [$m_multiviews GetView2]");
    app->Script("set m_v3 [$m_multiviews GetView3]");
    app->Script("set m_v4 [$m_multiviews GetView4]");
  }
}

//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::OnMenuNewTab()
{
  unsigned int tabNb = 1;
  std::string prefix="Tab (";
  std::string tabTitle;
  char buffer[16];

  bool ok = false;
  while ( !ok )
  {
    ok = true;
    sprintf(buffer, "%d", tabNb);
    tabTitle = prefix + buffer + ")";
    if ( this->GetPage(tabTitle.c_str()) )
    {
      tabNb++;
      ok = false;
    }		
  }

  this->CreateNewPage(tabTitle.c_str(), (vtkImageData*)(NULL));
}

//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::OnMenuDeleteCurrentTab()
{
  vtkKWPageView* page = this->GetCurrentPage();
  if (!page)
    return;

  std::vector<vtkMetaDataSet*> metadatasetlist = this->GetDataManager()->GetMetaDataSetListFromTag (page->GetTag());

  for (unsigned int i=0; i<metadatasetlist.size(); i++)
  {
    if (metadatasetlist[i]->GetType() == vtkMetaDataSet::VTK_META_IMAGE_DATA)
    {
      this->PreviewPage->RemovePreviewImage (vtkImageData::SafeDownCast(metadatasetlist[i]->GetDataSet() ));
      page->RemoveMetaDataSet (metadatasetlist[i]);
      this->GetDataManager()->RemoveMetaDataSet (metadatasetlist[i]);
      break;
    }
  }

  this->RemovePage(page->GetTag());

  

  this->Update();
}


//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::OnMenuFileOpen()
{
  
  vtkKWLoadSaveDialog *dialog = vtkKWLoadSaveDialog::New() ;
  
  dialog->SetParent(this);
  dialog->Create();
  dialog->MultipleSelectionOn();
  
  dialog->RetrieveLastPathFromRegistry("DataPath");
  dialog->SetTitle ("Open a File");

  dialog->SetFileTypes (kwsupportedformats.c_str());
  
  if ( dialog->Invoke () == 0 )
  {
    dialog->Delete();
    return ;
  }
  
  bool allopened = true;


  vtkKWWindowBase *win = this->GetApplication()->GetNthWindow(0);

  if (!win)
    return;
  
  win->SetStatusText(ks_("Progress|Opening files..."));
  win->GetProgressGauge()->SetValue(0);
  
  
  for (int i=0; i<dialog->GetNumberOfFileNames(); i++)
  {
    win->GetProgressGauge()->SetValue((int)(100.0 * i / dialog->GetNumberOfFileNames()));
      
    try
    {
      this->ReadFile (dialog->GetFileNames()->GetValue (i));
    }
    catch (vtkErrorCode::ErrorIds)
    {
      allopened = false;
    }
  }


  vtksys_stl::string end_msg(win->GetStatusText());
  end_msg += " -- ";
  end_msg += ks_("Progress|Done");  
  win->SetStatusText(end_msg.c_str());
  win->GetProgressGauge()->SetValue(0);

  if (!allopened)
    vtkKWPopupErrorMessage(this, "One or more file(s) have not been opened !\nSee Log for more info");
  
  dialog->SaveLastPathToRegistry("DataPath");
  dialog->Delete();
  
  this->Update();

  // this->GetViewNotebook()->RaisePage(0);
  // this->GetViewNotebook()->RaiseCallback(0);
}


//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::ReadFileAndUpdate(const char* filename)
{
  try
  {
    this->ReadFile (filename);
    this->Update();
  }
  catch (vtkErrorCode::ErrorIds)
  {
    vtkKWPopupErrorMessage(this, "Cannot Read file !\nSee Log for more info");
    return;
  }
  
  if (this->GetCurrentPage())
    this->GetCurrentPage()->Render();

}


//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::ReadFile(const char* filename)
{
  if (!vtksys::SystemTools::FileExists(filename))
  {
    vtkErrorMacro(<<"file "<<filename<<"does not exist"<<endl);
    throw vtkErrorCode::CannotOpenFileError;
  }
  
  std::string ext = vtksys::SystemTools::GetFilenameLastExtension (filename);
  
  if ((strcmp(ext.c_str(), ".tcl") == 0)  )
  {
    std::string source = "";
    std::string line = "";
    ifstream ifs(filename);
    while (vtksys::SystemTools::GetLineFromStream(ifs, line))
    {
      source += line;
      source += "\n";
    }
    ifs.close();
    
    this->Script(source.c_str());
    
    return;
  }

  if ( (strcmp(ext.c_str(), ".cv3d") == 0) ||
       (strcmp(ext.c_str(), ".dmf") == 0) ||
       (strcmp(ext.c_str(), ".xml") == 0))
  {
#if VTK_MAJOR_VERSION>=5 && VTK_MINOR_VERSION>=1

    vtkDataManagerReader* reader = vtkDataManagerReader::New();
    reader->SetFileName (filename);
    reader->Update();
    for (unsigned int i=0; i<reader->GetOutput()->GetNumberOfMetaDataSet(); i++)
      this->GetDataManager()->AddMetaDataSet (reader->GetOutput()->GetMetaDataSet (i));
    reader->Delete();
    return;

#else // VTK_MAJOR_VERSION>=5 && VTK_MINOR_VERSION>=1

    std::string source = "";
    std::string line = "";
    ifstream ifs(filename);
    while (vtksys::SystemTools::GetLineFromStream(ifs, line))
    {
      source += line;
      source += "\n";
    }
    ifs.close();
    this->Script(source.c_str());
    return;
    
#endif
    
  }
  

  try
  {
    this->DataManager->ReadFile (filename);
  }
  catch (vtkErrorCode::ErrorIds e)
  {
//     vtkErrorMacro(<<"cannot open "<<filename<<endl);
    throw e;
  }

  this->AddRecentFile(filename, this, "ReadFileAndUpdate");
  
  
}


//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::OnMenuFileOpen4D()
{


  vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::New();
  
  vtkKWSequenceImporter *dlg = vtkKWSequenceImporter::New();
  dlg->SetMasterWindow (this);
  dlg->Create();
  dlg->SetDisplayPositionToDefault();
  dlg->SetPosition (100,100);
  dlg->SetSequence (sequence); 

  dlg->Invoke(); 
  
  if (dlg->GetStatus() == vtkKWDialog::StatusOK)
  {
    
    this->DataManager->AddMetaDataSet(sequence);
    this->Update();
    if (this->GetCurrentPage())
      this->GetCurrentPage()->Render();
  }

  dlg->Delete();
  sequence->Delete();
    
}

//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::SaveManagerCallback()
{

  
  vtkKWLoadSaveDialog *dialog = vtkKWLoadSaveDialog::New() ;
  
  dialog->SetParent(this);
  dialog->Create();
  dialog->MultipleSelectionOff();
  dialog->SaveDialogOn ();
  dialog->RetrieveLastPathFromRegistry("DataPath");
  dialog->SetTitle ("Save the manager to file");

  std::string managerformat = "{{XML manager file} {.xml .dmf}}";
  
  dialog->SetFileTypes (managerformat.c_str());
  
  if ( dialog->Invoke () == 0 )
  {
    dialog->Delete();
    return;
  }

  std::string managerfile = dialog->GetFileName();
  std::string ext = vtksys::SystemTools::GetFilenameLastExtension (managerfile.c_str());

  dialog->Delete();
  
  if ( strcmp(ext.c_str(), ".xml") &&
       strcmp(ext.c_str(), ".dmf") )
  {
    managerfile+=".dmf";
  }

#if VTK_MAJOR_VERSION>=5 && VTK_MINOR_VERSION>=1
  vtkDataManagerWriter* writer = vtkDataManagerWriter::New();
  writer->SetInput (this->GetDataManager());
  writer->SetFileName (managerfile.c_str());
  writer->Update();
  writer->Delete();

  
#else //  VTK_MAJOR_VERSION>=5 && VTK_MINOR_VERSION>=1
  
  std::ostringstream os;

  os <<"$m_window RemoveAllMetaDataSets"<<"\n";
  
  std::vector<vtkKWPageView*> pagelist = this->GetPageList();
  
  for (unsigned int i=0; i<pagelist.size(); i++)
  {
    
    std::vector<vtkMetaDataSet*> datasetlist = this->GetDataManager()->GetMetaDataSetListFromTag(pagelist[i]->GetTag());


    for (unsigned int j=0; j<datasetlist.size(); j++)
    {
      
      vtkMetaDataSet* metadata = datasetlist[j];
      std::string filepath = metadata->GetFilePath();

      
      
      if (!filepath.size())
      {
	vtkKWPopupWarningMessage (this, "dataset not written, please save to file");
	vtkKWLoadSaveDialog *dialog2 = vtkKWLoadSaveDialog::New() ;
	
	dialog2->SetParent(this);
	dialog2->Create();
	dialog2->MultipleSelectionOff();
	int val = metadata->GetType();
	if (vtkMetaDataSetSequence::SafeDownCast(metadata) &&
	    (val != vtkMetaDataSet::VTK_META_IMAGE_DATA))
	  dialog2->ChooseDirectoryOn();
	dialog2->SaveDialogOn ();
	dialog2->RetrieveLastPathFromRegistry("DataPath");
	dialog2->SetTitle ("Save dataset to file");
	
	std::string ext;
	
	switch (val)
	{
	    case vtkMetaDataSet::VTK_META_IMAGE_DATA :
	      ext = kwsupportedimageformats_global + kwsupportedimageformats;
	      break;
	      
	    case vtkMetaDataSet::VTK_META_SURFACE_MESH :
	    case vtkMetaDataSet::VTK_META_VOLUME_MESH :
	      ext = kwsupportedmeshformats_global + kwsupportedmeshformats;
	      break;
	    default :
	      ext = kwsupportedformats;
	      break;
	}
	
	dialog2->SetFileTypes (ext.c_str());
	
	int dialog_ret = dialog2->Invoke();
	
	if (dialog_ret)
	{
	  
	  const char* filename = dialog2->GetFileName();
	  try
	  {
	    metadata->Write (filename);
	    dialog->SaveLastPathToRegistry("DataPath");
	  }
	  catch (vtkErrorCode::ErrorIds &e)
	  {
	    std::cerr<<e<<std::endl;
	    vtkErrorMacro(<<"cannot save file "<<dialog->GetFileName()<<endl);
	  }
	}
	
	dialog2->Delete();
	
      }



      
      filepath = metadata->GetFilePath();
      
      if (!filepath.size())
      {
	vtkKWPopupWarningMessage (this, "skipping dataset");
	continue;
      }

      
      os <<"$m_manager ReadFile "<<filepath.c_str()<<" "<<metadata->GetName()<<"\n";
    
    }
    os <<"$m_window Update"<<"\n";

  }

  // open the file:
  std::ofstream buffer (managerfile.c_str());
  if( buffer.fail() )
  {
    vtkErrorMacro(<<"Cannot write in file !"<<endl);
    buffer.close();
  }  

  buffer << os.str().c_str();
  buffer.close();

#endif // VTK_MAJOR_VERSION>=5 && VTK_MINOR_VERSION>=1
  
  
}






void vtkKWMainWindowInteractor::Update()
{
  if (!this->IsCreated())
    return;

  // Recover the list of datasets contained in the manager
  
  std::vector<vtkMetaDataSet*> metadatasetlist = this->DataManager->GetMetaDataSetList();

  // At first update : create aliases for scripting purposes
  if (!metadatasetlist.size())
  {
    this->CreateAliases();
  }

  // iterate on the dataset list
  for (unsigned int i=0; i<metadatasetlist.size(); i++)
  {

    vtkKWPageView* pageview = NULL;

    // This test returns true if a page is already associated to this dataset
    // AND if this page actually contains the dataset
    // If this is the case, we don't have to do anything with this dataset, go to next
    if (this->GetPage(metadatasetlist[i]->GetTag ()) &&
	this->GetPage(metadatasetlist[i]->GetTag ())->HasDataSet(metadatasetlist[i]->GetDataSet()))
      continue;
    
    // From this point the dataset we are dealing with has to be plugged into the views

    if (metadatasetlist[i]->GetType() == vtkMetaDataSet::VTK_META_IMAGE_DATA)
    {
      // If the dataset is of image type, we have to create a new page (set of 4 views)
      // and add the image to it
      vtkMetaImageData* metaimage = 0;
      vtkMetaDataSetSequence* sequence = 0;
      
      metaimage = vtkMetaImageData::SafeDownCast(metadatasetlist[i]);
      if (!metaimage)
      {
	sequence = vtkMetaDataSetSequence::SafeDownCast (metadatasetlist[i]);
	if (sequence)
	  metaimage = vtkMetaImageData::SafeDownCast(sequence->GetMetaDataSet (0));
      }  

      if (metaimage)
	pageview = this->CreateNewPage (metadatasetlist[i]->GetName(), vtkImageData::SafeDownCast(metadatasetlist[i]->GetDataSet() ), metaimage->GetOrientationMatrix() );
      else
	pageview = this->CreateNewPage (metadatasetlist[i]->GetName(), vtkImageData::SafeDownCast(metadatasetlist[i]->GetDataSet() ) );
	
      // This method is actually not used anywhere yet
      metadatasetlist[i]->SetProperty (pageview->GetView4()->GetVolumeProperty());

      char buffer[1024];
      bool do_preview = false;
      
      bool valid = this->GetApplication()->GetRegistryValue(1, "RunTime", "ShowPreview", buffer);
      if (valid)
      {
      	if (*buffer)
      	{
      	  std::istringstream is;
      	  is.str (buffer);
      	  int val = 0;
      	  is >> val;
      	  if (!val)
      	    do_preview = false;
	  else
     	    do_preview = true;
	    
      	}
      }    

      if (do_preview)
      {
	this->PreviewPage->AddPreviewImage (vtkImageData::SafeDownCast(metadatasetlist[i]->GetDataSet() ), metadatasetlist[i]->GetName(), metaimage->GetOrientationMatrix());
      }
      
    }
    else
    {
      // recovering the page that is supposed to contain the dataset
      pageview = this->GetPage(metadatasetlist[i]->GetTag ());

      // if this page is not found : we choose to add it to the current page
      if (!pageview)
	pageview = this->GetCurrentPage();

      if (!pageview)
	pageview = this->CreateNewPage(metadatasetlist[i]->GetName(), (vtkImageData*)(NULL));
      
      pageview->AddMetaDataSet (metadatasetlist[i]);
    }
    

    // Associate to the metadataset a TclName variable (an alias) that can be recovered later
    // for scripting purposes
    std::ostringstream name;
    name <<"m_"<<vtksys::SystemTools::MakeCindentifier (metadatasetlist[i]->GetName());
    metadatasetlist[i]->SetMetaData<std::string>("TclName",(std::string (name.str().c_str())));

    // Actually create the tcl alias
    this->Script("set %s [$m_manager GetMetaDataSet %s]", name.str().c_str(), metadatasetlist[i]->GetName());

    // Associate to the metadataset the name of the page it is displayed
    // for visualization purposes
    metadatasetlist[i]->SetTag (pageview->GetTag());
  }

  

  // Updating the toolboxes so that they know
  // something has changed in the data manager
  for (unsigned int j=0; j<this->GetToolBoxList().size(); j++)
  {
    this->GetToolBoxList()[j]->Update();
  }


  // Updating the manager widget so that the freshly added
  // dataset appear in it
  this->ManagerWidget->Update();

  bool dopreview = false;
  char buffer[1024];
  bool valid = this->GetApplication()->GetRegistryValue(1, "RunTime", "ShowPreview", buffer);
  
  if (valid)
  {
    if (*buffer)
    {
      std::istringstream is;
      is.str (buffer);
      int val = 0;
      is >> val;
      if (val)
  	dopreview = 1;
      else
  	dopreview = 0;
    }
  }
  else
    dopreview = 0;

  if (dopreview)
    this->PreviewPage->Update();

}

void vtkKWMainWindowInteractor::LoadToolBox (vtkKWToolBox* toolbox, const char* name)
{
  if (!toolbox->IsCreated())
  {

    if (this->ToolboxManager->IsCreated())
    {
      this->ToolboxManager->InsertToolbox(toolbox);
    }
  }
  
  toolbox->SetMetaDataSetChangedCommand(this, "Update");
  
//   this->ToolBoxList.push_back (toolbox);
//   this->Update();
  this->PackSelf();
}


//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::OnMenuFileOpenDICOM ()
{


  
  vtkKWDICOMImporter2 *dlg = vtkKWDICOMImporter2::New();
  dlg->SetMasterWindow (this);
  dlg->Create();
  dlg->SetDisplayPositionToDefault();
  dlg->SetPosition (100,100);
  
  dlg->Invoke(); 

  if (dlg->GetStatus() != vtkKWDialog::StatusOK)
  {
    dlg->Delete();
    return;
  }

  std::vector<vtkMetaDataSet*> list = dlg->GetOutputList();
  for (unsigned int i=0; i<list.size(); i++)
  {
    this->GetDataManager()->AddMetaDataSet(list[i]);
    list[i]->Delete();
  }
  list.clear();
  dlg->Delete();

  this->Update();
  
}


//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::OnDataSetSave(void)
{

  vtkMetaDataSet* metadataset = this->ManagerWidget->GetSelectedMetaDataSet();

  if (!metadataset)
    return;

  vtkKWLoadSaveDialog *dialog = vtkKWLoadSaveDialog::New() ;
  
  dialog->SetParent(this);
  dialog->Create();
  dialog->MultipleSelectionOff();

  bool do_directory = true;
  
  if (vtkMetaDataSetSequence::SafeDownCast(metadataset))
  {
    if (metadataset->GetType() == vtkMetaDataSet::VTK_META_IMAGE_DATA)
    {
      vtkKWMessageDialog* dialog2 = vtkKWMessageDialog::New();
      dialog2->SetApplication (this->GetApplication());
      dialog2->SetStyleToYesNo ();
      dialog2->Create();
      dialog2->SetTitle("Info !");
      
      std::ostringstream os;
      os << "Dataset ";
      os<<metadataset->GetName()<<std::endl;
      
      os<<"is an image sequence"<<std::endl;
      os<<"Do you want to save it as a single file ?"<<std::endl;
      
      dialog2->SetText(os.str().c_str());
      
      if ( dialog2->Invoke () == 1)
      {
	do_directory = false;
      }
      else
	do_directory = true;
      
      dialog2->Delete();
    }
  }
  else
    do_directory = false;
  
  if (do_directory)
    dialog->ChooseDirectoryOn();
  else
    dialog->ChooseDirectoryOff();
  
  dialog->SaveDialogOn ();
  dialog->RetrieveLastPathFromRegistry("DataPath");
  std::ostringstream os2;
  os2 << "Save ";
  os2<<metadataset->GetName();
  
  os2<<" to file...";
  
  dialog->SetTitle (os2.str().c_str());

  int val = metadataset->GetType();
  std::string ext;
  
  switch (val)
  {
      case vtkMetaDataSet::VTK_META_IMAGE_DATA :
	ext = kwsupportedimageformats_global + kwsupportedimageformats;
	break;
	
      case vtkMetaDataSet::VTK_META_SURFACE_MESH :
      case vtkMetaDataSet::VTK_META_VOLUME_MESH :
	ext = kwsupportedmeshformats_global + kwsupportedmeshformats;
	break;
      default :
	ext = kwsupportedformats;
	break;
  }
  
  
  dialog->SetFileTypes (ext.c_str());
  
  if ( dialog->Invoke () == 0 )
  {
    dialog->Delete();
    return ;
  }
  
  std::string filename = dialog->GetFileName();
  std::string extension = vtksys::SystemTools::GetFilenameLastExtension(filename);
  if ((!vtkMetaDataSetSequence::SafeDownCast(metadataset) && (extension.size()<2)) )
  {
    if(metadataset->GetType() == vtkMetaDataSet::VTK_META_IMAGE_DATA)
      filename+=".hdr";
    else
      filename+=".vtk";
  }
  
  try
  {
    metadataset->Write (filename.c_str());
    //     vtkDataManagerWriter* writer = vtkDataManagerWriter::New();
    //     writer->SetInput(metadataset->GetDataSet());
    //     writer->SetDataManager(this->DataManager);
    //     writer->SetFileName(filename);
    //     writer->Write();
    
  }
  catch (vtkErrorCode::ErrorIds)
  {
    std::cerr<<"cannot save file "<<dialog->GetFileName()<<std::endl;
    vtkErrorMacro(<<"Error when saving file "<<dialog->GetFileName());    
  }

  dialog->SaveLastPathToRegistry("DataPath");
  dialog->Delete();

}


//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::OnDataSetDelete(void)
{
  std::vector<vtkMetaDataSet*> metadatasets = this->ManagerWidget->GetSelectedMetaDataSets();
  if (!metadatasets.size())
    return;
  
  vtkKWMessageDialog* dialog = vtkKWMessageDialog::New();
  dialog->SetApplication (this->GetApplication());
  dialog->SetStyleToYesNo ();
  dialog->Create();
  dialog->SetTitle("Warning !");
  
  std::ostringstream os;
  os << "Are you sure you want to remove ";
  if (metadatasets.size()>1)
  {
    os<<"these metadatasets"<<std::endl;
    for (unsigned int i=0; i<metadatasets.size(); i++)
      os<<metadatasets[i]->GetName()<<std::endl;
  }
  else
    os<<metadatasets[0]->GetName()<<std::endl;

  os<<"from the manager ?";
  
  dialog->SetText(os.str().c_str());
  
  if ( dialog->Invoke () != 1)
  {
    dialog->Delete();
    return;
  }
  dialog->Delete();
  
  for (unsigned int i=0; i<metadatasets.size(); i++)
    this->RemoveMetaDataSet (metadatasets[i]);
  
  this->Update();
  
  if (this->GetCurrentPage())
    this->GetCurrentPage()->Render();  
  
}

//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::RemoveAllMetaDataSets (void)
{
  std::vector<vtkMetaDataSet*> list = this->GetDataManager()->GetMetaDataSetList();

  for (unsigned int i=0; i<list.size(); i++)
  {
    vtkMetaDataSet* metadataset = list[i];
    
    if (this->GetPage(metadataset->GetTag()))
    {
//       this->GetPage(metadataset->GetTag())->RemoveDataSet (metadataset->GetDataSet());
      this->GetPage(metadataset->GetTag())->RemoveMetaDataSet (metadataset);
    }
    
    if (metadataset->GetType() == vtkMetaDataSet::VTK_META_IMAGE_DATA)
    {
      this->RemovePage(metadataset->GetTag());
      this->PreviewPage->RemovePreviewImage (vtkImageData::SafeDownCast(metadataset->GetDataSet() ));
    }
    
    this->DataManager->RemoveMetaDataSet (metadataset);
  }

  this->Update();
  
}


//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::PopdownManager ()
{
  this->PanelSplitFrame->Frame2VisibilityOn ();

  if (this->ManagerDialog->IsCreated())
  {  
    this->ManagerDialog->Withdraw();
    this->ManagerDialog->UnpackChildren();
  }

  vtkKWFrame* temp = vtkKWFrame::New();
  temp->SetParent (this->PanelSplitFrame->GetFrame2());
  temp->Create();
  vtkKWLabel* label = vtkKWLabel::New();
  label->SetParent (temp);
  label->Create();
  label->SetText ("Data Manager");
  vtkKWPushButton* button = vtkKWPushButton::New();
  button->SetParent (temp);
  button->Create();
  button->SetImageToPixels(image_popup, image_popup_width, image_popup_height, image_popup_pixel_size, image_popup_length); 
  button->SetCommand (this, "PopupManager");
  button->SetBalloonHelpString("Popup the Data Manager");

  this->Script("pack %s -side left -anchor nw -expand n", 
	       label->GetWidgetName());
  this->Script("pack %s -side right -anchor ne -expand n", 
	       button->GetWidgetName());
  this->Script("pack %s -side top -fill x -anchor nw -expand n", 
	       temp->GetWidgetName());
  label->Delete();
  temp->Delete();
  button->Delete();
  
  
  vtkKWDataManagerWidget* widget = vtkKWDataManagerWidget::New();
  widget->SetParent(this->PanelSplitFrame->GetFrame2());
  widget->SetParentObject (this);
  widget->Create();
  widget->SetHeight (60);
  
  this->Script("pack %s -fill both -anchor nw -side top -expand yes", 
	       widget->GetWidgetName());
  
  widget->Update();
  this->SetDataManagerWidget (widget);
  widget->Delete();
  
}

//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::PopupManager ()
{

  if (!this->ManagerDialog->IsCreated())
  {
    this->ManagerDialog->SetMasterWindow(this);
    this->ManagerDialog->SetApplication(this->GetApplication());
    this->ManagerDialog->Create();
    this->ManagerDialog->SetTitle("The Data Manager");
    this->ManagerDialog->SetDeleteWindowProtocolCommand(this, "PopdownManager");
  }
  
  this->ManagerDialog->SetDeleteWindowProtocolCommand(this, "PopdownManager");
  vtkKWDataManagerWidget* widget = vtkKWDataManagerWidget::New();
  widget->SetParent(this->ManagerDialog);
  widget->Create();
  this->Script("pack %s -fill both -side top -expand t", 
	       widget->GetWidgetName());
  
  widget->SetParentObject (this);
  widget->Update();
  this->SetDataManagerWidget (widget);
  widget->Delete();

  
  
  // Get the position of the mouse, the size of the top level window.

  int px, py, tw, th, sw, sh;

  vtkKWTkUtilities::GetMousePointerCoordinates(this, &px, &py);
  vtkKWTkUtilities::GetWidgetRequestedSize(this->ManagerDialog, &tw, &th);
  vtkKWTkUtilities::GetScreenSize(this, &sw, &sh);

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

  this->ManagerDialog->SetPosition(px, py);
  this->ManagerDialog->DeIconify();
  this->ManagerDialog->Raise();
  
  this->PanelSplitFrame->Frame2VisibilityOff ();
  this->PanelSplitFrame->GetFrame2()->UnpackChildren();  

}



//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::RemoveMetaDataSet (vtkMetaDataSet* metadataset, bool popup)
{

  if (!metadataset)
    return;
  
  
  if (this->GetPage(metadataset->GetTag()))
  {
//     this->GetPage(metadataset->GetTag())->RemoveDataSet (metadataset->GetDataSet());
    this->GetPage(metadataset->GetTag())->RemoveMetaDataSet (metadataset);
  }
  
  if (metadataset->GetType() == vtkMetaDataSet::VTK_META_IMAGE_DATA)
  {
    if (this->GetPage(metadataset->GetTag()))
      this->RemovePage(metadataset->GetTag());

    this->PreviewPage->RemovePreviewImage (vtkImageData::SafeDownCast(metadataset->GetDataSet() ));

  }

  this->DataManager->RemoveMetaDataSet (metadataset);
}



//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::OnMenuFileSaveDICOM ()
{
  
  
  // vtkKWDICOMExporter *dlg = vtkKWDICOMExporter::New();
  // dlg->SetMasterWindow (this);
  
  // dlg->Create();
  // dlg->SetDisplayPositionToDefault();
  // dlg->SetPosition (100,100);


  // dlg->SetDataManager(this->GetDataManager());
  
  // dlg->Invoke(); 

  // if (dlg->GetStatus() != vtkKWDialog::StatusOK)
  // {
  //   dlg->Delete();
  //   return;
  // }


  // dlg->Delete();


}

//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::OnSelectInteraction()
{
  
  if (!this->GetCurrentPage())
  {
    this->PreviewPage->SetInteractionMode(vtkInteractorStyleImageView2D::InteractionTypeSlice);
    return;
  }

  this->GetCurrentPage()->GetPool()->SyncSetLeftButtonInteractionStyle(vtkInteractorStyleImageView2D::InteractionTypeWindowLevel);
  this->GetCurrentPage()->GetPool()->SyncSetMiddleButtonInteractionStyle(vtkInteractorStyleImageView2D::InteractionTypePan);
  this->GetCurrentPage()->GetPool()->SyncSetRightButtonInteractionStyle(vtkInteractorStyleImageView2D::InteractionTypeZoom);
  this->GetCurrentPage()->GetPool()->SyncSetKeyboardInteractionStyle(vtkInteractorStyleImageView2D::InteractionTypeSlice);
}
//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::OnWindowLevelInteraction()
{
  if (!this->GetCurrentPage())
  {
    this->PreviewPage->SetInteractionMode (vtkInteractorStyleImageView2D::InteractionTypeWindowLevel);
    return;
  }
  
  this->GetCurrentPage()->GetPool()->SyncSetInteractionStyle(vtkInteractorStyleImageView2D::InteractionTypeWindowLevel);
}
//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::OnZoomInteraction()
{
  if (!this->GetCurrentPage())
  {
    this->PreviewPage->SetInteractionMode (vtkInteractorStyleImageView2D::InteractionTypeZoom);
    return;
  }
  
  this->GetCurrentPage()->GetPool()->SyncSetInteractionStyle(vtkInteractorStyleImageView2D::InteractionTypeZoom);
  this->GetCurrentPage()->GetPool()->SyncSetMiddleButtonInteractionStyle(vtkInteractorStyleImageView2D::InteractionTypePan);
}
//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::OnRenderingModeToVR()
{
  if (!this->GetCurrentPage())
    return;
  if ( this->GetCurrentPage()->GetView4()->GetRenderingMode() == vtkImageView3D::VOLUME_RENDERING)
    this->GetCurrentPage()->GetView4()->SetRenderingModeToPlanar();
  else
    this->GetCurrentPage()->GetView4()->SetRenderingModeToVR();
    
  this->GetCurrentPage()->GetView4()->Render();


}

//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::OnFullScreenView4()
{
  if (!this->GetCurrentPage())
    return;

  this->GetCurrentPage()->ToggleFullScreenView4();

}

//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::OnFullScreenAxial()
{
  if (!this->GetCurrentPage())
  {
    // this->PreviewPage->SetViewOrientationMode (vtkImageView2D::VIEW_ORIENTATION_AXIAL);
    return;
  }
  
  this->GetCurrentPage()->ToggleFullScreenAxial();
}

//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::OnFullScreenCoronal()
{
  if (!this->GetCurrentPage())
  {
    // this->PreviewPage->SetViewOrientationMode (vtkImageView::VIEW_ORIENTATION_CORONAL);
    return;
  }
  
  this->GetCurrentPage()->ToggleFullScreenCoronal();
}

//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::OnFullScreenSagittal()
{
  if (!this->GetCurrentPage())
  {
    // this->PreviewPage->SetViewOrientationMode (vtkImageView::VIEW_ORIENTATION_SAGITTAL);
    return;
  }
  this->GetCurrentPage()->ToggleFullScreenSagittal();
}


//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::OnIllustrationMode()
{
  // if ( !IllustrationMode && !ModifiedPage )
  // {
  //   // Turn on illustration mode
  //   ModifiedPage = this->GetCurrentPage();
  //   IllustrationMode = true;

  //   ShowAnnotations = ModifiedPage->GetView1()->GetShowAnnotations();
  //   ShowRulerWidget = ModifiedPage->GetView1()->GetRulerWidgetVisibility();
  //   Show2DAxis      = ModifiedPage->GetView1()->GetShow2DAxis();
  //   ShowScalarBar   = ModifiedPage->GetView1()->GetScalarBarVisibility();
  //   ShowDirections  = ModifiedPage->GetView1()->GetShowDirections();

  //   ModifiedPage->GetView1()->SetBackgroundColor(255, 255, 255);
  //   ModifiedPage->GetView1()->ShowAnnotationsOff();
  //   ModifiedPage->GetView1()->SetRulerWidgetVisibility(0);
  //   ModifiedPage->GetView1()->Show2DAxisOff();
  //   ModifiedPage->GetView1()->SetScalarBarVisibility(0);
  //   ModifiedPage->GetView1()->SetShowDirections(0);
  //   ModifiedPage->GetView1()->Render();

  //   ModifiedPage->GetView2()->SetBackgroundColor(255, 255, 255);
  //   ModifiedPage->GetView2()->ShowAnnotationsOff();
  //   ModifiedPage->GetView2()->SetRulerWidgetVisibility(0);
  //   ModifiedPage->GetView2()->Show2DAxisOff();
  //   ModifiedPage->GetView2()->SetScalarBarVisibility(0);
  //   ModifiedPage->GetView2()->SetShowDirections(0);
  //   ModifiedPage->GetView2()->Render();

  //   ModifiedPage->GetView3()->SetBackgroundColor(255, 255, 255);
  //   ModifiedPage->GetView3()->ShowAnnotationsOff();
  //   ModifiedPage->GetView3()->SetRulerWidgetVisibility(0);
  //   ModifiedPage->GetView3()->Show2DAxisOff();
  //   ModifiedPage->GetView3()->SetScalarBarVisibility(0);
  //   ModifiedPage->GetView3()->SetShowDirections(0);
  //   ModifiedPage->GetView3()->Render();

  //   ModifiedPage->GetView4()->SetBackgroundColor(255, 255, 255);
  //   ModifiedPage->GetView4()->ShowAnnotationsOff();
  //   ModifiedPage->GetView4()->SetScalarBarVisibility(0);
  //   ModifiedPage->GetView4()->Render();
  // }
  // else
  // {
  //   ModifiedPage->GetView1()->SetBackgroundColor(0, 0, 0);
  //   ModifiedPage->GetView1()->SetShowAnnotations(ShowAnnotations);
  //   ModifiedPage->GetView1()->SetRulerWidgetVisibility(ShowRulerWidget);
  //   ModifiedPage->GetView1()->SetShow2DAxis(Show2DAxis);
  //   ModifiedPage->GetView1()->SetScalarBarVisibility(ShowScalarBar);
  //   ModifiedPage->GetView1()->SetShowDirections(ShowDirections);
  //   ModifiedPage->GetView1()->Render();

  //   ModifiedPage->GetView2()->SetBackgroundColor(0, 0, 0);
  //   ModifiedPage->GetView2()->SetShowAnnotations(ShowAnnotations);
  //   ModifiedPage->GetView2()->SetRulerWidgetVisibility(ShowRulerWidget);
  //   ModifiedPage->GetView2()->SetShow2DAxis(Show2DAxis);
  //   ModifiedPage->GetView2()->SetScalarBarVisibility(ShowScalarBar);
  //   ModifiedPage->GetView2()->SetShowDirections(ShowDirections);
  //   ModifiedPage->GetView2()->Render();

  //   ModifiedPage->GetView3()->SetBackgroundColor(0, 0, 0);
  //   ModifiedPage->GetView3()->SetShowAnnotations(ShowAnnotations);
  //   ModifiedPage->GetView3()->SetRulerWidgetVisibility(ShowRulerWidget);
  //   ModifiedPage->GetView3()->SetShow2DAxis(Show2DAxis);
  //   ModifiedPage->GetView3()->SetScalarBarVisibility(ShowScalarBar);
  //   ModifiedPage->GetView3()->SetShowDirections(ShowDirections);
  //   ModifiedPage->GetView3()->Render();

  //   ModifiedPage->GetView4()->SetBackgroundColor(255, 255, 255);
  //   ModifiedPage->GetView4()->SetShowAnnotations(ShowAnnotations);
  //   ModifiedPage->GetView4()->SetScalarBarVisibility(ShowScalarBar);
  //   ModifiedPage->GetView4()->Render();

  //   IllustrationMode = false;
  //   ModifiedPage = NULL;
  // }
  if (this->GetCurrentPage())
    this->GetCurrentPage()->Render();
}

//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::OnLandmarkDialog()
{
  if (!this->GetCurrentPage())
    return;
  
  if (!this->LandmarkManagerDialog->IsCreated())
  {
    this->LandmarkManagerDialog->SetMasterWindow(this);
    this->LandmarkManagerDialog->SetApplication(this->GetApplication());
    this->LandmarkManagerDialog->Create();
    this->LandmarkManagerDialog->SetTitle("Landmark mamager");
    this->LandmarkManagerDialog->SetDeleteWindowProtocolCommand(this->LandmarkManagerDialog, "Withdraw");
  }

  if (this->LandmarkManagerDialog->IsMapped())
    return;
  

  this->LandmarkManagerDialog->UnpackChildren();

  std::ostringstream os;
  os << this->GetCurrentPage()->GetTag()
     << " : Landmark Manager";
  
  this->LandmarkManagerDialog->SetTitle(os.str().c_str());

  
  vtkKWLandmarkManagerWidget* widget = vtkKWLandmarkManagerWidget::New();
  widget->SetParent(this->LandmarkManagerDialog);
  widget->Create();
  widget->SetLandmarkManager(this->GetCurrentPage()->GetLandmarkManager());
  //   widget->SetLandmarkChangedCommand (this->GetCurrentPage(), "Render");
  widget->Update();

  this->Script("pack %s -fill both -side top -expand t", 
	       widget->GetWidgetName());
  
  
  widget->Delete();

  
  this->LandmarkManagerDialog->DeIconify();
  this->LandmarkManagerDialog->Raise();
}



//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::OnSnapshotHandler(bool exportmovie)
{
  if (!this->GetCurrentPage())
    return;

  
  if (!this->SnapshotHandler->IsCreated())
  {
    this->SnapshotHandler->SetMasterWindow(this);
    this->SnapshotHandler->SetApplication(this->GetApplication());
    this->SnapshotHandler->Create();
    //this->SnapshotHandler->SetDeleteWindowProtocolCommand(this->SnapshotHandler, "OK");

  }

  if (this->SnapshotHandler->IsMapped())
  {
    this->SnapshotHandler->Withdraw();
    return;
  }
  
  
  if (!this->GetCurrentPage()->GetIsFullScreen())
  {
    vtkKWPopupWarningMessage(this, "Only available in full screen mode\n Select a view !")
    return;
  }

  vtkImageView2D* view = vtkImageView2D::SafeDownCast (this->GetCurrentPage()->GetActiveView());
  if (view)
  {
    char buffer[1024];
    bool valid = this->GetApplication()->GetRegistryValue(1, "RunTime", "ShowAxesWhenExport", buffer);
    if (!valid)
      view->ShowImageAxisOff();
    else
    {
      if (*buffer)
      {
	std::istringstream is;
	is.str (buffer);
	int val = 0;
	is >> val;
	view->SetShowImageAxis(val);
      }
    }
  }
  
  if (!exportmovie)
  {

    vtkKWSnapshotHandler::Snap (this->GetCurrentPage()->GetActiveRenderWidget());
    
    if (view)
      view->ShowImageAxisOn();
    
    return;
  }

  unsigned int maxnumber = this->GetDataManager()->GetSequencesMaxNumber();

  if (this->GetCurrentPage()->GetIsFullScreen() == 4)
  {
    this->SnapshotHandler->AllowSliceTypeOff();
    this->SnapshotHandler->AllowCameraTypeOn();
    if (this->SnapshotHandler->GetSnapshotType() == vtkKWSnapshotHandler::SNAPSHOTTYPE_SLICE)
      this->SnapshotHandler->SetSnapshotType(vtkKWSnapshotHandler::SNAPSHOTTYPE_CAMERA);
  }
  else
  {
    this->SnapshotHandler->AllowSliceTypeOn();
    this->SnapshotHandler->AllowCameraTypeOff();
    if (this->SnapshotHandler->GetSnapshotType() == vtkKWSnapshotHandler::SNAPSHOTTYPE_CAMERA)
      this->SnapshotHandler->SetSnapshotType(vtkKWSnapshotHandler::SNAPSHOTTYPE_SLICE);
  }
  
  if (maxnumber == 0)
    this->SnapshotHandler->AllowTimeTypeOff();
  else
    this->SnapshotHandler->AllowTimeTypeOn();
  
  this->SnapshotHandler->SetView(this->GetCurrentPage()->GetActiveView());
  this->SnapshotHandler->SetRenderWidget(this->GetCurrentPage()->GetActiveRenderWidget());
  this->SnapshotHandler->SetTimeUpdateCommand(this, "UpdateToTime");
  double timemin = this->GetDataManager()->GetSequencesRangeMin();
  double timemax = this->GetDataManager()->GetSequencesRangeMax();  
  this->SnapshotHandler->SetTimeSettings (timemin, timemax, maxnumber);
  
  this->SnapshotHandler->Update();
   
  this->SnapshotHandler->Invoke();

  if (view)
    view->ShowImageAxisOn();

}


//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::UpdateToTime(double time)
{

  vtkKWPageView* page = this->GetCurrentPage();
  if (!page)
  {
    vtkKWPreviewPage* preview = this->GetPreviewPage();
    if (preview->IsMapped())
    {
      this->GetDataManager()->UpdateSequencesToTime (time);  
    }
  }
  else
  {
    this->GetDataManager()->UpdateSequencesMatchingTagToTime (page->GetTag(), time);
    
    vtkLandmarkManager* landmarkmanager = page->GetLandmarkManager();
    if (landmarkmanager->GetNumberOfLandmarks())
    {
      double newpos[3] = {0,0,0};
      vtkLandmark* landmark = 0;
      double id = -1;
      vtkMetaDataSet* metadataset = this->ManagerWidget->GetSelectedMetaDataSet();
      if (metadataset && metadataset->GetDataSet())
      {
	for (unsigned int i=0; i<landmarkmanager->GetNumberOfLandmarks(); i++)
	{
	  landmark = landmarkmanager->GetLandmark (i);
	  if (!landmark->HasFlag ("PointIdToFollow"))
	    continue;
	  
	  id = landmark->GetFlag ("PointIdToFollow");
	  metadataset->GetDataSet()->GetPoint ((vtkIdType)(id), newpos);
	  landmark->SetPosition (newpos);
	}
	
	landmarkmanager->UpdateLinker();
      }
    }
  }
}

//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::Render()
{
  vtkKWPageView* page = this->GetCurrentPage();
  if (page)
    page->Render();
  else if (this->GetPreviewPage()->IsMapped())
    this->GetPreviewPage()->Render();
}


//----------------------------------------------------------------------------
void vtkKWMainWindowInteractor::SelectMetaDataSet(vtkMetaDataSet* metadataset)
{
  this->ManagerWidget->SetSelectedMetaDataSet (metadataset);
  
  vtkKWPageView* page = this->GetPage(metadataset->GetTag());      
  if (page)
  {
    this->GetViewNotebook()->RaisePage(metadataset->GetTag());
    this->GetViewNotebook()->RaiseCallback(this->GetViewNotebook()->GetRaisedPageId());
  }
  
  std::vector<vtkKWToolBox*> tblist = this->GetToolBoxList();
  
  for (unsigned int j=0; j<tblist.size(); j++)
  {
    if (page)
    {
      tblist[j]->SetMetaDataSetModifiedCommand(page, "Render");
    }
    tblist[j]->SetMetaDataSet (metadataset);
  }
}
