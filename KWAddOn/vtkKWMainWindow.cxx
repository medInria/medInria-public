/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWMainWindow.cxx 1311 2009-11-02 14:07:46Z tmansi $
Language:  C++
Author:    $Author: tmansi $
Date:      $Date: 2009-11-02 15:07:46 +0100 (Mon, 02 Nov 2009) $
Version:   $Revision: 1311 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkKWMainWindow.h"

#include "vtkImageData.h"
#include "vtkViewImage3D.h"
#include "vtkViewImage2D.h"
#include <vtkRenderWindowInteractor.h>
#include <vtkKWSplitFrame.h>
#include <vtkKWEvent.h>
#include "vtkKWToolbar.h"
#include "vtkKWToolbarSet.h"
#include "vtkKWPushButton.h"
#include "vtkKWIcon.h"
#include <vtkKWFrameWithLabel.h>
#include "vtkKWApplication.h"
#include "vtkKWPageView.h"
#include "vtkKWMenu.h"
#include "vtkKWMenuButton.h"
#include <vtkKWMenuButtonWithLabel.h>
#include <vtkKWPushButtonWithMenu.h>

#include "vtkKWNotebook.h"
#include <vtkKWSplashScreen.h>

#include <pixmap/KWAddOnResources.h>

#include <vtkLandmarkManager.h>
#include <vtkKWLandmarkManagerWidget.h>
#include <vtkKWTopLevel.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWMainWindow );
vtkCxxRevisionMacro( vtkKWMainWindow, "$Revision: 1311 $");

//----------------------------------------------------------------------------
vtkKWMainWindow::vtkKWMainWindow()
{
  this->OpenToolbar = vtkKWToolbar::New();
  this->ViewToolbar = vtkKWToolbar::New();

  this->PageNumberIncrement = 0;
  
}

//----------------------------------------------------------------------------
vtkKWMainWindow::~vtkKWMainWindow()
{
  std::cout<<"deleting mainwindow"<<std::endl;
  
  this->OpenToolbar->Delete();
  this->ViewToolbar->Delete();

  
  
}

//----------------------------------------------------------------------------
void vtkKWMainWindow::CreateOpenToolbar()
{
  this->OpenToolbar->SetName("Main ToolBar");
  this->OpenToolbar->SetParent(this->GetMainToolbarSet()->GetToolbarsFrame());
  this->OpenToolbar->Create();
  
  this->GetMainToolbarSet()->AddToolbar(this->OpenToolbar);

  vtkKWPushButton* button0= vtkKWPushButton::New();
  button0->SetParent(this->OpenToolbar->GetFrame());
  button0->Create();
  button0->SetImageToPixels(image_fileopen, image_fileopen_width, image_fileopen_height, image_fileopen_pixel_size, image_fileopen_length);
  button0->SetCommand(this, "OnMenuFileOpen");
  button0->SetBalloonHelpString("Open file(s)... (Ctrl+O)");
  this->OpenToolbar->AddWidget(button0);
  button0->Delete();

//   vtkKWPushButton* button1= vtkKWPushButton::New();
//   button1->SetParent(this->OpenToolbar->GetFrame());
//   button1->Create();
//   button1->SetImageToPixels(image_filesave, image_filesave_width, image_filesave_height, image_filesave_pixel_size, image_filesave_length);
//   button1->SetCommand(this, "SaveManagerCallback");
//   button1->SetBalloonHelpString("Save the manager into file...");
//   this->OpenToolbar->AddWidget(button1);
//   button1->Delete();
  
  vtkKWPushButtonWithMenu *button1 = vtkKWPushButtonWithMenu::New();
  button1->SetParent(this->OpenToolbar->GetFrame());
  button1->Create();
  button1->GetPushButton()->SetImageToPixels(image_filesave, image_filesave_width, image_filesave_height, image_filesave_pixel_size, image_filesave_length);
  button1->GetPushButton()->SetCommand(this, "OnDataSetSave");
  button1->SetBalloonHelpString(
    "Save data into file (Ctrl+S)");

  vtkKWMenu *menu = button1->GetMenu();
  menu->AddCommand("Save selected dataset into file... (Ctrl+S)", this, "OnDataSetSave");
  menu->AddCommand("Save environement into file...", this, "SaveManagerCallback");

  
  this->OpenToolbar->AddWidget(button1);
  button1->Delete();

  vtkKWPushButtonWithMenu *button2 = vtkKWPushButtonWithMenu::New();
  button2->SetParent(this->OpenToolbar->GetFrame());
  button2->Create();
  button2->GetPushButton()->SetImageToPixels(image_cdload, image_cdload_width, image_cdload_height, image_cdload_pixel_size, image_cdload_length);
  button2->GetPushButton()->SetCommand(this, "OnMenuFileOpenDICOM");
  button2->SetBalloonHelpString(
    "Import a DICOM exam");

  vtkKWMenu *menu2 = button2->GetMenu();
  menu2->AddCommand("Import ...", this, "OnMenuFileOpenDICOM");
  menu2->AddCommand("Export ...", this, "OnMenuFileSaveDICOM");

  
  this->OpenToolbar->AddWidget(button2);
  button2->Delete();
  
  vtkKWPushButton* button000= vtkKWPushButton::New();
  button000->SetParent(this->OpenToolbar->GetFrame());
  button000->Create();
  button000->SetImageToPixels(image_mplayer, image_mplayer_width, image_mplayer_height, image_mplayer_pixel_size, image_mplayer_length);
  button000->SetCommand(this, "OnMenuFileOpen4D");
  button000->SetBalloonHelpString("Create Sequence... (Ctrl+Alt+O)");
  this->OpenToolbar->AddWidget(button000);
  button000->Delete();
}

