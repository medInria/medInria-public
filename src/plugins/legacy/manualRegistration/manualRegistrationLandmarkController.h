#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "manualRegistrationLandmark.h"
#include "manualRegistrationPluginExport.h"
#include "manualRegistrationToolBox.h"

#include <medAbstractLayeredView.h>

#include <vtkCollection.h>

class manualRegistrationLandmarkController;

class MANUALREGISTRATIONPLUGIN_EXPORT manualRegistrationLandmarkControllerCommand : public vtkCommand
{

public:
    static manualRegistrationLandmarkControllerCommand* New()
    {
        return new manualRegistrationLandmarkControllerCommand;
    }

    virtual void Execute ( vtkObject *caller, unsigned long, void* );

    void SetController (manualRegistrationLandmarkController* arg);

protected:
    manualRegistrationLandmarkControllerCommand();
    ~manualRegistrationLandmarkControllerCommand();

private:
    manualRegistrationLandmarkController* Controller;
};

class VTK_EXPORT manualRegistrationLandmarkController : public vtkObject
{
public:
    static manualRegistrationLandmarkController* New();
    vtkTypeMacro(manualRegistrationLandmarkController, vtkObject)

    vtkGetObjectMacro (InteractorCollection, vtkCollection)
    void SetInteractorCollection(vtkCollection*);

    vtkGetObjectMacro (ViewMoving, medAbstractLayeredView)
    vtkGetObjectMacro (ViewFixed, medAbstractLayeredView)
    vtkGetObjectMacro (ViewFuse, medAbstractLayeredView)
    vtkGetObjectMacro (Output, medAbstractData)

    QList<manualRegistrationLandmark*> * getPoints_Moving();
    QList<manualRegistrationLandmark*> * getPoints_Fixed();
    
    void SetTbx(manualRegistrationToolBox*);
    void SetViewMoving(medAbstractLayeredView*);
    void SetViewFixed(medAbstractLayeredView*);
    void SetViewFuse(medAbstractLayeredView*);
    void SetEnabled (unsigned int arg);
    vtkGetMacro (Enabled, unsigned int)
    vtkBooleanMacro (Enabled, unsigned int)
    void AddPoint(manualRegistrationLandmark * landmark,unsigned int i);
    void ClearUselessLandmarks();
    void RequestDeletion(manualRegistrationLandmark * landmark);
    int checkLandmarks();
    void Reset();

private:
    vtkCollection* InteractorCollection;
    manualRegistrationLandmarkControllerCommand* Command;
    medAbstractLayeredView* ViewMoving;
    medAbstractLayeredView* ViewFixed;
    medAbstractLayeredView* ViewFuse;
    unsigned int Enabled;
    QList<manualRegistrationLandmark*>* Points_Moving;
    QList<manualRegistrationLandmark*>* Points_Fixed;
    dtkSmartPointer<medAbstractData> Output;
    QList<QColor> Colors;
    manualRegistrationToolBox* Tbx;

protected:
    manualRegistrationLandmarkController();
    ~manualRegistrationLandmarkController();
};



