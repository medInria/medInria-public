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

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewInteractor.h>
#include <dtkCore/dtkSmartPointer.h>

#include <itkMorphologicalFiltersProcessBase.h>

#include <medAbstractDataFactory.h>
#include <medAbstractData.h>
#include <medAbstractImageData.h>
#include <medAbstractToolBox.h>
#include <medJobManager.h>
#include <medPluginManager.h>
#include <medProgressionStack.h>
#include <medRunnableProcess.h>
#include <medSelectorToolBox.h>
#include <medToolBoxFactory.h>

#include <QtGui>

class itkMorphologicalFiltersToolBoxPrivate
{
public:
    QDoubleSpinBox * kernelSize;
    QRadioButton *mmButton, *pixelButton;
    
    medComboBox * filters;
    dtkSmartPointer <itkMorphologicalFiltersProcessBase> process;
    
    medProgressionStack * progressionStack;
};

itkMorphologicalFiltersToolBox::itkMorphologicalFiltersToolBox ( QWidget *parent ) : medAbstractToolBox ( parent ), d ( new itkMorphologicalFiltersToolBoxPrivate )
{
    //Filters selection combobox
    d->filters = new medComboBox;
    d->filters->setObjectName("Dilate");
    QStringList filtersList;
    filtersList << "Dilate "
                << "Erode "
                << "Close "
                << "Open "
                << "Binary Close"
                << "Binary Open";
    
    d->filters->addItems ( filtersList );

    // We use the same widget for all the morphological filters
    QWidget *filterWidget = new QWidget(this);
    d->kernelSize = new QDoubleSpinBox;
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
    d->pixelButton->setObjectName("pixels");
    d->pixelButton->setToolTip(tr("If \"pixels\" is selected, the dimensions of the structuring element will be calculated in pixels."));
    connect(d->pixelButton, SIGNAL(toggled(bool)), this, SLOT(roundSpinBox(bool)));

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
    layout->addWidget ( filterWidget );
    layout->addWidget ( runButton );
    layout->addWidget ( d->progressionStack );
    layout->addStretch ( 1 );

    widget->setLayout ( layout );

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

void itkMorphologicalFiltersToolBox::run ( void )
{
    if ( this->selectorToolBox() )
    {
        if ( this->selectorToolBox()->data() )
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

            if ( d->process )
            {
                d->process->setInput ( this->selectorToolBox()->data() );
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
}

void itkMorphologicalFiltersToolBox::roundSpinBox(bool param)
{
    if(param)
    {
        d->kernelSize->setDecimals(0);
    }
    else
    {
        d->kernelSize->setDecimals(2);
    }
}

dtkPlugin* itkMorphologicalFiltersToolBox::plugin()
{
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "itkFiltersPlugin" );
    return plugin;
}
