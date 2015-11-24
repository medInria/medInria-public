/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
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

#include <itkMorphologicalFiltersProcessBase.h>

#include <QtGui>

class itkMorphologicalFiltersToolBoxPrivate
{
public:
    QLabel * dataTypeValue;

    QSpinBox * kernelSize;
    QRadioButton *mmButton, *pixelButton;
    
    QComboBox * filters;
    dtkSmartPointer <itkMorphologicalFiltersProcessBase> process;
    
    medProgressionStack * progressionStack;
};

itkMorphologicalFiltersToolBox::itkMorphologicalFiltersToolBox ( QWidget *parent ) : medFilteringAbstractToolBox ( parent ), d ( new itkMorphologicalFiltersToolBoxPrivate )
{
    //Filters selection combobox
    d->filters = new QComboBox;
    d->filters->setObjectName("Dilate");
    QStringList filtersList;
    filtersList << "Dilate "
                << "Erode "
                << "Close "
                << "Open "
                << "Binary Close"
                << "Binary Open";
    
    d->filters->addItems ( filtersList );

    QLabel * dataTypeLabel = new QLabel ( tr ( "Data type :" ) );
    d->dataTypeValue = new QLabel ( tr ( "Unknown" ) );

    QHBoxLayout * dataTypeLayout = new QHBoxLayout;
    dataTypeLayout->addWidget ( dataTypeLabel );
    dataTypeLayout->addWidget ( d->dataTypeValue );

    // We use the same widget for all the morphological filters
    QWidget *filterWidget = new QWidget(this);
    d->kernelSize = new QSpinBox;
    d->kernelSize->setMaximum ( 10 );
    d->kernelSize->setValue ( 1 );
    d->kernelSize->setObjectName("kernelSize");

    QLabel * morphoFilterLabel = new QLabel ( tr ( "Kernel radius:" ) );
    QHBoxLayout * morphoFilterLayout = new QHBoxLayout;
    d->mmButton = new QRadioButton(tr("mm"), this);
    d->mmButton->setObjectName("mm");
    d->mmButton->setToolTip(tr("If \"mm\" is selected, the dimensions of the structuring element will be calculated in mm."));
    d->mmButton->setChecked(true);

    d->pixelButton = new QRadioButton(tr("pixels"), this);
    d->pixelButton->setToolTip(tr("If \"pixels\" is selected, the dimensions of the structuring element will be calculated in pixels."));

    morphoFilterLayout->addWidget ( morphoFilterLabel );
    morphoFilterLayout->addWidget ( d->kernelSize );
    morphoFilterLayout->addWidget ( d->mmButton );
    morphoFilterLayout->addWidget ( d->pixelButton );
    morphoFilterLayout->addStretch ( 1 );
    filterWidget->setLayout ( morphoFilterLayout );

    // Run button:
    QPushButton *runButton = new QPushButton ( tr ( "Run" ) );
    runButton->setObjectName("Run");
    runButton->setFocusPolicy ( Qt::NoFocus );
    runButton->setToolTip(tr("Launch the selected filter"));

    // Principal layout:
    QWidget *widget = new QWidget ( this );

    d->progressionStack = new medProgressionStack ( widget );

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget ( d->filters );
    layout->addLayout ( dataTypeLayout );
    layout->addWidget ( filterWidget );
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
    return factory->registerToolBox<itkMorphologicalFiltersToolBox> ();
}


medAbstractData* itkMorphologicalFiltersToolBox::processOutput()
{
    if ( !d->process )
    {
        return NULL;
    }
    return d->process->output();
}

void itkMorphologicalFiltersToolBox::clear()
{
    d->dataTypeValue->setText ( "Unknown" );    
}

void itkMorphologicalFiltersToolBox::update(medAbstractData* data)
{
    if (!data)
        this->clear();
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

void itkMorphologicalFiltersToolBox::run ( void )
{
    if ( this->parentToolBox() )
    {
        if ( this->parentToolBox()->data() )
        {
            switch ( d->filters->currentIndex() )
            {
            case 0: // dilate filter
                d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkDilateProcess" );
                break;
            case 1: // erode filter
                d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkErodeProcess" );
                break;
            case 2: // close filter
                d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkCloseProcess" );
                break;
            case 3: // open filter
                d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkOpenProcess" );
                break;
            case 4: // binary close filter
                d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkFiltersBinaryCloseProcess" );
                break;
            case 5: // binary open filter
                d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkFiltersBinaryOpenProcess" );
                break;
            }

            if (! d->process)
                return;

            d->process->setInput ( this->parentToolBox()->data() );
            d->process->setParameter ( (double)d->kernelSize->value(), (d->pixelButton->isChecked())? 1:0 );

            medRunnableProcess *runProcess = new medRunnableProcess;
            runProcess->setProcess ( d->process );

            d->progressionStack->addJobItem ( runProcess, tr ( "Progress:" ) );

            connect ( runProcess, SIGNAL ( success ( QObject* ) ),  this, SIGNAL ( success () ) );
            connect ( runProcess, SIGNAL ( failure ( QObject* ) ),  this, SIGNAL ( failure () ) );

            medJobManager::instance()->registerJobItem ( runProcess );
            QThreadPool::globalInstance()->start ( dynamic_cast<QRunnable*> ( runProcess ) );
        }
    }
}

dtkPlugin* itkMorphologicalFiltersToolBox::plugin()
{
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "itkFiltersPlugin" );
    return plugin;
}
