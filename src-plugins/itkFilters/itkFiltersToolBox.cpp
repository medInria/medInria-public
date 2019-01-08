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

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewInteractor.h>
#include <dtkCore/dtkSmartPointer.h>

#include <itkFiltersProcessBase.h>
#include <itkFiltersAddProcess.h>
#include <itkFiltersBinaryThresholdingProcess.h>
#include <itkFiltersComponentSizeThresholdProcess.h>
#include <itkFiltersDivideProcess.h>
#include <itkFiltersGaussianProcess.h>
#include <itkFiltersMedianProcess.h>
#include <itkFiltersMultiplyProcess.h>
#include <itkFiltersShrinkProcess.h>
#include <itkFiltersSubtractProcess.h>
#include <itkFiltersThresholdingProcess.h>
#include <itkFiltersWindowingProcess.h>
#include <medDoubleParameter.h>

#include <medAbstractDataFactory.h>
#include <medAbstractData.h>
#include <medAbstractImageData.h>
#include <medAbstractSelectableToolBox.h>
#include <medPluginManager.h>
#include <medRunnableProcess.h>
#include <medSelectorToolBox.h>
#include <medToolBoxFactory.h>
#include <medUtilitiesITK.h>

#include <QtGui>

#include <statsROI.h>

class itkFiltersToolBoxPrivate
{
public:
    QWidget * addFilterWidget;
    QWidget * subtractFilterWidget;
    QWidget * multiplyFilterWidget;
    QWidget * divideFilterWidget;
    QWidget * gaussianFilterWidget;
    QWidget * normalizeFilterWidget;
    QWidget * medianFilterWidget;
    QWidget * invertFilterWidget;
    QWidget * shrinkFilterWidget;
    QWidget * intensityFilterWidget;
    QWidget * thresholdFilterWidget;
    QWidget * componentSizeThresholdFilterWidget;

    QDoubleSpinBox * addFilterValue;
    QDoubleSpinBox * subtractFilterValue;
    QDoubleSpinBox * medianSizeFilterValue;
    QDoubleSpinBox * multiplyFilterValue;
    QDoubleSpinBox * divideFilterValue;
    QDoubleSpinBox * gaussianFilterValue;
    medDoubleParameter * thresholdFilterValue;
    medDoubleParameter * thresholdLowerValue;
    medDoubleParameter * thresholdUpperValue;

    QWidget * thresholdValueWidget;
    QWidget * thresholdUpperWidget;
    QWidget * thresholdLowerWidget;
    QWidget * buttonGroupWidget;
    QSpinBox * thresholdFilterValue2;
    QButtonGroup * valueButtonGroup;
    QRadioButton * binaryThreshold;
    QLabel * thresholdValueLabel;
    QSpinBox * shrink0Value;
    QSpinBox * shrink1Value;
    QSpinBox * shrink2Value;
    QSpinBox * componentSizeThresholdFilterValue;

    QDoubleSpinBox * intensityMinimumValue;
    QDoubleSpinBox * intensityMaximumValue;
    QDoubleSpinBox * intensityOutputMinimumValue;
    QDoubleSpinBox * intensityOutputMaximumValue;

    medComboBox * filters;
    dtkSmartPointer <itkFiltersProcessBase> process;
};

