#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.

=========================================================================*/

#include <medCoreLegacyExport.h>

#include <dtkCoreSupport/dtkSmartPointer.h>
#include <dtkCoreSupport/dtkAbstractDataReader.h>

class medAbstractData;
class medAbstractDataReaderPrivate;

class MEDCORELEGACY_EXPORT medAbstractDataReader : public dtkAbstractDataReader
{
    Q_OBJECT

public:
    medAbstractDataReader(void);
    medAbstractDataReader(const medAbstractDataReader& other);
    virtual ~medAbstractDataReader(void);

    int getNumberOfData() const;
    QVector<medAbstractData*> getData() const;

signals:
    void finishedReadingData(int dataNumber);

protected:
    QVector<medAbstractData*> m_data;
};
