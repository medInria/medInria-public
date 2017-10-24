/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataTensorImagePlugin.h>


#include <itkDataTensorImageDouble3.h>
#include <itkDataTensorImageFloat3.h>
#include <itkDataTensorImageVtkViewInteractor.h>

#include <itkMetaDataTensorImageWriter.h>
#include <itkNiftiDataTensorImageWriter.h>
#include <itkNrrdDataTensorImageWriter.h>

#include <itkNrrdDataTensorImageReader.h>
#include <itkNiftiDataTensorImageReader.h>
#include <itkMetaDataTensorImageReader.h>

#include <dtkLog/dtkLog.h>

#include <itkLogForwarder.h>


// /////////////////////////////////////////////////////////////////
// itkDataTensorImagePluginPrivate
// /////////////////////////////////////////////////////////////////

class itkDataTensorImagePluginPrivate
{
public:
  itkDataTensorImagePluginPrivate()
  {
      forwarder = itk::LogForwarder::New();
  }
  ~itkDataTensorImagePluginPrivate()
  {

  }
  // Class variables go here.
  itk::LogForwarder::Pointer forwarder;
};

// /////////////////////////////////////////////////////////////////
// itkDataTensorImagePlugin
// /////////////////////////////////////////////////////////////////

itkDataTensorImagePlugin::itkDataTensorImagePlugin(QObject *parent) : medPlugin(parent), d(new itkDataTensorImagePluginPrivate)
{

}

itkDataTensorImagePlugin::~itkDataTensorImagePlugin()
{
    delete d;

    d = NULL;
}

bool itkDataTensorImagePlugin::initialize()
{
    if (!itkDataTensorImageDouble3::registered()) { dtkWarn() << "Unable to register itkDataTensorImageDouble3 type"; }
    if (!itkDataTensorImageFloat3::registered())  { dtkWarn() << "Unable to register itkDataTensorImageFloat3 type";  }
    if (!itkDataTensorImageVtkViewInteractor::registered())  { dtkWarn() << "Unable to register itkDataTensorImageVtkViewInteractor type";  }

    if (!itkMetaDataTensorImageWriter::registered())  { dtkWarn() << "Unable to register itkMetaDataTensorImageWriter type";  }
    if (!itkNiftiDataTensorImageWriter::registered()) { dtkWarn() << "Unable to register itkNiftiDataTensorImageWriter type"; }
    if (!itkNrrdDataTensorImageWriter::registered())  { dtkWarn() << "Unable to register itkNrrdDataTensorImageWriter type";  }

    if (!itkMetaDataTensorImageReader::registered())  { dtkWarn() << "Unable to register itkMetaDataTensorImageReader type";  }
    if (!itkNiftiDataTensorImageReader::registered()) { dtkWarn() << "Unable to register itkNiftiDataTensorImageReader type"; }
    if (!itkNrrdDataTensorImageReader::registered())  { dtkWarn() << "Unable to register itkNrrdDataTensorImageReader type";  }

    return true;
}

QString itkDataTensorImagePlugin::name() const
{
    return "ITK Data Tensor Image";
}

QString itkDataTensorImagePlugin::description() const
{
    return "Handle ITK tensor image types";
}

QStringList itkDataTensorImagePlugin::tags() const
{
    return QStringList() << "itk" << "data" << "tensor" << "image" << "reader" << "writer";
}

QString itkDataTensorImagePlugin::version() const
{
    return ITKDATATENSORIMAGEPLUGIN_VERSION;
}

QStringList itkDataTensorImagePlugin::types() const
{
    return QStringList() << "itkDataTensorImageFloat3"
			                   << "itkDataTensorImageDouble3"
                         << "itkMetaDataTensorImageWriter"
                         << "itkNiftiDataTensorImageWriter"
                         << "itkNrrdDataTensorImageWriter"
                         << "itkMetaDataTensorImageReader"
                         << "itkNiftiDataTensorImageReader"
                         << "itkNrrdDataTensorImageReader";
}

Q_EXPORT_PLUGIN2(itkDataTensorImagePlugin, itkDataTensorImagePlugin)
