// /////////////////////////////////////////////////////////////////
// Generated by dtkPluginGenerator
// /////////////////////////////////////////////////////////////////
#include "itkDataShImagePlugin.h"


#include <itkDataShImageDouble3.h>
#include <itkDataShImageFloat3.h>

#include <dtkLog/dtkLog.h>

#include <dtkItkLogForwarder/itkLogForwarder.h>


// /////////////////////////////////////////////////////////////////
// itkDataShImagePluginPrivate
// /////////////////////////////////////////////////////////////////

class itkDataShImagePluginPrivate
{
public:
  itkDataShImagePluginPrivate()
  {
      forwarder = itk::LogForwarder::New();
  }
  ~itkDataShImagePluginPrivate()
  {

  }
  // Class variables go here.
  itk::LogForwarder::Pointer forwarder;
};

// /////////////////////////////////////////////////////////////////
// itkDataShImagePlugin
// /////////////////////////////////////////////////////////////////

itkDataShImagePlugin::itkDataShImagePlugin(QObject *parent) : dtkPlugin(parent), d(new itkDataShImagePluginPrivate)
{

}

itkDataShImagePlugin::~itkDataShImagePlugin(void)
{
    delete d;

    d = NULL;
}

bool itkDataShImagePlugin::initialize(void)
{
    if(!itkDataShImageDouble3::registered()) dtkWarn() << "Unable to register itkDataShImageDouble3 type";
    if(!itkDataShImageFloat3::registered()) dtkWarn() << "Unable to register itkDataShImageFloat3 type";

    return true;
}

bool itkDataShImagePlugin::uninitialize(void)
{
    return true;
}

QString itkDataShImagePlugin::name(void) const
{
    return "itkDataShImagePlugin";
}

QString itkDataShImagePlugin::description(void) const
{
    return "Handle ITK Sh image types";
}

QStringList itkDataShImagePlugin::tags(void) const
{
    return QStringList() << "itk" << "data" << "Sh" << "image";
}

QStringList itkDataShImagePlugin::types(void) const
{
    return QStringList() << "itkDataShImageFloat3"
                         << "itkDataShImageDouble3";
}

Q_EXPORT_PLUGIN2(itkDataShImagePlugin, itkDataShImagePlugin)
