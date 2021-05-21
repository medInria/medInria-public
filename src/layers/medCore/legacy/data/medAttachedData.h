#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractData.h>
#include <medCoreLegacyExport.h>

class medAttachedDataPrivate;

/**
 * Class to store attachments to datasets
 */
class MEDCORELEGACY_EXPORT medAttachedData : public medAbstractData
{
    Q_OBJECT

public:

    medAttachedData( medAttachedData *parent = nullptr );
    virtual ~medAttachedData();

    medAbstractData * parentData() const;
    void setParentData( medAbstractData * mdata );

private:
    medAttachedDataPrivate* d;
};
