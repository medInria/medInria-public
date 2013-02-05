/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWDICOMImporter2.h 1 2007-11-20 17:46:10Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2007-11-20 18:46:10 +0100 (Tue, 20 Nov 2007) $
Version:   $Revision: 1 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtkKWDICOMImporter2_h_
#define _vtkKWDICOMImporter2_h_

#include "vtkINRIA3DConfigure.h"


#include <vtkKWMessageDialog.h>
#include <itkGDCMImporter3.h>


class vtkKWMessageWithLabel;
class vtkKWPushButton;
class vtkKWFrame;
class vtkKWCheckButtonWithLabel;
class vtkKWEntryWithLabel;
class vtkImageView2D;
class vtkKWRenderWidget;
class vtkKWMultiColumnList;
class vtkKWFrameWithLabel;
class vtkKWMenuButton;
class vtkMetaDataSet;


class KW_ADDON_EXPORT vtkKWDICOMImporter2: public vtkKWMessageDialog
{
 public:

  static vtkKWDICOMImporter2* New();
  vtkTypeRevisionMacro(vtkKWDICOMImporter2,vtkKWMessageDialog);

  //BTX
  enum InteractiveStatus
  {
    STATUS_DISABLED = 0,
    STATUS_PLAY,
    STATUS_STOP
  };
  //ETX

  //BTX
  typedef signed short ImageComponentType ;
  typedef itk::Image<ImageComponentType, 4> ImageType;
  typedef ImageType::Pointer ImagePointerType;
  
  typedef itk::GDCMImporter3<ImageComponentType> ImporterType;
  typedef ImporterType::GDCMVolumeType GDCMVolume;
  //ETX
  
  virtual void OpenFileCallback();
  
  virtual void OpenDirectoryCallback();
  virtual void ResetCallback();
  virtual void RemoveVolumeCallback();
  virtual void AutoOrganizeCallback();
  virtual void AutoOrganizePositionBasedCallback();
  
  virtual void OnceVolumeCallback();

  virtual void SelectionChangedCallback();

  virtual void InteractiveModeCallback(int val);

  void DoubleClickCallback();

  void SequenceCheckButtonCallback (int val);
  void CellUpdateCallback (int row, int col, int val);

  
  //BTX
  ImporterType::Pointer GetGDCMImporter (void) const
  {
    return this->GDCMImporter;
  }
  std::vector<vtkMetaDataSet*> GetOutputList (void)
  {
    return this->OutputList;
  }
  //ETX

  vtkSetMacro(InteractiveMode,int);
  vtkGetMacro(InteractiveMode,int);
  vtkBooleanMacro(InteractiveMode, int);

  vtkSetMacro(InteractiveStatus,unsigned int);
  vtkGetMacro(InteractiveStatus,unsigned int);
  vtkBooleanMacro(InteractiveStatus, unsigned int);

  virtual int  PreInvoke();
  virtual void PostInvoke();
  
 protected:
  vtkKWDICOMImporter2();
  ~vtkKWDICOMImporter2();
  
  // Description:
  // Create the widget.

  virtual void CreateWidget();
  virtual void CreateButtons();
  virtual void CreatePreview();
  virtual void CreateMultiColumnList();
  
  virtual void PackSelf();

  virtual void Update(void);
  virtual void UpdatePreview (void);
  //BTX
  virtual void UpdatePreview (GDCMVolume::Pointer volume);
  //ETX
  virtual void UpdateMultiColumnList (void);

  virtual void StartInteractive(void);
  virtual void StopInteractive(void);

  virtual void PackSequenceFrame (void);
  
  virtual void SetOutputsAs2DtSequence(void);
  virtual void SetOutputsAs3DSequence(void);
  virtual void SetOutputsAsVolumes(void);
  
/*   vtkKWPushButton* OpenDirectoryButton; */
  vtkKWPushButton* ResetButton;
  vtkKWPushButton* RemoveVolumeButton;
  vtkKWMenuButton* AutoOrganizeButton;
  vtkKWPushButton* OnceVolumeButton;
  
  vtkKWCheckButtonWithLabel* RecursiveCheckButton;

  vtkKWFrame* TopFrame;
  vtkKWFrame* BottomFrame;
  vtkKWFrame* MLeftFrame;
  vtkKWFrame* MRightFrame;
  vtkKWFrame* ButtonFrame;
  vtkKWCheckButtonWithLabel* InteractiveModeButton;
  
  vtkKWRenderWidget*      RenderWidget;
  vtkImageView2D*           Preview;

  vtkKWMultiColumnList* MultiColumnList;

  vtkKWFrameWithLabel* SequenceFrame;
  vtkKWCheckButtonWithLabel* SequenceCheckButton;
  vtkKWEntryWithLabel*       SequenceDurationEntry;

  unsigned int InteractiveStatus;

 private:
  
  vtkKWDICOMImporter2(const vtkKWDICOMImporter2&);      // Not implemented.
  void operator=(const vtkKWDICOMImporter2&);        // Not implemented.


  //BTX
  ImporterType::Pointer GDCMImporter;
  //ETX

  int InteractiveMode;
  unsigned int InteractiveVolume;

  //BTX
  std::vector<vtkMetaDataSet*> OutputList;
  std::vector<GDCMVolume::Pointer> VolumeList;
  //ETX
    
};

#endif
