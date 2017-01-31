#include <medAbstractDataWriter.h>


void medAbstractDataWriter::setData(QList<medAbstractData*> data)
{
	dataList = data;
}

QList<medAbstractData*> medAbstractDataWriter::getDataList()
{
	return dataList;
}
