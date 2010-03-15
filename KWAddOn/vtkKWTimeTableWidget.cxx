/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWTimeTableWidget.cxx 501 2007-11-28 15:45:48Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2007-11-28 16:45:48 +0100 (Wed, 28 Nov 2007) $
Version:   $Revision: 1 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <vtkKWTimeTableWidget.h>
#include "vtkObjectFactory.h"

#include <vtksys/SystemTools.hxx>

#include <vtkKWPushButton.h>
#include <vtkKWApplication.h>
#include <vtkKWEntryWithLabel.h>
#include <vtkKWEntry.h>
#include <vtkKWTkUtilities.h>

#include <pixmap/KWAddOnResources.h>
#include <vtkKWDialog.h>
#include <vtkKWIcon.h>

#include <vtkXYPlotActor.h>
#include <vtkKWRenderWidget.h>
#include <vtkXYPlotActor.h>
#include <vtkMetaDataSetSequence.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkDataSetCollection.h>
#include <vtkRenderer.h>
#include <vtkProperty2D.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkLineSource.h>
#include <vtkAxisActor2D.h>
#include <vtkCellArray.h>
#include <vtkCellType.h>
#include <vtkCamera.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkKWFrame.h>
#include <vtkKWEntry.h>
#include <vtkKWMultiColumnList.h>
#include <vtkKWMultiColumnListWithScrollbars.h>
#include <vtkActorCollection.h>
#include <vtkDataArrayCollection.h>
#include <vtkPolyDataCollection.h>
#include <vtkKWPushButton.h>
#include <vtkKWPushButtonWithMenu.h>
#include <vtkKWMenu.h>
#include "vtkMath.h"
#include <vtkKWSnapshotHandler.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkKWLoadSaveDialog.h>
#include <kwcommon.h>
#include <vtkCommand.h>
#include <time.h>
#include <math.h>

//extern int vtkrint (double a);

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWTimeTableWidget );
vtkCxxRevisionMacro( vtkKWTimeTableWidget, "$Revision: 1 $");

//----------------------------------------------------------------------------
vtkKWTimeTableWidget::vtkKWTimeTableWidget()
{
  this->TimeSource = vtkLineSource::New();
  this->RenderWidget = vtkKWRenderWidget::New();
  this->Sequence = NULL;
  this->RangeX[0] = 0;
  this->RangeX[1] = 0;
  this->RangeY[0] = 0;
  this->RangeY[1] = 0;
  this->ValuesScaleFactor = 1;
  

  this->GraphFrame = vtkKWFrame::New();
  this->DataFrame = vtkKWFrame::New();
  this->MultiColumnList = NULL;

  this->ActorCollection = vtkActorCollection::New();
  this->ArrayCollection = vtkDataArrayCollection::New();
  this->PolyDataCollection = vtkPolyDataCollection::New();

  this->SnapshotHandler = vtkKWSnapshotHandler::New();

  this->HorizontalGrid = vtkCollection::New();
  this->VerticalGrid = vtkCollection::New();
  this->HorizontalGridActors = vtkActorCollection::New();
  this->VerticalGridActors = vtkActorCollection::New();
  this->ScaleBar = vtkPolyData::New();
  
}

//----------------------------------------------------------------------------
vtkKWTimeTableWidget::~vtkKWTimeTableWidget()
{
  this->TimeSource->Delete();
  this->RenderWidget->SetParent (NULL);
  this->RenderWidget->Delete();

  this->GraphFrame->Delete();
  this->DataFrame->Delete();

  this->ActorCollection->Delete();
  this->ArrayCollection->Delete();
  this->PolyDataCollection->Delete();

  this->SnapshotHandler->Delete();

  this->HorizontalGrid->Delete();
  this->VerticalGrid->Delete();
  this->HorizontalGridActors->Delete();
  this->VerticalGridActors->Delete();
  this->ScaleBar->Delete();
  
  
  
}


