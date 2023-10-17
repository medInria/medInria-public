/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersToolBox.h>

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

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
#include <itkMinimumMaximumImageCalculator.h>

#include <medClutEditorToolBox.h>
#include <medComboBox.h>
#include <medDoubleParameterL.h>
#include <medIntParameterL.h>
#include <medPluginManager.h>
#include <medRunnableProcess.h>
#include <medSelectorToolBox.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medUtilitiesITK.h>
#include <medViewContainer.h>

#include <statsROI.h>

class itkFiltersToolBoxPrivate
{
public:
    QWidget *addFilterWidget;
    QWidget *subtractFilterWidget;
    QWidget *multiplyFilterWidget;
    QWidget *divideFilterWidget;
    QWidget *gaussianFilterWidget;
    QWidget *normalizeFilterWidget;
    QWidget *medianFilterWidget;
    QWidget *invertFilterWidget;
    QWidget *shrinkFilterWidget;
    QWidget *intensityFilterWidget;
    QWidget *thresholdFilterWidget;
    QWidget *componentSizeThresholdFilterWidget;

    medDoubleParameterL *addFilterValue;
    medDoubleParameterL *subtractFilterValue;
    medDoubleParameterL *medianSizeFilterValue;
    medDoubleParameterL *multiplyFilterValue;
    medDoubleParameterL *divideFilterValue;
    medDoubleParameterL *gaussianFilterValue;
    medDoubleParameterL *thresholdFilterValue;
    medDoubleParameterL *thresholdLowerValue;
    medDoubleParameterL *thresholdUpperValue;

    QPointer<medClutEditorToolBox> clutEditor;
    QLabel *infoThreshold;
    QWidget *thresholdValueWidget;
    QWidget *thresholdUpperWidget;
    QWidget *thresholdLowerWidget;
    QWidget *buttonGroupWidget;
    QSpinBox *thresholdFilterValue2;
    QButtonGroup *valueButtonGroup;
    QRadioButton *binaryThreshold;
    QCheckBox *histogram;
    QLabel *thresholdValueLabel;
    QSpinBox *shrink0Value;
    QSpinBox *shrink1Value;
    QSpinBox *shrink2Value;
    medIntParameterL *componentSizeThresholdFilterValue;
    QCheckBox *binaryComponentThreshold;

    QDoubleSpinBox *intensityMinimumValue;
    QDoubleSpinBox *intensityMaximumValue;
    QDoubleSpinBox *intensityOutputMinimumValue;
    QDoubleSpinBox *intensityOutputMaximumValue;

    double minValueImage;
    double maxValueImage;
    QColor minColor, maxColor, thresholdColor;
    medComboBox *filters;
    dtkSmartPointer <itkFiltersProcessBase> process;
};

