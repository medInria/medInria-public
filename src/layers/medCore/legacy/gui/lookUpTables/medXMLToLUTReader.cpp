/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medXMLToLUTReader.h>

#include <QXmlStreamReader>
#include <QStringList>

#include <dtkLog>

class medXMLToLUTReaderPrivate{
public:
    QXmlStreamReader xml;
    QList<medClutEditorTable *> *tables;
    medClutEditorScene* scene;
    void readLUT();
    void readTable();
    void readNode(medClutEditorTable & table);
};

medXMLToLUTReader::medXMLToLUTReader (QList<medClutEditorTable*> * tables,
                                      medClutEditorScene* scene):
    d(new medXMLToLUTReaderPrivate)
{
    d->tables = tables;
    d->scene = scene;
}

medXMLToLUTReader::~medXMLToLUTReader()
{
    delete d;
    d = nullptr;
}

bool medXMLToLUTReader::read(QIODevice *device)
{
    d->xml.setDevice(device);

    if (d->xml.readNextStartElement())
    {
        if (d->xml.name() == "medLUTs" && d->xml.attributes().value("version") == "1.1")
        {
            d->readLUT();
        }
        else
        {
            d->xml.raiseError(QObject::tr("The file is not a medLUT version 1.1 file."));
        }
    }

    return !d->xml.error();
}

QString medXMLToLUTReader::errorString() const
{
    return QObject::tr("%1\nLine %2, column %3")
            .arg(d->xml.errorString())
            .arg(d->xml.lineNumber())
            .arg(d->xml.columnNumber());
}

void medXMLToLUTReaderPrivate::readLUT()
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "medLUTs");

    while (xml.readNextStartElement())
    {
        if (xml.name() == "table")
        {
            readTable();
        }
        else
        {
            xml.skipCurrentElement();
        }
    }
}

void medXMLToLUTReaderPrivate::readTable()
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "table");
    medClutEditorTable * table = new medClutEditorTable(
                xml.attributes().value("title").toString());

    while (xml.readNextStartElement())
    {
        if (xml.name() == "node")
        {
            readNode(*table);
        }
        else
        {
            xml.skipCurrentElement();
        }
    }
    tables->append(table);
}

void medXMLToLUTReaderPrivate::readNode(medClutEditorTable & table)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "node");
    QString node = xml.readElementText();
    QStringList sl = node.split(';');

    if (sl.count() != 6)
    {
        qDebug() << "node not compliant with 6 coordinates scheme";
        return;
    }

    QPointF nodeValue (sl.at(0).toDouble(), sl.at(1).toDouble());
    QPointF nodeCoord = scene->valueToCoordinate(nodeValue);

    table.addVertex(new medClutEditorVertex(nodeValue,
                                            nodeCoord,
                                            QColor(sl.at(2).toInt(),
                                                   sl.at(3).toInt(),
                                                   sl.at(4).toInt(),
                                                   sl.at(5).toInt())));
}
