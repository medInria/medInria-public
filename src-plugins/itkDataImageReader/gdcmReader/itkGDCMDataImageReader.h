/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractDataReader.h>

#include <itkDataImageReaderPluginExport.h>
#include <itkGDCMImageIO.h>
#include <itkImageSeriesReader.h>

#include <gdcmScanner.h>

#include <sstream>
#include <vector>
#include <string>
#include <map>

class itkGDCMDataImageReaderPrivate;

namespace itk
{
  class SliceReadCommand;
}

class ITKDATAIMAGEREADERPLUGIN_EXPORT itkGDCMDataImageReader: public dtkAbstractDataReader {
    Q_OBJECT

public:

    typedef std::vector<std::string>        FileList;
    typedef std::map<std::string, FileList> FileListMapType;
    typedef std::map<double, FileList>      SortedMapType;

    itkGDCMDataImageReader();
    virtual ~itkGDCMDataImageReader();

    
    virtual QString identifier()  const;
    virtual QString description() const;

    virtual QStringList handled() const;
    static bool registered();

    friend class itk::SliceReadCommand;

public slots:

    bool canRead (QString path);
    bool canRead (QStringList paths);

    void readInformation (QString path);
    void readInformation (QStringList paths);

    bool read (QString path);
    bool read (QStringList paths);

    void setProgress (int value);

private:  

    FileListMapType sort (FileList filelist);
    FileList unfoldMap (FileListMapType map);

    itkGDCMDataImageReaderPrivate *d;
    gdcm::Scanner m_Scanner;

    static const char ID[];
};

dtkAbstractDataReader *createItkGDCMDataImageReader();


