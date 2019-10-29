/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersAddProcess.h>
#include <itkFiltersBinaryCloseProcess.h>
#include <itkFiltersBinaryOpenProcess.h>
#include <itkFiltersBinaryThresholdingProcess.h>
#include <itkFiltersComponentSizeThresholdProcess.h>
#include <itkFiltersDilateProcess.h>
#include <itkFiltersDivideProcess.h>
#include <itkFiltersErodeProcess.h>
#include <itkFiltersGaussianProcess.h>
#include <itkFiltersGrayscaleCloseProcess.h>
#include <itkFiltersGrayscaleOpenProcess.h>
#include <itkFiltersInvertProcess.h>
#include <itkFiltersMedianProcess.h>
#include <itkFiltersMultiplyProcess.h>
#include <itkFiltersNormalizeProcess.h>
#include <itkFiltersPlugin.h>
#include <itkFiltersShrinkProcess.h>
#include <itkFiltersSubtractProcess.h>
#include <itkFiltersThresholdingProcess.h>
#include <itkFiltersToolBox.h>
#include <itkFiltersWindowingProcess.h>
#include <itkMorphologicalFiltersToolBox.h>

itkFiltersPlugin::itkFiltersPlugin(QObject* parent) : medPluginLegacy (parent)
{
}

bool itkFiltersPlugin::initialize()
{
    if ( !itkFiltersAddProcess::registered() )
    {
        qWarning() << "Unable to register itkFilters add process type";
    }
    if ( !itkFiltersBinaryThresholdingProcess::registered() )
    {
        qWarning() << "Unable to register itkFilters binary thresholding filter process type";
    }
    if ( !itkFiltersSubtractProcess::registered() )
    {
        qWarning() << "Unable to register itkFilters subtract process type";
    }
    if ( !itkFiltersMultiplyProcess::registered() )
    {
        qWarning() << "Unable to register itkFilters multiply process type";
    }
    if ( !itkFiltersDivideProcess::registered() )
    {
        qWarning() << "Unable to register itkFilters divide process type";
    }
    if ( !itkFiltersGaussianProcess::registered() )
    {
        qWarning() << "Unable to register itkFilters gaussian filter process type";
    }
    if ( !itkFiltersMedianProcess::registered() )
    {
        qWarning() << "Unable to register itkFilters median filter process type";
    }
    if ( !itkFiltersNormalizeProcess::registered() )
    {
        qWarning() << "Unable to register itkFilters normalize filter process type";
    }
    if ( !itkFiltersWindowingProcess::registered() )
    {
        qWarning() << "Unable to register itkFilters windowing filter process type";
    }
    if ( !itkFiltersInvertProcess::registered() )
    {
        qWarning() << "Unable to register itkFilters invert filter process type";
    }
    if ( !itkFiltersThresholdingProcess::registered() )
    {
        qWarning() << "Unable to register itkFilters thresholding filter process type";
    }
    if ( !itkFiltersComponentSizeThresholdProcess::registered() )
    {
        qWarning() << "Unable to register itkFilters component size thresholding filter process type";
    }
    if ( !itkFiltersShrinkProcess::registered() )
    {
        qWarning() << "Unable to register itkFilters shrink filter process type";
    }
    if ( !itkFiltersDilateProcess::registered() )
    {
        qWarning() << "Unable to register itkFilters dilate filter process type";
    }
    if ( !itkFiltersErodeProcess::registered() )
    {
        qWarning() << "Unable to register itkFilters erode filter process type";
    }
    if ( !itkFiltersToolBox::registered() )
    {
        qWarning() << "Unable to register itkFilters toolbox";
    }
    if ( !itkMorphologicalFiltersToolBox::registered() )
    {
        qWarning() << "Unable to register itkMorphologicalFilters toolbox";
    }
    if ( !itkFiltersGrayscaleCloseProcess::registered() )
    {
        qWarning() << "Unable to register itkFiltersGrayscaleCloseProcess type";
    }
    if ( !itkFiltersGrayscaleOpenProcess::registered() )
    {
        qWarning() << "Unable to register itkFiltersGrayscaleOpenProcess type";
    }
    if ( !itkFiltersBinaryCloseProcess::registered() )
    {
        qWarning() << "Unable to register itkFiltersBinaryCloseProcess type";
    }
    if ( !itkFiltersBinaryOpenProcess::registered() )
    {
        qWarning() << "Unable to register itkFiltersBinaryOpenProcess type";
    }

    return true;
}

QString itkFiltersPlugin::description() const
{
  QString description = \
          "This plugin implements two toolboxes:  \
          <br><h5>Morphological Filters</h5><br>  \
          Processes: <i>dilate, erode, binary close, binary open, grayscale close, grayscale open</i>.\
          <br><h5>ITK Basic Filters</h5><br>  \
          Processes: <i>add constant, subtract constant, multiply by constant, \
          divide by constant, gaussian filter, normalize filter, \
          median filter, invert filter, shrink filter, intensity filter, \
          threshold and isolated pixels removal</i>.  \
          <br><br>This plugin uses the <a href=\"https://itk.org/\" style=\"color: #cc0000\" >ITK library</a>.";
  return description;
}

QString itkFiltersPlugin::name() const
{
    return "ITK Filters";
}

QString itkFiltersPlugin::version() const
{
    return ITKFILTERSPLUGIN_VERSION;
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
                         << "itkWindowingProcess"
                         << "itkDilateProcess"
                         << "itkErodeProcess"
                         << "itkBinaryCloseProcess"
                         << "itkBinaryOpenProcess"
                         << "itkGrayscaleCloseProcess"
                         << "itkGrayscaleOpenProcess";
}