//----------------------------------------------------------------------------
void vtkKWTimeTableWidget::CreateWidget()
{
  // Check if already created
  if (this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }
  
  // Call the superclass to create the whole widget
  this->Superclass::CreateWidget();

  this->SetWidth (400);
  this->SetHeight (40);
  
  this->SetBorderWidth(2);
  this->SetReliefToFlat();


  this->GraphFrame->SetParent (this);
  this->GraphFrame->Create();
  
  this->RenderWidget->SetParent (this->GraphFrame);
  this->RenderWidget->Create();

  this->RenderWidget->SetWidth (700);

  this->RenderWidget->GetRenderer()->SetBackground (0.9, 0.9, 0.9);

  vtkInteractorStyleSwitch* switcher = vtkInteractorStyleSwitch::SafeDownCast (this->RenderWidget->GetRenderWindowInteractor()->GetInteractorStyle());
  if (switcher)
  {
    this->RenderWidget->GetRenderWindowInteractor()->RemoveObservers (vtkCommand::LeftButtonPressEvent);
    switcher->SetCurrentStyleToTrackballCamera();
  }
  else
    this->RenderWidget->GetRenderWindowInteractor()->Disable();
  
  this->TimeSource->SetPoint1(0,0,0);   
  this->TimeSource->SetPoint2(0,1,0);  

  vtkPolyDataMapper* mapper =  vtkPolyDataMapper::New();
  mapper->SetInputConnection( this->TimeSource->GetOutputPort() );
  vtkActor* actor = vtkActor::New();
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor (1.0,0.0,0.0);
  actor->GetProperty()->SetLineWidth (2);
  actor->SetVisibility (1);

  this->RenderWidget->GetRenderer()->AddActor (actor);

  actor->Delete();
  mapper->Delete();


  for (unsigned int i=0; i<5; i++)
  {
    vtkLineSource* line = vtkLineSource::New();
    line->SetResolution (10);
    line->SetPoint1 (0,0,0);
    line->SetPoint2 (1,0,0);
    this->HorizontalGrid->AddItem (line);
    vtkPolyDataMapper* linemapper =  vtkPolyDataMapper::New();
    linemapper->SetInputConnection( line->GetOutputPort() );
    vtkActor* lineactor = vtkActor::New();
    lineactor->SetMapper(linemapper);
    lineactor->GetProperty()->SetColor (0.5,0.5,0.5);
    lineactor->GetProperty()->SetLineWidth (2);
    lineactor->SetVisibility (0);
    this->HorizontalGridActors->AddItem (lineactor);
    this->RenderWidget->GetRenderer()->AddActor (lineactor);
    lineactor->Delete();
    linemapper->Delete();
    line->Delete();
  }
  for (unsigned int i=0; i<10; i++)
  {
    vtkLineSource* line = vtkLineSource::New();
    line->SetResolution (10);
    line->SetPoint1 (0,0,0);
    line->SetPoint2 (0,1,0);
    this->VerticalGrid->AddItem (line);
    vtkPolyDataMapper* linemapper =  vtkPolyDataMapper::New();
    linemapper->SetInputConnection( line->GetOutputPort() );
    vtkActor* lineactor = vtkActor::New();
    lineactor->SetMapper(linemapper);
    lineactor->GetProperty()->SetColor (0.5,0.5,0.5);
    lineactor->GetProperty()->SetLineWidth (2);
    lineactor->SetVisibility (0);
    this->VerticalGridActors->AddItem (lineactor);
    this->RenderWidget->GetRenderer()->AddActor (lineactor);
    lineactor->Delete();
    linemapper->Delete();
    line->Delete();
  }
    

  this->Script ("pack %s -side left -fill both -anchor nw -expand yes -padx 5 -pady 5",
		this->GraphFrame->GetWidgetName());
  this->Script ("pack %s -side left -fill both -anchor nw -expand yes -padx 0 -pady 0",
		this->RenderWidget->GetWidgetName());
  

  this->DataFrame->SetParent (this);
  this->DataFrame->Create();

  vtkKWMultiColumnListWithScrollbars* mclsb = vtkKWMultiColumnListWithScrollbars::New();
  mclsb->SetParent (this->DataFrame);
  mclsb->Create();
  mclsb->HorizontalScrollbarVisibilityOff();
  this->MultiColumnList = mclsb->GetWidget();

  this->MultiColumnList->MovableColumnsOn();
  this->MultiColumnList->SetWidth(40);
  this->MultiColumnList->SetHeight(8);
  this->MultiColumnList->SetSelectionBackgroundColor(1,0.9647,0.6314);  
  this->MultiColumnList->SetSelectionForegroundColor (0,0,0);
  this->MultiColumnList->SetSelectionModeToBrowse ();

  int col_index;
  
  // Add the columns (make some of them editable)
  
  col_index = this->MultiColumnList->AddColumn("Name");
  this->MultiColumnList->SetColumnEditable(col_index, 0);  

  col_index = this->MultiColumnList->AddColumn(NULL);
  this->MultiColumnList->SetColumnFormatCommandToEmptyOutput(col_index);
  this->MultiColumnList->SetColumnLabelImageToPredefinedIcon(col_index, vtkKWIcon::IconEye);
  this->MultiColumnList->SetColumnAlignmentToCenter(col_index);

  col_index = this->MultiColumnList->AddColumn("Color");
  this->MultiColumnList->ColumnEditableOn(col_index);
  this->MultiColumnList->SetColumnFormatCommandToEmptyOutput(col_index);

  col_index = this->MultiColumnList->AddColumn("Value");
  this->MultiColumnList->SetColumnEditable(col_index, 1);
  this->MultiColumnList->SetColumnWidth(col_index, 10);
  
  this->MultiColumnList->SetCellUpdatedCommand (this, "CellUpdateCallback");

  this->Script ("pack %s -side right -fill both -anchor ne -expand no",
		this->DataFrame->GetWidgetName()); 
  this->Script ("pack %s -side top -fill y -anchor nw -expand yes -padx 5 -pady 5",
		mclsb->GetWidgetName());
  mclsb->Delete();

  vtkKWFrame* buttonframe = vtkKWFrame::New();
  buttonframe->SetParent (this->DataFrame);
  buttonframe->Create();

  this->Script ("pack %s -side top -fill both -anchor nw -expand yes -padx 0 -pady 0",
		buttonframe->GetWidgetName());
  
  vtkKWPushButton* buttonsave= vtkKWPushButton::New();
  buttonsave->SetParent(buttonframe);
  buttonsave->Create();
  buttonsave->SetImageToPixels(image_saveas, image_saveas_width, image_saveas_height, image_saveas_pixel_size, image_saveas_length);
  buttonsave->SetCommand(this, "ButtonSaveAsCallback");
  buttonsave->SetBalloonHelpString("Save visible data as csv file");
  this->Script ("pack %s -side left -anchor nw -expand no -padx 3 -pady 3",
		buttonsave->GetWidgetName());
  buttonsave->Delete();


  vtkKWPushButtonWithMenu* menubutton= vtkKWPushButtonWithMenu::New();
  menubutton->SetParent(buttonframe);
  menubutton->Create();
  menubutton->GetPushButton()->SetImageToPixels(image_snapshot, image_snapshot_width, image_snapshot_height, image_snapshot_pixel_size, image_snapshot_length);
  menubutton->GetPushButton()->SetCommand(this, "ButtonSnapCallback 0");
  menubutton->GetPushButton()->SetBalloonHelpString("Take a snaphot of the current view\nor export a movie !");
  this->Script ("pack %s -side left -anchor nw -expand no -padx 3 -pady 3",
		menubutton->GetWidgetName());
  
  vtkKWMenu *menu = menubutton->GetMenu();
  menu->AddCommand("Quick snapshot...", this, "ButtonSnapCallback 0");
  menu->AddCommand("Export a movie...", this, "ButtonSnapCallback 1");
  menu->SetBalloonHelpString("Take a snaphot of the current view\nor export a movie !");
  menubutton->Delete();

  vtkKWPushButton* buttonzoomin= vtkKWPushButton::New();
  buttonzoomin->SetParent(buttonframe);
  buttonzoomin->Create();
  buttonzoomin->SetImageToPixels(image_zoomin, image_zoomin_width, image_zoomin_height, image_zoomin_pixel_size, image_zoomin_length);
  buttonzoomin->SetCommand(this, "ButtonZoomCallback 1.2");
  buttonzoomin->SetBalloonHelpString("Zoom in");
  this->Script ("pack %s -side left -anchor nw -expand no -padx 3 -pady 3",
		buttonzoomin->GetWidgetName());
  buttonzoomin->Delete();

  vtkKWPushButton* buttonzoomout= vtkKWPushButton::New();
  buttonzoomout->SetParent(buttonframe);
  buttonzoomout->Create();
  buttonzoomout->SetImageToPixels(image_zoomout, image_zoomout_width, image_zoomout_height, image_zoomout_pixel_size, image_zoomout_length);
  buttonzoomout->SetCommand(this, "ButtonZoomCallback 0.8");
  buttonzoomout->SetBalloonHelpString("Zoom in");
  this->Script ("pack %s -side left -anchor nw -expand no -padx 3 -pady 3",
		buttonzoomout->GetWidgetName());
  buttonzoomout->Delete();

  vtkKWPushButton* buttonzoomfit= vtkKWPushButton::New();
  buttonzoomfit->SetParent(buttonframe);
  buttonzoomfit->Create();
  buttonzoomfit->SetImageToPixels(image_zoomfit, image_zoomfit_width, image_zoomfit_height, image_zoomfit_pixel_size, image_zoomfit_length);
  buttonzoomfit->SetCommand(this, "ButtonZoomCallback -1.0");
  buttonzoomfit->SetBalloonHelpString("Zoom in");
  this->Script ("pack %s -side left -anchor nw -expand no -padx 3 -pady 3",
		buttonzoomfit->GetWidgetName());
  buttonzoomfit->Delete();
  
  buttonframe->Delete();
  
  this->Pack();
  
}


