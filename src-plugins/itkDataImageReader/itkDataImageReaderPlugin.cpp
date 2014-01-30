/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifdef ITK_USE_SYSTEM_GDCM
#include <itkGDCMDataImageReader.h>
#endif
#include <itkMetaDataImageReader.h>
#include <itkPNGDataImageReader.h>
#include <itkBMPDataImageReader.h>
#include <itkJPEGDataImageReader.h>
#include <itkTIFFDataImageReader.h>
#include <itkNiftiDataImageReader.h>
#include <itkNrrdDataImageReader.h>
#include <itkGiplDataImageReader.h>
#include <itkVTKDataImageReader.h>
#include <itkPhilipsRECDataImageReader.h>
#include <itkGISDataImageReader.h>

#include <itkDataImageReaderPlugin.h>
#include <itkLogForwarder.h>

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// itkDataImageReaderPluginPrivate
// /////////////////////////////////////////////////////////////////

class itkDataImageReaderPluginPrivate
{
public:
    itkDataImageReaderPluginPrivate()
    {
       forwarder = itk::LogForwarder::New();
    }
    ~itkDataImageReaderPluginPrivate()
    {

    }
    // Class variables go here.
    itk::LogForwarder::Pointer forwarder;
};

// /////////////////////////////////////////////////////////////////
// itkDataImageReaderPlugin
// /////////////////////////////////////////////////////////////////

itkDataImageReaderPlugin::itkDataImageReaderPlugin(QObject *parent) : dtkPlugin(parent), d(new itkDataImageReaderPluginPrivate)
{

}

itkDataImageReaderPlugin::~itkDataImageReaderPlugin()
{
    delete d;
    d = NULL;
}

bool itkDataImageReaderPlugin::initialize()
{
    if (!itkMetaDataImageReader::registered())       { dtkWarn() << "Unable to register itkMetaDataImageReader type";       }
#ifdef ITK_USE_SYSTEM_GDCM
    if (!itkGDCMDataImageReader::registered())       { dtkWarn() << "Unable to register  itkGDCMDataImageReader type";      }
#endif
    if (!itkNiftiDataImageReader::registered())      { dtkWarn() << "Unable to register itkNiftiDataImageReader type";      }
    if (!itkNrrdDataImageReader::registered())       { dtkWarn() << "Unable to register itkNrrdDataImageReader type";       }
    if (!itkGiplDataImageReader::registered())       { dtkWarn() << "Unable to register itkGiplDataImageReader type";       }
    if (!itkVTKDataImageReader::registered())        { dtkWarn() << "Unable to register itkVTKDataImageReader type";        }
    if (!itkPNGDataImageReader::registered())        { dtkWarn() << "Unable to register itkPNGDataImageReader type";        }
    if (!itkBMPDataImageReader::registered())        { dtkWarn() << "Unable to register itkBMPDataImageReader type";        }
    if (!itkJPEGDataImageReader::registered())       { dtkWarn() << "Unable to register itkJPEGDataImageReader type";       }
    if (!itkTIFFDataImageReader::registered())       { dtkWarn() << "Unable to register itkTIFFDataImageReader type";       }
    if (!itkPhilipsRECDataImageReader::registered()) { dtkWarn() << "Unable to register itkPhilipsRECDataImageReader type"; }
    if (!itkGISDataImageReader::registered())        { dtkWarn() << "Unable to register itkGISDataImageReader type"; }

    return true;
}

bool itkDataImageReaderPlugin::uninitialize()
{
    return true;
}

QString itkDataImageReaderPlugin::name() const
{
    return "itkDataImageReaderPlugin";
}

QString itkDataImageReaderPlugin::description() const
{
    return "Readers for any ITK image data type.";
}

QString itkDataImageReaderPlugin::version() const
{
    return ITKDATAIMAGEREADERPLUGIN_VERSION;
}

QStringList itkDataImageReaderPlugin::tags() const
{
    return QStringList() << "itk" << "data" << "image" << "reader";
}

QStringList itkDataImageReaderPlugin::types() const
{
    return QStringList() << "itkMetaDataImageReader"
#ifdef ITK_USE_SYSTEM_GDCM
            << "itkGDCMDataImageReader"
#endif
            << "itkNiftiDataImageReader"
            << "itkAnalyzeDataImageReader"
            << "itkNrrdDataImageReader"
            << "itkGiplDataImageReader"
            << "itkVTKDataImageReader"
            << "itkPNGDataImageReader"
            << "itkBMPDataImageReader"
            << "itkJPEGDataImageReader"
            << "itkTIFFDataImageReader"
            << "itkPhilipsRECDataImageReader"
            << "itkGISDataImageReader";
}

Q_EXPORT_PLUGIN2(itkDataImageReaderPlugin, itkDataImageReaderPlugin)
