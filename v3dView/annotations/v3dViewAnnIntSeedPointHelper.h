/*
 * v3dViewAnnIntSeedPointHelper.h
 *
 *  Created on: 19 oct. 2011 08:34:58
 *      Author: jstark
 */

#ifndef V3DVIEWANNINTSEEDPOINTHELPER_H_
#define V3DVIEWANNINTSEEDPOINTHELPER_H_

class medSeedPointAnnotationData;
class v3dViewAnnotationInteractor;
class v3dViewAnnIntSeedPointHelperPrivate;
class vtkSeedWidget;

class v3dViewAnnIntSeedPointHelper {

public:
    v3dViewAnnIntSeedPointHelper(v3dViewAnnotationInteractor * annInt);
    virtual ~v3dViewAnnIntSeedPointHelper();

    bool addSeedPointAnnotation( medSeedPointAnnotationData * annData );
    void removeSeedPointAnnotation( medSeedPointAnnotationData * annData );
    void seedPointModified( medSeedPointAnnotationData * annData );

    void refreshFromWidget(vtkSeedWidget * spW);
private:
    v3dViewAnnotationInteractor * m_v3dViewAnnInt;
    v3dViewAnnIntSeedPointHelperPrivate * d;
};

#endif /* V3DVIEWANNINTSEEDPOINTHELPER_H_ */

