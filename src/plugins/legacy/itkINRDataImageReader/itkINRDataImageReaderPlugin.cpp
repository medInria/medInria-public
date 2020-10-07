/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 All rights reserved.
 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkINRDataImageReader.h"
#include "itkINRDataImageReaderPlugin.h"

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// itkINRDataImageReaderPlugin
// /////////////////////////////////////////////////////////////////

itkINRDataImageReaderPlugin::itkINRDataImageReaderPlugin(QObject *parent) : dtkPlugin(parent)
{
}

itkINRDataImageReaderPlugin::~itkINRDataImageReaderPlugin()
{
}

bool itkINRDataImageReaderPlugin::initialize()
{
    if(!itkINRDataImageReader::registered())
    {
        dtkWarn() << "Unable to register itkINRDataImageReader type";
    }
    
    return true;
}

bool itkINRDataImageReaderPlugin::uninitialize(void)
{
    return true;
}

QString itkINRDataImageReaderPlugin::name(void) const
{
    return "itkINRDataImageReaderPlugin";
}

QString itkINRDataImageReaderPlugin::contact(void) const
{
    return QString::fromUtf8("benoit.bleuze@inria.fr");
}

QStringList itkINRDataImageReaderPlugin::authors(void) const
{
    QStringList list;
    list << QString::fromUtf8("Benoît Bleuzé");
    return list;
}

QString itkINRDataImageReaderPlugin::description(void) const
{
    return tr("INRImage reader plugin");
}

QString itkINRDataImageReaderPlugin::version(void) const
{
    return ITKINRDATAIMAGEREADERPLUGIN_VERSION;
}

QStringList itkINRDataImageReaderPlugin::tags(void) const
{
    return QStringList();
}

QStringList itkINRDataImageReaderPlugin::types(void) const
{
    return QStringList() << "itkINRDataImageReader";
}
