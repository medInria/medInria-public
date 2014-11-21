/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkImagePlugin.h>
#include <medItkChar3ImageData.h>
#include <medItkChar4ImageData.h>
#include <medItkUChar4ImageData.h>
#include <medItkUChar3ImageData.h>
#include <medItkShort3ImageData.h>
#include <medItkShort4ImageData.h>
#include <medItkUShort3ImageData.h>
#include <medItkUShort4ImageData.h>
#include <medItkInt3ImageData.h>
#include <medItkInt4ImageData.h>
#include <medItkLong4ImageData.h>
#include <medItkUInt4ImageData.h>
#include <medItkULong4ImageData.h>
#include <medItkUInt3ImageData.h>
#include <medItkLong3ImageData.h>
#include <medItkULong3ImageData.h>
#include <medItkFloat3ImageData.h>
#include <medItkFloat4ImageData.h>
#include <medItkDouble4ImageData.h>
#include <medItkDouble3ImageData.h>
#include <medItkVectorUChar3ImageData.h>
#include <medItkVectorFloat3ImageData.h>
#include <medItkVectorDouble3ImageData.h>
#include <medItkRgb3ImageData.h>
#include <medItkRgba3ImageData.h>

#include <medVtkViewItk4DScalarImageInteractor.h>
#include <medVtkViewItk4DScalarImageInteractor.h>
#include <medVtkViewItkVectorImageInteractor.h>

#include <medVtkViewItkScalarImageNavigator.h>

#ifdef ITK_USE_SYSTEM_GDCM
#include <medItkImageDataGdcmReader.h>
#endif
#include <medItkImageMetaDataReader.h>
#include <medItkImageDataPngReader.h>
#include <medItkImageDataBmpReader.h>
#include <medItkImageDataJpegReader.h>
#include <medItkImageDataTiffReader.h>
#include <medItkImageDataNiftiReader.h>
#include <medItkImageDataNrrdReader.h>
#include <medItkImageDataGiplReader.h>
#include <medItkImageDataVtkReader.h>
#include <medItkImageDataPhilipsRecReader.h>
#include <medItkImageDataGisReader.h>
#include <medItkImageDataDcmtkReader.h>

#include <medItkImageMetaDataWriter.h>
#include <medItkImageDataNiftiWriter.h>
#include <medItkImageDataNrrdWriter.h>
#include <medItkImageDataGiplWriter.h>
#include <medItkImageDataVtkWriter.h>
#include <medItkImageDataGisWriter.h>

#include <dtkLog/dtkLog.h>
#include <itkLogForwarder.h>

// /////////////////////////////////////////////////////////////////
// medItkImagePluginPrivate
// /////////////////////////////////////////////////////////////////

class medItkImagePluginPrivate
{
public:
    medItkImagePluginPrivate()
    {
        forwarder = itk::LogForwarder::New();
    }
    ~medItkImagePluginPrivate()
    {

    }
    // Class variables go here.
    itk::LogForwarder::Pointer forwarder;
};

// /////////////////////////////////////////////////////////////////
// medItkImagePlugin
// /////////////////////////////////////////////////////////////////

medItkImagePlugin::medItkImagePlugin(QObject *parent) : dtkPlugin(parent), d(new medItkImagePluginPrivate)
{

}

medItkImagePlugin::~medItkImagePlugin()
{
    delete d;

    d = NULL;
}

