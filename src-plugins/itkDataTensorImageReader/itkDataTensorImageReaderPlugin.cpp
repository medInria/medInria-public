/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkDataTensorImageReaderPlugin.h"

#include <dtkLog/dtkLog.h>

#include <dtkItkLogForwarder/itkLogForwarder.h>

#include <itkNrrdDataTensorImageReader.h>
#include <itkNiftiDataTensorImageReader.h>
#include <itkMetaDataTensorImageReader.h>

// /////////////////////////////////////////////////////////////////
// itkDataTensorImageReaderPluginPrivate
// /////////////////////////////////////////////////////////////////

class itkDataTensorImageReaderPluginPrivate
{
public:
  itkDataTensorImageReaderPluginPrivate()
  {
      forwarder = itk::LogForwarder::New();
  }
  ~itkDataTensorImageReaderPluginPrivate()
  {

  }
  // Class variables go here.
  itk::LogForwarder::Pointer forwarder;
};

// /////////////////////////////////////////////////////////////////
// itkDataTensorImageReaderPlugin
// /////////////////////////////////////////////////////////////////

itkDataTensorImageReaderPlugin::itkDataTensorImageReaderPlugin(QObject *parent) : dtkPlugin(parent), d(new itkDataTensorImageReaderPluginPrivate)
{

}

itkDataTensorImageReaderPlugin::~itkDataTensorImageReaderPlugin()
{
    delete d;

    d = NULL;
}

bool itkDataTensorImageReaderPlugin::initialize()
{
    if (!itkMetaDataTensorImageReader::registered())  { dtkWarn() << "Unable to register itkMetaDataTensorImageReader type";  }
    if (!itkNiftiDataTensorImageReader::registered()) { dtkWarn() << "Unable to register itkNiftiDataTensorImageReader type"; }
    if (!itkNrrdDataTensorImageReader::registered())  { dtkWarn() << "Unable to register itkNrrdDataTensorImageReader type";  }

    return true;
}

bool itkDataTensorImageReaderPlugin::uninitialize()
{
    return true;
}

QString itkDataTensorImageReaderPlugin::name() const
{
    return "itkDataTensorImageReaderPlugin";
}

QString itkDataTensorImageReaderPlugin::description() const
{
    return "ITK tensor data image readers";
}

QString itkDataTensorImageReaderPlugin::version() const
{
    return ITKDATATENSORIMAGEREADERPLUGIN_VERSION;
}

QStringList itkDataTensorImageReaderPlugin::tags() const
{
    return QStringList() << "itk" << "data" << "tensor" << "image" << "reader";
}

QStringList itkDataTensorImageReaderPlugin::types() const
{
    return QStringList() << "itkMetaDataTensorImageReader"
			 << "itkNiftiDataTensorImageReader"
			 << "itkNrrdDataTensorImageReader";
}

Q_EXPORT_PLUGIN2(itkDataTensorImageReaderPlugin, itkDataTensorImageReaderPlugin)