//----------------------------------------------------------------------------
void vtkKWMainWindow::CreateViewToolbar()
{
  this->ViewToolbar->SetName("View ToolBar");
  this->ViewToolbar->SetParent(this->GetMainToolbarSet()->GetToolbarsFrame());
  this->ViewToolbar->Create();

  this->GetMainToolbarSet()->AddToolbar(this->ViewToolbar);

  vtkKWPushButton* button1= vtkKWPushButton::New();
  button1->SetParent(this->ViewToolbar->GetFrame());
  button1->Create();
  button1->SetImageToPixels(image_selector, image_selector_width, image_selector_height, image_selector_pixel_size, image_selector_length);
  button1->SetCommand(this, "OnSelectInteraction");
  button1->SetBalloonHelpString("Select Interaction");
  this->ViewToolbar->AddWidget(button1);
  button1->Delete();
  
  vtkKWPushButton* button2= vtkKWPushButton::New();
  button2->SetParent(this->ViewToolbar->GetFrame());
  button2->Create();
  button2->SetImageToPixels(image_greyscale, image_greyscale_width, image_greyscale_height, image_greyscale_pixel_size, image_greyscale_length);
  button2->SetCommand(this, "OnWindowLevelInteraction");
  button2->SetBalloonHelpString("Window-Level Interaction");
  this->ViewToolbar->AddWidget(button2);
  button2->Delete();

  vtkKWPushButton* button3= vtkKWPushButton::New();
  button3->SetParent(this->ViewToolbar->GetFrame());
  button3->Create();
  button3->SetImageToPixels(image_zoom, image_zoom_width, image_zoom_height, image_zoom_pixel_size, image_zoom_length);
  button3->SetCommand(this, "OnZoomInteraction");
  button3->SetBalloonHelpString("Zoom Interaction");
  this->ViewToolbar->AddWidget(button3);
  button3->Delete();

  vtkKWPushButton* button4= vtkKWPushButton::New();
  button4->SetParent(this->ViewToolbar->GetFrame());
  button4->Create();
  button4->SetImageToPixels(image_configure, image_configure_width, image_configure_height, image_configure_pixel_size, image_configure_length);
  button4->SetCommand(this, "OnLandmarkDialog");
  button4->SetBalloonHelpString("Manage your landmarks (points of interest)");
  this->ViewToolbar->AddWidget(button4);
  button4->Delete();

  vtkKWPushButton* button7= vtkKWPushButton::New();
  button7->SetParent(this->ViewToolbar->GetFrame());
  button7->Create();
  button7->SetImageToPixels(image_axial, image_axial_width, image_axial_height, image_axial_pixel_size, image_axial_length);
  button7->SetCommand(this, "OnFullScreenAxial");
  button7->SetBalloonHelpString("Axial full screen");
  this->ViewToolbar->AddWidget(button7);
  button7->Delete();

  vtkKWPushButton* button8= vtkKWPushButton::New();
  button8->SetParent(this->ViewToolbar->GetFrame());
  button8->Create();
  button8->SetImageToPixels(image_coronal, image_coronal_width, image_coronal_height, image_coronal_pixel_size, image_coronal_length);
  button8->SetCommand(this, "OnFullScreenCoronal");
  button8->SetBalloonHelpString("Coronal full screen");
  this->ViewToolbar->AddWidget(button8);
  button8->Delete();

  vtkKWPushButton* button9= vtkKWPushButton::New();
  button9->SetParent(this->ViewToolbar->GetFrame());
  button9->Create();
  button9->SetImageToPixels(image_sagittal, image_sagittal_width, image_sagittal_height, image_sagittal_pixel_size, image_sagittal_length);
  button9->SetCommand(this, "OnFullScreenSagittal");
  button9->SetBalloonHelpString("Sagittal full screen");
  this->ViewToolbar->AddWidget(button9);
  button9->Delete();

  
  vtkKWPushButton* button6= vtkKWPushButton::New();
  button6->SetParent(this->ViewToolbar->GetFrame());
  button6->Create();
  button6->SetImageToPixels(image_fullscreen, image_fullscreen_width, image_fullscreen_height, image_fullscreen_pixel_size, image_fullscreen_length);
  button6->SetCommand(this, "OnFullScreenView4");
  button6->SetBalloonHelpString("3D full screen");
  this->ViewToolbar->AddWidget(button6);
  button6->Delete();

  vtkKWPushButton* button10= vtkKWPushButton::New();
  button10->SetParent(this->ViewToolbar->GetFrame());
  button10->Create();
  button10->SetImageToPixels(image_volumerendering, image_volumerendering_width, image_volumerendering_height, image_volumerendering_pixel_size, image_volumerendering_length);
  button10->SetCommand(this, "OnRenderingModeToVR");
  button10->SetBalloonHelpString("Toggle rendering mode");
  this->ViewToolbar->AddWidget(button10);
  button10->Delete();


  vtkKWPushButtonWithMenu* button11= vtkKWPushButtonWithMenu::New();
  button11->SetParent(this->ViewToolbar->GetFrame());
  button11->Create();
  button11->GetPushButton()->SetImageToPixels(image_snapshot, image_snapshot_width, image_snapshot_height, image_snapshot_pixel_size, image_snapshot_length);
  button11->GetPushButton()->SetCommand(this, "OnSnapshotHandler 0");
  button11->GetPushButton()->SetBalloonHelpString("Take a snaphot of the current view\nor export a movie !");
  this->ViewToolbar->AddWidget(button11);
  button11->Delete();

  vtkKWMenu *menu = button11->GetMenu();
  menu->AddCommand("Quick snapshot...", this, "OnSnapshotHandler 0");
  menu->AddCommand("Export a movie...", this, "OnSnapshotHandler 1");
  menu->SetBalloonHelpString("Take a snaphot of the current view\nor export a movie !");
  

}




