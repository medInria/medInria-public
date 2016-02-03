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
#include <medAbstractData.h>
#include <vtkDataMeshPluginExport.h>
#include <vtkFieldData.h>
#include <vtkSmartPointer.h>

class vtkDataSetReader;

class VTKDATAMESHPLUGIN_EXPORT vtkDataMeshReader: public dtkAbstractDataReader {
    Q_OBJECT

public:
    vtkDataMeshReader();
    virtual ~vtkDataMeshReader();

    virtual QStringList handled() const;

    static QStringList s_handled();

public slots:

    virtual bool canRead(const QString& path);
    virtual bool canRead(const QStringList& paths);

    virtual bool readInformation(const QString& path);
    virtual bool readInformation(const QStringList& paths);

    virtual bool read(const QString& path);
    virtual bool read(const QStringList& paths);

    virtual void setProgress(int value);

    virtual QString identifier()  const;
    virtual QString description() const;

    static bool registered();	

private:

    static const char ID[];
    QStringList metaDataKeysToCopy();
    void parseHeaderVtk(QString header, medAbstractData* medData);
    void parseHeaderVtp(vtkSmartPointer<vtkFieldData> field, medAbstractData *medData);
};


dtkAbstractDataReader *createVtkDataMeshReader();



