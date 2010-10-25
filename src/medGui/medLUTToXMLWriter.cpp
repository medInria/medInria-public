#include "medLUTtoXMLWriter.h"

#include <QXmlStreamWriter>
#include <QStringList>

class medLUTToXMLWriterPrivate{
public:
    QXmlStreamWriter xml;
    QList<medClutEditorTable *> tables;

    void writeTable(medClutEditorTable & table);
    void writeNode(medClutEditorVertex & vertex);
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
    foreach (medClutEditorTable * table, d->tables)
        d->writeTable(*table);
    d->xml.writeEndDocument();
    return true;
}

void medLUTToXMLWriterPrivate::writeTable(medClutEditorTable & table){
    xml.writeStartElement("table");
    xml.writeAttribute("title", table.title());
    qDebug()<< table.title();
    qDebug()<< "size: " << table.vertices().count();
    foreach (medClutEditorVertex * vertex, table.vertices())
        writeNode(*vertex);
    xml.writeEndElement();
}

void medLUTToXMLWriterPrivate::writeNode(medClutEditorVertex & vertex)
{
    QString node;
    //position
    qDebug()<< "vertex: "<< (long int )&vertex;
    qDebug() << "node:" << vertex.pos();
    qDebug() << "color" <<vertex.color();
    node.sprintf("%.4f;%.4f;%d;%d;%d;%d",vertex.pos().x(),
                 vertex.pos().y(),
                 vertex.color().red(),
                 vertex.color().green(),
                 vertex.color().blue(),
                 vertex.color().alpha());
    xml.writeTextElement("node",node);
}
