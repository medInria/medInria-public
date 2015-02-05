/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataImagePlugin.h>
#include <itkDataImageChar3.h>
#include <itkDataImageChar4.h>
#include <itkDataImageUChar4.h>
#include <itkDataImageUChar3.h>
#include <itkDataImageShort3.h>
#include <itkDataImageShort4.h>
#include <itkDataImageUShort3.h>
#include <itkDataImageUShort4.h>
#include <itkDataImageInt3.h>
#include <itkDataImageInt4.h>
#include <itkDataImageLong4.h>
#include <itkDataImageUInt4.h>
#include <itkDataImageULong4.h>
#include <itkDataImageUInt3.h>
#include <itkDataImageLong3.h>
#include <itkDataImageULong3.h>
#include <itkDataImageFloat3.h>
#include <itkDataImageFloat4.h>
#include <itkDataImageDouble4.h>
#include <itkDataImageDouble3.h>
#include <itkDataImageVectorUChar3.h>
#include <itkDataImageVectorFloat3.h>
#include <itkDataImageVectorDouble3.h>
#include <itkDataImageRGB3.h>
#include <itkDataImageRGB4.h>
#include <itkDataImageRGBA3.h>

#include <medVtkViewItkDataImageInteractor.h>
#include <medVtkViewItkDataImage4DInteractor.h>
#include <medVtkViewItkVectorFieldInteractor.h>

#include <medVtkViewItkDataImageNavigator.h>

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
#include <itkDCMTKDataImageReader.h>

#include <itkMetaDataImageWriter.h>
#include <itkNiftiDataImageWriter.h>
#include <itkNrrdDataImageWriter.h>
#include <itkGiplDataImageWriter.h>
#include <itkVTKDataImageWriter.h>
#include <itkGISDataImageWriter.h>

#include <dtkLog/dtkLog.h>
#include <itkLogForwarder.h>

// /////////////////////////////////////////////////////////////////
// itkDataImagePluginPrivate
// /////////////////////////////////////////////////////////////////

class itkDataImagePluginPrivate
{
public:
    itkDataImagePluginPrivate()
    {
        forwarder = itk::LogForwarder::New();
    }
    ~itkDataImagePluginPrivate()
    {

    }
    // Class variables go here.
    itk::LogForwarder::Pointer forwarder;
};

// /////////////////////////////////////////////////////////////////
// itkDataImagePlugin
// /////////////////////////////////////////////////////////////////

itkDataImagePlugin::itkDataImagePlugin(QObject *parent) : dtkPlugin(parent), d(new itkDataImagePluginPrivate)
{

}

itkDataImagePlugin::~itkDataImagePlugin()
{
    delete d;

    d = NULL;
}

