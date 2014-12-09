/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkFiltersToolBox.h>

#include <limits>

#include <medAbstractData.h>
#include <medAbstractImageData.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medJobManager.h>
#include <medPluginManager.h>

#include <medToolBoxHeader.h>
#include <medToolBoxFactory.h>

#include <medFilteringSelectorToolBox.h>
#include <medFilteringAbstractToolBox.h>

#include <medItkFiltersProcessBase.h>

#include <QtGui>

class medItkFiltersToolBoxPrivate
{
public:
    QLabel * dataTypeValue;
};

medItkFiltersToolBox::medItkFiltersToolBox ( QWidget *parent ) : medProcessSelectorToolBox ( parent ), d ( new medItkFiltersToolBoxPrivate )
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
    dtkPlugin* plugin = pm->plugin ( "medItkFiltersPlugin" );
    setAboutPluginButton ( plugin );
    setAboutPluginVisibility ( true );
}

medItkFiltersToolBox::~medItkFiltersToolBox()
{
    delete d;
    d = NULL;
}

bool medItkFiltersToolBox::registered()
{
    medToolBoxFactory* factory = medToolBoxFactory::instance();
    return factory->registerToolBox<medItkFiltersToolBox> ();
}


void medItkFiltersToolBox::clear()
{
    qDebug() << "Clear itk filters toolbox";

    d->dataTypeValue->setText ( "Unknown" );
}

void medItkFiltersToolBox::update (medAbstractData *data )
{
    if (!data)
        clear();
    else
    {
        QString identifier = data->identifier();

        if ( identifier == "medItkChar3ImageData" )
        {
            d->dataTypeValue->setText ( "Char" );
        }
        else if ( identifier == "medItkUChar3ImageData" )
        {
            d->dataTypeValue->setText ( "Unsigned char" );
        }
        else if ( identifier == "medItkShort3ImageData" )
        {
            d->dataTypeValue->setText ( "Short" );
        }
        else if ( identifier == "medItkUShort3ImageData" )
        {
            d->dataTypeValue->setText ( "Unsigned short" );
        }
        else if ( identifier == "medItkInt3ImageData" )
        {
            d->dataTypeValue->setText ( "Int" );
        }
        else if ( identifier == "medItkUInt3ImageData" )
        {
            d->dataTypeValue->setText ( "Unsigned int" );
        }
        else if ( identifier == "medItkLong3ImageData" )
        {
            d->dataTypeValue->setText ( "Long" );
        }
        else if ( identifier== "medItkULong3ImageData" )
        {
            d->dataTypeValue->setText ( "Unsigned long" );
        }
        else if ( identifier == "medItkFloat3ImageData" )
        {
            d->dataTypeValue->setText ( "Float" );
        }
        else if ( identifier == "medItkDouble3ImageData" )
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


medToolBox *createmedItkFiltersToolBox ( QWidget *parent )
{
    return new medItkFiltersToolBox ( parent );
}