itkFiltersToolBox::itkFiltersToolBox ( QWidget *parent ) : medAbstractSelectableToolBox ( parent ), d ( new itkFiltersToolBoxPrivate )
{
    //Filters selection combobox
    d->filters = new medComboBox(this);
    d->filters->setObjectName("Add constant");
    QStringList filtersList;
    filtersList << "Add constant"
                << "Subtract constant"
                << "Multiply by constant"
                << "Divide by constant"
                << "Gaussian blur" 
                << "Normalize image"
                << "Median filter"
                << "Invert intensity"
                << "Shrink image"
                << "Intensity windowing"
                << "Threshold an image"
                << "Remove isolated voxels/pixels";
    
    d->filters->addItems ( filtersList );

    QObject::connect ( d->filters, SIGNAL ( currentIndexChanged ( int ) ), this, SLOT ( onFiltersActivated ( int ) ) );

    //Initialise filters widget (probably need to find a dynamic way of doing this, Factory ?)
    //Add filter widgets
    d->addFilterWidget = new QWidget(this);
    d->addFilterValue = new QDoubleSpinBox;
    d->addFilterValue->setMaximum ( 1000000000 );
    d->addFilterValue->setValue ( itkFiltersAddProcess::defaultAddValue );
    QLabel * addFilterLabel = new QLabel ( tr ( "Constant value:" ) );
    QHBoxLayout * addFilterLayout = new QHBoxLayout;
    addFilterLayout->addWidget ( addFilterLabel );
    addFilterLayout->addWidget ( d->addFilterValue );
    addFilterLayout->addStretch ( 1 );
    d->addFilterWidget->setLayout ( addFilterLayout );

    //Subtract filter widgets
    d->subtractFilterWidget = new QWidget(this);
    d->subtractFilterValue = new QDoubleSpinBox;
    d->subtractFilterValue->setMaximum ( 1000000000 );
    d->subtractFilterValue->setValue ( itkFiltersSubtractProcess::defaultSubtractValue );
    QLabel * subtractFilterLabel = new QLabel ( tr ( "Constant value:" ) );
    QHBoxLayout * subtractFilterLayout = new QHBoxLayout;
    subtractFilterLayout->addWidget ( subtractFilterLabel );
    subtractFilterLayout->addWidget ( d->subtractFilterValue );
    subtractFilterLayout->addStretch ( 1 );
    d->subtractFilterWidget->setLayout ( subtractFilterLayout );

    //Multiply filter widgets
    d->multiplyFilterWidget = new QWidget(this);
    d->multiplyFilterValue = new QDoubleSpinBox;
    d->multiplyFilterValue->setValue ( itkFiltersMultiplyProcess::defaultMultiplyFactor );
    d->multiplyFilterValue->setMaximum ( 1000000000 );
    QLabel * multiplyFilterLabel = new QLabel ( tr ( "Constant value:" ) );
    QHBoxLayout * multiplyFilterLayout = new QHBoxLayout;
    multiplyFilterLayout->addWidget ( multiplyFilterLabel );
    multiplyFilterLayout->addWidget ( d->multiplyFilterValue );
    multiplyFilterLayout->addStretch ( 1 );
    d->multiplyFilterWidget->setLayout ( multiplyFilterLayout );

    //Divide filter widgets
    d->divideFilterWidget = new QWidget(this);
    d->divideFilterValue = new QDoubleSpinBox;
    d->divideFilterValue->setValue ( itkFiltersDivideProcess::defaultDivideFactor );
    d->divideFilterValue->setMaximum ( 1000000000 );
    d->divideFilterValue->setMinimum(1);
    QLabel * divideFilterLabel = new QLabel ( tr ( "Constant value:" ) );
    QHBoxLayout * divideFilterLayout = new QHBoxLayout;
    divideFilterLayout->addWidget ( divideFilterLabel );
    divideFilterLayout->addWidget ( d->divideFilterValue );
    divideFilterLayout->addStretch ( 1 );
    d->divideFilterWidget->setLayout ( divideFilterLayout );

    //Gaussian filter widgets
    d->gaussianFilterWidget = new QWidget(this);
    d->gaussianFilterValue = new QDoubleSpinBox;
    d->gaussianFilterValue->setValue ( itkFiltersGaussianProcess::defaultSigma );
    d->gaussianFilterValue->setMaximum ( 10.0 );
    QLabel * gaussianFilterLabel = new QLabel ( tr ( "Sigma value:" ) );
    QHBoxLayout * gaussianFilterLayout = new QHBoxLayout;
    gaussianFilterLayout->addWidget ( gaussianFilterLabel );
    gaussianFilterLayout->addWidget ( d->gaussianFilterValue );
    gaussianFilterLayout->addStretch ( 1 );
    d->gaussianFilterWidget->setLayout ( gaussianFilterLayout );

    d->normalizeFilterWidget = new QWidget(this);
    QLabel* normExplanation = new QLabel("Normalize an image by setting its mean to zero and variance to one.");
    normExplanation->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    normExplanation->setWordWrap(true);
    QHBoxLayout * normalizeFilterLayout = new QHBoxLayout;
    normalizeFilterLayout->addWidget(normExplanation);
    d->normalizeFilterWidget->setLayout(normalizeFilterLayout);

    //Median filter widgets
    d->medianFilterWidget = new QWidget(this);
    d->medianSizeFilterValue = new QDoubleSpinBox;
    d->medianSizeFilterValue->setMaximum ( 1000000000 );
    d->medianSizeFilterValue->setValue ( itkFiltersMedianProcess::defaultMedianSize );
    QLabel * medianSizeFilterLabel = new QLabel ( tr ( "Neighborhood size:" ) );
    QHBoxLayout * medianSizeFilterLayout = new QHBoxLayout;
    medianSizeFilterLayout->addWidget ( medianSizeFilterLabel );
    medianSizeFilterLayout->addWidget ( d->medianSizeFilterValue );
    medianSizeFilterLayout->addStretch ( 1 );
    d->medianFilterWidget->setLayout ( medianSizeFilterLayout );


    d->invertFilterWidget = new QWidget(this);

    //Shrink filter widgets
    d->shrinkFilterWidget = new QWidget;
    d->shrink0Value = new QSpinBox;
    d->shrink0Value->setValue (itkFiltersShrinkProcess::defaultShrinkFactors[0]);
    d->shrink0Value->setMinimum ( 1 );
    d->shrink0Value->setMaximum ( 10 );

    d->shrink1Value = new QSpinBox;
    d->shrink1Value->setValue (itkFiltersShrinkProcess::defaultShrinkFactors[1]);
    d->shrink1Value->setMinimum ( 1 );
    d->shrink1Value->setMaximum ( 10 );

    d->shrink2Value = new QSpinBox;
    d->shrink2Value->setValue (itkFiltersShrinkProcess::defaultShrinkFactors[2]);
    d->shrink2Value->setMinimum ( 1 );
    d->shrink2Value->setMaximum ( 10 );

    QLabel * shrinkFilterLabel = new QLabel ( tr ( "Shrink factors (X,Y,Z):" ) );
    QFormLayout * shrinkFilterLayout = new QFormLayout;

    QVBoxLayout * shrinkFilterValueLayout = new QVBoxLayout;
    shrinkFilterValueLayout->addWidget ( d->shrink0Value );
    shrinkFilterValueLayout->addWidget ( d->shrink1Value );
    shrinkFilterValueLayout->addWidget ( d->shrink2Value );
    shrinkFilterValueLayout->addStretch ( 1 );
    shrinkFilterLayout->addRow ( shrinkFilterLabel, shrinkFilterValueLayout );
    d->shrinkFilterWidget->setLayout ( shrinkFilterLayout );

    //Intensity windowing filter widget
    d->intensityFilterWidget = new QWidget(this);
    d->intensityMinimumValue = new QDoubleSpinBox;
    d->intensityMaximumValue = new QDoubleSpinBox;
    d->intensityOutputMinimumValue = new QDoubleSpinBox;
    d->intensityOutputMaximumValue = new QDoubleSpinBox;
    clear();

    QLabel * intensityMinimumLabel = new QLabel ( tr ( "Minimum:" ) );
    QHBoxLayout * intensityMinimumLayout = new QHBoxLayout;
    intensityMinimumLayout->addWidget ( intensityMinimumLabel );
    intensityMinimumLayout->addWidget ( d->intensityMinimumValue );

    QLabel * intensityMaximumLabel = new QLabel ( tr ( "Maximum:" ) );
    QHBoxLayout * intensityMaximumLayout = new QHBoxLayout;
    intensityMaximumLayout->addWidget ( intensityMaximumLabel );
    intensityMaximumLayout->addWidget ( d->intensityMaximumValue );

    QLabel * intensityOutputMinimumLabel = new QLabel ( tr ( "Output minimum:" ) );
    QHBoxLayout * intensityOutputMinimumLayout = new QHBoxLayout;
    intensityOutputMinimumLayout->addWidget ( intensityOutputMinimumLabel );
    intensityOutputMinimumLayout->addWidget ( d->intensityOutputMinimumValue );

    QLabel * intensityOutputMaximumLabel = new QLabel ( tr ( "Output maximum:" ) );
    QHBoxLayout * intensityOutputMaximumLayout = new QHBoxLayout;
    intensityOutputMaximumLayout->addWidget ( intensityOutputMaximumLabel );
    intensityOutputMaximumLayout->addWidget ( d->intensityOutputMaximumValue );

    QVBoxLayout * intensityFilterLayout = new QVBoxLayout;
    intensityFilterLayout->addLayout ( intensityMinimumLayout );
    intensityFilterLayout->addLayout ( intensityMaximumLayout );
    intensityFilterLayout->addLayout ( intensityOutputMinimumLayout );
    intensityFilterLayout->addLayout ( intensityOutputMaximumLayout );
    intensityFilterLayout->addStretch ( 0 );

    d->intensityFilterWidget->setLayout ( intensityFilterLayout );

    //Threshold filter widgets
    d->thresholdFilterWidget = new QWidget(this);

    d->thresholdFilterValue = new medDoubleParameter( tr("Threshold Value"), this);
    d->thresholdFilterValue->setRange ( -10000, 10000 );
    d->thresholdFilterValue->setValue ( itkFiltersThresholdingProcess::defaultThreshold );
    d->thresholdFilterValue->setObjectName("thresholdValue");

    d->thresholdLowerValue = new medDoubleParameter( tr("Lower Value"), this);
    d->thresholdLowerValue->setRange ( -10000, 10000 );
    d->thresholdLowerValue->setValue ( itkFiltersThresholdingProcess::defaultLower );
    d->thresholdLowerValue->setObjectName("lowerValue");

    d->thresholdUpperValue = new medDoubleParameter( tr("Upper Value"), this);
    d->thresholdUpperValue->setRange ( -10000, 10000 );
    d->thresholdUpperValue->setValue ( itkFiltersThresholdingProcess::defaultUpper );
    d->thresholdUpperValue->setObjectName("upperValue");

    d->thresholdFilterValue2 = new QSpinBox;
    d->thresholdFilterValue2->setRange ( -10000, 10000 );
    d->thresholdFilterValue2->setValue ( itkFiltersThresholdingProcess::defaultOutsideValue );
    d->thresholdFilterValue2->setObjectName("outsideThresholdValue");

    d->binaryThreshold = new QRadioButton(tr("Binarize Image"), this);
    d->binaryThreshold->setObjectName("binaryThresholdButton");
    connect(d->binaryThreshold, SIGNAL(toggled(bool)), this, SLOT(checkBinaryThreshold(bool)));

    QRadioButton* greaterButton = new QRadioButton(tr(" greater "), this);
    greaterButton->setObjectName("greaterButton");
    QRadioButton* lowerButton = new QRadioButton(tr(" lower "), this);
    lowerButton->setObjectName("lowerButton");
    QRadioButton* outsideButton = new QRadioButton(tr(" outside "), this);
    outsideButton->setObjectName("outsideButton");

    d->valueButtonGroup = new QButtonGroup;
    d->valueButtonGroup->addButton(greaterButton, itkFiltersThresholdingProcess::upperButtonId);
    d->valueButtonGroup->addButton(lowerButton, itkFiltersThresholdingProcess::lowerButtonId);
    d->valueButtonGroup->addButton(outsideButton, itkFiltersThresholdingProcess::outsideButtonId);

    QLabel * thresholdFilterLabel = new QLabel ( tr ( "Threshold Type  :" ) );
    QLabel * thresholdFilterLabel2 = new QLabel ( tr ( "Outside Value :" ) );

    d->buttonGroupWidget = new QWidget(this);
    QHBoxLayout * greaterLowerOutsideLayout = new QHBoxLayout;
    greaterLowerOutsideLayout->addWidget(greaterButton);
    greaterLowerOutsideLayout->addWidget(lowerButton);
    greaterLowerOutsideLayout->addWidget(outsideButton);
    greaterLowerOutsideLayout->addStretch(-1);
    d->buttonGroupWidget->setLayout(greaterLowerOutsideLayout);

    d->thresholdLowerWidget = new QWidget(this);
    QHBoxLayout * thresholdLowerLayout = new QHBoxLayout;
    d->thresholdLowerValue->getSlider()->setOrientation(Qt::Horizontal);
    thresholdLowerLayout->addWidget(d->thresholdLowerValue->getLabel());
    thresholdLowerLayout->addWidget(d->thresholdLowerValue->getSlider());
    thresholdLowerLayout->addWidget(d->thresholdLowerValue->getSpinBox());
    d->thresholdLowerWidget->setLayout(thresholdLowerLayout);

    d->thresholdUpperWidget = new QWidget(this);
    QHBoxLayout * thresholdUpperLayout = new QHBoxLayout;
    d->thresholdUpperValue->getSlider()->setOrientation(Qt::Horizontal);
    thresholdUpperLayout->addWidget(d->thresholdUpperValue->getLabel());
    thresholdUpperLayout->addWidget(d->thresholdUpperValue->getSlider());
    thresholdUpperLayout->addWidget(d->thresholdUpperValue->getSpinBox());
    d->thresholdUpperWidget->setLayout(thresholdUpperLayout);

    d->thresholdValueWidget = new QWidget(this);
    QHBoxLayout * thresholdValueLayout = new QHBoxLayout;
    d->thresholdFilterValue->getSlider()->setOrientation(Qt::Horizontal);
    thresholdValueLayout->addWidget(d->thresholdFilterValue->getLabel());
    thresholdValueLayout->addWidget(d->thresholdFilterValue->getSlider());
    thresholdValueLayout->addWidget(d->thresholdFilterValue->getSpinBox());
    d->thresholdValueWidget->setLayout(thresholdValueLayout);

    QWidget * outsideValueWidget = new QWidget(this);
    QHBoxLayout * outsideValueLayout = new QHBoxLayout;
    outsideValueLayout->addWidget ( thresholdFilterLabel2 );
    outsideValueLayout->addWidget ( d->thresholdFilterValue2 );
    outsideValueLayout->setSizeConstraint(QLayout::SetMaximumSize);
    outsideValueWidget->setLayout(outsideValueLayout);

    QVBoxLayout * thresholdFilterLayout1 = new QVBoxLayout;
    thresholdFilterLayout1->addWidget( d->binaryThreshold );
    thresholdFilterLayout1->addWidget( thresholdFilterLabel );
    thresholdFilterLayout1->addWidget( d->buttonGroupWidget );
    thresholdFilterLayout1->addWidget( d->thresholdLowerWidget );
    thresholdFilterLayout1->addWidget( d->thresholdUpperWidget );
    thresholdFilterLayout1->addWidget( d->thresholdValueWidget );
    thresholdFilterLayout1->addWidget( outsideValueWidget );
    thresholdFilterLayout1->addStretch ( 1 );

    QVBoxLayout * thresholdFilterLayout = new QVBoxLayout;
    thresholdFilterLayout->addLayout(thresholdFilterLayout1);
    thresholdFilterLayout->addStretch(-1);

    d->thresholdFilterWidget->setLayout ( thresholdFilterLayout );

    connect(d->valueButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(updateThresholdToolboxBehaviour(int)));
    greaterButton->click();

    //Size threshold Widget
    d->componentSizeThresholdFilterWidget = new QWidget(this);
    d->componentSizeThresholdFilterValue = new QSpinBox;
    d->componentSizeThresholdFilterValue->setObjectName("Minimum size of an object: ");
    d->componentSizeThresholdFilterValue->setMaximum ( 100000 );
    d->componentSizeThresholdFilterValue->setValue ( itkFiltersComponentSizeThresholdProcess::defaultMinimumSize );
    QLabel * componentSizeThresholdFilterLabel = new QLabel ( tr ( "Minimum size in pixel of an object:" ) );
    QHBoxLayout * componentSizeThresholdFilterLayout = new QHBoxLayout;
    componentSizeThresholdFilterLayout->addWidget ( componentSizeThresholdFilterLabel );
    componentSizeThresholdFilterLayout->addWidget ( d->componentSizeThresholdFilterValue );
    componentSizeThresholdFilterLayout->addStretch ( 1 );
    d->componentSizeThresholdFilterWidget->setLayout ( componentSizeThresholdFilterLayout );

    // Run button:
    QPushButton *runButton = new QPushButton ( tr ( "Run" ) );
    runButton->setObjectName("Run");
    runButton->setFocusPolicy ( Qt::NoFocus );
    runButton->setToolTip(tr("Launch the selected filter"));

    // Principal layout:
    QWidget *widget = new QWidget ( this );

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget ( d->filters );
    layout->addWidget ( d->addFilterWidget );
    layout->addWidget ( d->subtractFilterWidget );
    layout->addWidget ( d->multiplyFilterWidget );
    layout->addWidget ( d->divideFilterWidget );
    layout->addWidget ( d->gaussianFilterWidget );
    layout->addWidget ( d->normalizeFilterWidget );
    layout->addWidget ( d->medianFilterWidget );
    layout->addWidget ( d->invertFilterWidget );
    layout->addWidget ( d->shrinkFilterWidget );
    layout->addWidget ( d->intensityFilterWidget );
    layout->addWidget ( d->thresholdFilterWidget );
    layout->addWidget ( d->componentSizeThresholdFilterWidget );
    layout->addWidget ( runButton );
    layout->addStretch ( 1 );

    this->onFiltersActivated ( 0 );
    widget->setLayout ( layout );

    this->addWidget ( widget );

    connect ( runButton, SIGNAL ( clicked() ), this, SLOT ( run() ) );

    if (this->selectorToolBox()) // empty in pipelines
    {
        connect(this->selectorToolBox(), SIGNAL(inputChanged()), this, SLOT(update()));
        update();
    }
}

