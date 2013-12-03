/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <v3dViewAnnotationInteractor.h>

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
    v3dViewAnnIntImageMaskHelperPrivate * d;
};



