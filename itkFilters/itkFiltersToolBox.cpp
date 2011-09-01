/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2011  Alexandre <email>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "itkFilters.h"
#include "itkFiltersToolBox.h"

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>

#include <medCore/medAbstractDataImage.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewInteractor.h>

#include <medRunnableProcess.h>
#include <medJobManager.h>

#include <medToolBoxFactory.h>
#include <medToolBoxFiltering.h>
#include <medToolBoxFilteringCustom.h>
#include <medProgressionStack.h>

#include <QtGui>

class itkFiltersToolBoxPrivate
{
public:
    QStackedWidget * filtersStack;
    QDoubleSpinBox * addFiltersValue;
    QDoubleSpinBox * multiplyFiltersValue;
    QDoubleSpinBox * divideFiltersValue;
    QDoubleSpinBox * gaussianFiltersValue;
    QComboBox * filters;
    dtkAbstractProcess *process;
    medProgressionStack * progression_stack;
};

itkFiltersToolBox::itkFiltersToolBox ( QWidget *parent ) : medToolBoxFilteringCustom ( parent ), d ( new itkFiltersToolBoxPrivate )
{
    d->filters = new QComboBox;
    QStringList filtersList;
    filtersList << "Add Constant to Image" << "Multiply image by constant" << "Divide image by constant";
    filtersList << "Gaussian blur" << "NULL filter" << "Median filter";
    d->filters->addItems ( filtersList );

    d->filtersStack = new QStackedWidget;
    QObject::connect ( d->filters, SIGNAL ( activated ( int ) ), d->filtersStack, SLOT ( setCurrentIndex ( int ) ) );

    //Initialise filters widget (probably need to find a dynamic way of doing this, Factory ?)
    QWidget * addFilterWidget = new QWidget;
    d->addFiltersValue = new QDoubleSpinBox;
    d->addFiltersValue->setValue(100.0);
    d->addFiltersValue->setMaximum ( 1000000000 );
    QLabel * addFilterLabel = new QLabel ( "Constant value:" );
    QHBoxLayout * addFilterLayout = new QHBoxLayout;
    addFilterLayout->addWidget ( addFilterLabel );
    addFilterLayout->addWidget ( d->addFiltersValue );
    addFilterWidget->setLayout ( addFilterLayout );

    QWidget * multiplyFilterWidget = new QWidget;
    d->multiplyFiltersValue = new QDoubleSpinBox;
    d->multiplyFiltersValue->setValue(2.0);
    d->multiplyFiltersValue->setMaximum ( 1000000000 );
    QLabel * multiplyFilterLabel = new QLabel ( "Constant value:" );
    QHBoxLayout * multiplyFilterLayout = new QHBoxLayout;
    multiplyFilterLayout->addWidget ( multiplyFilterLabel );
    multiplyFilterLayout->addWidget ( d->multiplyFiltersValue );
    multiplyFilterWidget->setLayout ( multiplyFilterLayout );

    QWidget * divideFilterWidget = new QWidget;
    d->divideFiltersValue = new QDoubleSpinBox;
    d->divideFiltersValue->setValue(2.0);
    d->divideFiltersValue->setMaximum ( 1000000000 );
    QLabel * divideFilterLabel = new QLabel ( "Constant value:" );
    QHBoxLayout * divideFilterLayout = new QHBoxLayout;
    divideFilterLayout->addWidget ( divideFilterLabel );
    divideFilterLayout->addWidget ( d->divideFiltersValue );
    divideFilterWidget->setLayout ( divideFilterLayout );

    QWidget * gaussianFilterWidget = new QWidget;
    d->gaussianFiltersValue = new QDoubleSpinBox;
    d->gaussianFiltersValue->setValue ( 1.0 );
    d->gaussianFiltersValue->setMaximum ( 10.0 );
    QLabel * gaussianFilterLabel = new QLabel ( "Sigma value:" );
    QHBoxLayout * gaussianFilterLayout = new QHBoxLayout;
    gaussianFilterLayout->addWidget ( gaussianFilterLabel );
    gaussianFilterLayout->addWidget ( d->gaussianFiltersValue );
    gaussianFilterWidget->setLayout ( gaussianFilterLayout );

    QWidget * laplacianFilterWidget = new QWidget;

    QWidget * medianFilterWidget = new QWidget;

    //Add filters widget to the QStacked widget
    d->filtersStack->addWidget ( addFilterWidget );
    d->filtersStack->addWidget ( multiplyFilterWidget );
    d->filtersStack->addWidget ( divideFilterWidget );
    d->filtersStack->addWidget ( gaussianFilterWidget );
    d->filtersStack->addWidget ( laplacianFilterWidget );
    d->filtersStack->addWidget ( medianFilterWidget );

    // Parameters:

    // Run button:
    QPushButton *runButton = new QPushButton ( tr ( "Run" ) );
    runButton->setFocusPolicy ( Qt::NoFocus );

    // Principal layout:
    QWidget *widget = new QWidget ( this );

    d->progression_stack = new medProgressionStack ( widget );

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget ( d->filters );
    layout->addWidget ( d->filtersStack );
    layout->addWidget ( runButton );
    layout->addWidget ( d->progression_stack );

    widget->setLayout ( layout );

    // Main toolbox:
    this->setTitle ( "ITK Filters" );
    this->addWidget ( widget );

    connect ( runButton, SIGNAL ( clicked() ), this, SLOT ( run() ) );

}

