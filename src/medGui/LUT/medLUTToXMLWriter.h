#ifndef MEDLUTTOXMLWRITER_H
#define MEDLUTTOXMLWRITER_H

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



#endif // MEDLUTTOXMLWRITER_H