itkFiltersToolBox::itkFiltersToolBox(QWidget *parent)
    : medAbstractSelectableToolBox(parent), d(new itkFiltersToolBoxPrivate)
{
    // Filters selection combobox
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
    
    d->filters->addItems(filtersList);

    QObject::connect(d->filters, SIGNAL(currentIndexChanged(int)), this, SLOT(onFiltersActivated(int)), Qt::UniqueConnection);

    //Add filter widgets
    d->addFilterWidget = new QWidget(this);
    d->addFilterValue = new medDoubleParameterL( tr("Constant value"), this);
    d->addFilterValue->setRange ( -1000000000.0, 1000000000.0 );
    d->addFilterValue->setValue ( itkFiltersAddProcess::defaultAddValue );
    d->addFilterValue->setObjectName("addFilterValue");
    QHBoxLayout *addFilterLayout = new QHBoxLayout;
    addFilterLayout->addWidget(d->addFilterValue->getLabel());
    addFilterLayout->addWidget(d->addFilterValue->getSpinBox());
    d->addFilterWidget->setLayout(addFilterLayout);

    //Subtract filter widgets
    d->subtractFilterWidget = new QWidget(this);
    d->subtractFilterValue = new medDoubleParameterL( tr("Constant value"), this);
    d->subtractFilterValue->setRange ( -1000000000.0, 1000000000.0 );
    d->subtractFilterValue->setValue ( itkFiltersSubtractProcess::defaultSubtractValue );
    d->subtractFilterValue->setObjectName("subtractFilterValue");
    QLabel* subtractExplanation = new QLabel("Subtract an image by a constant in the limits of its image type.\nFor instance, "
                                             "some pixel type are not compatible with negative values: the lower value would be 0.");
    subtractExplanation->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    subtractExplanation->setWordWrap(true);
    subtractExplanation->setStyleSheet("font: italic");
    QVBoxLayout *subtractGlobalFilterLayout = new QVBoxLayout;
    subtractGlobalFilterLayout->addWidget(subtractExplanation);
    QHBoxLayout *subtractFilterLayout = new QHBoxLayout;
    subtractFilterLayout->addWidget(d->subtractFilterValue->getLabel());
    subtractFilterLayout->addWidget(d->subtractFilterValue->getSpinBox());
    subtractGlobalFilterLayout->addLayout(subtractFilterLayout);
    d->subtractFilterWidget->setLayout(subtractGlobalFilterLayout);

    //Multiply filter widgets
    d->multiplyFilterWidget = new QWidget(this);
    d->multiplyFilterValue = new medDoubleParameterL( tr("Constant value"), this);
    d->multiplyFilterValue->setRange ( -1000000000.0, 1000000000.0 );
    d->multiplyFilterValue->setValue ( itkFiltersMultiplyProcess::defaultMultiplyFactor );
    d->multiplyFilterValue->setObjectName("multiplyFilterValue");
    QHBoxLayout *multiplyFilterLayout = new QHBoxLayout;
    multiplyFilterLayout->addWidget(d->multiplyFilterValue->getLabel());
    multiplyFilterLayout->addWidget(d->multiplyFilterValue->getSpinBox());
    d->multiplyFilterWidget->setLayout(multiplyFilterLayout);

    //Divide filter widgets
    d->divideFilterWidget = new QWidget(this);
    d->divideFilterValue = new medDoubleParameterL( tr("Constant value"), this);
    d->divideFilterValue->setRange ( 1.0, 1000000000.0 );
    d->divideFilterValue->setValue ( itkFiltersDivideProcess::defaultDivideFactor );
    d->divideFilterValue->setObjectName("divideFilterValue");
    QHBoxLayout * divideFilterLayout = new QHBoxLayout;
    divideFilterLayout->addWidget(d->divideFilterValue->getLabel());
    divideFilterLayout->addWidget(d->divideFilterValue->getSpinBox());
    d->divideFilterWidget->setLayout(divideFilterLayout);

    //Gaussian filter widgets
    d->gaussianFilterWidget = new QWidget(this);
    d->gaussianFilterValue = new medDoubleParameterL( tr("Sigma value"), this);
    d->gaussianFilterValue->setRange ( 0.0, 10.0 );
    d->gaussianFilterValue->setValue ( itkFiltersGaussianProcess::defaultSigma );
    d->gaussianFilterValue->setObjectName("gaussianFilterValue");
    QHBoxLayout *gaussianFilterLayout = new QHBoxLayout;
    gaussianFilterLayout->addWidget(d->gaussianFilterValue->getLabel());
    gaussianFilterLayout->addWidget(d->gaussianFilterValue->getSpinBox());
    d->gaussianFilterWidget->setLayout(gaussianFilterLayout);

    //Normalize filter widgets
    d->normalizeFilterWidget = new QWidget(this);
    QLabel* normExplanation = new QLabel("Normalize an image by setting its mean to zero and variance to one.");
    normExplanation->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    normExplanation->setWordWrap(true);
    normExplanation->setStyleSheet("font: italic");
    QHBoxLayout *normalizeFilterLayout = new QHBoxLayout;
    normalizeFilterLayout->addWidget(normExplanation);
    d->normalizeFilterWidget->setLayout(normalizeFilterLayout);

    //Median filter widgets
    d->medianFilterWidget = new QWidget(this);
    d->medianSizeFilterValue = new medDoubleParameterL( tr("Neighborhood size"), this);
    d->medianSizeFilterValue->setRange ( 0.0, 1000000000.0 );
    d->medianSizeFilterValue->setValue ( itkFiltersMedianProcess::defaultMedianSize );
    d->medianSizeFilterValue->setObjectName("medianSizeFilterValue");
    QHBoxLayout *medianSizeFilterLayout = new QHBoxLayout;
    medianSizeFilterLayout->addWidget(d->medianSizeFilterValue->getLabel());
    medianSizeFilterLayout->addWidget(d->medianSizeFilterValue->getSpinBox());
    d->medianFilterWidget->setLayout(medianSizeFilterLayout);

    //Invert filter widgets
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

    QLabel *shrinkFilterLabel = new QLabel ( tr ( "Shrink factors (X,Y,Z)" ) );
    QFormLayout *shrinkFilterLayout = new QFormLayout;

    QVBoxLayout *shrinkFilterValueLayout = new QVBoxLayout;
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

    QLabel *intensityMinimumLabel = new QLabel ( tr ( "Minimum" ) );
    QHBoxLayout *intensityMinimumLayout = new QHBoxLayout;
    intensityMinimumLayout->addWidget ( intensityMinimumLabel );
    intensityMinimumLayout->addWidget ( d->intensityMinimumValue );

    QLabel *intensityMaximumLabel = new QLabel ( tr ( "Maximum" ) );
    QHBoxLayout *intensityMaximumLayout = new QHBoxLayout;
    intensityMaximumLayout->addWidget ( intensityMaximumLabel );
    intensityMaximumLayout->addWidget ( d->intensityMaximumValue );

    QLabel *intensityOutputMinimumLabel = new QLabel ( tr ( "Output minimum" ) );
    QHBoxLayout *intensityOutputMinimumLayout = new QHBoxLayout;
    intensityOutputMinimumLayout->addWidget ( intensityOutputMinimumLabel );
    intensityOutputMinimumLayout->addWidget ( d->intensityOutputMinimumValue );

    QLabel *intensityOutputMaximumLabel = new QLabel ( tr ( "Output maximum" ) );
    QHBoxLayout *intensityOutputMaximumLayout = new QHBoxLayout;
    intensityOutputMaximumLayout->addWidget ( intensityOutputMaximumLabel );
    intensityOutputMaximumLayout->addWidget ( d->intensityOutputMaximumValue );

    QVBoxLayout *intensityFilterLayout = new QVBoxLayout;
    intensityFilterLayout->addLayout ( intensityMinimumLayout );
    intensityFilterLayout->addLayout ( intensityMaximumLayout );
    intensityFilterLayout->addLayout ( intensityOutputMinimumLayout );
    intensityFilterLayout->addLayout ( intensityOutputMaximumLayout );
    intensityFilterLayout->addStretch ( 0 );

    d->intensityFilterWidget->setLayout ( intensityFilterLayout );

    //Threshold filter widgets
    d->thresholdFilterWidget = new QWidget(this);

    d->thresholdFilterValue = new medDoubleParameterL( tr("Value"), this);
    d->thresholdFilterValue->setToolTip(tr("Only keep values according to the threshold type"));
    d->thresholdFilterValue->setRange(-10000, 10000);
    d->thresholdFilterValue->setValue(itkFiltersThresholdingProcess::defaultThreshold);
    d->thresholdFilterValue->setObjectName("thresholdFilterValue");

    d->thresholdLowerValue = new medDoubleParameterL( tr("Lower Value"), this);
    d->thresholdLowerValue->setToolTip(tr("Choose the boundaries of your threshold"));
    d->thresholdLowerValue->setRange(-10000, 10000);
    d->thresholdLowerValue->setValue ( itkFiltersThresholdingProcess::defaultLower );
    d->thresholdLowerValue->setObjectName("thresholdLowerValue");
    d->thresholdLowerValue->setDecimals(10);

    d->thresholdUpperValue = new medDoubleParameterL( tr("Upper Value"), this);
    d->thresholdUpperValue->setToolTip(tr("Choose the boundaries of your threshold"));
    d->thresholdUpperValue->setRange(-10000, 10000);
    d->thresholdUpperValue->setValue ( itkFiltersThresholdingProcess::defaultUpper );
    d->thresholdUpperValue->setObjectName("thresholdUpperValue");
    d->thresholdUpperValue->setDecimals(10);

    QSignalMapper *signalMapper = new QSignalMapper(this);
    signalMapper->setMapping(d->thresholdLowerValue, 0);
    signalMapper->setMapping(d->thresholdUpperValue, 1);
    signalMapper->setMapping(d->thresholdFilterValue, 2);

    connect(d->thresholdLowerValue,  SIGNAL(valueChanged(double)), signalMapper, SLOT (map()), Qt::UniqueConnection);
    connect(d->thresholdUpperValue,  SIGNAL(valueChanged(double)), signalMapper, SLOT (map()), Qt::UniqueConnection);
    connect(d->thresholdFilterValue, SIGNAL(valueChanged(double)), signalMapper, SLOT (map()), Qt::UniqueConnection);
    connect(signalMapper, SIGNAL(mapped(int)),this, SLOT(updateClutEditorValue(int)), Qt::UniqueConnection);

    d->thresholdFilterValue2 = new QSpinBox;
    d->thresholdFilterValue2->setRange ( -10000, 10000 );
    d->thresholdFilterValue2->setValue ( itkFiltersThresholdingProcess::defaultOutsideValue );
    d->thresholdFilterValue2->setObjectName("outsideThresholdValue");

    d->binaryThreshold = new QRadioButton(tr("Binarize Image"), this);
    d->binaryThreshold->setObjectName("binaryThresholdButton");
    d->infoThreshold = new QLabel("Values equal to the threshold value are not set to Outside Value");
    d->infoThreshold->setWordWrap(true);
    d->infoThreshold->setStyleSheet("font: italic");
    connect(d->binaryThreshold, SIGNAL(toggled(bool)), this, SLOT(checkBinaryThreshold(bool)), Qt::UniqueConnection);

    d->histogram = new QCheckBox(tr("Open Histogram"), this);
    d->histogram->setObjectName("histogram");
    d->histogram->setDisabled(true);
    connect(d->histogram, SIGNAL(stateChanged(int)), this, SLOT(showHistogram(int)), Qt::UniqueConnection);
    clear();

    QRadioButton *greaterButton = new QRadioButton(tr(" greater "), this);
    greaterButton->setObjectName("greaterButton");
    QRadioButton *lowerButton = new QRadioButton(tr(" lower "), this);
    lowerButton->setObjectName("lowerButton");
    QRadioButton *outsideButton = new QRadioButton(tr(" outside "), this);
    outsideButton->setObjectName("outsideButton");

    d->valueButtonGroup = new QButtonGroup;
    d->valueButtonGroup->addButton(greaterButton, itkFiltersThresholdingProcess::upperButtonId);
    d->valueButtonGroup->addButton(lowerButton,   itkFiltersThresholdingProcess::lowerButtonId);
    d->valueButtonGroup->addButton(outsideButton, itkFiltersThresholdingProcess::outsideButtonId);

    QLabel *thresholdFilterLabel = new QLabel(tr("Type"));

    d->buttonGroupWidget = new QWidget(this);
    QHBoxLayout *greaterLowerOutsideLayout = new QHBoxLayout;
    greaterLowerOutsideLayout->addWidget( thresholdFilterLabel );
    greaterLowerOutsideLayout->addWidget(greaterButton);
    greaterLowerOutsideLayout->addWidget(lowerButton);
    greaterLowerOutsideLayout->addWidget(outsideButton);
    greaterLowerOutsideLayout->addStretch(-1);
    d->buttonGroupWidget->setLayout(greaterLowerOutsideLayout);

    d->thresholdLowerWidget = new QWidget(this);
    QHBoxLayout *thresholdLowerLayout = new QHBoxLayout;
    d->thresholdLowerValue->getSlider()->setOrientation(Qt::Horizontal);
    thresholdLowerLayout->addWidget(d->thresholdLowerValue->getLabel());
    thresholdLowerLayout->addWidget(d->thresholdLowerValue->getSlider());
    thresholdLowerLayout->addWidget(d->thresholdLowerValue->getSpinBox());
    d->thresholdLowerWidget->setLayout(thresholdLowerLayout);

    d->thresholdUpperWidget = new QWidget(this);
    QHBoxLayout *thresholdUpperLayout = new QHBoxLayout;
    d->thresholdUpperValue->getSlider()->setOrientation(Qt::Horizontal);
    thresholdUpperLayout->addWidget(d->thresholdUpperValue->getLabel());
    thresholdUpperLayout->addWidget(d->thresholdUpperValue->getSlider());
    thresholdUpperLayout->addWidget(d->thresholdUpperValue->getSpinBox());
    d->thresholdUpperWidget->setLayout(thresholdUpperLayout);

    d->thresholdValueWidget = new QWidget(this);
    QHBoxLayout *thresholdValueLayout = new QHBoxLayout;
    d->thresholdFilterValue->getSlider()->setOrientation(Qt::Horizontal);
    thresholdValueLayout->addWidget(d->thresholdFilterValue->getLabel());
    thresholdValueLayout->addWidget(d->thresholdFilterValue->getSlider());
    thresholdValueLayout->addWidget(d->thresholdFilterValue->getSpinBox());
    d->thresholdValueWidget->setLayout(thresholdValueLayout);

    QWidget *outsideValueWidget = new QWidget(this);
    QLabel *thresholdFilterLabel2 = new QLabel(tr("Outside Value "));
    QHBoxLayout *outsideValueLayout = new QHBoxLayout;
    outsideValueLayout->addWidget ( thresholdFilterLabel2 );
    outsideValueLayout->addWidget ( d->thresholdFilterValue2 );
    outsideValueLayout->setSizeConstraint(QLayout::SetMaximumSize);
    outsideValueWidget->setLayout(outsideValueLayout);

    QVBoxLayout *thresholdFilterLayout1 = new QVBoxLayout;
    thresholdFilterLayout1->addWidget( d->binaryThreshold );
    thresholdFilterLayout1->addWidget( d->infoThreshold );
    thresholdFilterLayout1->addWidget( d->buttonGroupWidget );
    thresholdFilterLayout1->addWidget( d->thresholdLowerWidget );
    thresholdFilterLayout1->addWidget( d->thresholdUpperWidget );
    thresholdFilterLayout1->addWidget( d->thresholdValueWidget );
    thresholdFilterLayout1->addWidget( outsideValueWidget );
    thresholdFilterLayout1->addWidget( d->histogram );
    thresholdFilterLayout1->addStretch ( 1 );

    QVBoxLayout *thresholdFilterLayout = new QVBoxLayout;
    thresholdFilterLayout->addLayout(thresholdFilterLayout1);
    thresholdFilterLayout->addStretch(-1);

    d->thresholdFilterWidget->setLayout ( thresholdFilterLayout );

    connect(d->valueButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(updateThresholdToolboxBehaviour(int)), Qt::UniqueConnection);
    greaterButton->click();
    d->clutEditor = nullptr;

    //Size threshold Widget
    d->componentSizeThresholdFilterWidget = new QWidget(this);
    d->componentSizeThresholdFilterValue = new medIntParameterL( tr("Minimum size in pixel of an object"), this);
    d->componentSizeThresholdFilterValue->setRange ( 0, 100000 );
    d->componentSizeThresholdFilterValue->setValue ( itkFiltersComponentSizeThresholdProcess::defaultMinimumSize );
    d->componentSizeThresholdFilterValue->setObjectName("componentSizeThresholdFilterValue");

    d->binaryComponentThreshold = new QCheckBox(tr("Binarize"));
    d->binaryComponentThreshold->setChecked(itkFiltersComponentSizeThresholdProcess::defaultBinarize);
    d->binaryComponentThreshold->setObjectName("componentSizeThresholdCheckBox");

    QHBoxLayout *componentSizeThresholdValueLayout = new QHBoxLayout;
    componentSizeThresholdValueLayout->addWidget(d->componentSizeThresholdFilterValue->getLabel());
    componentSizeThresholdValueLayout->addWidget(d->componentSizeThresholdFilterValue->getSpinBox());

    QVBoxLayout *componentSizeThresholdFilterLayout = new QVBoxLayout;
    componentSizeThresholdFilterLayout->addWidget(d->binaryComponentThreshold);
    componentSizeThresholdFilterLayout->addLayout (componentSizeThresholdValueLayout);
    d->componentSizeThresholdFilterWidget->setLayout(componentSizeThresholdFilterLayout);

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

    this->onFiltersActivated(0);

    widget->setLayout(layout);
    this->addWidget(widget);

    connect(runButton, SIGNAL(clicked()), this, SLOT(run()), Qt::UniqueConnection);

    if (this->selectorToolBox()) // empty in pipelines
    {
        update();
        connect(this->selectorToolBox(), SIGNAL(inputChanged()), this, SLOT(update()), Qt::UniqueConnection);
    }
}

