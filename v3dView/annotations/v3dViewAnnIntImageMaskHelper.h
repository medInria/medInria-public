/*
 * v3dViewAnnIntImageMaskHelper.h
 *
 *  Created on: 19 oct. 2011 08:34:58
 *      Author: jstark
 */

#ifndef v3dViewAnnIntImageMaskHelper_H_
#define v3dViewAnnIntImageMaskHelper_H_

#include "v3dViewAnnotationInteractor.h"

class v3dViewAnnotationInteractor;
class v3dViewAnnIntImageMaskHelperPrivate;

class v3dViewAnnIntImageMaskHelper : public v3dViewAnnIntHelper {

public:
    v3dViewAnnIntImageMaskHelper(v3dViewAnnotationInteractor * annInt);
    virtual ~v3dViewAnnIntImageMaskHelper();

    enum { AnnotationId = 2 };
    bool addAnnotation( medAnnotationData * annData );
    void removeAnnotation( medAnnotationData * annData );
    void annotationModified( medAnnotationData * annData );

private:
    v3dViewAnnotationInteractor * m_v3dViewAnnInt;
    v3dViewAnnIntImageMaskHelperPrivate * d;
};

#endif /* v3dViewAnnIntImageMaskHelper_H_ */

