/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore/QObject>
#include <QVector>
#include <medPacsExport.h>

class medAbstractPacsKey;

class MEDPACS_EXPORT medAbstractPacsResultDataset : public QObject
{
    Q_OBJECT

public:
    medAbstractPacsResultDataset() = default;
    ~medAbstractPacsResultDataset() override = default;

    virtual const char* getStudyInstanceUID();

    virtual const char* getSeriesInstanceUID();

    virtual const char* getSOPInstanceUID();

    virtual QVector<medAbstractPacsKey*> getKeyContainer();

    virtual const char* findKeyValue(int group, int elem);
};
