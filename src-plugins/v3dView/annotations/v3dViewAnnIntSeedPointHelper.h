/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "v3dViewAnnotationInteractor.h"

#include <map>

#include <vtkSmartPointer.h>

class medSeedPointAnnotationData;
class v3dViewAnnotationInteractor;
class v3dViewAnnIntSeedPointHelperPrivate;
class vtkSeedWidget;
class vtkAbstractWidget;

class v3dViewAnnIntSeedPointHelper : public v3dViewAnnIntHelper {

public:
    v3dViewAnnIntSeedPointHelper(v3dViewAnnotationInteractor * annInt);
    virtual ~v3dViewAnnIntSeedPointHelper();

    bool addAnnotation( medAnnotationData* annData );
    void removeAnnotation( medAnnotationData * annData );
    void annotationModified( medAnnotationData* annData );

    void refreshFromWidget(vtkSeedWidget * spW);
private:
    friend class v3dViewAnnIntSeedPointHelperPrivate;

    typedef vtkSmartPointer <vtkAbstractWidget>  WidgetSmartPointer;
    struct ActorInfo {
        WidgetSmartPointer actor2d;
        WidgetSmartPointer actor3d;
    };

    typedef std::map<medSeedPointAnnotationData*, ActorInfo> ActorMap;

    ActorMap & getActorMap();

    bool findActorMapForWidget(vtkAbstractWidget * w, ActorMap::iterator & it);

    v3dViewAnnIntSeedPointHelperPrivate * d;
};