//----------------------------------------------------------------------------
void vtkKWTimeTableWidget::Pack (void)
{
   
}
 


//----------------------------------------------------------------------------
void vtkKWTimeTableWidget::Initialize (void)
{
  if (!this->GetSequence())
    return;
  
  this->UpdateTime (0);


  this->ActorCollection->RemoveAllItems();
  this->MultiColumnList->DeleteAllRows();
  
  this->RenderWidget->GetRenderer()->Render();
  
}
 


//----------------------------------------------------------------------------
void vtkKWTimeTableWidget::UpdateTime (double time)
{

  if (!this->GetSequence())
    return;

  this->TimeSource->SetPoint1(time,this->RangeY[0], 0);
  this->TimeSource->SetPoint2(time,this->RangeY[1], 0);  


  double duration = this->GetSequence()->GetSequenceDuration();
  double mintime = this->GetSequence()->GetMinTime();
  
  for (int i=0; i<this->ArrayCollection->GetNumberOfItems(); i++)
  {
    vtkDoubleArray* array = vtkDoubleArray::SafeDownCast(this->ArrayCollection->GetItem (i));
    if (!array)
      continue;
    
    int index = (int)vtkMath::Round((time - mintime)*(double)(array->GetNumberOfTuples()-1)/duration) + 1;
    if (index >= array->GetNumberOfTuples())
      index = array->GetNumberOfTuples()-1;
    else if ( index < 0)
      index = 0;
    
    this->MultiColumnList->InsertCellTextAsDouble (i, 3, array->GetValue (index));
    
  }
  
  this->RenderWidget->Render();  
}




