#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAnnotationInteractor.h>

#include <map>

#include <vtkSmartPointer.h>

class medSeedPointAnnotationData;
class medAnnotationInteractor;
class medAnnIntSeedPointHelperPrivate;
class vtkSeedWidget;
class vtkAbstractWidget;

class medAnnIntSeedPointHelper : public msegAnnIntHelper {

public:
    medAnnIntSeedPointHelper(medAnnotationInteractor * annInt);
    virtual ~medAnnIntSeedPointHelper();

    bool addAnnotation( medAnnotationData* annData );
    void removeAnnotation( medAnnotationData * annData );
    void annotationModified( medAnnotationData* annData );

    void refreshFromWidget(vtkSeedWidget * spW);
private:
    friend class medAnnIntSeedPointHelperPrivate;

    typedef vtkSmartPointer <vtkAbstractWidget>  WidgetSmartPointer;
    struct ActorInfo {
        WidgetSmartPointer actor2d;
        WidgetSmartPointer actor3d;
    };

    typedef std::map<medSeedPointAnnotationData*, ActorInfo> ActorMap;

    ActorMap & getActorMap();

    bool findActorMapForWidget(vtkAbstractWidget * w, ActorMap::iterator & it);

    medAnnIntSeedPointHelperPrivate * d;
};