itkFiltersToolBox::~itkFiltersToolBox()
{
    d->process.releasePointer();
    
    delete d;
    d = NULL;
}

bool itkFiltersToolBox::registered()
{
    medToolBoxFactory* factory = medToolBoxFactory::instance();
    return factory->registerToolBox<itkFiltersToolBox> ();
}


medAbstractData* itkFiltersToolBox::processOutput()
{
    if ( !d->process )
        return NULL;

    return d->process->output();
}

void itkFiltersToolBox::clear()
{
    d->intensityMinimumValue->setMinimum (itkFiltersWindowingProcess::defaultMinimumIntensityValue);
    d->intensityMinimumValue->setMaximum (itkFiltersWindowingProcess::defaultMaximumIntensityValue);
    d->intensityMinimumValue->setValue (itkFiltersWindowingProcess::defaultMinimumIntensityValue);

    d->intensityMaximumValue->setMinimum (itkFiltersWindowingProcess::defaultMinimumIntensityValue);
    d->intensityMaximumValue->setMaximum (itkFiltersWindowingProcess::defaultMaximumIntensityValue);
    d->intensityMaximumValue->setValue (itkFiltersWindowingProcess::defaultMaximumIntensityValue);

    d->intensityOutputMinimumValue->setMinimum (itkFiltersWindowingProcess::defaultMinimumIntensityValue);
    d->intensityOutputMinimumValue->setMaximum (itkFiltersWindowingProcess::defaultMaximumIntensityValue);
    d->intensityOutputMinimumValue->setValue (itkFiltersWindowingProcess::defaultMinimumIntensityValue);

    d->intensityOutputMaximumValue->setMinimum (itkFiltersWindowingProcess::defaultMinimumIntensityValue);
    d->intensityOutputMaximumValue->setMaximum (itkFiltersWindowingProcess::defaultMaximumIntensityValue);
    d->intensityOutputMaximumValue->setValue (itkFiltersWindowingProcess::defaultMaximumIntensityValue);
}

