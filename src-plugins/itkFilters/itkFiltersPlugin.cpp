#include "itkFiltersPlugin.h"

#include "itkFiltersAddProcess.h"
#include "itkFiltersSubtractProcess.h"
#include "itkFiltersMultiplyProcess.h"
#include "itkFiltersDivideProcess.h"
#include "itkFiltersGaussianProcess.h"
#include "itkFiltersMedianProcess.h"
#include "itkFiltersNormalizeProcess.h"
#include "itkFiltersInvertProcess.h"
#include "itkFiltersShrinkProcess.h"
#include "itkFiltersWindowingProcess.h"
#include "itkFiltersToolBox.h"
#include "itkFiltersDefinitions.h"

#include <dtkLog/dtkLog.h>

class itkFiltersPluginPrivate
{
public:
};

itkFiltersPlugin::itkFiltersPlugin ( QObject* parent ) : dtkPlugin ( parent ), d ( new itkFiltersPluginPrivate )
{

}

itkFiltersPlugin::~itkFiltersPlugin()
{
    delete d;

    d = NULL;
}

bool itkFiltersPlugin::initialize()
{
//    if ( !itkFilters::registered() )                 { dtkWarn() << "Unable to register itkFilters type";                         }
    if ( !itkFiltersAddProcess::registered() )       { dtkWarn() << "Unable to register itkFilters add process type";             }
    if ( !itkFiltersSubtractProcess::registered() )  { dtkWarn() << "Unable to register itkFilters subtract process type";        }
    if ( !itkFiltersMultiplyProcess::registered() )  { dtkWarn() << "Unable to register itkFilters multiply process type";        }
    if ( !itkFiltersDivideProcess::registered() )    { dtkWarn() << "Unable to register itkFilters divide process type";          }
    if ( !itkFiltersGaussianProcess::registered() )  { dtkWarn() << "Unable to register itkFilters gaussian filter process type"; }
    if ( !itkFiltersMedianProcess::registered() )  { dtkWarn() << "Unable to register itkFilters median filter process type"; }
    if ( !itkFiltersNormalizeProcess::registered() ) { dtkWarn() << "Unable to register itkFilters normalize filter process type"; }
    if ( !itkFiltersWindowingProcess::registered() ) { dtkWarn() << "Unable to register itkFilters windowing filter process type"; }
    if ( !itkFiltersInvertProcess::registered() )    { dtkWarn() << "Unable to register itkFilters invert filter process type"; }
    if ( !itkFiltersShrinkProcess::registered() )    { dtkWarn() << "Unable to register itkFilters shrink filter process type"; }
    if ( !itkFiltersToolBox::registered() )          { dtkWarn() << "Unable to register itkFilters toolbox";                      }

    return true;
}

bool itkFiltersPlugin::uninitialize()
{
    return true;
}

QString itkFiltersPlugin::description() const
{
  QString description;
  description = "This plugin implements some basic filters of ITK:      \
                 add constant, subtract constant, multiply by constant, \
                 divide by constant, gaussian filter, normalize filter, \
                 median filter, invert filter, shrink filter and intensity filter";
  return description;
}

QString itkFiltersPlugin::name() const
{
    return "itkFiltersPlugin";
}

QString itkFiltersPlugin::contact() const
{
    return QString::fromUtf8("Alexandre Abadie <Alexandre.Abadie@inria.fr>");
}

QString itkFiltersPlugin::version() const
{
    return PLUGIN_VERSION;
}

QStringList itkFiltersPlugin::authors() const
{
    return QStringList() << "Alexandre Abadie <Alexandre.Abadie@inria.fr>";
}

QStringList itkFiltersPlugin::contributors() const
{
    return QStringList() << QString::fromUtf8("Benoit Bleuzé <Benoit.Bleuze@inria.fr>");
}

QStringList itkFiltersPlugin::dependencies() const
{
    return dtkPlugin::dependencies();
}

QStringList itkFiltersPlugin::tags() const
{
    return QStringList() << "ITK" << "process" << "preprocessing" << "filtering";
}

QStringList itkFiltersPlugin::types() const
{
    return QStringList() << "itkAddProcess"
                         << "itkSubtractProcess"
                         << "itkMultiplyProcess"
                         << "itkDivideProcess"
                         << "itkGaussianProcess"
                         << "itkMedianProcess"
                         << "itkNormalizeProcess"
                         << "itkInvertProcess"
                         << "itkShrinkProcess"
                         << "itkWindowingProcess";
}

Q_EXPORT_PLUGIN2 ( itkFiltersPlugin, itkFiltersPlugin )