//----------------------------------------------------------------------------
void vtkKWTimeTableWidget::AddDataArray (vtkDoubleArray* array, double* rgb)
{
  if (!this->GetSequence())
    return;

  srand (clock());
  float r,g,b;
  
  if (!rgb)
  {
    // put a random color
    int i1 = 1 + (int) (100.0 * ((double)rand() / (RAND_MAX + 1.0)));
    int i2 = 1 + (int) (100.0 * ((double)rand() / (RAND_MAX + 1.0)));
    int i3 = 1 + (int) (100.0 * ((double)rand() / (RAND_MAX + 1.0)));
    double rand_1 = (double)(i1)/(100.0);
    double rand_2 = (double)(i2)/(100.0);
    double rand_3 = (double)(i3)/(100.0);
    r = rand_1;
    g = rand_2;
    b = rand_3;
    if (i1 < 33)
      r = 1;
    else if (i1 < 66)
      g = 1;
    else if (i1 < 100)
      b = 1;
  }
  else
  {
    r = rgb[0];
    g = rgb[1];
    b = rgb[2];
  }

  std::stringstream os;
  os<<r<<" "<<g<<" "<<b;
  
  unsigned int iter = this->ActorCollection->GetNumberOfItems();
  
  this->MultiColumnList->InsertCellText(iter, 0, array->GetName());
//   this->MultiColumnList->InsertCellTextAsInt(iter, 1, !this->ActorCollection->GetNumberOfItems());    
  this->MultiColumnList->InsertCellTextAsInt(iter, 1, false);    
  this->MultiColumnList->SetCellWindowCommandToCheckButton(iter, 1);
  this->MultiColumnList->InsertCellText(iter, 2, os.str().c_str());
  this->MultiColumnList->SetCellWindowCommandToColorButton(iter, 2);
  
  double mintime = this->GetSequence()->GetMinTime();
  double maxtime = this->GetSequence()->GetMaxTime();
  double duration = maxtime - mintime;
    
  vtkPolyData* poly = vtkPolyData::New();
  vtkPoints* points = vtkPoints::New();
  poly->SetPoints (points);
  poly->Allocate();

  for (int i=0; i<array->GetNumberOfTuples(); i++)
  {

    double time = mintime + (duration/((double)(array->GetNumberOfTuples()-1)))*(double)(i);
    
    double val  = array->GetValue (i);
    val*=this->ValuesScaleFactor;
      
    points->InsertNextPoint (time,val,0);
    
    if (i>0)
    {
      vtkIdList* idlist = vtkIdList::New();
      idlist->InsertNextId ( i);
      idlist->InsertNextId ( i-1);
      poly->InsertNextCell (VTK_LINE, idlist);
      idlist->Delete();
    }
  }

  
  vtkPolyDataMapper* mapper =  vtkPolyDataMapper::New();
  mapper->SetInput( poly );
  vtkActor* actor = vtkActor::New();
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor (r,g,b);
  actor->GetProperty()->SetLineWidth (2);
//   actor->SetVisibility (!this->ActorCollection->GetNumberOfItems());
  actor->SetVisibility (false);
  
  this->RenderWidget->GetRenderer()->AddActor (actor);
 
  this->ActorCollection->AddItem (actor);
  this->ArrayCollection->AddItem (array);
  this->PolyDataCollection->AddItem (poly);
  
  points->Delete();
  mapper->Delete();
  actor->Delete();
  poly->Delete();

  
}
 

