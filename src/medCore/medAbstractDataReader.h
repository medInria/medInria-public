#pragma once

#include <dtkCore/dtkAbstractDataReader.h>

class medAbstractDataReader : public dtkAbstractDataReader
{
    Q_OBJECT
public:
    medAbstractDataReader();
    virtual ~medAbstractDataReader();

    virtual QStringList supportedFileExtensions() const = 0;
};
