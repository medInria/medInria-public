#pragma once

#include <QRegExp>
#include <QVariant>

class medStyleSheetParser
{
public:
    medStyleSheetParser(QString qss);
    
    QString result() const;

private:
    QString output;
};
