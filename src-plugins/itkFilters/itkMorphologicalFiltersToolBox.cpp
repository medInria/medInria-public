/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkMorphologicalFiltersToolBox.h>

#include <limits>

#include <medAbstractDataFactory.h>
#include <medAbstractData.h>

#include <medAbstractImageData.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewInteractor.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medRunnableProcess.h>
#include <medJobManager.h>
#include <medPluginManager.h>

#include <medToolBoxFactory.h>
#include <medFilteringSelectorToolBox.h>
#include <medFilteringAbstractToolBox.h>
#include <medProgressionStack.h>

#include <itkFiltersProcessBase.h>

#include <QtGui>

class itkMorphologicalFiltersToolBoxPrivate
{
public:
    QLabel * dataTypeValue;

    QWidget * filterWidget;
    QDoubleSpinBox * kernelSize;
    
    QComboBox * filters;
    dtkSmartPointer <itkFiltersProcessBase> process;
    
    medProgressionStack * progressionStack;
};

itkMorphologicalFiltersToolBox::itkMorphologicalFiltersToolBox ( QWidget *parent ) : medFilteringAbstractToolBox ( parent ), d ( new itkMorphologicalFiltersToolBoxPrivate )
{
    qDebug() << "itkMorphologicalFiltersToolBox";
    //Filters selection combobox
    d->filters = new QComboBox;
    QStringList filtersList;
    filtersList << "Dilate "
                << "Erode "
                << "Close "
                << "Open ";
    
    d->filters->addItems ( filtersList );

    QLabel * dataTypeLabel = new QLabel ( tr ( "Data type :" ) );
    d->dataTypeValue = new QLabel ( tr ( "Unknown" ) );

    QHBoxLayout * dataTypeLayout = new QHBoxLayout;
    dataTypeLayout->addWidget ( dataTypeLabel );
    dataTypeLayout->addWidget ( d->dataTypeValue );

    // We use the same widget for all the morphological filters
    d->filterWidget = new QWidget(this);
    d->kernelSize = new QDoubleSpinBox;
    d->kernelSize->setMaximum ( 10 );
    d->kernelSize->setValue ( 1 );
    QLabel * morphoFilterLabel = new QLabel ( tr ( "Kernel radius:" ) );
    QHBoxLayout * morphoFilterLayout = new QHBoxLayout;
    QLabel * morphoFilterLabel2 = new QLabel ( tr ( " pixels" ) );
    morphoFilterLayout->addWidget ( morphoFilterLabel );
    morphoFilterLayout->addWidget ( d->kernelSize );
    morphoFilterLayout->addWidget ( morphoFilterLabel2 );
    morphoFilterLayout->addStretch ( 1 );
    d->filterWidget->setLayout ( morphoFilterLayout );


    // Run button:
    QPushButton *runButton = new QPushButton ( tr ( "Run" ) );
    runButton->setFocusPolicy ( Qt::NoFocus );
    runButton->setToolTip(tr("Launch the selected filter"));

    // Principal layout:
    QWidget *widget = new QWidget ( this );

    d->progressionStack = new medProgressionStack ( widget );

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget ( d->filters );
    layout->addLayout ( dataTypeLayout );
    layout->addWidget ( d->filterWidget );
    layout->addWidget ( runButton );
    layout->addWidget ( d->progressionStack );
    layout->addStretch ( 1 );

    widget->setLayout ( layout );

    // Main toolbox:
    this->setTitle ( tr ( "ITK Morphological Filters" ) );
    this->addWidget ( widget );

    // Add about plugin
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "itkFiltersPlugin" );
    setAboutPluginButton ( plugin );
    setAboutPluginVisibility ( true );

    connect ( runButton, SIGNAL ( clicked() ), this, SLOT ( run() ) );

}

itkMorphologicalFiltersToolBox::~itkMorphologicalFiltersToolBox()
{
    d->process.releasePointer();
    
    delete d;
    d = NULL;
}

bool itkMorphologicalFiltersToolBox::registered()
{
    medToolBoxFactory* factory = medToolBoxFactory::instance();
    return factory->registerToolBox<itkMorphologicalFiltersToolBox> (
                "itkMorphologicalFilters",
                "ITK Morphological Filters",
                "ITK morphological filters, processing examples",
                QStringList()<<"filtering"/*,
                createitkMorphologicalFiltersToolBox */);
}


medAbstractData* itkMorphologicalFiltersToolBox::processOutput()
{
    if ( !d->process )
        return NULL;

    return d->process->output();
}

void itkMorphologicalFiltersToolBox::clear()
{
    qDebug() << "Clear itk morphological filters toolbox";

    d->dataTypeValue->setText ( "Unknown" );
    
}

void itkMorphologicalFiltersToolBox::update ( dtkAbstractView* view )
{
    if ( !view )
    {
        clear();
    }
    else
    {
        if ( !this->parentToolBox()->data() )
        {
            return;
        }

        QString identifier = this->parentToolBox()->data()->identifier();

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

void itkMorphologicalFiltersToolBox::setupItkDilateProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkDilateProcess" );

    if (!d->process)
        return;
    
    d->process->setInput ( this->parentToolBox()->data() );
    d->process->setParameter ( d->kernelSize->value(), 0 );
}

void itkMorphologicalFiltersToolBox::setupItkErodeProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkErodeProcess" );

    if (!d->process)
        return;
    
    d->process->setInput ( this->parentToolBox()->data() );
    d->process->setParameter ( d->kernelSize->value(), 0 );
}

void itkMorphologicalFiltersToolBox::setupItkCloseProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkCloseProcess" );

    if (!d->process)
        return;
    
    d->process->setInput ( this->parentToolBox()->data() );
    d->process->setParameter ( d->kernelSize->value(), 0 );
}

void itkMorphologicalFiltersToolBox::setupItkOpenProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkOpenProcess" );

    if (!d->process)
        return;
    
    d->process->setInput ( this->parentToolBox()->data() );
    d->process->setParameter ( d->kernelSize->value(), 0 );
}

void itkMorphologicalFiltersToolBox::run ( void )
{
    if ( !this->parentToolBox() )
        return;

    if ( !this->parentToolBox()->data() )
        return;

//    if (d->process) {
//        d->process->deleteLater();
//    }
    
    //Set parameters :
    //   channel 0 : filter type
    //   channel 1,2,..,N : filter parameters
    switch ( d->filters->currentIndex() )
    {
    case 0: // dilate filter
        this->setupItkDilateProcess();
        break;
    case 1: // erode filter
        this->setupItkErodeProcess();
        break;
    case 2: // close filter
        this->setupItkCloseProcess();
        break;
    case 3: // open filter
        this->setupItkOpenProcess();
        break;
    }

    if (! d->process)
        return;

    medRunnableProcess *runProcess = new medRunnableProcess;
    runProcess->setProcess ( d->process );

    d->progressionStack->addJobItem ( runProcess, tr ( "Progress:" ) );

    connect ( runProcess, SIGNAL ( success ( QObject* ) ),  this, SIGNAL ( success () ) );
    connect ( runProcess, SIGNAL ( failure ( QObject* ) ),  this, SIGNAL ( failure () ) );

    medJobManager::instance()->registerJobItem ( runProcess );
    QThreadPool::globalInstance()->start ( dynamic_cast<QRunnable*> ( runProcess ) );
}

dtkPlugin* itkMorphologicalFiltersToolBox::plugin()
{
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "itkFiltersPlugin" );
    return plugin;
}


medToolBox *createitkMorphologicalFiltersToolBox ( QWidget *parent )
{
    return new itkMorphologicalFiltersToolBox ( parent );
}
