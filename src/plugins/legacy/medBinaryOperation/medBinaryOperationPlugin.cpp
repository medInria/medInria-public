/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkAndOperator.h"
#include "itkNotOperator.h"
#include "itkOrOperator.h"
#include "itkXorOperator.h"
#include "medBinaryOperationPlugin.h"
#include "medBinaryOperationToolBox.h"

medBinaryOperationPlugin::medBinaryOperationPlugin(QObject *parent) : dtkPlugin(parent)
{
}

bool medBinaryOperationPlugin::initialize()
{
    if(!itkAndOperator::registered())
    {
        dtkWarn() << "Unable to register itkAndOperator type";
    }

    if (!itkNotOperator::registered())
    {
        dtkWarn() << "Unable to register itkNotOperator type";
    }

    if(!itkOrOperator::registered())
    {
        dtkWarn() << "Unable to register itkOrOperator type";
    }
    
    if(!itkXorOperator::registered())
    {
        dtkWarn() << "Unable to register itkXorOperator type";
    }

    if (!medBinaryOperationToolBox::registered())
    {
        dtkWarn() << "Unable to register medBinaryOperation toolbox";
    }

    return true;
}

QString medBinaryOperationPlugin::name() const
{
    return "Binary Operation";
}

QString medBinaryOperationPlugin::description() const
{
    QString description = \
            "Apply binary operations on masks:<br><br> \
            <a href=\"https://itk.org/Doxygen/html/classitk_1_1XorImageFilter.html\" style=\"color: #cc0000\" >XOR</a>, \
            <a href=\"https://itk.org/Doxygen/html/classitk_1_1AndImageFilter.html\" style=\"color: #cc0000\" >AND</a>, \
            <a href=\"https://itk.org/Doxygen/html/classitk_1_1OrImageFilter.html\" style=\"color: #cc0000\" >OR</a> and \
            <a href=\"https://itk.org/Doxygen/html/classitk_1_1BinaryNotImageFilter.html\" style=\"color: #cc0000\" >NOT</a>. \
            <br><br>This plugin uses the <a href=\"https://itk.org/\" style=\"color: #cc0000\" >ITK library</a>.";
    return description;
}

QString medBinaryOperationPlugin::version() const
{
    return MEDBINARYOPERATIONPLUGIN_VERSION;
}

QStringList medBinaryOperationPlugin::types() const
{
    return QStringList() << "medBinaryOperation";
}

// TODO medPlugin
bool medBinaryOperationPlugin::uninitialize()
{
    return true;
}

// TODO medPlugin
QStringList medBinaryOperationPlugin::tags() const
{
    return QStringList();
}