itkFiltersToolBox::~itkFiltersToolBox()
{
    d->process.releasePointer();
    
    delete d;
    d = nullptr;
}

bool itkFiltersToolBox::registered()
{
    medToolBoxFactory *factory = medToolBoxFactory::instance();
    return factory->registerToolBox<itkFiltersToolBox> ();
}

medAbstractData* itkFiltersToolBox::processOutput()
{
    if (!d->process)
    {
        return nullptr;
    }
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

    d->histogram->setDisabled(true);
    d->minColor = Qt::white;
    d->maxColor = Qt::white;
    d->thresholdColor = Qt::black;
    d->minValueImage = d->maxValueImage = 0.;
    d->process = nullptr;
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
        if (DISPATCH_ON_3D_PIXEL_TYPE(&itkFiltersToolBox::setupSpinBoxValues, this, data) != medAbstractProcessLegacy::SUCCESS)
        {
            qWarning() << "itkFiltersToolBox error: pixel type not yet implemented ("
                       << data->identifier()
                       << ")";
            clear();
            return;
        }

        // Get minimum and maximum of the data
        statsROI statsProcess;
        statsProcess.setInput(data, 0); //data
        statsProcess.setParameter(statsROI::MINMAX);

        d->minValueImage = itkFiltersWindowingProcess::defaultMinimumIntensityValue;
        d->maxValueImage = itkFiltersWindowingProcess::defaultMaximumIntensityValue;

        if(statsProcess.update() == medAbstractProcessLegacy::SUCCESS)
        {
            d->minValueImage = statsProcess.output().at(0);
            d->maxValueImage = statsProcess.output().at(1);

            // Threshold range update
            d->thresholdFilterValue->setRange(d->minValueImage, d->maxValueImage);
            d->thresholdLowerValue->setRange( d->minValueImage, d->maxValueImage);
            d->thresholdUpperValue->setRange( d->minValueImage, d->maxValueImage);
        }
        d->intensityMinimumValue->setValue(d->minValueImage);
        d->intensityMaximumValue->setValue(d->maxValueImage);
        d->intensityOutputMinimumValue->setValue(d->minValueImage);
        d->intensityOutputMaximumValue->setValue (d->maxValueImage);

        d->thresholdLowerValue->setValue(d->minValueImage);
        d->thresholdUpperValue->setValue(d->maxValueImage);
        if (d->valueButtonGroup->checkedId()==itkFiltersThresholdingProcess::lowerButtonId ||
                d->valueButtonGroup->checkedId()==itkFiltersThresholdingProcess::upperButtonId )
        {
            d->thresholdFilterValue->setValue((d->minValueImage+d->maxValueImage)/2);
        }

        // Histogram
        d->histogram->setEnabled(true);
        updateHistogramView();
    }
}