void itkFiltersToolBox::update()
{
    medAbstractData *data = this->selectorToolBox()->data();
    if (!data)
    {
        clear();
    }
    else
    {
        if (DISPATCH_ON_3D_PIXEL_TYPE(&itkFiltersToolBox::setupSpinBoxValues, this, data) != DTK_SUCCEED)
        {
            qWarning() << "itkFiltersToolBox Error: pixel type not yet implemented ("
                       << data->identifier()
                       << ")";
            clear();
            return;
        }

        // Get minimum and maximum of the data
        statsROI statsProcess;
        statsProcess.setInput(data, 0); //data
        statsProcess.setParameter(statsROI::MINMAX);

        double m_MinValueImage = itkFiltersWindowingProcess::defaultMinimumIntensityValue;
        double m_MaxValueImage = itkFiltersWindowingProcess::defaultMaximumIntensityValue;

        if(statsProcess.update() == DTK_SUCCEED)
        {
            m_MinValueImage = statsProcess.output().at(0);
            m_MaxValueImage = statsProcess.output().at(1);
        }
        d->intensityMinimumValue->setValue(m_MinValueImage);
        d->intensityMaximumValue->setValue(m_MaxValueImage);
        d->intensityOutputMinimumValue->setValue(m_MinValueImage);
        d->intensityOutputMaximumValue->setValue (m_MaxValueImage);
    }
}

