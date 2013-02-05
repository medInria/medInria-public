/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkPolyDataManager.h 608 2008-01-14 08:21:23Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2008-01-14 09:21:23 +0100 (Mon, 14 Jan 2008) $
Version:   $Revision: 608 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtk_PolyDataManager_h_
#define _vtk_PolyDataManager_h_

#include "vtkINRIA3DConfigure.h"

#include <vtkObject.h>
#include <vtkRenderer.h>
#include <vector>
#include <vtkProperty.h>


class vtkPolyData;
class vtkActor;


/** EXPERIMENTAL CODE --- DO NOT USE FOR NOW -- Nicolas Toussaint */

class VTK_VISUMANAGEMENT_EXPORT vtkPolyDataManager: public vtkObject
{
  
 public:
  
  static vtkPolyDataManager* New();
  vtkTypeRevisionMacro(vtkPolyDataManager, vtkObject);

  vtkSetObjectMacro (Renderer, vtkRenderer);
  vtkGetObjectMacro (Renderer, vtkRenderer);

  void         AddPolyData    (vtkPolyData* polydata);
  void         RemovePolyData (vtkPolyData* polydata);
  int          HasPolyData    (vtkPolyData* polydata);
  

  //BTX
  /** Get PolyDatas as vtkActorList */
  std::vector<vtkActor*> GetActorList (void) const
  { return ActorList; }
  std::vector<vtkPolyData*> GetPolyDataList (void) const
  { return PolyDataList; }
  //ETX
  vtkActor*    GetActor    (unsigned int i);
  vtkPolyData* GetPolyData (unsigned int id);
  /** Reset all data from the manager */
  void ResetData(void);
  /** Specify the visibility of a polydata */
  void SetVisibility (int IsVisible);
  /** Specify the visibility of a polydata */
  void SetVisibility (unsigned int i, int IsVisible);
  /** Get the visibility of the polydatas */
  int GetVisibility (void) const
  { return Visibility; }
  /** Get the visibility of the polydata i */
  int GetVisibility (unsigned int i) const;
  /** Set the opacity to use for polydatas */
  void SetOpacity (double val);
  /** Set the opacity of a specific polydata */
  void SetOpacity (unsigned int i, double val);
  /** Get the opacity used for polydatas */
  double GetOpacity(void) const
  { return Opacity; }
  /** Get the opacity used for polydata i */
  double GetOpacity(unsigned int i) const;

  /** Specify the color of a polydata */
  void SetColor (unsigned int i, double color[3])
  {
    if (i<0 || i>this->ActorList.size())
      return;
    double c[4] = {color[0],color[1],color[2],1};
    this->ActorList[i]->GetProperty()->SetColor(c);
  }
  
    
    

  
 protected:
  
  vtkPolyDataManager();
  ~vtkPolyDataManager();

 private:

  vtkRenderer*               Renderer;    
  //BTX
  std::vector<vtkPolyData*>  PolyDataList;
  std::vector<vtkActor*>     ActorList;
  //ETX
  double*                    DefaultColor;
  int                        Visibility;
  double                     Opacity;

};


#endif



  
