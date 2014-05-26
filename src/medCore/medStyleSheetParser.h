#pragma once

#include <QRegExp>
#include <QVariant>
#include <medCoreExport.h>

class MEDCORE_EXPORT medStyleSheetParser
{
public:
    medStyleSheetParser(QString qss);
    
    QString result() const;

private:
    QString output;
};
