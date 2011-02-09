#include "vtkDataMeshReader.h"

#include "vtkDataMeshReaderPlugin.h"

#include "vtkLogForwarder.h"
#include <dtkCore/dtkLog.h>


// /////////////////////////////////////////////////////////////////
// vtkDataMeshReaderPluginPrivate
// /////////////////////////////////////////////////////////////////

class vtkDataMeshReaderPluginPrivate 
{
public:
  vtkDataMeshReaderPluginPrivate()
  {
      forwarder = vtkLogForwarder::New();
  }
  ~vtkDataMeshReaderPluginPrivate()
  {
      forwarder->Delete();
  }
  // Class variables go here.
  vtkLogForwarder* forwarder;
};

// /////////////////////////////////////////////////////////////////
// vtkDataMeshReaderPlugin
// /////////////////////////////////////////////////////////////////

vtkDataMeshReaderPlugin::vtkDataMeshReaderPlugin(QObject *parent) : dtkPlugin(parent), d(new vtkDataMeshReaderPluginPrivate)
{

}

vtkDataMeshReaderPlugin::~vtkDataMeshReaderPlugin(void)
{
  delete d;

  d = NULL;
}

bool vtkDataMeshReaderPlugin::initialize(void)
{
  if(!vtkDataMeshReader::registered())     dtkWarning() << "Unable to register vtkDataMeshReader type";
  return true;
}

bool vtkDataMeshReaderPlugin::uninitialize(void)
{
  return true;
}

QString vtkDataMeshReaderPlugin::name(void) const
{
  return "vtkDataMeshReaderPlugin";
}

QString vtkDataMeshReaderPlugin::description(void) const
{
  return "Readers for any VTK point data type.";
}

QStringList vtkDataMeshReaderPlugin::tags(void) const
{
  return QStringList() << "vtk" << "data" << "mesh" << "reader";
}

QStringList vtkDataMeshReaderPlugin::types(void) const
{
  return QStringList() << "vtkDataMeshReader";
}

Q_EXPORT_PLUGIN2(vtkDataMeshReaderPlugin, vtkDataMeshReaderPlugin)