bool itkDataImagePlugin::initialize()
{
    if(!itkDataImageChar3::registered()) { dtkWarn() << "Unable to register itkDataImageChar3 type"; }
    if(!itkDataImageChar4::registered()) { dtkWarn() << "Unable to register itkDataImageChar4 type"; }
    if(!itkDataImageInt4::registered()) { dtkWarn() << "Unable to register itkDataImageInt4 type"; }
    if(!itkDataImageLong4::registered()) { dtkWarn() << "Unable to register itkDataImageLong4 type"; }
    if(!itkDataImageUChar4::registered()) { dtkWarn() << "Unable to register itkDataImageUChar4 type"; }
    if(!itkDataImageUChar3::registered()) { dtkWarn() << "Unable to register itkDataImageUChar3 type"; }
    if(!itkDataImageShort3::registered()) { dtkWarn() << "Unable to register itkDataImageShort3 type"; }
    if(!itkDataImageShort4::registered()) { dtkWarn() << "Unable to register itkDataImageShort4 type"; }
    if(!itkDataImageUShort3::registered()) { dtkWarn() << "Unable to register itkDataImageUShort3 type"; }
    if(!itkDataImageUShort4::registered()) { dtkWarn() << "Unable to register itkDataImageUShort4 type"; }
    if(!itkDataImageUInt4::registered()) { dtkWarn() << "Unable to register itkDataImageUInt4 type"; }
    if(!itkDataImageULong4::registered()) { dtkWarn() << "Unable to register itkDataImageULong4 type"; }
    if(!itkDataImageInt3::registered()) { dtkWarn() << "Unable to register itkDataImageInt3 type"; }
    if(!itkDataImageUInt3::registered()) { dtkWarn() << "Unable to register itkDataImageUInt3 type"; }
    if(!itkDataImageLong3::registered()) { dtkWarn() << "Unable to register itkDataImageLong3 type"; }
    if(!itkDataImageULong3::registered()) { dtkWarn() << "Unable to register itkDataImageULong3 type"; }
    if(!itkDataImageFloat3::registered()) { dtkWarn() << "Unable to register itkDataImageFloat3 type"; }
    if(!itkDataImageFloat4::registered()) { dtkWarn() << "Unable to register itkDataImageFloat4 type"; }
    if(!itkDataImageDouble3::registered()) { dtkWarn() << "Unable to register itkDataImageDouble3 type"; }
    if(!itkDataImageDouble4::registered()) { dtkWarn() << "Unable to register itkDataImageDouble4 type"; }
    if(!itkDataImageVectorUChar3::registered()) { dtkWarn() << "Unable to register itkDataImageVectorUChar3 type"; }
    if(!itkDataImageVectorFloat3::registered()) { dtkWarn() << "Unable to register itkDataImageVectorFloat3 type"; }
    if(!itkDataImageVectorDouble3::registered()) { dtkWarn() << "Unable to register itkDataImageVectorDouble3 type"; }
    if(!itkDataImageRGB3::registered()) { dtkWarn() << "Unable to register itkDataImageRGB3 type"; }
    if(!itkDataImageRGB4::registered()) { dtkWarn() << "Unable to register itkDataImageRGB4 type"; }
    if(!itkDataImageRGBA3::registered()) { dtkWarn() << "Unable to register itkDataImageRGBA3 type"; }

    if(!medVtkViewItkDataImageInteractor::registered()) { dtkWarn() << "Unable to register medVtkViewItkDataImageInteractor"; }
    if(!medVtkViewItkDataImage4DInteractor::registered()) { dtkWarn() << "Unable to register medVtkViewItkDataImage4DInteractor"; }
    if(!medVtkViewItkVectorFieldInteractor::registered()) { dtkWarn() << "Unable to register medVtkViewItkVectorFieldInteractor"; }

    if(!medVtkViewItkDataImageNavigator::registered()) { dtkWarn() << "Unable to register medVtkViewItkDataImageNavigator"; }

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
    if (!itkDCMTKDataImageReader::registered())      { dtkWarn() << "Unable to register itkDCMTKDataImageReader type"; }

    if (!itkMetaDataImageWriter::registered())       { dtkWarn() << "Unable to register itkMetaDataImageWriter type";       }
    if (!itkNiftiDataImageWriter::registered())      { dtkWarn() << "Unable to register itkNiftiDataImageWriter type";      }
    if (!itkNrrdDataImageWriter::registered())       { dtkWarn() << "Unable to register itkNrrdDataImageWriter type";       }
    if (!itkGiplDataImageWriter::registered())       { dtkWarn() << "Unable to register itkGiplDataImageWriter type";       }
    if (!itkVTKDataImageWriter::registered())        { dtkWarn() << "Unable to register itkVTKDataImageWriter type";        }
    if (!itkGISDataImageWriter::registered())        { dtkWarn() << "Unable to register itkGISDataImageWriter type"; }

    return true;
}

bool itkDataImagePlugin::uninitialize()
{
    return true;
}

QString itkDataImagePlugin::name() const
{
    return "itkDataImagePlugin";
}

QString itkDataImagePlugin::description() const
{
    return "Plugin containing all types of ITK images";
}

QString itkDataImagePlugin::version() const
{
    return ITKDATAIMAGEPLUGIN_VERSION;
}

QStringList itkDataImagePlugin::tags() const
{
    return QStringList() << "itk" << "data" << "image";
}

QStringList itkDataImagePlugin::types() const
{
    return QStringList() << "itkDataImageChar3"
            << "itkDataImageChar4"
            << "itkDataImageUChar3"
            << "itkDataImageUChar4"
            << "itkDataImageShort3"
            << "itkDataImageShort4"
            << "itkDataImageUShort4"
            << "itkDataImageUShort3"
            << "itkDataImageInt3"
            << "itkDataImageInt4"
            << "itkDataImageLong4"
            << "itkDataImageUInt4"
            << "itkDataImageULong4"
            << "itkDataImageUInt3"
            << "itkDataImageLong3"
            << "itkDataImageULong3"
            << "itkDataImageFloat3"
            << "itkDataImageFloat4"
            << "itkDataImageDouble3"
            << "itkDataImageDouble4"
            << "itkDataImageVectorUChar3"
            << "itkDataImageVectorFloat3"
            << "itkDataImageVectorDouble3"
            << "itkDataImageRGB3"
            << "itkDataImageRGB4"
            << "itkDataImageRGBA3";
}

Q_EXPORT_PLUGIN2(itkDataImagePlugin, itkDataImagePlugin)