template <typename ImageType>
int itkFiltersToolBox::setupSpinBoxValues(medAbstractData*)
{
    typedef typename ImageType::PixelType PixelType;

    d->addFilterValue->setMaximum(std::numeric_limits<PixelType>::max());
    d->subtractFilterValue->setMaximum(std::numeric_limits<PixelType>::max());
    d->intensityMinimumValue->setMinimum(std::numeric_limits<PixelType>::min());
    d->intensityMinimumValue->setMaximum(std::numeric_limits<PixelType>::max());
    d->intensityMaximumValue->setMinimum(std::numeric_limits<PixelType>::min());
    d->intensityMaximumValue->setMaximum(std::numeric_limits<PixelType>::max());
    d->intensityOutputMinimumValue->setMinimum(std::numeric_limits<PixelType>::min());
    d->intensityOutputMinimumValue->setMaximum(std::numeric_limits<PixelType>::max());
    d->intensityOutputMaximumValue->setMinimum(std::numeric_limits<PixelType>::min());
    d->intensityOutputMaximumValue->setMaximum(std::numeric_limits<PixelType>::max());
    d->thresholdLowerValue->setRange(itk::NumericTraits<PixelType>::NonpositiveMin(),
                                      itk::NumericTraits<PixelType>::max());
    d->thresholdLowerValue->setValue(itk::NumericTraits<PixelType>::NonpositiveMin());
    d->thresholdUpperValue->setRange(itk::NumericTraits<PixelType>::NonpositiveMin(),
                                      itk::NumericTraits<PixelType>::max());
    d->thresholdUpperValue->setValue(itk::NumericTraits<PixelType>::max());
    d->thresholdFilterValue->setRange(itk::NumericTraits<PixelType>::NonpositiveMin(),
                                       itk::NumericTraits<PixelType>::max());
    if ( d->binaryThreshold->isChecked() )
    {
        d->thresholdFilterValue->setValue ( itkFiltersBinaryThresholdingProcess::defaultInsideValue );

    }
    else
    {
        d->thresholdFilterValue->setValue (  itkFiltersThresholdingProcess::defaultThreshold  );

    }

    return DTK_SUCCEED;
}

