/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkFiltersPlugin.h>

#include <medItkBasicFiltersProcess.h>
//#include <itkMorphologicalFilters.h>
#include <medItkAddProcess.h>
//#include <itkFiltersSubtractProcess.h>
#include <medItkMultiplyProcess.h>
#include <medItkDivideProcess.h>
#include <medItkFiltersGaussianProcess.h>
#include <medItkMedianProcess.h>
#include <medItkNormalizeProcess.h>
#include <medItkInvertProcess.h>
//#include <itkFiltersShrinkProcess.h>
//#include <itkFiltersWindowingProcess.h>
#include <medItkDilateProcess.h>
#include <medItkErodeProcess.h>
#include <medItkCloseProcess.h>
#include <medItkOpenProcess.h>
#include <medItkFiltersToolBox.h>
//#include <itkMorphologicalFiltersToolBox.h>
#include <medItkSubtractImageFilter.h>

#include <dtkLog/dtkLog.h>

class medItkFiltersPluginPrivate
{
public:
};

medItkFiltersPlugin::medItkFiltersPlugin ( QObject* parent ) : dtkPlugin ( parent ), d ( new medItkFiltersPluginPrivate )
{

}

medItkFiltersPlugin::~medItkFiltersPlugin()
{
    delete d;

    d = NULL;
}

bool medItkFiltersPlugin::initialize()
{
//    if ( !itkFilters::registered() )                 { dtkWarn() << "Unable to register itkFilters type";                         }
    if ( !medItkBasicFiltersProcess::registered() )       { dtkWarn() << "Unable to register medItkBasicFiltersProcess type";             }
//    if ( !itkMorphologicalFilters::registered() )       { dtkWarn() << "Unable to register itkMorphologicalFilters type";             }
    if ( !medItkAddProcess::registered() )       { dtkWarn() << "Unable to register medItkAddProcess add process type";             }
//    if ( !itkFiltersSubtractProcess::registered() )  { dtkWarn() << "Unable to register itkFilters subtract process type";        }
    if ( !medItkMultiplyProcess::registered() )  { dtkWarn() << "Unable to register itkFilters multiply process type";        }
    if ( !medItkDivideProcess::registered() )    { dtkWarn() << "Unable to register itkFilters divide process type";          }
    if ( !medItkFiltersGaussianProcess::registered() )  { dtkWarn() << "Unable to register itkFilters gaussian filter process type"; }
    if ( !medItkMedianProcess::registered() )    { dtkWarn() << "Unable to register itkFilters median filter process type";   }
    if ( !medItkNormalizeProcess::registered() ) { dtkWarn() << "Unable to register itkFilters normalize filter process type";}
//    if ( !itkFiltersWindowingProcess::registered() ) { dtkWarn() << "Unable to register itkFilters windowing filter process type";}
    if ( !medItkInvertProcess::registered() )    { dtkWarn() << "Unable to register itkFilters invert filter process type";   }
//    if ( !itkFiltersShrinkProcess::registered() )    { dtkWarn() << "Unable to register itkFilters shrink filter process type";   }
    if ( !medItkDilateProcess::registered() )    { dtkWarn() << "Unable to register itkFilters dilate filter process type";   }
    if ( !medItkErodeProcess::registered() )     { dtkWarn() << "Unable to register itkFilters erode filter process type";    }
    if ( !medItkCloseProcess::registered() )     { dtkWarn() << "Unable to register itkFilters close filter process type";    }
    if ( !medItkOpenProcess::registered() )      { dtkWarn() << "Unable to register itkFilters open filter process type";     }
    if ( !medItkFiltersToolBox::registered() )          { dtkWarn() << "Unable to register itkFilters toolbox";                      }
//    if ( !itkMorphologicalFiltersToolBox::registered() )          { dtkWarn() << "Unable to register itkMorphologicalFilters toolbox";                      }
    if ( !medItkSubtractImageFilter::registered() )    { dtkWarn() << "Unable to register medItkSubtractImageFilter type";   }


    return true;
}

bool medItkFiltersPlugin::uninitialize()
{
    return true;
}

QString medItkFiltersPlugin::description() const
{
  QString description;
  description = "This plugin implements some basic filters of ITK:      \
                 add constant, subtract constant, multiply by constant, \
                 divide by constant, gaussian filter, normalize filter, \
                 median filter, invert filter, shrink filter and intensity filter";
  return description;
}

QString medItkFiltersPlugin::name() const
{
    return "medItkFiltersPlugin";
}

QString medItkFiltersPlugin::contact() const
{
    return QString::fromUtf8("Alexandre Abadie <Alexandre.Abadie@inria.fr>");
}

QString medItkFiltersPlugin::version() const
{
    return MEDITKFILTERSPLUGIN_VERSION;
}

QStringList medItkFiltersPlugin::authors() const
{
    return QStringList() << "Alexandre Abadie <Alexandre.Abadie@inria.fr>";
}

QStringList medItkFiltersPlugin::contributors() const
{
    return QStringList() << QString::fromUtf8("Benoit Bleuzé <Benoit.Bleuze@inria.fr>");
}

QStringList medItkFiltersPlugin::dependencies() const
{
    return dtkPlugin::dependencies();
}

QStringList medItkFiltersPlugin::tags() const
{
    return QStringList() << "ITK" << "process" << "preprocessing" << "filtering";
}

QStringList medItkFiltersPlugin::types() const
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
                         << "itkWindowingProcess"
                         << "itkDilateProcess"
                         << "itkErodeProcess"
                         << "itkCloseProcess"
                         << "itkOpenProcess";
}

Q_EXPORT_PLUGIN2 ( medItkFiltersPlugin, medItkFiltersPlugin )
