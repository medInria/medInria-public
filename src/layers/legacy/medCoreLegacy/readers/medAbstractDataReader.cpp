/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.

=========================================================================*/

#include "medAbstractDataReader.h"

#include <medAbstractData.h>
#include <dtkCoreSupport/dtkAbstractDataReader.h>

medAbstractDataReader::medAbstractDataReader()
    : dtkAbstractDataReader()
{
}

medAbstractDataReader::medAbstractDataReader(const medAbstractDataReader& other)
    : dtkAbstractDataReader(other)
{
}

medAbstractDataReader::~medAbstractDataReader()
{
}

int medAbstractDataReader::getNumberOfData() const
{
    return m_data.size();
}

QVector<medAbstractData*> medAbstractDataReader::getData() const
{
    return m_data;
}