//----------------------------------------------------------------------------
void vtkKWTimeTableWidget::CellUpdateCallback (int row, int col, char* text)
{
  vtkActor* actor = vtkActor::SafeDownCast (this->ActorCollection->GetItemAsObject (row));
  if (!actor)
    return;
    
  if (col == 1)
  {
    actor->SetVisibility(this->MultiColumnList->GetCellTextAsInt (row, col));
    
  }
  else if (col == 2)
  {
    std::istringstream is;
    is.str (text);
    float r=1, g=0, b=0;
    is >> r >> g >> b;
    actor->GetProperty()->SetColor (r,g,b);
  }

  this->UpdateRanges();

  this->RenderWidget->GetRenderer()->ResetCamera();
  this->RenderWidget->GetRenderer()->GetActiveCamera()->Zoom (3);
  this->RenderWidget->Render();
  
}


 

//----------------------------------------------------------------------------
void vtkKWTimeTableWidget::UpdateRanges(void)
{
  double mintime = this->GetSequence()->GetMinTime();
  double maxtime = this->GetSequence()->GetMaxTime();
  double duration = maxtime - mintime;

  this->ValuesScaleFactor = vtkDataArray::GetDataTypeMax(VTK_DOUBLE);
  
  for (int n=0; n<this->ArrayCollection->GetNumberOfItems(); n++)
  {
    vtkActor* actor = vtkActor::SafeDownCast (this->ActorCollection->GetItemAsObject(n));
    vtkDoubleArray* array = vtkDoubleArray::SafeDownCast (this->ArrayCollection->GetItem (n));
    if (!array)
      continue;

    if (!actor->GetVisibility())
    {
      continue;
    }    

    double* range = array->GetRange();
    double localfactor = 1.0;
    
    if (range[1] - range[0] > 0.000001)
    {
      localfactor = duration/(5.0*(range[1]-range[0]));
//       std::cout<<"local factor for "<<array->GetName()<<" is "<<localfactor<<std::endl;
      if (this->ValuesScaleFactor > localfactor)
      {
	this->ValuesScaleFactor = localfactor;
      }
    }
  }

  this->RangeX[0] = 0;
  this->RangeX[1] = 0;
  this->RangeY[0] = vtkDataArray::GetDataTypeMax(VTK_DOUBLE);
  this->RangeY[1] = vtkDataArray::GetDataTypeMin(VTK_DOUBLE);

  bool isthersomething = 0;
  
  for (int n=0; n<this->ArrayCollection->GetNumberOfItems(); n++)
  {
    vtkActor* actor = vtkActor::SafeDownCast (this->ActorCollection->GetItemAsObject(n));
    vtkDoubleArray* array = vtkDoubleArray::SafeDownCast (this->ArrayCollection->GetItem (n));
    if (!array)
      continue;

    if (!actor->GetVisibility())
    {
      continue;
    }    

    isthersomething = 1;
    
    vtkPolyData* polydata = vtkPolyData::SafeDownCast (this->PolyDataCollection->GetItemAsObject (n));
    
    for (int i=0; i<array->GetNumberOfTuples(); i++)
    {    
      double time = mintime + (duration/((double)(array->GetNumberOfTuples()-1)))*(double)(i);
      double val  = this->ValuesScaleFactor * array->GetValue (i);
      
      if (this->RangeX[0] > time)
	this->RangeX[0] = time;
      if (this->RangeX[1] < time)
	this->RangeX[1] = time;
      
      if (this->RangeY[0] > val)
	this->RangeY[0] = val;
      if (this->RangeY[1] < val)
	this->RangeY[1] = val;
      
      if (polydata)
      {
	polydata->GetPoints()->SetPoint (i, time, val, 0);
	polydata->GetPoints()->Modified();
	polydata->Modified();
      }
    }
  }

  for (int i=0; i<this->HorizontalGridActors->GetNumberOfItems(); i++)
  {
    vtkLineSource* line = vtkLineSource::SafeDownCast (this->HorizontalGrid->GetItemAsObject (i));
    vtkActor* actor = vtkActor::SafeDownCast (this->HorizontalGridActors->GetItemAsObject (i));
    double p = (double)(i)/(double)(this->HorizontalGridActors->GetNumberOfItems() - 1);
    p *= this->RangeY[1] - this->RangeY[0];
    p += this->RangeY[0];
    line->SetPoint1 (this->RangeX[0], p, 0);
    line->SetPoint2 (this->RangeX[1], p, 0);
    
    actor->SetVisibility (isthersomething);
    
  }
  for (int i=0; i<this->VerticalGridActors->GetNumberOfItems(); i++)
  {
    vtkLineSource* line = vtkLineSource::SafeDownCast (this->VerticalGrid->GetItemAsObject (i));
    vtkActor* actor = vtkActor::SafeDownCast (this->VerticalGridActors->GetItemAsObject (i));
    double p = (double)(i)/(double)(this->VerticalGridActors->GetNumberOfItems() - 1);
    p *= this->RangeX[1] - this->RangeX[0];
    p += this->RangeX[0];
    line->SetPoint1 (p, this->RangeY[0], 0);
    line->SetPoint2 (p, this->RangeY[1], 0);

    actor->SetVisibility (isthersomething);
  }

    
  
  
}