void itkFiltersToolBox::setupItkAddProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkAddProcess" );
    
    if (!d->process)
        return;

    d->process->setInput ( this->selectorToolBox()->data() );
    d->process->setParameter ( d->addFilterValue->value(), 0 );
}

void itkFiltersToolBox::setupItkSubtractProcess()
{
    qDebug() << "Subtract parameter (run) : " << d->subtractFilterValue->value();
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkSubtractProcess" );
    
    if (!d->process)
        return;

    d->process->setInput ( this->selectorToolBox()->data() );
    d->process->setParameter ( d->subtractFilterValue->value(), 0 );
}

void itkFiltersToolBox::setupItkMultiplyProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkMultiplyProcess" );
    
    if (!d->process)
        return;
    
    d->process->setInput ( this->selectorToolBox()->data() );
    d->process->setParameter ( d->multiplyFilterValue->value(), 0 );
}

void itkFiltersToolBox::setupItkDivideProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkDivideProcess" );
    
    if (!d->process)
        return;
    
    d->process->setInput ( this->selectorToolBox()->data() );
    d->process->setParameter ( d->divideFilterValue->value(), 0 );
}

void itkFiltersToolBox::setupItkGaussianProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkGaussianProcess" );
    
    if (!d->process)
        return;
    
    d->process->setInput ( this->selectorToolBox()->data() );
    d->process->setParameter ( d->gaussianFilterValue->value(), 0);
}

