#include "medLUTtoXMLWriter.h"

#include <QXmlStreamWriter>
#include <QStringList>

class medLUTToXMLWriterPrivate{
public:
    QXmlStreamWriter xml;
    QList<medClutEditorTable *> tables;

    void writeTable(const medClutEditorTable & table);
    void writeNode(const medClutEditorVertex & vertex);
};


medLUTToXMLWriter::medLUTToXMLWriter (const QList<medClutEditorTable *> & tables):
        d(new medLUTToXMLWriterPrivate)
{
    d->tables = tables;
}


medLUTToXMLWriter::~medLUTToXMLWriter()
{
   delete d;
   d = NULL;
}


bool medLUTToXMLWriter::writeFile(QIODevice *device)
{
    d->xml.setDevice(device);
    d->xml.writeStartDocument();
    d->xml.writeDTD("<!DOCTYPE medLUTs>");
    d->xml.writeStartElement("medLUTs");
    d->xml.writeAttribute("version", "1.0");
    for (QList<medClutEditorTable*>::Iterator i = d->tables.begin();
            i < d->tables.end(); ++i)
        d->writeTable(i*);
    d->xml.writeEndDocument();
    return true;
}

void medLUTToXMLWriterPrivate::writeTable(const medClutEditorTable & table){
    xml.writeStartElement("table");
    xml.writeAttribute("title", table.title());
    for (QList<medClutEditorVertex*>::iterator i = table.vertices().begin();
            i < table.vertices().end(); ++i)
        writeNode(i*);
    xml.writeEndElement();
}

void medLUTToXMLWriterPrivate::writeNode(const medClutEditorVertex & vertex)
{
    QString node;
    //position
    node.append(QString(float(vertex.pos().x())));
    node.append(QString((float)vertex.pos().y()));
    //color
    node.append(QString(vertex.color().red()));
    node.append(QString(vertex.color().green()));
    node.append(QString(vertex.color().blue()));
    node.append(QString(vertex.color().alpha()));
    xml.writeTextElement("node", node);
}
