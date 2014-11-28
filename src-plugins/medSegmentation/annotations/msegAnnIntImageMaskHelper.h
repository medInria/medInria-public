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

class msegAnnIntImageMaskHelperPrivate;

class msegAnnIntImageMaskHelper : public msegAnnIntHelper {

public:
    msegAnnIntImageMaskHelper(medAnnotationInteractor * annInt);
    virtual ~msegAnnIntImageMaskHelper();

    enum { AnnotationId = 2 };
    bool addAnnotation( medAnnotationData * annData );
    void removeAnnotation( medAnnotationData * annData );
    void annotationModified( medAnnotationData * annData );

private:
    void setColorLookupTable ( QList<double> scalars, QList<QColor> colors );

private:
    msegAnnIntImageMaskHelperPrivate * d;
};