//----------------------------------------------------------------------------
void vtkKWMainWindow::CreateMenu()
{

  int index = -1;
  
  vtkKWMenu *file_menu = this->GetFileMenu() ;
  index = file_menu->InsertCommand(this->GetFileMenu()->GetNumberOfItems()-2, "&New Tab", this, "OnMenuNewTab");
	file_menu->SetItemAccelerator(index, "Ctrl+T");
  file_menu->SetBindingForItemAccelerator(index, file_menu->GetParentTopLevel());
  file_menu->SetItemHelpString(index, "New empty tab");

  index = file_menu->InsertCommand(this->GetFileMenu()->GetNumberOfItems()-2, "&Delete Current Tab ...", this, "OnMenuDeleteCurrentTab");
  file_menu->SetItemAccelerator(index, "Ctrl+W");
  file_menu->SetBindingForItemAccelerator(index, file_menu->GetParentTopLevel());
  file_menu->SetItemHelpString(index, "Delete the current tab from screen");

  index = file_menu->InsertCommand(this->GetFileMenu()->GetNumberOfItems()-2, "&Open ...", this, "OnMenuFileOpen");
  file_menu->SetItemAccelerator(index, "Ctrl+O");
  file_menu->SetBindingForItemAccelerator(index, file_menu->GetParentTopLevel());
  file_menu->SetItemHelpString(index, "Read and import dataset(s) from file(s)");

  index = file_menu->InsertCommand(this->GetFileMenu()->GetNumberOfItems()-2, "Open &4D ...", this, "OnMenuFileOpen4D");
  file_menu->SetItemAccelerator(index, "Ctrl+Alt+O");
  file_menu->SetBindingForItemAccelerator(index, file_menu->GetParentTopLevel());
  file_menu->SetItemHelpString(index, "Open a sequence importer for data time sequences");

  index = file_menu->InsertCommand(this->GetFileMenu()->GetNumberOfItems()-2, "&Save...", this, "OnDataSetSave");
  file_menu->SetItemAccelerator(index, "Ctrl+S");
  file_menu->SetBindingForItemAccelerator(index, file_menu->GetParentTopLevel());
  file_menu->SetItemHelpString(index, "Save the selected dataset to file");

  index = file_menu->InsertCommand(this->GetFileMenu()->GetNumberOfItems()-2, "&Delete", this, "OnDataSetDelete");
  file_menu->SetItemAccelerator(index, "Ctrl+D");
  file_menu->SetBindingForItemAccelerator(index, file_menu->GetParentTopLevel());
  file_menu->SetItemHelpString(index, "Remove the selected dataset from the data manager");


  this->InsertRecentFilesMenu(file_menu->GetNumberOfItems()-2, this);
  file_menu->InsertSeparator (file_menu->GetNumberOfItems()-2) ;
  
  index = file_menu->InsertCommand(file_menu->GetNumberOfItems()-2, "&Popup manager ...", this, "PopupManager");
  file_menu->SetItemAccelerator(index, "Ctrl+P");
  file_menu->SetBindingForItemAccelerator(index, file_menu->GetParentTopLevel());
  file_menu->SetItemHelpString(index, "pop up the data manager");

  this->GetFileMenu()->InsertSeparator (this->GetFileMenu()->GetNumberOfItems()-2) ;

  
  vtkKWMenu *window_menu = this->GetViewMenu();
  
  index = window_menu->InsertCommand( this->GetViewMenu()->GetNumberOfItems()-1, "Full-screen &Axial", this, "OnFullScreenAxial");
  window_menu->SetItemAccelerator(index, "Alt+A");
  window_menu->SetBindingForItemAccelerator(index, window_menu->GetParentTopLevel());
  window_menu->SetItemHelpString(index, "Display the axial view in full screen (toggle)");
  index = window_menu->InsertCommand( this->GetViewMenu()->GetNumberOfItems()-1, "Full-screen &Coronal", this, "OnFullScreenCoronal");
  window_menu->SetItemAccelerator(index, "Alt+C");
  window_menu->SetBindingForItemAccelerator(index, window_menu->GetParentTopLevel());
  window_menu->SetItemHelpString(index, "Display the coronal view in full screen (toggle)");
  index = window_menu->InsertCommand( this->GetViewMenu()->GetNumberOfItems()-1, "Full-screen &Sagittal", this, "OnFullScreenSagittal");
  window_menu->SetItemAccelerator(index, "Alt+S");
  window_menu->SetBindingForItemAccelerator(index, window_menu->GetParentTopLevel());
  window_menu->SetItemHelpString(index, "Display the sagittal view in full screen (toggle)");
  index = window_menu->InsertCommand( this->GetViewMenu()->GetNumberOfItems()-1, "Full-screen 3&D", this, "OnFullScreenView4");
  window_menu->SetItemAccelerator(index, "Alt+D");
  window_menu->SetBindingForItemAccelerator(index, window_menu->GetParentTopLevel());
  window_menu->SetItemHelpString(index, "Display the 3D view in full screen (toggle)");

  this->GetFileMenu()->InsertSeparator (this->GetFileMenu()->GetNumberOfItems()-2) ;

  index = window_menu->InsertCommand( this->GetViewMenu()->GetNumberOfItems()-1, "Illustration Mode On/Off", this, "OnIllustrationMode");
  window_menu->SetItemAccelerator(index, "Alt+I");
  window_menu->SetBindingForItemAccelerator(index, window_menu->GetParentTopLevel());
  window_menu->SetItemHelpString(index, "Turn On/Off the annotations in all the views. Background color is set to white (toggle)");

  this->GetViewMenu()->InsertSeparator (this->GetViewMenu()->GetNumberOfItems()-1) ;
  
  this->GetWindowMenu()->SetItemAccelerator(6, "");


//   custom_menu->AddCommand ("&Parameters...", this, "OnDataParameters");

//   vtkKWMenu *colorby_menu = vtkKWMenu::New();
//   colorby_menu->SetParent(custom_menu);
//   colorby_menu->Create();
//   colorby_menu->AddRadioButton ("Surface");
//   custom_menu->AddCascade ("&Color by", colorby_menu);
//   colorby_menu->Delete();

//   custom_menu->AddCommand ("&Duplicate", this, "OnDataDuplicate");
//   custom_menu->AddCommand ("&Save...", this, "OnDataSave");
//   custom_menu->AddCommand ("&Delete", this, "OnDataDelete");
//   custom_menu->Delete();
  
}