bool medItkImagePlugin::initialize()
{
    if(!medItkChar3ImageData::registered()) { dtkWarn() << "Unable to register medItkChar3ImageData type"; }
    if(!medItkChar4ImageData::registered()) { dtkWarn() << "Unable to register medItkChar4ImageData type"; }
    if(!medItkInt4ImageData::registered()) { dtkWarn() << "Unable to register medItkInt4ImageData type"; }
    if(!medItkLong4ImageData::registered()) { dtkWarn() << "Unable to register medItkLong4ImageData type"; }
    if(!medItkUChar4ImageData::registered()) { dtkWarn() << "Unable to register medItkUChar4ImageData type"; }
    if(!medItkUChar3ImageData::registered()) { dtkWarn() << "Unable to register medItkUChar3ImageData type"; }
    if(!medItkShort3ImageData::registered()) { dtkWarn() << "Unable to register medItkShort3ImageData type"; }
    if(!medItkShort4ImageData::registered()) { dtkWarn() << "Unable to register medItkShort4ImageData type"; }
    if(!medItkUShort3ImageData::registered()) { dtkWarn() << "Unable to register medItkUShort3ImageData type"; }
    if(!medItkUShort4ImageData::registered()) { dtkWarn() << "Unable to register medItkUShort4ImageData type"; }
    if(!medItkUInt4ImageData::registered()) { dtkWarn() << "Unable to register medItkUInt4ImageData type"; }
    if(!medItkULong4ImageData::registered()) { dtkWarn() << "Unable to register medItkULong4ImageData type"; }
    if(!medItkInt3ImageData::registered()) { dtkWarn() << "Unable to register medItkInt3ImageData type"; }
    if(!medItkUInt3ImageData::registered()) { dtkWarn() << "Unable to register medItkUInt3ImageData type"; }
    if(!medItkLong3ImageData::registered()) { dtkWarn() << "Unable to register medItkLong3ImageData type"; }
    if(!medItkULong3ImageData::registered()) { dtkWarn() << "Unable to register medItkULong3ImageData type"; }
    if(!medItkFloat3ImageData::registered()) { dtkWarn() << "Unable to register medItkFloat3ImageData type"; }
    if(!medItkFloat4ImageData::registered()) { dtkWarn() << "Unable to register medItkFloat4ImageData type"; }
    if(!medItkDouble3ImageData::registered()) { dtkWarn() << "Unable to register medItkDouble3ImageData type"; }
    if(!medItkDouble4ImageData::registered()) { dtkWarn() << "Unable to register medItkDouble4ImageData type"; }
    if(!medItkVectorUChar3ImageData::registered()) { dtkWarn() << "Unable to register medItkVectorUChar3ImageData type"; }
    if(!medItkVectorFloat3ImageData::registered()) { dtkWarn() << "Unable to register medItkVectorFloat3ImageData type"; }
    if(!medItkVectorDouble3ImageData::registered()) { dtkWarn() << "Unable to register medItkVectorDouble3ImageData type"; }
    if(!medItkRgb3ImageData::registered()) { dtkWarn() << "Unable to register medItkRgb3ImageData type"; }
    if(!medItkRgba3ImageData::registered()) { dtkWarn() << "Unable to register medItkRgba3ImageData type"; }

    if(!medVtkViewItkScalarImageInteractor::registered()) { dtkWarn() << "Unable to register medVtkViewItkScalarImageInteractor"; }
    if(!medVtkViewItk4DScalarImageInteractor::registered()) { dtkWarn() << "Unable to register medVtkViewItk4DScalarImageInteractor"; }
    if(!medVtkViewItkVectorImageInteractor::registered()) { dtkWarn() << "Unable to register medVtkViewItkVectorImageInteractor"; }

    if(!medVtkViewItkScalarImageNavigator::registered()) { dtkWarn() << "Unable to register medVtkViewItkScalarImageNavigator"; }

    if (!medItkImageMetaDataReader::registered())       { dtkWarn() << "Unable to register medItkImageMetaDataReader type";       }
#ifdef ITK_USE_SYSTEM_GDCM
    if (!medItkImageDataGdcmReader::registered())       { dtkWarn() << "Unable to register  medItkImageDataGdcmReader type";      }
#endif
    if (!medItkImageDataNiftiReader::registered())      { dtkWarn() << "Unable to register medItkImageDataNiftiReader type";      }
    if (!medItkImageDataNrrdReader::registered())       { dtkWarn() << "Unable to register medItkImageDataNrrdReader type";       }
    if (!medItkImageDataGiplReader::registered())       { dtkWarn() << "Unable to register medItkImageDataGiplReader type";       }
    if (!medItkImageDataVtkReader::registered())        { dtkWarn() << "Unable to register medItkImageDataVtkReader type";        }
    if (!medItkImageDataPngReader::registered())        { dtkWarn() << "Unable to register medItkImageDataPngReader type";        }
    if (!medItkImageDataBmpReader::registered())        { dtkWarn() << "Unable to register medItkImageDataBmpReader type";        }
    if (!medItkImageDataJpegReader::registered())       { dtkWarn() << "Unable to register medItkImageDataJpegReader type";       }
    if (!medItkImageDataTiffReader::registered())       { dtkWarn() << "Unable to register medItkImageDataTiffReader type";       }
    if (!medItkImageDataPhilipsRecReader::registered()) { dtkWarn() << "Unable to register medItkImageDataPhilipsRecReader type"; }
    if (!medItkImageDataGisReader::registered())        { dtkWarn() << "Unable to register medItkImageDataGisReader type"; }
    if (!medItkImageDataDcmtkReader::registered())      { dtkWarn() << "Unable to register medItkImageDataDcmtkReader type"; }

    if (!medItkImageMetaDataWriter::registered())       { dtkWarn() << "Unable to register medItkImageMetaDataWriter type";       }
    if (!medItkImageDataNiftiWriter::registered())      { dtkWarn() << "Unable to register medItkImageDataNiftiWriter type";      }
    if (!medItkImageDataNrrdWriter::registered())       { dtkWarn() << "Unable to register medItkImageDataNrrdWriter type";       }
    if (!medItkImageDataGiplWriter::registered())       { dtkWarn() << "Unable to register medItkImageDataGiplWriter type";       }
    if (!medItkImageDataVtkWriter::registered())        { dtkWarn() << "Unable to register medItkImageDataVtkWriter type";        }
    if (!medItkImageDataGisWriter::registered())        { dtkWarn() << "Unable to register medItkImageDataGisWriter type"; }

    return true;
}

bool medItkImagePlugin::uninitialize()
{
    return true;
}

QString medItkImagePlugin::name() const
{
    return "medItkImagePlugin";
}

QString medItkImagePlugin::description() const
{
    return "Plugin containing all types of ITK images";
}

QString medItkImagePlugin::version() const
{
    return MEDITKIMAGEPLUGIN_VERSION;
}

QStringList medItkImagePlugin::tags() const
{
    return QStringList() << "itk" << "data" << "image";
}

QStringList medItkImagePlugin::types() const
{
    return QStringList() << "medItkChar3ImageData"
            << "medItkChar4ImageData"
            << "medItkUChar3ImageData"
            << "medItkUChar4ImageData"
            << "medItkShort3ImageData"
            << "medItkShort4ImageData"
            << "medItkUShort4ImageData"
            << "medItkUShort3ImageData"
            << "medItkInt3ImageData"
            << "medItkInt4ImageData"
            << "medItkLong4ImageData"
            << "medItkUInt4ImageData"
            << "medItkULong4ImageData"
            << "medItkUInt3ImageData"
            << "medItkLong3ImageData"
            << "medItkULong3ImageData"
            << "medItkFloat3ImageData"
            << "medItkFloat4ImageData"
            << "medItkDouble3ImageData"
            << "medItkDouble4ImageData"
            << "medItkVectorUChar3ImageData"
            << "medItkVectorFloat3ImageData"
            << "medItkVectorDouble3ImageData"
            << "medItkRgb3ImageData"
            << "medItkRgba3ImageData";
}

Q_EXPORT_PLUGIN2(medItkImagePlugin, medItkImagePlugin)
