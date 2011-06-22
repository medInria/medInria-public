#include "CompositeDataTypePlugin.h"
#include "dtkCompositeDataSet.h"

#include <dtkCore/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// CompositeDataTypePluginPrivate
// /////////////////////////////////////////////////////////////////

class CompositeDataTypePluginPrivate
{
public:
  CompositeDataTypePluginPrivate()
  {
      // forwarder = vtkLogForwarder::New();
  }
  ~CompositeDataTypePluginPrivate()
  {
      // forwarder->Delete();
  }
  // Class variables go here.
  // vtkLogForwarder* forwarder;
};

// /////////////////////////////////////////////////////////////////
// CompositeDataTypePlugin
// /////////////////////////////////////////////////////////////////

CompositeDataTypePlugin::CompositeDataTypePlugin(QObject *parent) : dtkPlugin(parent), d(new CompositeDataTypePluginPrivate)
{

}

CompositeDataTypePlugin::~CompositeDataTypePlugin(void)
{
    delete d;
    d = NULL;
}

bool CompositeDataTypePlugin::initialize(void)
{
  if(!dtkCompositeDataSet::registered()) qDebug() << "Unable to register dtkCompositeDataSet type";
  return true;
}

bool CompositeDataTypePlugin::uninitialize(void)
{
    return true;
}

QString CompositeDataTypePlugin::name(void) const
{
    return "CompositeDataTypePlugin";
}

QString CompositeDataTypePlugin::description(void) const
{
    return "Plugin containing types of composite datasets";
}

QStringList CompositeDataTypePlugin::tags(void) const
{
  return QStringList() << "composite";
}

QStringList CompositeDataTypePlugin::types(void) const
{
  return QStringList() << "dtkCompositeDataSet";
}

Q_EXPORT_PLUGIN2(CompositeDataTypePlugin, CompositeDataTypePlugin)
