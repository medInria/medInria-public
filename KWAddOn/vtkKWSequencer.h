/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWSequencer.h 877 2008-06-04 16:10:40Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-06-04 17:10:40 +0100 (Wed, 04 Jun 2008) $
Version:   $Revision: 877 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtkKWSequencer_h_
#define _vtkKWSequencer_h_

#include <vtkKWToolBox.h>
#include <string>
#include <vector>

class vtkMetaDataSet;
class vtkTimerLog;

class vtkKWScaleSet;
class vtkKWScaleWithLabel;
class vtkKWPushButton;
class vtkMetaDataSetSequence;
class vtkKWFrame;
class vtkKWCheckButtonWithLabel;
class vtkKWTimeAnimationWidget;
class vtkKWFrameWithLabel;
class vtkKWSpinBoxWithLabel;
class vtkKWComboBox;
class vtkKWTimeTableWidget;
class vtkKWTopLevel;
class vtkKWMessageDialog;

class vtkLandmarkManager;


class KW_ADDON_EXPORT vtkKWSequencer: public vtkKWToolBox
{
 public:

  static vtkKWSequencer* New();
  vtkTypeRevisionMacro(vtkKWSequencer,vtkKWToolBox);
  virtual void PrintSelf(ostream& os, vtkIndent indent);


  //BTX
  enum StatusType
  {
    STATUS_PLAY,
    STATUS_STOP
  } ;
  //ETX

  vtkGetObjectMacro (TimerLog, vtkTimerLog);
  
  virtual void Start (void);
  virtual void Stop  (void);
  virtual void Next  (void);
  virtual void Prev  (void);
  virtual void Initialize (void){};
  virtual void RemoveAllSequences();

  virtual void Update();
  
  //Callbacks
  void TimeScaleCallback (double time);
  void RealTimeCheckboxCallback (int val);
  void SpeedSpinBoxCallback(double val);
  void ManageSequenceButtonCallback (void);
  void TimeTableButtonCallback (void);
  void LandmarkTimeTableButtonCallback (void);
  void SetTimeTableWidget (vtkKWTimeTableWidget* widget);
  void LandmarkDialogOked(void);
  void CycleModeButtonCallback (void);

  virtual void PrepareToPop (void);
  
  
  
  
 protected:
  vtkKWSequencer();
  ~vtkKWSequencer();

  // Description:
  // Create the widget.
  virtual void CreateWidget();
  virtual void Pack();

  vtkKWCheckButtonWithLabel* RealTimeCheckbox;
  vtkKWScaleSet*   TimeScaleSet;
  vtkKWPushButton* PlayButton;
  vtkKWPushButton* NextButton;
  vtkKWPushButton* PrevButton;
  vtkKWTimeAnimationWidget* AnimationWidget;
  vtkKWFrameWithLabel *AnimationFrame;
  vtkKWSpinBoxWithLabel* SpeedSpinBox;
  vtkKWScaleWithLabel*   TimeScale;
  vtkKWPushButton* ManageSequenceButton;
  vtkKWPushButton* TimeTableButton;
  vtkKWPushButton* LandmarkTimeTableButton;
  vtkKWTimeTableWidget* TimeTableWidget;
  vtkKWTopLevel* TimeTableDialog;
  vtkKWTopLevel* LandmarkDialog;
  vtkTimerLog*   TimerLog;
  vtkKWPushButton* CycleModeButton;
  
  StatusType SequencerStatus;
  bool LinkScales;
  bool RealTimeState;
  double SpeedRatio;
  double FinestResolution;
  bool CycleState;

  vtkKWFrame* ButtonFrame1;
  vtkKWFrame* ButtonFrame11;
  vtkKWFrame* ButtonFrame12;
  vtkKWFrame* ButtonFrame2;
  vtkKWFrame* ButtonFrame3;  
  
  vtkLandmarkManager* LandmarkManager;

  int CurrentSign;

 private:
  
  vtkKWSequencer(const vtkKWSequencer&);      // Not implemented.
  void operator=(const vtkKWSequencer&);        // Not implemented.

  
};

#endif
