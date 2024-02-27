/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractDataReader.h>
#include <medAbstractData.h>

void medAbstractDataReader::setData(QList<medAbstractData*> data)
{
	dataList = data;
}

void medAbstractDataReader::setData(dtkAbstractData* data)
{
    dtkAbstractDataReader::setData(data);
    dataList = QList<medAbstractData*>() << dynamic_cast<medAbstractData*>(data);
}

QList<medAbstractData*> medAbstractDataReader::getDataList()
{
	return dataList;
}
