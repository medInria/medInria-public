#pragma once
/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2020. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include <medAbstractDataWriter.h>

#include <medVtkFibersDataPluginExport.h>

class MEDVTKFIBERSDATAPLUGIN_EXPORT medVtkFibersDataWriter : public medAbstractDataWriter
{
    Q_OBJECT

public:
    medVtkFibersDataWriter();
    virtual ~medVtkFibersDataWriter();

    virtual QStringList handled() const;
    
    virtual QString identifier()  const;
    virtual QString description() const;

    static bool registered();

    /** Override base class */
    virtual QStringList supportedFileExtensions() const;

public slots:
    bool write    (const QString& path);
    bool canWrite (const QString& path);
};

dtkAbstractDataWriter *create_medVtkFibersDataWriter();
