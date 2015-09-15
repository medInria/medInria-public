/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAnnotationInteractor.h>

class medAnnIntImageMaskHelperPrivate;

class medAnnIntImageMaskHelper : public msegAnnIntHelper {

public:
    medAnnIntImageMaskHelper(medAnnotationInteractor * annInt);
    virtual ~medAnnIntImageMaskHelper();

    enum { AnnotationId = 2 };
    bool addAnnotation( medAnnotationData * annData );
    void removeAnnotation( medAnnotationData * annData );
    void annotationModified( medAnnotationData * annData );

private:
    void setColorLookupTable ( QList<double> scalars, QList<QColor> colors );

private:
    medAnnIntImageMaskHelperPrivate * d;
};



