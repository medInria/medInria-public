/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractDataWriter.h>
#include <medAbstractData.h>

void medAbstractDataWriter::setData(QList<medAbstractData*> data)
{
	dataList = data;
}

void medAbstractDataWriter::setData(dtkAbstractData* data)
{
    dataList = QList<medAbstractData*>() << dynamic_cast<medAbstractData*>(data);
}

QList<medAbstractData*> medAbstractDataWriter::getDataList()
{
	return dataList;
}