void itkFiltersToolBox::setupItkMedianProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkMedianProcess" );
    d->process->setParameter ( d->medianSizeFilterValue->value());
    
    if (!d->process)
        return;
    
    d->process->setInput ( this->selectorToolBox()->data() );
}

void itkFiltersToolBox::setupItkNormalizeProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkNormalizeProcess" );
    
    if (!d->process)
        return;
    
    d->process->setInput ( this->selectorToolBox()->data() );
}

void itkFiltersToolBox::setupItkInvertProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkInvertProcess" );
    
    if (!d->process)
        return;
    
    d->process->setInput ( this->selectorToolBox()->data() );
}

void itkFiltersToolBox::setupItkShrinkProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkShrinkProcess" );
    
    if (!d->process)
        return;
    
    d->process->setInput ( this->selectorToolBox()->data() );
    d->process->setParameter ( d->shrink0Value->value(), 0 );
    d->process->setParameter ( d->shrink1Value->value(), 1 );
    d->process->setParameter ( d->shrink2Value->value(), 2 );
}

void itkFiltersToolBox::setupItkWindowingProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkWindowingProcess" );
    
    if (!d->process)
        return;
    
    d->process->setInput ( this->selectorToolBox()->data() );
    d->process->setParameter ( d->intensityMinimumValue->value(), 0);
    d->process->setParameter ( d->intensityMaximumValue->value(), 1);
    d->process->setParameter ( d->intensityOutputMinimumValue->value(), 2 );
    d->process->setParameter ( d->intensityOutputMaximumValue->value(), 3 );
}

void itkFiltersToolBox::setupItkThresholdingProcess()
{
    if ( d->binaryThreshold->isChecked() )
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer( "itkBinaryThresholdingProcess" );
        if (!d->process)
            return;
        d->process->setInput ( this->selectorToolBox()->data() );

        d->process->setParameter ( (int)d->thresholdFilterValue->value(), 0);
        d->process->setParameter ( (int)d->thresholdFilterValue2->value(), 1);

        d->process->setParameter ( d->thresholdLowerValue->value(), 0);
        d->process->setParameter ( d->thresholdUpperValue->value(), 1);

    }
    else
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkThresholdingProcess" );
        if (!d->process)
            return;

        d->process->setInput ( this->selectorToolBox()->data() );

        d->process->setParameter(d->valueButtonGroup->checkedId());

        d->process->setParameter ( d->thresholdFilterValue->value(), 0);
        d->process->setParameter ( d->thresholdLowerValue->value(), 1);
        d->process->setParameter ( d->thresholdUpperValue->value(), 2);
        d->process->setParameter ( (double)d->thresholdFilterValue2->value(), 3);
    }
}

void itkFiltersToolBox::setupItkComponentSizeThresholdProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkComponentSizeThresholdProcess" );

    if (!d->process)
        return;

    d->process->setInput ( this->selectorToolBox()->data() );
    d->process->setParameter ( d->componentSizeThresholdFilterValue->value(), 0 );
}

