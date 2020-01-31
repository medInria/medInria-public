#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAttachedData.h>
#include <medCoreLegacyExport.h>

class medAbstractData;

/**
 * Base class for annotations : a specialization of medAttachedData.
 *
 */
class MEDCORELEGACY_EXPORT medAnnotationData : public medAttachedData
{
    Q_OBJECT

public:
    medAnnotationData( );
    virtual ~medAnnotationData();

private:
    class medAnnotationDataPrivate;
    medAnnotationDataPrivate * d;
};