//----------------------------------------------------------------------------
void vtkKWMainWindow::CreateWidget()
{
  // Check if already created

  if (this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }
  
  // Call the superclass to create the whole widget
  this->Superclass::CreateWidget();


  vtkKWApplication *app = this->GetApplication();
  
  if (app->GetSplashScreenVisibility()) 
  {
    app->GetSplashScreen()->SetProgressMessage("Creating toolbars...");
  }
  this->CreateOpenToolbar();
  this->CreateViewToolbar();
  
  if (app->GetSplashScreenVisibility()) 
  {
    app->GetSplashScreen()->SetProgressMessage("Creating menus...");
  }
  this->CreateMenu();
  
  this->PackSelf();

  this->SetDisplayPositionToDefault();

  this->GetMainSplitFrame()->SetFrame1Size(250);

  this->SetPosition (50,50);

//   this->GetViewNotebook()->ShowOnlyMostRecentPagesOn();
//   this->GetViewNotebook()->SetNumberOfMostRecentPages(6);
  
}

void vtkKWMainWindow::PackSelf()
{
  if (!this->IsCreated())
    return;  
}




vtkKWPageView* vtkKWMainWindow::GetCurrentPage (void)
{
  vtkKWFrame* page = this->GetViewNotebook()->GetFrame (this->GetViewNotebook()->GetRaisedPageId ());
  if (!page)
    return NULL;
  vtkKWPageView* res =
    vtkKWPageView::SafeDownCast(page->GetNthChild (0));
  if (res)
    return res;
  return NULL;
}


