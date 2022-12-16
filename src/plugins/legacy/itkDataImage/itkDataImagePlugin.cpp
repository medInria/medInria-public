/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
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
#include <itkDataImageRGBA3.h>

#include <medVtkViewItkDataImageInteractor.h>
#include <medVtkViewItkDataImage4DInteractor.h>
#include <medVtkViewItkVectorFieldInteractor.h>

#include <medVtkViewItkDataImageNavigator.h>

#include <dicomRtImageWriter.h>

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
#include <itkDicomDataImageWriter.h>

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

    d = nullptr;
}

bool itkDataImagePlugin::initialize()
{
    if(!itkDataImageChar3::registered()) { qWarning() << "Unable to register itkDataImageChar3 type"; }
    if(!itkDataImageChar4::registered()) { qWarning() << "Unable to register itkDataImageChar4 type"; }
    if(!itkDataImageInt4::registered()) { qWarning() << "Unable to register itkDataImageInt4 type"; }
    if(!itkDataImageLong4::registered()) { qWarning() << "Unable to register itkDataImageLong4 type"; }
    if(!itkDataImageUChar4::registered()) { qWarning() << "Unable to register itkDataImageUChar4 type"; }
    if(!itkDataImageUChar3::registered()) { qWarning() << "Unable to register itkDataImageUChar3 type"; }
    if(!itkDataImageShort3::registered()) { qWarning() << "Unable to register itkDataImageShort3 type"; }
    if(!itkDataImageShort4::registered()) { qWarning() << "Unable to register itkDataImageShort4 type"; }
    if(!itkDataImageUShort3::registered()) { qWarning() << "Unable to register itkDataImageUShort3 type"; }
    if(!itkDataImageUShort4::registered()) { qWarning() << "Unable to register itkDataImageUShort4 type"; }
    if(!itkDataImageUInt4::registered()) { qWarning() << "Unable to register itkDataImageUInt4 type"; }
    if(!itkDataImageULong4::registered()) { qWarning() << "Unable to register itkDataImageULong4 type"; }
    if(!itkDataImageInt3::registered()) { qWarning() << "Unable to register itkDataImageInt3 type"; }
    if(!itkDataImageUInt3::registered()) { qWarning() << "Unable to register itkDataImageUInt3 type"; }
    if(!itkDataImageLong3::registered()) { qWarning() << "Unable to register itkDataImageLong3 type"; }
    if(!itkDataImageULong3::registered()) { qWarning() << "Unable to register itkDataImageULong3 type"; }
    if(!itkDataImageFloat3::registered()) { qWarning() << "Unable to register itkDataImageFloat3 type"; }
    if(!itkDataImageFloat4::registered()) { qWarning() << "Unable to register itkDataImageFloat4 type"; }
    if(!itkDataImageDouble3::registered()) { qWarning() << "Unable to register itkDataImageDouble3 type"; }
    if(!itkDataImageDouble4::registered()) { qWarning() << "Unable to register itkDataImageDouble4 type"; }
    if(!itkDataImageVectorUChar3::registered()) { qWarning() << "Unable to register itkDataImageVectorUChar3 type"; }
    if(!itkDataImageVectorFloat3::registered()) { qWarning() << "Unable to register itkDataImageVectorFloat3 type"; }
    if(!itkDataImageVectorDouble3::registered()) { qWarning() << "Unable to register itkDataImageVectorDouble3 type"; }
    if(!itkDataImageRGB3::registered()) { qWarning() << "Unable to register itkDataImageRGB3 type"; }
    if(!itkDataImageRGBA3::registered()) { qWarning() << "Unable to register itkDataImageRGBA3 type"; }

    if(!medVtkViewItkDataImageInteractor::registered()) { qWarning() << "Unable to register medVtkViewItkDataImageInteractor"; }
    if(!medVtkViewItkDataImage4DInteractor::registered()) { qWarning() << "Unable to register medVtkViewItkDataImage4DInteractor"; }
    if(!medVtkViewItkVectorFieldInteractor::registered()) { qWarning() << "Unable to register medVtkViewItkVectorFieldInteractor"; }

    if(!medVtkViewItkDataImageNavigator::registered()) { qWarning() << "Unable to register medVtkViewItkDataImageNavigator"; }

    if (!itkMetaDataImageReader::registered())       { qWarning() << "Unable to register itkMetaDataImageReader type";       }
#ifdef ITK_USE_SYSTEM_GDCM
    if (!itkGDCMDataImageReader::registered())       { qWarning() << "Unable to register  itkGDCMDataImageReader type";      }
#endif
    if (!itkNiftiDataImageReader::registered())      { qWarning() << "Unable to register itkNiftiDataImageReader type";      }
    if (!itkNrrdDataImageReader::registered())       { qWarning() << "Unable to register itkNrrdDataImageReader type";       }
    if (!itkGiplDataImageReader::registered())       { qWarning() << "Unable to register itkGiplDataImageReader type";       }
    if (!itkVTKDataImageReader::registered())        { qWarning() << "Unable to register itkVTKDataImageReader type";        }
    if (!itkPNGDataImageReader::registered())        { qWarning() << "Unable to register itkPNGDataImageReader type";        }
    if (!itkBMPDataImageReader::registered())        { qWarning() << "Unable to register itkBMPDataImageReader type";        }
    if (!itkJPEGDataImageReader::registered())       { qWarning() << "Unable to register itkJPEGDataImageReader type";       }
    if (!itkTIFFDataImageReader::registered())       { qWarning() << "Unable to register itkTIFFDataImageReader type";       }
    if (!itkPhilipsRECDataImageReader::registered()) { qWarning() << "Unable to register itkPhilipsRECDataImageReader type"; }
    if (!itkGISDataImageReader::registered())        { qWarning() << "Unable to register itkGISDataImageReader type"; }
    if (!itkDCMTKDataImageReader::registered())      { qWarning() << "Unable to register itkDCMTKDataImageReader type"; }

    if (!itkMetaDataImageWriter::registered())       { qWarning() << "Unable to register itkMetaDataImageWriter type";       }
    if (!itkNiftiDataImageWriter::registered())      { qWarning() << "Unable to register itkNiftiDataImageWriter type";      }
    if (!itkNrrdDataImageWriter::registered())       { qWarning() << "Unable to register itkNrrdDataImageWriter type";       }
    if (!itkGiplDataImageWriter::registered())       { qWarning() << "Unable to register itkGiplDataImageWriter type";       }
    if (!itkVTKDataImageWriter::registered())        { qWarning() << "Unable to register itkVTKDataImageWriter type";        }
    if (!itkGISDataImageWriter::registered())        { qWarning() << "Unable to register itkGISDataImageWriter type"; }
    if (!itkDicomDataImageWriter::registered())      { qWarning() << "Unable to register itkDicomDataImageWriter type"; }

    if (!DicomRtImageWriter::registered())           { qWarning() << "Unable to register DicomRtImageWriter type"; }

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
            << "itkDataImageRGBA3";
}