template <typename ImageType>
int itkFiltersToolBox::setupSpinBoxValues(medAbstractData* data)
{
    Q_UNUSED(data);
    typedef typename ImageType::PixelType PixelType;

    d->addFilterValue->setRange(     std::numeric_limits<PixelType>::min(), std::numeric_limits<PixelType>::max());
    d->subtractFilterValue->setRange(std::numeric_limits<PixelType>::min(), std::numeric_limits<PixelType>::max());
    d->multiplyFilterValue->setRange(std::numeric_limits<PixelType>::min(), std::numeric_limits<PixelType>::max());
    d->divideFilterValue->setRange(    1, std::numeric_limits<PixelType>::max());
    d->medianSizeFilterValue->setRange(0, std::numeric_limits<PixelType>::max());

    d->intensityMinimumValue->setMinimum(std::numeric_limits<PixelType>::min());
    d->intensityMinimumValue->setMaximum(std::numeric_limits<PixelType>::max());
    d->intensityMaximumValue->setMinimum(std::numeric_limits<PixelType>::min());
    d->intensityMaximumValue->setMaximum(std::numeric_limits<PixelType>::max());
    d->intensityOutputMinimumValue->setMinimum(std::numeric_limits<PixelType>::min());
    d->intensityOutputMinimumValue->setMaximum(std::numeric_limits<PixelType>::max());
    d->intensityOutputMaximumValue->setMinimum(std::numeric_limits<PixelType>::min());
    d->intensityOutputMaximumValue->setMaximum(std::numeric_limits<PixelType>::max());

    if ( d->binaryThreshold->isChecked() )
    {
        d->thresholdFilterValue->setValue ( itkFiltersBinaryThresholdingProcess::defaultInsideValue );
    }
    else
    {
        d->thresholdFilterValue->setValue (  itkFiltersThresholdingProcess::defaultThreshold  );
    }

    return medAbstractProcessLegacy::SUCCESS;
}

