/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <msegAnnotationInteractor.h>

#include <map>

#include <vtkSmartPointer.h>

class medSeedPointAnnotationData;
class msegAnnotationInteractor;
class msegAnnIntSeedPointHelperPrivate;
class vtkSeedWidget;
class vtkAbstractWidget;

class msegAnnIntSeedPointHelper : public msegAnnIntHelper {

public:
    msegAnnIntSeedPointHelper(msegAnnotationInteractor * annInt);
    virtual ~msegAnnIntSeedPointHelper();

    bool addAnnotation( medAnnotationData* annData );
    void removeAnnotation( medAnnotationData * annData );
    void annotationModified( medAnnotationData* annData );

    void refreshFromWidget(vtkSeedWidget * spW);
private:
    friend class msegAnnIntSeedPointHelperPrivate;

    typedef vtkSmartPointer <vtkAbstractWidget>  WidgetSmartPointer;
    struct ActorInfo {
        WidgetSmartPointer actor2d;
        WidgetSmartPointer actor3d;
    };

    typedef std::map<medSeedPointAnnotationData*, ActorInfo> ActorMap;

    ActorMap & getActorMap();

    bool findActorMapForWidget(vtkAbstractWidget * w, ActorMap::iterator & it);

    msegAnnIntSeedPointHelperPrivate * d;
};