//----------------------------------------------------------------------------
void vtkKWTimeTableWidget::ButtonSnapCallback (bool do_movie)
{
  if (!do_movie)
  {
    vtkKWSnapshotHandler::Snap (this->RenderWidget);
    return;
  }

  if (!this->GetSequence())
    return;

  if (!this->SnapshotHandler->IsCreated())
  {
    this->SnapshotHandler->SetMasterWindow (this->GetParentTopLevel());
    this->SnapshotHandler->SetApplication (this->GetApplication());
    this->SnapshotHandler->Create();
  }
  
  if (this->SnapshotHandler->IsMapped())
  {
    this->SnapshotHandler->Withdraw();
    return;
  }

//   vtkKWPopupWarningMessage (this, "Careful : Keep the view clear \nwhile writing the movie !\n  The writing process also might cause some trouble on UNIX systems");
  
  unsigned int maxnumber = this->GetSequence()->GetNumberOfMetaDataSets();
  if (maxnumber == 0)
    return;
  
  this->SnapshotHandler->AllowSliceTypeOff();
  this->SnapshotHandler->AllowCameraTypeOff();
  this->SnapshotHandler->SetSnapshotType(vtkKWSnapshotHandler::SNAPSHOTTYPE_TIME);  
  
  this->SnapshotHandler->SetRenderWidget(this->RenderWidget);
  this->SnapshotHandler->SetTimeUpdateCommand(this, "UpdateTime");
  double timemin = this->GetSequence()->GetMinTime();
  double timemax = this->GetSequence()->GetMaxTime();  
  this->SnapshotHandler->SetTimeSettings (timemin, timemax, maxnumber);
  this->SnapshotHandler->Update();
  this->SnapshotHandler->Invoke();

  this->SnapshotHandler->Withdraw();
  
  
}