void itkFiltersToolBox::setupItkAddProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkAddProcess" );
    
    d->process->setInput(this->selectorToolBox()->data());
    d->process->setParameter(d->addFilterValue->value());
}

void itkFiltersToolBox::setupItkSubtractProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer("itkSubtractProcess");
    
    d->process->setInput(this->selectorToolBox()->data());
    d->process->setParameter(d->subtractFilterValue->value());
}

void itkFiltersToolBox::setupItkMultiplyProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer("itkMultiplyProcess");
        
    d->process->setInput(this->selectorToolBox()->data());
    d->process->setParameter(d->multiplyFilterValue->value());
}

void itkFiltersToolBox::setupItkDivideProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer("itkDivideProcess");
        
    d->process->setInput(this->selectorToolBox()->data());
    d->process->setParameter(d->divideFilterValue->value());
}

void itkFiltersToolBox::setupItkGaussianProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer("itkGaussianProcess");
        
    d->process->setInput(this->selectorToolBox()->data());
    d->process->setParameter(d->gaussianFilterValue->value());
}

void itkFiltersToolBox::setupItkMedianProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkMedianProcess" );
    d->process->setParameter ( d->medianSizeFilterValue->value());
        
    d->process->setInput ( this->selectorToolBox()->data() );
}

