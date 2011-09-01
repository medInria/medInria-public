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
    QSpinBox * shrink0Value;
    QSpinBox * shrink1Value;
    QSpinBox * shrink2Value;
    QComboBox * filters;
    dtkAbstractProcess *process;
    medProgressionStack * progression_stack;
};

itkFiltersToolBox::itkFiltersToolBox ( QWidget *parent ) : medToolBoxFilteringCustom ( parent ), d ( new itkFiltersToolBoxPrivate )
{
    d->filters = new QComboBox;
    QStringList filtersList;
    filtersList << "Add Constant to Image" << "Multiply image by constant" << "Divide image by constant";
    filtersList << "Gaussian blur" << "Normalize image filter" << "Median filter" << "Invert intensity filter" << "Shrink image filter";
    d->filters->addItems ( filtersList );

    d->filtersStack = new QStackedWidget;
    QObject::connect ( d->filters, SIGNAL ( activated ( int ) ), d->filtersStack, SLOT ( setCurrentIndex ( int ) ) );

    //Initialise filters widget (probably need to find a dynamic way of doing this, Factory ?)
    //Add filter widgets
    QWidget * addFilterWidget = new QWidget;
    d->addFiltersValue = new QDoubleSpinBox;
    d->addFiltersValue->setValue ( 100.0 );
    d->addFiltersValue->setMaximum ( 1000000000 );
    QLabel * addFilterLabel = new QLabel ( "Constant value:" );
    QHBoxLayout * addFilterLayout = new QHBoxLayout;
    addFilterLayout->addWidget ( addFilterLabel );
    addFilterLayout->addWidget ( d->addFiltersValue );
    addFilterWidget->setLayout ( addFilterLayout );

    //Multiply filter widgets
    QWidget * multiplyFilterWidget = new QWidget;
    d->multiplyFiltersValue = new QDoubleSpinBox;
    d->multiplyFiltersValue->setValue ( 2.0 );
    d->multiplyFiltersValue->setMaximum ( 1000000000 );
    QLabel * multiplyFilterLabel = new QLabel ( "Constant value:" );
    QHBoxLayout * multiplyFilterLayout = new QHBoxLayout;
    multiplyFilterLayout->addWidget ( multiplyFilterLabel );
    multiplyFilterLayout->addWidget ( d->multiplyFiltersValue );
    multiplyFilterWidget->setLayout ( multiplyFilterLayout );

    //Divide filter widgets
    QWidget * divideFilterWidget = new QWidget;
    d->divideFiltersValue = new QDoubleSpinBox;
    d->divideFiltersValue->setValue ( 2.0 );
    d->divideFiltersValue->setMaximum ( 1000000000 );
    QLabel * divideFilterLabel = new QLabel ( "Constant value:" );
    QHBoxLayout * divideFilterLayout = new QHBoxLayout;
    divideFilterLayout->addWidget ( divideFilterLabel );
    divideFilterLayout->addWidget ( d->divideFiltersValue );
    divideFilterWidget->setLayout ( divideFilterLayout );

    //Gaussian filter widgets
    QWidget * gaussianFilterWidget = new QWidget;
    d->gaussianFiltersValue = new QDoubleSpinBox;
    d->gaussianFiltersValue->setValue ( 1.0 );
    d->gaussianFiltersValue->setMaximum ( 10.0 );
    QLabel * gaussianFilterLabel = new QLabel ( "Sigma value:" );
    QHBoxLayout * gaussianFilterLayout = new QHBoxLayout;
    gaussianFilterLayout->addWidget ( gaussianFilterLabel );
    gaussianFilterLayout->addWidget ( d->gaussianFiltersValue );
    gaussianFilterWidget->setLayout ( gaussianFilterLayout );

    QWidget * normalizeFilterWidget = new QWidget;
    QWidget * medianFilterWidget = new QWidget;
    QWidget * invertFilterWidget = new QWidget;

    //Shrink filter widgets
    QWidget * shrinkFilterWidget = new QWidget;
    d->shrink0Value = new QSpinBox;
    d->shrink0Value->setValue ( 1 );
    d->shrink0Value->setMaximum ( 10 );

    d->shrink1Value = new QSpinBox;
    d->shrink1Value->setValue ( 1 );
    d->shrink1Value->setMaximum ( 10 );

    d->shrink2Value = new QSpinBox;
    d->shrink2Value->setValue ( 1 );
    d->shrink2Value->setMaximum ( 10 );
    
    QLabel * shrinkFilterLabel = new QLabel ( "Shrink factors (X,Y,Z):" );
    QHBoxLayout * shrinkFilterLayout = new QHBoxLayout;
    shrinkFilterLayout->addWidget ( shrinkFilterLabel );
    shrinkFilterLayout->addWidget ( d->shrink0Value );
    shrinkFilterLayout->addWidget ( d->shrink1Value );
    shrinkFilterLayout->addWidget ( d->shrink2Value );
    shrinkFilterWidget->setLayout ( shrinkFilterLayout );


    //Add filters widget to the QStacked widget
    d->filtersStack->addWidget ( addFilterWidget );
    d->filtersStack->addWidget ( multiplyFilterWidget );
    d->filtersStack->addWidget ( divideFilterWidget );
    d->filtersStack->addWidget ( gaussianFilterWidget );
    d->filtersStack->addWidget ( normalizeFilterWidget );
    d->filtersStack->addWidget ( medianFilterWidget );
    d->filtersStack->addWidget ( invertFilterWidget );
    d->filtersStack->addWidget ( shrinkFilterWidget );

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
    this->setTitle ( "ITK Basic Filters" );
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
    return medToolBoxFactory::instance()->registerCustomFilteringToolBox ( "ITK Basic Filters", createitkFiltersToolBox );
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
    //   channel 0 : filter type
    //   channel 1,2,..,N : filter parameters
    switch ( d->filters->currentIndex() )
    {
    case 0: // add filter
        d->process->setParameter ( 0.0,0 );
        d->process->setParameter ( d->addFiltersValue->value(),1 );
        break;
    case 1: // multiply filter
        d->process->setParameter ( 1.0,0 );
        d->process->setParameter ( d->multiplyFiltersValue->value(),1 );
        break;
    case 2: // divide filter
        d->process->setParameter ( 2.0,0 );
        d->process->setParameter ( d->divideFiltersValue->value(),1 );
        break;
    case 3: // gaussian filter
        d->process->setParameter ( 3.0,0 );
        d->process->setParameter ( d->gaussianFiltersValue->value(),1 );
        break;
    case 4: // normalize filter
        d->process->setParameter ( 4.0,0 );
        break;
    case 5: // median filter
        d->process->setParameter ( 5.0,0 );
        break;
    case 6: // invert intensity filter
        d->process->setParameter ( 6.0,0 );
        break;
    case 7: // invert intensity filter
        d->process->setParameter ( 7.0,0 );
        d->process->setParameter ( (double) d->shrink0Value->value(),1 );
        d->process->setParameter ( (double) d->shrink1Value->value(),2 );
        d->process->setParameter ( (double) d->shrink2Value->value(),3 );
        break;
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
