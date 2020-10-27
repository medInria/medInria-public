#pragma once
/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2020. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/
#include "vtkDataMeshPluginExport.h"

#include <dtkCoreSupport/dtkAbstractDataReader.h>

class vtkMetaDataSet;

class VTKDATAMESHPLUGIN_EXPORT vtkDataMeshReaderBase: public dtkAbstractDataReader
{
    Q_OBJECT

public:
    vtkDataMeshReaderBase();

    QString medMetaDataRetrocompatility(QString key);

public slots:

    bool canRead        (const QStringList& paths) override;
    bool readInformation(const QStringList& paths) override;
    bool read           (const QStringList& paths) override;

    void setProgress(int value) override;

    bool extractMetaDataFromFieldData(vtkMetaDataSet* dataSet);

    // Functions needed to be declared in child classes
    bool canRead        (const QString& path) override {return false;}
    bool readInformation(const QString& path) override {return false;}
    bool read           (const QString& path) override {return false;}

protected:
    QMap<QString, QString> m_oldKeyToNewKey;
};
