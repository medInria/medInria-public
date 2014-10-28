/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersToolBox.h>

#include <limits>

#include <medAbstractData.h>
#include <medAbstractImageData.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medRunnableProcess.h>
#include <medJobManager.h>
#include <medPluginManager.h>

#include <medToolBoxHeader.h>
#include <medToolBoxFactory.h>

#include <medFilteringSelectorToolBox.h>
#include <medFilteringAbstractToolBox.h>

#include <itkFiltersProcessBase.h>

#include <QtGui>

class itkFiltersToolBoxPrivate
{
public:
    QLabel * dataTypeValue;
};

itkFiltersToolBox::itkFiltersToolBox ( QWidget *parent ) : medProcessSelectorToolBox ( parent ), d ( new itkFiltersToolBoxPrivate )
{
    QLabel * dataTypeLabel = new QLabel ( tr ( "Data type :" ) );
    d->dataTypeValue = new QLabel ( tr ( "Unknown" ) );

    QHBoxLayout * dataTypeLayout = new QHBoxLayout;
    dataTypeLayout->addWidget ( dataTypeLabel );
    dataTypeLayout->addWidget ( d->dataTypeValue );

    // Principal layout:
    QWidget *widget = new QWidget ( this );
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->addLayout ( dataTypeLayout );
    layout->addStretch ( 1 );

    // Main toolbox:
    this->setTitle ( tr ( "ITK Basic Filters" ) );
    this->addWidget ( widget );

    // Add about plugin
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "itkFiltersPlugin" );
    setAboutPluginButton ( plugin );
    setAboutPluginVisibility ( true );

    this->runButton()->hide();
}

itkFiltersToolBox::~itkFiltersToolBox()
{
    delete d;
    d = NULL;
}

bool itkFiltersToolBox::registered()
{
    medToolBoxFactory* factory = medToolBoxFactory::instance();
    return factory->registerToolBox<itkFiltersToolBox> ();
}


void itkFiltersToolBox::clear()
{
    qDebug() << "Clear itk filters toolbox";

    d->dataTypeValue->setText ( "Unknown" );
}

void itkFiltersToolBox::update (medAbstractData *data )
{
    if (!data)
        clear();
    else
    {
        QString identifier = data->identifier();

        if ( identifier == "itkDataImageChar3" )
        {
            d->dataTypeValue->setText ( "Char" );
        }
        else if ( identifier == "itkDataImageUChar3" )
        {
            d->dataTypeValue->setText ( "Unsigned char" );
        }
        else if ( identifier == "itkDataImageShort3" )
        {
            d->dataTypeValue->setText ( "Short" );
        }
        else if ( identifier == "itkDataImageUShort3" )
        {
            d->dataTypeValue->setText ( "Unsigned short" );
        }
        else if ( identifier == "itkDataImageInt3" )
        {
            d->dataTypeValue->setText ( "Int" );
        }
        else if ( identifier == "itkDataImageUInt3" )
        {
            d->dataTypeValue->setText ( "Unsigned int" );
        }
        else if ( identifier == "itkDataImageLong3" )
        {
            d->dataTypeValue->setText ( "Long" );
        }
        else if ( identifier== "itkDataImageULong3" )
        {
            d->dataTypeValue->setText ( "Unsigned long" );
        }
        else if ( identifier == "itkDataImageFloat3" )
        {
            d->dataTypeValue->setText ( "Float" );
        }
        else if ( identifier == "itkDataImageDouble3" )
        {
            d->dataTypeValue->setText ( "Double" );
        }
        else
        {
            qWarning() << "Error : pixel type not yet implemented ("
            << identifier
            << ")";
        }
    } 
}


medToolBox *createitkFiltersToolBox ( QWidget *parent )
{
    return new itkFiltersToolBox ( parent );
}
