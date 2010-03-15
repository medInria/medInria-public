/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWTimeTableWidget.h 488 2007-11-22 10:47:11Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2007-11-22 11:47:11 +0100 (Thu, 22 Nov 2007) $
Version:   $Revision: 488 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtkKWTimeTableWidget_h_
#define _vtkKWTimeTableWidget_h_

#include "vtkINRIA3DConfigure.h"

#include <vtkKWFrame.h>
#include <string>
#include <vector>

class vtkXYPlotActor;
class vtkKWRenderWidget;
class vtkMetaDataSetSequence;
class vtkDoubleArray;
class vtkActor;
class vtkLineSource;
class vtkKWFrame;
class vtkKWMultiColumnList;
class vtkActorCollection;
class vtkDataArrayCollection;
class vtkPolyDataCollection;
class vtkKWPushButton;
class vtkKWPushButtonWithMenu;
class vtkKWSnapshotHandler;
class vtkCollection;
class vtkPolyData;




class KW_ADDON_EXPORT vtkKWTimeTableWidget: public vtkKWFrame
{
 public:

  static vtkKWTimeTableWidget* New();
  vtkTypeRevisionMacro(vtkKWTimeTableWidget,vtkKWFrame);

  vtkGetObjectMacro(XYActor, vtkXYPlotActor);
  vtkGetObjectMacro(RenderWidget, vtkKWRenderWidget);
  vtkGetObjectMacro(Sequence, vtkMetaDataSetSequence);

  virtual void SetSequence (vtkMetaDataSetSequence* seq)
  { this->Sequence = seq; }

  virtual void Initialize(void);
  virtual void UpdateTime (double time);
  virtual void AddDataArray (vtkDoubleArray* array, double* rgb = NULL);
  virtual void UpdateRanges(void);

  //callbacks
  virtual void CellUpdateCallback (int row, int col, char* text);
  virtual void ButtonSaveAsCallback (void);
  virtual void ButtonSnapCallback (bool do_movie);
  virtual void ButtonZoomCallback (double val);

  
 protected:
  vtkKWTimeTableWidget();
  ~vtkKWTimeTableWidget();


  
  // Description:
  // Create the widget.
  virtual void CreateWidget();
  virtual void Pack();

  vtkKWFrame* GraphFrame;
  vtkKWFrame* DataFrame;
  vtkKWMultiColumnList* MultiColumnList;
  vtkKWSnapshotHandler* SnapshotHandler;
  
 private:
  
  vtkKWTimeTableWidget(const vtkKWTimeTableWidget&);      // Not implemented.
  void operator=(const vtkKWTimeTableWidget&);        // Not implemented.

  vtkXYPlotActor*    XYActor;
  vtkKWRenderWidget* RenderWidget;
  vtkMetaDataSetSequence* Sequence;
  vtkLineSource*    TimeSource;
  double RangeX[2];
  double RangeY[2];
  double ValuesScaleFactor;

  vtkActorCollection* ActorCollection;
  vtkDataArrayCollection* ArrayCollection;
  vtkPolyDataCollection* PolyDataCollection;
  vtkCollection* HorizontalGrid;
  vtkCollection* VerticalGrid;
  vtkActorCollection* HorizontalGridActors;
  vtkActorCollection* VerticalGridActors;
  vtkPolyData* ScaleBar;
  
  
};

#endif