vtkKWPageView* vtkKWMainWindow::GetPage (const char* title)
{
  vtkKWFrame* page = this->GetViewNotebook()->GetFrame (title);
  if (!page)
    return NULL;
  vtkKWPageView* res =
    vtkKWPageView::SafeDownCast(page->GetNthChild (0));
  if (res)
    return res;
  return NULL;
}

std::vector<vtkKWPageView*> vtkKWMainWindow::GetPageList ()
{
  std::vector<vtkKWPageView*> ret;

  for (unsigned int i=0; i<this->PageNumberIncrement; i++)
  {
    
    vtkKWFrame* page = this->GetViewNotebook()->GetFrame (i);
    if (!page)
      continue;
    
    vtkKWPageView* res =
      vtkKWPageView::SafeDownCast(page->GetNthChild (0));
    if (res)
      ret.push_back(res);
  }
  
  return ret;
}


void vtkKWMainWindow::RaisePage (const char* title)
{
  this->GetViewNotebook()->RaisePage (title);
  return;
}


//----------------------------------------------------------------------------
void vtkKWMainWindow::OnMenuNewTab()
{
  std::cout<<"not implemented yet"<<std::endl;
}

//----------------------------------------------------------------------------
void vtkKWMainWindow::OnMenuDeleteCurrentTab()
{
  std::cout<<"not implemented yet"<<std::endl;
}

  
//----------------------------------------------------------------------------
void vtkKWMainWindow::OnMenuFileOpen()
{
  std::cout<<"not implemented yet"<<std::endl;
}

//----------------------------------------------------------------------------
void vtkKWMainWindow::OnMenuFileOpen4D()
{
  std::cout<<"not implemented yet"<<std::endl;
}

//----------------------------------------------------------------------------
void vtkKWMainWindow::OnMenuFileOpenDICOM()
{
  std::cout<<"not implemented yet"<<std::endl;
}

//----------------------------------------------------------------------------
void vtkKWMainWindow::OnMenuFileSaveDICOM()
{
  std::cout<<"not implemented yet"<<std::endl;
}



//----------------------------------------------------------------------------
void vtkKWMainWindow::OnDataSetSave(void)
{
  std::cout<<"not implemented yet"<<std::endl;
}


//----------------------------------------------------------------------------
void vtkKWMainWindow::OnDataSetDelete(void)
{
  std::cout<<"not implemented yet"<<std::endl;
}



//----------------------------------------------------------------------------
void vtkKWMainWindow::SaveManagerCallback(void)
{
  std::cout<<"not implemented yet"<<std::endl;
}

