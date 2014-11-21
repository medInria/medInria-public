/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTtkTensorImagePlugin.h>


#include <medTtkDouble3TensorImageData.h>
#include <medTtkFloat3TensorImageData.h>
#include <medVtkViewTtkTensorImageInteractor.h>

#include <medTtkTensorImageMetaDataWriter.h>
#include <medTtkTensorImageDataNiftiWriter.h>
#include <medTtkTensorImageDataNrrdWriter.h>

#include <medTtkTensorImageDataNrrdReader.h>
#include <medTtkTensorImageDataNiftiReader.h>
#include <medTtkTensorImageMetaDataReader.h>

#include <dtkLog/dtkLog.h>

#include <itkLogForwarder.h>


// /////////////////////////////////////////////////////////////////
// medTtkTensorImagePluginPrivate
// /////////////////////////////////////////////////////////////////

class medTtkTensorImagePluginPrivate
{
public:
  medTtkTensorImagePluginPrivate()
  {
      forwarder = itk::LogForwarder::New();
  }
  ~medTtkTensorImagePluginPrivate()
  {

  }
  // Class variables go here.
  itk::LogForwarder::Pointer forwarder;
};

// /////////////////////////////////////////////////////////////////
// medTtkTensorImagePlugin
// /////////////////////////////////////////////////////////////////

medTtkTensorImagePlugin::medTtkTensorImagePlugin(QObject *parent) : dtkPlugin(parent), d(new medTtkTensorImagePluginPrivate)
{

}

medTtkTensorImagePlugin::~medTtkTensorImagePlugin()
{
    delete d;

    d = NULL;
}

bool medTtkTensorImagePlugin::initialize()
{
    if (!medTtkDouble3TensorImageData::registered()) { dtkWarn() << "Unable to register medTtkDouble3TensorImageData type"; }
    if (!medTtkFloat3TensorImageData::registered())  { dtkWarn() << "Unable to register medTtkFloat3TensorImageData type";  }
    if (!medVtkViewTtkTensorImageInteractor::registered())  { dtkWarn() << "Unable to register medVtkViewTtkTensorImageInteractor type";  }

    if (!medTtkTensorImageMetaDataWriter::registered())  { dtkWarn() << "Unable to register medTtkTensorImageMetaDataWriter type";  }
    if (!medTtkTensorImageDataNiftiWriter::registered()) { dtkWarn() << "Unable to register medTtkTensorImageDataNiftiWriter type"; }
    if (!medTtkTensorImageDataNrrdWriter::registered())  { dtkWarn() << "Unable to register medTtkTensorImageDataNrrdWriter type";  }

    if (!medTtkTensorImageMetaDataReader::registered())  { dtkWarn() << "Unable to register medTtkTensorImageMetaDataReader type";  }
    if (!medTtkTensorImageDataNiftiReader::registered()) { dtkWarn() << "Unable to register medTtkTensorImageDataNiftiReader type"; }
    if (!medTtkTensorImageDataNrrdReader::registered())  { dtkWarn() << "Unable to register medTtkTensorImageDataNrrdReader type";  }

    return true;
}

bool medTtkTensorImagePlugin::uninitialize()
{
    return true;
}

QString medTtkTensorImagePlugin::name() const
{
    return "medTtkTensorImagePlugin";
}

QString medTtkTensorImagePlugin::description() const
{
    return "Handle ITK tensor image types";
}

QStringList medTtkTensorImagePlugin::tags() const
{
    return QStringList() << "itk" << "data" << "tensor" << "image" << "reader" << "writer";
}

QString medTtkTensorImagePlugin::version() const
{
    return MEDTTKTENSORIMAGEPLUGIN_VERSION;
}


QStringList medTtkTensorImagePlugin::types() const
{
    return QStringList() << "medTtkFloat3TensorImageData"
                         << "medTtkDouble3TensorImageData"
                         << "medTtkTensorImageMetaDataWriter"
                         << "medTtkTensorImageDataNiftiWriter"
                         << "medTtkTensorImageDataNrrdWriter"
                         << "medTtkTensorImageMetaDataReader"
                         << "medTtkTensorImageDataNiftiReader"
                         << "medTtkTensorImageDataNrrdReader";
}

Q_EXPORT_PLUGIN2(medTtkTensorImagePlugin, medTtkTensorImagePlugin)