void itkFiltersToolBox::setupItkNormalizeProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkNormalizeProcess" );
        
    d->process->setInput ( this->selectorToolBox()->data() );
}

void itkFiltersToolBox::setupItkInvertProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkInvertProcess" );
        
    d->process->setInput ( this->selectorToolBox()->data() );
}

void itkFiltersToolBox::setupItkShrinkProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer("itkShrinkProcess");
        
    d->process->setInput(this->selectorToolBox()->data());
    d->process->setParameter(d->shrink0Value->value(), 0);
    d->process->setParameter(d->shrink1Value->value(), 1);
    d->process->setParameter(d->shrink2Value->value(), 2);
}

void itkFiltersToolBox::setupItkWindowingProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkWindowingProcess" );
        
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

        d->process->setInput ( this->selectorToolBox()->data() );

        d->process->setParameter ( static_cast<int>(d->thresholdFilterValue->value()),  0);
        d->process->setParameter ( static_cast<int>(d->thresholdFilterValue2->value()), 1);

        d->process->setParameter ( d->thresholdLowerValue->value(), 0);
        d->process->setParameter ( d->thresholdUpperValue->value(), 1);
    }
    else
    {
        d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkThresholdingProcess" );

        d->process->setInput ( this->selectorToolBox()->data() );

        d->process->setParameter ( d->valueButtonGroup->checkedId());
        d->process->setParameter ( d->thresholdFilterValue->value(), 0);
        d->process->setParameter ( d->thresholdLowerValue->value(), 1);
        d->process->setParameter ( d->thresholdUpperValue->value(), 2);
        d->process->setParameter ( static_cast<double>(d->thresholdFilterValue2->value()), 3);
    }
}

void itkFiltersToolBox::setupItkComponentSizeThresholdProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer("itkComponentSizeThresholdProcess");

    d->process->setInput(this->selectorToolBox()->data());
    d->process->setParameter(d->componentSizeThresholdFilterValue->value(), 1);
    d->process->setParameter(static_cast<int>(d->binaryComponentThreshold->isChecked()), 2);
}

void itkFiltersToolBox::run()
{
    if ( !this->selectorToolBox() )
    {
        return;
    }
    if ( !this->selectorToolBox()->data() )
    {
        return;
    }

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

    if (d->process)
    {
        this->setToolBoxOnWaitStatus();

        medRunnableProcess *runProcess = new medRunnableProcess;
        runProcess->setProcess ( d->process );
        this->addConnectionsAndStartJob(runProcess);
    }
}

void itkFiltersToolBox::updateClutEditorValue(int label)
{
    if ( d->clutEditor != nullptr )
    {
        QList<medClutEditorVertex*>& vertices = d->clutEditor->getScene()->table()->vertices();

        if (vertices.count() > 0)
        {
            double amount;
            switch (label)
            {
                case 0:
                    amount = d->thresholdLowerValue->value() - vertices.first()->value().x();
                    d->clutEditor->getScene()->table()->vertices().first()->shiftValue(amount);
                    break;
                case 1:
                    amount = d->thresholdUpperValue->value() - vertices.last()->value().x();
                    d->clutEditor->getScene()->table()->vertices().last()->shiftValue(amount);
                    break;
                case 2:
                    if ( d->clutEditor->getScene()->table()->vertices().size() != 3 )
                    {
                        return;
                    }
                    amount = d->thresholdFilterValue->value() - vertices.at(1)->value().x();
                    d->clutEditor->getScene()->table()->vertices().at(1)->shiftValue(amount);
                    break;
                default:
                    return;
            }
            d->clutEditor->getScene()->updateCoordinates();
            d->clutEditor->getScene()->table()->finalizeMoveSelection();
            d->clutEditor->getScene()->table()->triggerVertexSet();
        }
    }
}

