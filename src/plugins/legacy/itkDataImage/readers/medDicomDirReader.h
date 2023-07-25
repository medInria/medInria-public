#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractDataReader.h>

#include <itkDataImagePluginExport.h>

#include <medAbstractDataFactory.h>
#include <medMetaDataKeys.h>

//VTK
#include <vtkDICOMImageReader.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>

//DCMTK
#include <dcmtk/dcmdata/dcdicdir.h>
#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/cmdlnarg.h>

//ITKGDCM
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkImageSeriesReader.h>
#include <gdcmReader.h>

class medDicomDirReaderPrivate;

class ITKDATAIMAGEPLUGIN_EXPORT medDicomDirReader: public dtkAbstractDataReader
{
    Q_OBJECT

public:
    medDicomDirReader();
    ~medDicomDirReader();

    virtual QString identifier()  const;
    virtual QString description() const;

    virtual QStringList handled() const;
    static bool registered();

public slots:

    bool canRead (const QString& path);
    bool canRead (const QStringList& paths);

    bool readInformation (const QString &path);
    bool readInformation (const QStringList &paths);

    bool read (const QString &path);
    bool read (const QStringList &paths);

    void setProgress (int value);

private:
    medDicomDirReaderPrivate *d;
    static const char ID[];

    void saveNeededPaths(QString dicomdirPath);
    void getDicomsDirectoryPath(OFString dicomsDirectoryName);
    void getFilenamesList();

    QString GetPixelType();

    bool loadDICOMImagesFromDirectory(const QString& dicomDirPath);

    template<typename TYPE>
    void read3DImage(medAbstractData* medData,
                     itk::GDCMImageIO::Pointer io,
                     std::vector<std::string> filelist);
};

dtkAbstractDataReader *createmedDicomDirReader();
