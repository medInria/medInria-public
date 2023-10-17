/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkMorphologicalFiltersToolBox.h>

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <itkMorphologicalFiltersProcessBase.h>

#include <medComboBox.h>
#include <medPluginManager.h>
#include <medRunnableProcess.h>
#include <medSelectorToolBox.h>
#include <medToolBoxFactory.h>

class itkMorphologicalFiltersToolBoxPrivate
{
public:
    QDoubleSpinBox *kernelSize;
    QRadioButton *mmButton, *pixelButton;
    QRadioButton *ballKernelButton, *crossKernelButton, *boxKernelButton;
    
    medComboBox *filters;
    dtkSmartPointer <itkMorphologicalFiltersProcessBase> process;
};

itkMorphologicalFiltersToolBox::itkMorphologicalFiltersToolBox ( QWidget *parent )
    : medAbstractSelectableToolBox ( parent ), d ( new itkMorphologicalFiltersToolBoxPrivate )
{
    // Filters selection combobox
    d->filters = new medComboBox;
    d->filters->setObjectName("Dilate");
    QStringList filtersList;
    filtersList << "Dilate "
                << "Erode "
                << "Binary Close "
                << "Binary Open "
                << "Grayscale Close "
                << "Grayscale Open ";
    
    d->filters->addItems ( filtersList );

    // We use the same widget for all the morphological filters
    QWidget *kernelSizeWidget = new QWidget(this);
    d->kernelSize = new QDoubleSpinBox;
    d->kernelSize->setMaximum(100);
    d->kernelSize->setValue(1);
    d->kernelSize->setObjectName("kernelSize");

    QLabel *kernelSizeLabel = new QLabel ( tr ( "Kernel radius:" ) );
    QHBoxLayout *kernelSizeLayout = new QHBoxLayout;
    d->mmButton = new QRadioButton(tr("mm"), this);
    d->mmButton->setObjectName("mm");
    d->mmButton->setToolTip(tr("If \"mm\" is selected, the dimensions of the structuring element will be calculated in mm"));
    d->mmButton->setChecked(true);

    d->pixelButton = new QRadioButton(tr("pixels"), this);
    d->pixelButton->setObjectName("pixels");
    d->pixelButton->setToolTip(tr("If \"pixels\" is selected, the dimensions of the structuring element will be calculated in pixels"));
    connect(d->pixelButton, SIGNAL(toggled(bool)), this, SLOT(roundSpinBox(bool)), Qt::UniqueConnection);

    kernelSizeLayout->addWidget ( kernelSizeLabel );
    kernelSizeLayout->addWidget ( d->kernelSize );
    kernelSizeLayout->addWidget ( d->mmButton );
    kernelSizeLayout->addWidget ( d->pixelButton );
    kernelSizeLayout->addStretch ( 1 );
    kernelSizeWidget->setLayout ( kernelSizeLayout );

    QWidget *kernelShapeWidget = new QWidget();
    kernelShapeWidget->setLayout(new QHBoxLayout());

    kernelShapeWidget->layout()->addWidget(new QLabel(tr("Kernel shape:")));

    d->ballKernelButton = new QRadioButton(tr("ball"));
    d->ballKernelButton->setObjectName("ballKernelButton");
    d->ballKernelButton->setChecked(true);
    kernelShapeWidget->layout()->addWidget(d->ballKernelButton);

    d->boxKernelButton = new QRadioButton(tr("box"));
    d->boxKernelButton->setObjectName("boxKernelButton");
    kernelShapeWidget->layout()->addWidget(d->boxKernelButton);

    d->crossKernelButton = new QRadioButton(tr("cross"));
    d->crossKernelButton->setObjectName("crossKernelButton");
    kernelShapeWidget->layout()->addWidget(d->crossKernelButton);

    // Run button:
    QPushButton *runButton = new QPushButton ( tr ( "Run" ) );
    runButton->setObjectName("Run");
    runButton->setFocusPolicy ( Qt::NoFocus );
    runButton->setToolTip(tr("Launch the selected filter"));

    // Principal layout:
    QWidget *widget = new QWidget ( this );

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget ( d->filters );
    layout->addWidget ( kernelSizeWidget );
    layout->addWidget ( kernelShapeWidget );
    layout->addWidget ( runButton );
    layout->addStretch ( 1 );

    widget->setLayout(layout);
    this->addWidget(widget);

    connect(runButton, SIGNAL(clicked()), this, SLOT(run()), Qt::UniqueConnection);
}

itkMorphologicalFiltersToolBox::~itkMorphologicalFiltersToolBox()
{
    d->process.releasePointer();
    
    delete d;
    d = nullptr;
}

bool itkMorphologicalFiltersToolBox::registered()
{
    medToolBoxFactory *factory = medToolBoxFactory::instance();
    return factory->registerToolBox<itkMorphologicalFiltersToolBox> ();
}

medAbstractData* itkMorphologicalFiltersToolBox::processOutput()
{
    if ( !d->process )
    {
        return nullptr;
    }
    return d->process->output();
}

void itkMorphologicalFiltersToolBox::run()
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
            case 2: // binary close filter
                d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkBinaryCloseProcess" );
                break;
            case 3: // binary open filter
                d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkBinaryOpenProcess" );
                break;
            case 4: // grayscale close filter
                d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkGrayscaleCloseProcess" );
                break;
            case 5: // grayscale open filter
                d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkGrayscaleOpenProcess" );
                break;
            }

            if ( d->process )
            {
                this->setToolBoxOnWaitStatus();

                d->process->setInput ( this->selectorToolBox()->data() );
                d->process->setParameter ( (double)d->kernelSize->value(), (d->pixelButton->isChecked())? 1:0 );

                if (d->ballKernelButton->isChecked())
                {
                    d->process->setParameter(itkMorphologicalFiltersProcessBase::BallKernel);
                }
                else if (d->crossKernelButton->isChecked())
                {
                    d->process->setParameter(itkMorphologicalFiltersProcessBase::CrossKernel);
                }
                else if (d->boxKernelButton->isChecked())
                {
                    d->process->setParameter(itkMorphologicalFiltersProcessBase::BoxKernel);
                }

                medRunnableProcess *runProcess = new medRunnableProcess;
                runProcess->setProcess ( d->process );
                this->addConnectionsAndStartJob(runProcess);
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
    return medPluginManager::instance().plugin("ITK Filters");
}
