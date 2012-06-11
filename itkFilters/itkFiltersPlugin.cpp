#include "itkFilters.h"
#include "itkFiltersPlugin.h"
#include "itkFiltersToolBox.h"
#include "itkFiltersDefinitions.h"

#include <dtkCore/dtkLog.h>

class itkFiltersPluginPrivate
{
public:
};

itkFiltersPlugin::itkFiltersPlugin ( QObject* parent ) : dtkPlugin ( parent ), d ( new itkFiltersPluginPrivate )
{

}

itkFiltersPlugin::~itkFiltersPlugin ( void )
{
    delete d;

    d = NULL;
}

bool itkFiltersPlugin::initialize ( void )
{
    if ( !itkFilters::registered() )
        dtkWarning() << "Unable to register itkFilters type";

    if ( !itkFiltersToolBox::registered() )
        dtkWarning() << "Unable to register itkFilters toolbox";

    return true;
}

bool itkFiltersPlugin::uninitialize ( void )
{
    return true;
}

QString itkFiltersPlugin::description ( void ) const
{
  QString description;
  description = "This plugin implements some basic filters of ITK:      \
                 add constant, subtract constant, multiply by constant, \
                 divide by constant, gaussian filter, normalize filter, \
                 median filter, invert filter, shrink filter and intensity filter";
  return description;
}

QString itkFiltersPlugin::name ( void ) const
{
    return "itkFiltersPlugin";
}

QString itkFiltersPlugin::contact(void) const
{
    return QString::fromUtf8("Alexandre Abadie <Alexandre.Abadie@inria.fr>");
}

QString itkFiltersPlugin::version(void) const
{
    return PLUGIN_VERSION;
}

QStringList itkFiltersPlugin::authors ( void ) const
{
    return QStringList() << "Alexandre Abadie <Alexandre.Abadie@inria.fr>";
}

QStringList itkFiltersPlugin::contributors ( void ) const
{
    return QStringList() << QString::fromUtf8("Benoit Bleuzé <Benoit.Bleuze@inria.fr>");
}

QStringList itkFiltersPlugin::dependencies ( void ) const
{
    return dtkPlugin::dependencies();
}


QStringList itkFiltersPlugin::tags ( void ) const
{
    return QStringList() << "ITK" << "process" << "preprocessing" << "filtering";
}

QStringList itkFiltersPlugin::types ( void ) const
{
    return QStringList() << "itkFilters";
}

Q_EXPORT_PLUGIN2 ( itkFiltersPlugin, itkFiltersPlugin )