void itkFiltersToolBox::updateClutEditorView()
{
    if ( d->clutEditor != nullptr )
    {
        QList<medClutEditorVertex*>& vertices = d->clutEditor->getScene()->table()->vertices();
        if ( vertices.size()==0 )
        {
            return;
        }

        if ( d->binaryThreshold->isChecked() || d->valueButtonGroup->checkedId()==itkFiltersThresholdingProcess::outsideButtonId )
        {
            QPointF minVal( d->thresholdLowerValue->value(), 1.);
            QPointF minCoord = d->clutEditor->getScene()->valueToCoordinate(minVal);

            QPointF maxVal( d->thresholdUpperValue->value(), 1.);
            QPointF maxCoord = d->clutEditor->getScene()->valueToCoordinate(maxVal);

            d->clutEditor->getScene()->table()->deleteAllVertices();

            d->clutEditor->getScene()->table()->addVertex(new medClutEditorVertex( minVal, minCoord, d->minColor, d->clutEditor->getScene()->table()));
            d->clutEditor->getScene()->table()->addVertex(new medClutEditorVertex( maxVal, maxCoord, d->maxColor, d->clutEditor->getScene()->table()));
        }
        else
        {
            QPointF minVal( d->thresholdLowerValue->value() , vertices.first()->value().y());
            QPointF minCoord = d->clutEditor->getScene()->valueToCoordinate(minVal);
            QColor minColor = Qt::white;

            QPointF maxVal( d->thresholdUpperValue->value(), vertices.last()->value().y());
            QPointF maxCoord = d->clutEditor->getScene()->valueToCoordinate(maxVal);
            QColor maxColor = Qt::white;

            QPointF value( d->thresholdFilterValue->value(), vertices.at(1)->value().y() );
            if ( vertices.size()==2 )
            {
                value.setY(1.);

            }
            QPointF coord = d->clutEditor->getScene()->valueToCoordinate(value);

            d->clutEditor->getScene()->table()->deleteAllVertices();

            d->clutEditor->getScene()->table()->addVertex(new medClutEditorVertex( minVal, minCoord, minColor, d->clutEditor->getScene()->table() ));
            d->clutEditor->getScene()->table()->addVertex(new medClutEditorVertex( maxVal, maxCoord, maxColor, d->clutEditor->getScene()->table() ));
            d->clutEditor->getScene()->table()->addVertex(new medClutEditorVertex( value, coord, d->thresholdColor, d->clutEditor->getScene()->table() ));

            connect(d->clutEditor->getScene()->table()->vertices().at(1)->getColorAction(), SIGNAL(triggered()), this, SLOT(setThresholdColor()), Qt::UniqueConnection);

            d->clutEditor->invertLUT(d->valueButtonGroup->checkedId()!=itkFiltersThresholdingProcess::upperButtonId);
        }
    }
}

void itkFiltersToolBox::setThresholdColor()
{
    if ( d->clutEditor->getScene()->table()->vertices().size() == 3 )
    {
        d->thresholdColor = d->clutEditor->getScene()->table()->vertices().at(1)->color();
    }
    else
    {
        d->thresholdColor = Qt::black;
    }
}

void itkFiltersToolBox::showHistogram(int state)
{
    if ( !this->selectorToolBox()->data() )
    {
        return;
    }

    if ( getWorkspace() != nullptr && getWorkspace()->tabbedViewContainers() != nullptr )
    {
        // Opening the histogram could be long, disable the use of the toolbox to avoid problems
        this->setToolBoxOnWaitStatusForNonRunnableProcess();

        if ( state == Qt::Checked )
        {
            medAbstractView* medVtkView = dynamic_cast<medAbstractView*>(this->getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView());
            if (d->clutEditor == nullptr)
            {
                d->clutEditor = new medClutEditorToolBox();
                medVtkView->mainWindow()->parentWidget()->layout()->addWidget(d->clutEditor);
                d->clutEditor->forceLayer(0);
                d->clutEditor->setData(selectorToolBox()->data());
                d->clutEditor->setView(medVtkView);
                d->clutEditor->setMaximumHeight(350);
                connect(d->clutEditor->getScene()->table(), SIGNAL(vertexAdded()), this, SLOT(addVertex()), Qt::UniqueConnection);
                connect(d->clutEditor->getScene()->table(), SIGNAL(vertexSet()), this, SLOT(updateSliders()), Qt::UniqueConnection);
                connect(medVtkView,SIGNAL(closed()),this,SLOT(onViewClosed()), Qt::UniqueConnection);
            }
            d->clutEditor->setDiscreteMode(true);
            d->clutEditor->show();
            updateClutEditorView();
        }
        else
        {
            if (d->clutEditor != nullptr)
            {
                d->histogram->setChecked(false);

                d->clutEditor->setDiscreteMode(false);
                d->clutEditor->getScene()->table()->deleteAllVertices();
                QPointF minVal( d->minValueImage , 0);
                QPointF minCoord = d->clutEditor->getScene()->valueToCoordinate(minVal);
                d->clutEditor->getScene()->table()->addVertex(new medClutEditorVertex( minVal, minCoord, QColor(Qt::white), d->clutEditor->getScene()->table() ));
                QPointF maxVal( d->maxValueImage, 1);
                QPointF maxCoord = d->clutEditor->getScene()->valueToCoordinate(maxVal);
                d->clutEditor->getScene()->table()->addVertex(new medClutEditorVertex( maxVal, maxCoord, QColor(Qt::white), d->clutEditor->getScene()->table() ));
                d->clutEditor->getScene()->table()->updateVerticesToDisplay();
                d->clutEditor->hide();
            }
        }

        this->setToolBoxOnReadyToUse();
    }
}