itkFiltersToolBox::~itkFiltersToolBox ( void )
{
    delete d;

    d = NULL;
}

bool itkFiltersToolBox::registered ( void )
{
    return medToolBoxFactory::instance()->registerCustomFilteringToolBox ( "ITK Filters", createitkFiltersToolBox );
}


dtkAbstractData* itkFiltersToolBox::processOutput ( void )
{
    if ( !d->process )
        return NULL;

    return d->process->output();
}


void itkFiltersToolBox::run ( void )
{
    if ( !this->parent() )
        return;

    d->process = dtkAbstractProcessFactory::instance()->create ( "itkFilters" );

    if ( !this->parent()->data() )
        return;

    d->process->setInput ( this->parent()->data() );

    //Set parameters :
    //   0 : filter type
    //   1,2,..,N : filter parameters
    
    switch ( d->filters->currentIndex() )
    {
    case 0: // add filter
        d->process->setParameter(0.0,0);
        d->process->setParameter(d->addFiltersValue->value(),1);
        break;
    case 1: // multiply filter
        d->process->setParameter(1.0,0);
        d->process->setParameter(d->multiplyFiltersValue->value(),1);
        break;
    case 2: // divide filter
        d->process->setParameter(2.0,0);
        d->process->setParameter(d->divideFiltersValue->value(),1);
        break;
    case 3: // gaussian filter
        d->process->setParameter(3.0,0);
        d->process->setParameter(d->gaussianFiltersValue->value(),1);
        break;
    case 4: // laplacian filter
        d->process->setParameter(4.0,0);
        break;
    case 5: // median filter
        break;
        d->process->setParameter(5.0,0);
    }



    medRunnableProcess *runProcess = new medRunnableProcess;
    runProcess->setProcess ( d->process );

    d->progression_stack->addJobItem ( runProcess, tr ( "Progress:" ) );

    connect ( runProcess, SIGNAL ( success ( QObject* ) ),  this, SIGNAL ( success () ) );
    connect ( runProcess, SIGNAL ( failure ( QObject* ) ),  this, SIGNAL ( failure () ) );

    medJobManager::instance()->registerJobItem ( runProcess );
    QThreadPool::globalInstance()->start ( dynamic_cast<QRunnable*> ( runProcess ) );

}

medToolBoxFilteringCustom *createitkFiltersToolBox ( QWidget *parent )
{
    return new itkFiltersToolBox ( parent );
}
