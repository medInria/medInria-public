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