void itkFiltersToolBox::addVertex()
{
    QList<medClutEditorVertex*> & vertices = d->clutEditor->getScene()->table()->vertices();

    int maxTableSize = 0;
    if (d->binaryThreshold->isChecked() || d->valueButtonGroup->checkedId()==itkFiltersThresholdingProcess::outsideButtonId )
    {
        maxTableSize = 2;
    }
    else
    {
        maxTableSize = 3;
    }
    if ( vertices.size() > maxTableSize )
    {
        d->clutEditor->getScene()->table()->setSelectedAllVertices(false);
        for(int i = 1; i < vertices.size()-1; i++) //don't take first and last vertices
        {
            if ( vertices.at(i)->value().x() != d->thresholdFilterValue->value() )
            {
                vertices.at(i)->setSelected(true);
            }
        }
        d->clutEditor->getScene()->table()->deleteSelection();
    }
}

void itkFiltersToolBox::updateSliders()
{
    QList<medClutEditorVertex*> &vertices = d->clutEditor->getScene()->table()->vertices();

    if ( vertices.first()->value().x() != d->thresholdLowerValue->value() )
    {
        double lowerValue = vertices.first()->value().x();
        d->thresholdLowerValue->blockSignals(true);
        d->thresholdLowerValue->setValue(lowerValue);
        d->thresholdLowerValue->blockSignals(false);
    }
    if ( vertices.last()->value().x() != d->thresholdUpperValue->value() )
    {
        double upperValue = vertices.last()->value().x();
        d->thresholdUpperValue->blockSignals(true);
        d->thresholdUpperValue->setValue(upperValue);
        d->thresholdUpperValue->blockSignals(false);
    }
    if ( vertices.size()==3  && vertices.at(1)->value().x() != d->thresholdFilterValue->value() )
    {
        double filterValue = vertices.at(1)->value().x();
        d->thresholdFilterValue->blockSignals(true);
        d->thresholdFilterValue->setValue(filterValue);
        d->thresholdFilterValue->blockSignals(false);
    }
}

void itkFiltersToolBox::updateHistogramView()
{
    if ( ! selectorToolBox() || !(selectorToolBox()->data()) )
    {
        return;
    }

    bool thresholdIsVisible =  d->thresholdFilterWidget->isVisible();
    d->histogram->setEnabled(thresholdIsVisible);

    if ( getWorkspace()  &&
         getWorkspace()->tabbedViewContainers() )
    {
        medViewContainer *viewContainer = getWorkspace()->tabbedViewContainers()->getFirstSelectedContainer();
        if ( viewContainer )
        {
            getWorkspace()->tabbedViewContainers()->removeTab(1);

            viewContainer->enableHistogramAction(!thresholdIsVisible);
            if ( thresholdIsVisible )
            {
                if ( viewContainer->histogramAction()->isChecked() )
                {
                    viewContainer->clickHistoAction(false);
                }
            }
            else
            {
                if ( d->clutEditor != nullptr &&
                     d->clutEditor->getScene() != nullptr )
                {
                    showHistogram(Qt::Unchecked);
                    getWorkspace()->tabbedViewContainers()->removeTab(1);
                }
            }
        }
    }
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

    updateHistogramView();
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
    if ( d->histogram->isChecked() )
    {
        updateClutEditorView();
    }
}

void itkFiltersToolBox::checkBinaryThreshold(bool checked)
{
    d->buttonGroupWidget->setVisible(!checked);

    updateThresholdToolboxBehaviour(d->valueButtonGroup->checkedId());

    d->thresholdFilterValue->getSlider()->setHidden(checked);
    if ( checked )
    {
        d->thresholdFilterValue->getLabel()->setText( tr ( "Inside Value " ) );
        d->thresholdFilterValue->setValue ( itkFiltersBinaryThresholdingProcess::defaultInsideValue );
        d->thresholdFilterValue->setDecimals(0);
        d->infoThreshold->setText("Values equal to either threshold is considered to be between the thresholds.");
    }
    else
    {
        d->thresholdFilterValue->getLabel()->setText( tr ( "Value" ) );
        d->thresholdFilterValue->setValue ( itkFiltersThresholdingProcess::defaultThreshold );
        d->infoThreshold->setText("Values equal to the threshold value are not set to Outside Value." );
    }
}

void itkFiltersToolBox::onViewClosed()
{
    if (getWorkspace() != nullptr && getWorkspace()->tabbedViewContainers() != nullptr )
    {
        getWorkspace()->tabbedViewContainers()->removeTab(1);
    }
    d->clutEditor->reset();
    delete d->clutEditor;
    d->clutEditor = nullptr;
    d->histogram->setChecked(false);
}

dtkPlugin* itkFiltersToolBox::plugin()
{
    return medPluginManager::instance().plugin("ITK Filters");
}

medToolBox *createitkFiltersToolBox(QWidget *parent)
{
    return new itkFiltersToolBox(parent);
}
