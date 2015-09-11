/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <medCompositeDataSetsPluginExport.h>
#include <medCompositeDataSetsBase.h>

#include <dirTools.h>

class MEDCOMPOSITEDATASETSPLUGIN_EXPORT medCompositeDataSetsReader: public dtkAbstractDataReader {
    Q_OBJECT

public:

    medCompositeDataSetsReader(): is_zip_file(false),desc(0),reader(0) { }

    virtual ~medCompositeDataSetsReader() { cleanup(); }

    void cleanup() {
        if (!desc)
            delete desc;
        desc = 0;
        if (!reader)
            delete reader;
        reader = 0;
        if (is_zip_file)
            RemoveDirectory(QDir(tmpdir));
    }

    virtual QString description() const { return "Reader for composite data sets";              }
    virtual QString identifier() const { return "medCompositeDataSetsReader";              }
    virtual QStringList handled() const { return MedInria::medCompositeDataSetsBase::handled(); }
    
    static bool initialize() {
        return dtkAbstractDataFactory::instance()->registerDataReaderType("medCompositeDataSetsReader",MedInria::medCompositeDataSetsBase::handled(),create);
    }

    static dtkAbstractDataReader* create() { return new medCompositeDataSetsReader(); }

public slots:

    virtual bool canRead(const QString& file);

    virtual bool read(const QString& file);
    virtual bool readInformation(const QString& path);
    virtual void setProgress(const int value);

private:

    bool       is_zip_file;
    bool       in_error;
    QString    basedir;
    QString    tmpdir;
    QIODevice* desc;

    MedInria::medCompositeDataSetsBase* reader;
};


