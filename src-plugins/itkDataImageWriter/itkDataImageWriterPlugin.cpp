/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/





#include "itkDataImageWriterPlugin.h"
#include "itkMetaDataImageWriter.h"
#include "itkAnalyzeDataImageWriter.h"
#include "itkPNGDataImageWriter.h"
#include "itkBMPDataImageWriter.h"
#include "itkJPEGDataImageWriter.h"
#include "itkTIFFDataImageWriter.h"
#include "itkNiftiDataImageWriter.h"
#include "itkNrrdDataImageWriter.h"
#include "itkGiplDataImageWriter.h"
#include "itkVTKDataImageWriter.h"
#include "itkPhilipsRECDataImageWriter.h"

#include <dtkLog/dtkLog.h>
#include <dtkItkLogForwarder/itkLogForwarder.h>

// /////////////////////////////////////////////////////////////////
// itkDataImageWriterPluginPrivate
// /////////////////////////////////////////////////////////////////

class itkDataImageWriterPluginPrivate
{
public:
    itkDataImageWriterPluginPrivate()
    {
        forwarder = itk::LogForwarder::New();
    }
    ~itkDataImageWriterPluginPrivate()
    {

    }
    // Class variables go here.
    itk::LogForwarder::Pointer forwarder;
};

// /////////////////////////////////////////////////////////////////
// itkDataImageWriterPlugin
// /////////////////////////////////////////////////////////////////

itkDataImageWriterPlugin::itkDataImageWriterPlugin(QObject *parent) : dtkPlugin(parent), d(new itkDataImageWriterPluginPrivate)
{

}

itkDataImageWriterPlugin::~itkDataImageWriterPlugin()
{
    delete d;

    d = NULL;
}

bool itkDataImageWriterPlugin::initialize()
{
#if 0
    if (!itkDataImageChar3Writer::registered())   { dtkWarn() << "Unable to register itkDataImageChar3Writer type";   }
    if (!itkDataImageUChar3Writer::registered())  { dtkWarn() << "Unable to register itkDataImageUChar3Writer type";  }
    if (!itkDataImageShort3Writer::registered())  { dtkWarn() << "Unable to register itkDataImageShort3Writer type";  }
    if (!itkDataImageUShort3Writer::registered()) { dtkWarn() << "Unable to register itkDataImageUShort3Writer type"; }
    if (!itkDataImageInt3Writer::registered())    { dtkWarn() << "Unable to register itkDataImageInt3Writer type";    }
    if (!itkDataImageUInt3Writer::registered())   { dtkWarn() << "Unable to register itkDataImageUInt3Writer type";   }
    if (!itkDataImageLong3Writer::registered())   { dtkWarn() << "Unable to register itkDataImageLong3Writer type";   }
    if (!itkDataImageULong3Writer::registered())  { dtkWarn() << "Unable to register itkDataImageULong3Writer type";  }
    if (!itkDataImageFloat3Writer::registered())  { dtkWarn() << "Unable to register itkDataImageFloat3Writer type";  }
    if (!itkDataImageDouble3Writer::registered()) { dtkWarn() << "Unable to register itkDataImageDouble3Writer type"; }
    if (!itkDataImageVector3Writer::registered()) { dtkWarn() << "Unable to register itkDataImageVector3Writer type"; }
    if (!itkDataImageRGB3Writer::registered())    { dtkWarn() << "Unable to register itkDataImageRGB3Writer type";    }
#endif

    if (!itkMetaDataImageWriter::registered())       { dtkWarn() << "Unable to register itkMetaDataImageWriter type";       }
    if (!itkNiftiDataImageWriter::registered())      { dtkWarn() << "Unable to register itkNiftiDataImageWriter type";      }
    if (!itkAnalyzeDataImageWriter::registered())    { dtkWarn() << "Unable to register itkAnalyzeDataImageWriter type";    }
    if (!itkNrrdDataImageWriter::registered())       { dtkWarn() << "Unable to register itkNrrdDataImageWriter type";       }
    if (!itkGiplDataImageWriter::registered())       { dtkWarn() << "Unable to register itkGiplDataImageWriter type";       }
    if (!itkVTKDataImageWriter::registered())        { dtkWarn() << "Unable to register itkVTKDataImageWriter type";        }
    if (!itkPNGDataImageWriter::registered())        { dtkWarn() << "Unable to register itkPNGDataImageWriter type";        }
    if (!itkBMPDataImageWriter::registered())        { dtkWarn() << "Unable to register itkBMPDataImageWriter type";        }
    if (!itkJPEGDataImageWriter::registered())       { dtkWarn() << "Unable to register itkJPEGDataImageWriter type";       }
    if (!itkTIFFDataImageWriter::registered())       { dtkWarn() << "Unable to register itkTIFFDataImageWriter type";       }
    if (!itkPhilipsRECDataImageWriter::registered()) { dtkWarn() << "Unable to register itkPhilipsRECDataImageWriter type"; }

    return true;
}

bool itkDataImageWriterPlugin::uninitialize()
{
    return true;
}

QString itkDataImageWriterPlugin::name() const
{
    return "itkDataImageWriterPlugin";
}

QString itkDataImageWriterPlugin::description() const
{
    return "Writers for any ITK image data type.";
}

QString itkDataImageWriterPlugin::version() const
{
    return ITKDATAIMAGEWRITERPLUGIN_VERSION;
}


QStringList itkDataImageWriterPlugin::tags() const
{
    return QStringList() << "itk" << "data" << "image" << "writer";
}

QStringList itkDataImageWriterPlugin::types() const
{
    return QStringList() << "itkMetaDataImageWriter"
            << "itkNiftiDataImageWriter"
            << "itkAnalyzeDataImageWriter"
            << "itkNrrdDataImageWriter"
            << "itkGiplDataImageWriter"
            << "itkVTKDataImageWriter"
            << "itkPNGDataImageWriter"
            << "itkBMPDataImageWriter"
            << "itkJPEGDataImageWriter"
            << "itkTIFFDataImageWriter";
}

Q_EXPORT_PLUGIN2(itkDataImageWriterPlugin, itkDataImageWriterPlugin)
