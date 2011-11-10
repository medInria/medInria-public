/*
 * v3dViewAnnIntSeedPointHelper.h
 *
 *  Created on: 19 oct. 2011 08:34:58
 *      Author: jstark
 */

#ifndef V3DVIEWANNINTSEEDPOINTHELPER_H_
#define V3DVIEWANNINTSEEDPOINTHELPER_H_

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

    v3dViewAnnotationInteractor * m_v3dViewAnnInt;
    v3dViewAnnIntSeedPointHelperPrivate * d;
};

#endif /* V3DVIEWANNINTSEEDPOINTHELPER_H_ */

