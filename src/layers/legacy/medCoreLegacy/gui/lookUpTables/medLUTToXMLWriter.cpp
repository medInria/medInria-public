/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medLUTToXMLWriter.h>

#include <QXmlStreamWriter>
#include <QStringList>

class medLUTToXMLWriterPrivate{
public:
    QXmlStreamWriter xml;
    QList<medClutEditorTable *> tables;

    void writeTable(const medClutEditorTable & table);
    void writeNode( const medClutEditorVertex & vertex);
};


medLUTToXMLWriter::medLUTToXMLWriter (const QList<medClutEditorTable *> & tables):
        d(new medLUTToXMLWriterPrivate)
{
    d->tables = tables;
}


medLUTToXMLWriter::~medLUTToXMLWriter()
{
   delete d;
   d = nullptr;
}


bool medLUTToXMLWriter::writeFile(QIODevice *device)
{
    d->xml.setDevice(device);
    d->xml.writeStartDocument();
    d->xml.writeDTD("<!DOCTYPE medLUTs>");
    d->xml.writeStartElement("medLUTs");
    d->xml.writeAttribute("version", "1.1");

    foreach (medClutEditorTable * table, d->tables)
    {
        d->writeTable(*table);
    }
    d->xml.writeEndDocument();
    return true;
}

void medLUTToXMLWriterPrivate::writeTable(const medClutEditorTable & table)
{
    xml.writeStartElement("table");
    xml.writeAttribute("title", table.title());
    qDebug()<< "Table Title -> "<<table.title();
    qDebug()<< "Number of vertices -> " << table.vertices().count();

    // If the table has at least a node
    if (table.vertices().count() > 0)
    {
        foreach (const medClutEditorVertex * vertex, table.vertices())
        {
            writeNode(*vertex);
        }
        xml.writeEndElement();
    }
}

void medLUTToXMLWriterPrivate::writeNode(const medClutEditorVertex & vertex)
{
    QString node;

    node.sprintf("%.4f;%.4f;%d;%d;%d;%d",
                 vertex.value().x(),
                 vertex.value().y(),
                 vertex.color().red(),
                 vertex.color().green(),
                 vertex.color().blue(),
                 vertex.color().alpha());
    xml.writeTextElement("node",node);
}