//----------------------------------------------------------------------------
void vtkKWTimeTableWidget::ButtonSaveAsCallback (void)
{
  vtkKWLoadSaveDialog *dialog = vtkKWLoadSaveDialog::New() ;
  
  dialog->SetParent(this);
  dialog->Create();
  dialog->MultipleSelectionOff();
  dialog->SaveDialogOn ();
  dialog->RetrieveLastPathFromRegistry("DataPath");
  std::ostringstream os;
  os << "Save visible graphs into file...";
  
  dialog->SetTitle (os.str().c_str());
  
  dialog->SetFileTypes ("{{csv files (*.csv)} {.csv .CSV}} {{all} {.*}}");
  
  if ( dialog->Invoke () == 0 )
  {
    dialog->Delete();
    return;
  }
  
  const char* filename = dialog->GetFileName();

  std::string s_filename (filename);
  vtksys_stl::string ext = vtksys::SystemTools::GetFilenameLastExtension (s_filename);

  if( ext=="" )
  {
    s_filename += ".csv";
    ext = vtksys::SystemTools::GetFilenameLastExtension (s_filename);
  }

  std::ofstream buffer( s_filename.c_str() );
  if( buffer.fail() )
  {
    vtkKWPopupErrorMessage (this, "Cannot open file for writing.");
    buffer.close();
    return;
  }

  std::vector<vtkDoubleArray*> actualarrays;
  unsigned int maxsize = 0;
  
  for (int n=0; n<this->ArrayCollection->GetNumberOfItems(); n++)
  {
    vtkActor* actor = vtkActor::SafeDownCast (this->ActorCollection->GetItemAsObject(n));
    vtkDoubleArray* array = vtkDoubleArray::SafeDownCast (this->ArrayCollection->GetItem (n));
    if (!array)
      continue;

    if (!actor->GetVisibility())
    {
      continue;
    }

    actualarrays.push_back (array);
    
    maxsize = std::max (maxsize, (unsigned int)(array->GetNumberOfTuples()));
    buffer << array->GetName() << "; " ;
    
  }
  
  buffer << std::endl;

  for (unsigned int i=0; i<maxsize; i++)
  {
    for (unsigned int j=0; j<actualarrays.size(); j++)
    {
      if ((int)i < actualarrays[j]->GetNumberOfTuples() )
      {
	buffer << actualarrays[j]->GetValue (i) <<"; ";
      }
    }

    buffer << std::endl;
    
  }
  
  buffer.close();
  
}




//----------------------------------------------------------------------------
void vtkKWTimeTableWidget::ButtonZoomCallback (double val)
{
  if (val < 0)
  {
    this->RenderWidget->GetRenderer()->ResetCamera();
  }
  else
  {
    this->RenderWidget->GetRenderer()->GetActiveCamera()->Zoom (val);
  }
  this->RenderWidget->Render();
}