void itkFiltersToolBox::run ( void )
{
    if ( !this->selectorToolBox() )
        return;

    if ( !this->selectorToolBox()->data() )
        return;

//    if (d->process) {
//        d->process->deleteLater();
//    }
    
    //Set parameters :
    //   channel 0 : filter type
    //   channel 1,2,..,N : filter parameters
    switch ( d->filters->currentIndex() )
    {
    case 0: // add filter
        this->setupItkAddProcess();
        break;
    case 1: // subtract filter
        this->setupItkSubtractProcess();
        break;
    case 2: // multiply filter
        this->setupItkMultiplyProcess();
        break;
    case 3: // divide filter
        this->setupItkDivideProcess();
        break;
    case 4: // gaussian filter
        this->setupItkGaussianProcess();
        break;
    case 5: // normalize filter
        this->setupItkNormalizeProcess();
        break;
    case 6: // median filter
        this->setupItkMedianProcess();
        break;
    case 7: // invert intensity filter
        this->setupItkInvertProcess();
        break;
    case 8: // shrink filter
        this->setupItkShrinkProcess();
        break;
    case 9: // intensity windowing filter
        this->setupItkWindowingProcess();
        break;
    case 10: // intensity windowing filter
        this->setupItkThresholdingProcess();
        break;
    case 11: // size thresholding filter
        this->setupItkComponentSizeThresholdProcess();
        break;    
    }

    if (! d->process)
        return;

    this->setToolBoxOnWaitStatus();

    medRunnableProcess *runProcess = new medRunnableProcess;
    runProcess->setProcess ( d->process );
    this->addConnectionsAndStartJob(runProcess);
}

void itkFiltersToolBox::onFiltersActivated ( int index )
{
    d->addFilterWidget->hide();
    d->subtractFilterWidget->hide();
    d->multiplyFilterWidget->hide();
    d->divideFilterWidget->hide();
    d->gaussianFilterWidget->hide();
    d->normalizeFilterWidget->hide();
    d->medianFilterWidget->hide();
    d->invertFilterWidget->hide();
    d->shrinkFilterWidget->hide();
    d->intensityFilterWidget->hide();
    d->thresholdFilterWidget->hide();
    d->componentSizeThresholdFilterWidget->hide();

    switch ( index )
    {
    case 0:
        d->addFilterWidget->show();
        break;
    case 1:
        d->subtractFilterWidget->show();
        break;
    case 2:
        d->multiplyFilterWidget->show();
        break;
    case 3:
        d->divideFilterWidget->show();
        break;
    case 4:
        d->gaussianFilterWidget->show();
        break;
    case 5:
        d->normalizeFilterWidget->show();
        break;
    case 6:
        d->medianFilterWidget->show();
        break;
    case 7:
        d->invertFilterWidget->show();
        break;
    case 8:
        d->shrinkFilterWidget->show();
        break;
    case 9:
        d->intensityFilterWidget->show();
        break;
    case 10:
        d->thresholdFilterWidget->show();
        break;
    case 11:
        d->componentSizeThresholdFilterWidget->show();
        break;
    default:
        d->addFilterWidget->show();
    }
}

void itkFiltersToolBox::updateThresholdToolboxBehaviour(int id)
{
    if ( d->binaryThreshold->isChecked() )
    {
        d->thresholdValueWidget->setVisible(true);
        d->thresholdLowerWidget->setVisible(true);
        d->thresholdUpperWidget->setVisible(true);
    }
    else
    {
        if ( id==itkFiltersThresholdingProcess::upperButtonId ||
             id==itkFiltersThresholdingProcess::lowerButtonId )
        {
            d->thresholdValueWidget->setVisible(true);
            d->thresholdLowerWidget->setVisible(false);
            d->thresholdUpperWidget->setVisible(false);
        }
        else if ( id==itkFiltersThresholdingProcess::outsideButtonId )
        {
            d->thresholdValueWidget->setVisible(false);
            d->thresholdLowerWidget->setVisible(true);
            d->thresholdUpperWidget->setVisible(true);
        }
    }
}

void itkFiltersToolBox::checkBinaryThreshold(bool checked)
{
    d->buttonGroupWidget->setVisible(!checked);

    updateThresholdToolboxBehaviour(d->valueButtonGroup->checkedId());

    d->thresholdFilterValue->getSlider()->setHidden(checked);
    if ( checked )
    {
        d->thresholdFilterValue->getLabel()->setText( tr ( "Inside Value  :" ) );
        d->thresholdFilterValue->setValue ( itkFiltersBinaryThresholdingProcess::defaultInsideValue );
        d->thresholdFilterValue->setDecimals(0);
    }
    else
    {
        d->thresholdFilterValue->getLabel()->setText( tr ( "Threshold Value  :" ) );
        d->thresholdFilterValue->setValue ( itkFiltersThresholdingProcess::defaultThreshold );
    }
}

dtkPlugin* itkFiltersToolBox::plugin()
{
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "ITK Filters" );
    return plugin;
}


medToolBox *createitkFiltersToolBox ( QWidget *parent )
{
    return new itkFiltersToolBox ( parent );
}
