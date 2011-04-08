/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWMainWindowInteractor.h 1366 2009-11-30 08:10:27Z tmansi $
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
#ifndef __vtkKWMainWindowInteractor_h
#define __vtkKWMainWindowInteractor_h

#include <vtkKWMainWindow.h>
#include <string>
#include <vector>

#include <vtkMetaImageData.h>
#include <itkImage.h>

class vtkDataManager;
class vtkImageData;
class vtkDataSet;
class vtkImageView;
class vtkKWToolBox;
class vtkDataManagerCallback;
class vtkKWMainCallback;
class vtkKWVisualizationToolBox;
class vtkKWDataManagerWidget;
class vtkKWSplitFrame;
class vtkKWTopLevel;
class vtkMetaDataSet;
class vtkKWToolboxManager;
class vtkKWPreviewPage;
class vtkKWSnapshotHandler;




class KW_ADDON_EXPORT vtkKWMainWindowInteractor : public vtkKWMainWindow
{
public:
  static vtkKWMainWindowInteractor* New();
  vtkTypeRevisionMacro(vtkKWMainWindowInteractor,vtkKWMainWindow);
  
  vtkGetObjectMacro (DataManager, vtkDataManager);
  void SetDataManager (vtkDataManager* manager);
  vtkGetObjectMacro(ManagerWidget, vtkKWDataManagerWidget);
  void SetDataManagerWidget (vtkKWDataManagerWidget* widget);


  vtkGetObjectMacro (PreviewPage, vtkKWPreviewPage);
  vtkGetObjectMacro (ToolboxManager, vtkKWToolboxManager);
  
  
  // Description:
  // Callbacks

  vtkKWPageView* CreateNewPage (const char* name, vtkImageData* image, vtkMatrix4x4* matrix = 0);
  //BTX
  vtkKWPageView* CreateNewPage (const char* name, itk::Image<float, 3>::Pointer image);
  //ETX
  void RemovePage (const char* title);



  
  virtual void OnSelectInteraction();
  virtual void OnWindowLevelInteraction();
  virtual void OnZoomInteraction();
  virtual void OnRenderingModeToVR();
  virtual void OnFullScreenView4();
  virtual void OnFullScreenAxial();
  virtual void OnFullScreenCoronal();
  virtual void OnFullScreenSagittal();
  virtual void OnIllustrationMode();
  virtual void OnLandmarkDialog();
  
  
  virtual void OnMenuNewTab();
  virtual void OnMenuDeleteCurrentTab();
  virtual void OnMenuFileOpen();
  virtual void OnMenuFileOpen4D();
  virtual void OnMenuFileOpenDICOM();
  virtual void OnMenuFileSaveDICOM();
  virtual void SaveManagerCallback (void);
  
  virtual void OnDataSetSave();
  virtual void OnDataSetDelete();
  //BTX
  std::vector<vtkKWToolBox*> GetToolBoxList();
  //ETX
  vtkKWToolBox* GetToolboxByID(unsigned int id);
  vtkKWToolBox* GetToolbox(unsigned int id);
	
  virtual void CreateAliases (void);
  virtual void LoadToolBox (vtkKWToolBox* toolbox, const char* name);

  
  virtual void PopupManager (void);
  virtual void PopdownManager (void);

  virtual void OnSnapshotHandler (bool exportmovie);
  
  virtual void RemoveMetaDataSet (vtkMetaDataSet* metadataset, bool popup=1);
  
  virtual void Update();

  virtual void ReadFile(const char* filename);
  virtual void ReadFileAndUpdate(const char* filename);
  virtual void RemoveAllMetaDataSets (void);

  virtual void UpdateToTime (double time);
  virtual void Render();

  virtual void SelectMetaDataSet (vtkMetaDataSet* metadataset);
  
  
protected:
  vtkKWMainWindowInteractor();
  ~vtkKWMainWindowInteractor();

  // Description:
  // Create the widget.
  virtual void CreateWidget();
  virtual void PackSelf();

  virtual void CreatePreviewPage(void);

  
private:
  
  vtkKWMainWindowInteractor(const vtkKWMainWindowInteractor&);   // Not implemented.
  void operator=(const vtkKWMainWindowInteractor&);  // Not implemented.

  vtkDataManager*            DataManager;
  vtkDataManagerCallback*    DataManagerCallback;
  vtkKWMainCallback*         MainCallback;
  vtkKWDataManagerWidget*    ManagerWidget;
  vtkKWTopLevel*             ManagerDialog;
  
  vtkKWNotebook*             ToolboxNotebook;
  //BTX
  std::vector<vtkKWToolBox*> ToolBoxList;
  //ETX

  vtkKWSplitFrame* PanelSplitFrame;

  vtkKWToolboxManager* ToolboxManager;

  
  
  vtkKWPreviewPage* PreviewPage;
  vtkKWSnapshotHandler* SnapshotHandler;

  vtkKWTopLevel* LandmarkManagerDialog;

  // Save visibility status of widgets
  bool IllustrationMode;
  bool ShowAnnotations;
  bool ShowRulerWidget;
  bool Show2DAxis;
  bool ShowScalarBar;
  bool ShowDirections;
  vtkKWPageView* ModifiedPage;
};

#endif
