#pragma once

#include "medClutEditor.h"
#include <qiodevice.h>
#include <qstring.h>

class medLUTToXMLWriterPrivate;

class medLUTToXMLWriter {
public:
    medLUTToXMLWriter(const QList<medClutEditorTable*> & tables);
    ~medLUTToXMLWriter();
    bool writeFile(QIODevice *device);
    QString errorString() const;
private:
    medLUTToXMLWriterPrivate * d;
};




