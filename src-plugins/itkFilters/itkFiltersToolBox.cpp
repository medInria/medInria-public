/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkFiltersToolBox.h"

#include <limits>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>

#include <medCore/medAbstractDataImage.h>

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

#include "itkFiltersProcessBase.h"

#include <QtGui>

class itkFiltersToolBoxPrivate
{
public:
    QLabel * dataTypeValue;

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
    QWidget * dilateFilterWidget;
    QWidget * erodeFilterWidget;
    QWidget * closeFilterWidget;

    QDoubleSpinBox * addFiltersValue;
    QDoubleSpinBox * subtractFiltersValue;
    QDoubleSpinBox * multiplyFiltersValue;
    QDoubleSpinBox * divideFiltersValue;
    QDoubleSpinBox * gaussianFiltersValue;
    QDoubleSpinBox * dilateFiltersValue;
    QDoubleSpinBox * erodeFiltersValue;
    QDoubleSpinBox * closeFiltersValue;
    QSpinBox * shrink0Value;
    QSpinBox * shrink1Value;
    QSpinBox * shrink2Value;

    QDoubleSpinBox * intensityMinimumValue;
    QDoubleSpinBox * intensityMaximumValue;
    QDoubleSpinBox * intensityOutputMinimumValue;
    QDoubleSpinBox * intensityOutputMaximumValue;

    QComboBox * filters;
//    dtkSmartPointer <dtkAbstractProcess> process;
    dtkSmartPointer <itkFiltersProcessBase> process;
    
    medProgressionStack * progression_stack;
};

itkFiltersToolBox::itkFiltersToolBox ( QWidget *parent ) : medFilteringAbstractToolBox ( parent ), d ( new itkFiltersToolBoxPrivate )
{
    qDebug() << "itkFiltersToolBox";
    //Filters selection combobox
    d->filters = new QComboBox;
    QStringList filtersList;
    filtersList << "Add Constant to Image" 
                << "Substract Constant from Image" 
                << "Multiply image by constant" 
                << "Divide image by constant"
                << "Gaussian blur" 
                << "Normalize image filter" 
                << "Median filter" 
                << "Invert intensity filter"
                << "Shrink image filter" 
                << "Intensity windowing filter"
                << "Binary Dilate filter"
                << "Binary Erode filter"
                << "Binary Close Filter";
    
    d->filters->addItems ( filtersList );

    QObject::connect ( d->filters, SIGNAL ( activated ( int ) ), this, SLOT ( onFiltersActivated ( int ) ) );

    QLabel * dataTypeLabel = new QLabel ( tr ( "Data type :" ) );
    d->dataTypeValue = new QLabel ( tr ( "Unknown" ) );

    QHBoxLayout * dataTypeLayout = new QHBoxLayout;
    dataTypeLayout->addWidget ( dataTypeLabel );
    dataTypeLayout->addWidget ( d->dataTypeValue );

    //Initialise filters widget (probably need to find a dynamic way of doing this, Factory ?)
    //Add filter widgets
    d->addFilterWidget = new QWidget(this);
    d->addFiltersValue = new QDoubleSpinBox;
    d->addFiltersValue->setMaximum ( 1000000000 );
    d->addFiltersValue->setValue ( 100.0 );
    QLabel * addFilterLabel = new QLabel ( tr ( "Constant value:" ) );
    QHBoxLayout * addFilterLayout = new QHBoxLayout;
    addFilterLayout->addWidget ( addFilterLabel );
    addFilterLayout->addWidget ( d->addFiltersValue );
    addFilterLayout->addStretch ( 1 );
    d->addFilterWidget->setLayout ( addFilterLayout );

    //Substract filter widgets
    d->subtractFilterWidget = new QWidget(this);
    d->subtractFiltersValue = new QDoubleSpinBox;
    d->subtractFiltersValue->setMaximum ( 1000000000 );
    d->subtractFiltersValue->setValue ( 100.0 );
    QLabel * subtractFilterLabel = new QLabel ( tr ( "Constant value:" ) );
    QHBoxLayout * subtractFilterLayout = new QHBoxLayout;
    subtractFilterLayout->addWidget ( subtractFilterLabel );
    subtractFilterLayout->addWidget ( d->subtractFiltersValue );
    subtractFilterLayout->addStretch ( 1 );
    d->subtractFilterWidget->setLayout ( subtractFilterLayout );

    //Multiply filter widgets
    d->multiplyFilterWidget = new QWidget(this);
    d->multiplyFiltersValue = new QDoubleSpinBox;
    d->multiplyFiltersValue->setValue ( 2.0 );
    d->multiplyFiltersValue->setMaximum ( 1000000000 );
    QLabel * multiplyFilterLabel = new QLabel ( tr ( "Constant value:" ) );
    QHBoxLayout * multiplyFilterLayout = new QHBoxLayout;
    multiplyFilterLayout->addWidget ( multiplyFilterLabel );
    multiplyFilterLayout->addWidget ( d->multiplyFiltersValue );
    multiplyFilterLayout->addStretch ( 1 );
    d->multiplyFilterWidget->setLayout ( multiplyFilterLayout );

    //Divide filter widgets
    d->divideFilterWidget = new QWidget(this);
    d->divideFiltersValue = new QDoubleSpinBox;
    d->divideFiltersValue->setValue ( 2.0 );
    d->divideFiltersValue->setMaximum ( 1000000000 );
    QLabel * divideFilterLabel = new QLabel ( tr ( "Constant value:" ) );
    QHBoxLayout * divideFilterLayout = new QHBoxLayout;
    divideFilterLayout->addWidget ( divideFilterLabel );
    divideFilterLayout->addWidget ( d->divideFiltersValue );
    divideFilterLayout->addStretch ( 1 );
    d->divideFilterWidget->setLayout ( divideFilterLayout );

    //Gaussian filter widgets
    d->gaussianFilterWidget = new QWidget(this);
    d->gaussianFiltersValue = new QDoubleSpinBox;
    d->gaussianFiltersValue->setValue ( 1.0 );
    d->gaussianFiltersValue->setMaximum ( 10.0 );
    QLabel * gaussianFilterLabel = new QLabel ( tr ( "Sigma value:" ) );
    QHBoxLayout * gaussianFilterLayout = new QHBoxLayout;
    gaussianFilterLayout->addWidget ( gaussianFilterLabel );
    gaussianFilterLayout->addWidget ( d->gaussianFiltersValue );
    gaussianFilterLayout->addStretch ( 1 );
    d->gaussianFilterWidget->setLayout ( gaussianFilterLayout );

    d->normalizeFilterWidget = new QWidget(this);
    d->medianFilterWidget = new QWidget(this);
    d->invertFilterWidget = new QWidget(this);

    //Shrink filter widgets
    d->shrinkFilterWidget = new QWidget;
    d->shrink0Value = new QSpinBox;
    d->shrink0Value->setValue ( 1 );
    d->shrink0Value->setMaximum ( 10 );

    d->shrink1Value = new QSpinBox;
    d->shrink1Value->setValue ( 1 );
    d->shrink1Value->setMaximum ( 10 );

    d->shrink2Value = new QSpinBox;
    d->shrink2Value->setValue ( 1 );
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
    d->intensityMinimumValue->setMaximum ( 255 );
    d->intensityMinimumValue->setValue ( 0 );
    d->intensityMaximumValue = new QDoubleSpinBox;
    d->intensityMaximumValue->setMaximum ( 255 );
    d->intensityMaximumValue->setValue ( 255 );
    d->intensityOutputMinimumValue = new QDoubleSpinBox;
    d->intensityOutputMinimumValue->setMaximum ( 255 );
    d->intensityOutputMinimumValue->setValue ( 0 );
    d->intensityOutputMaximumValue = new QDoubleSpinBox;
    d->intensityOutputMaximumValue->setMaximum ( 255 );
    d->intensityOutputMaximumValue->setValue ( 255 );

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


    //Dilate filter widgets
    d->dilateFilterWidget = new QWidget(this);
    d->dilateFiltersValue = new QDoubleSpinBox;
    d->dilateFiltersValue->setMaximum ( 20 );
    d->dilateFiltersValue->setValue ( 5 );
    QLabel * dilateFilterLabel = new QLabel ( tr ( "Kernel size:" ) );
    QHBoxLayout * dilateFilterLayout = new QHBoxLayout;
    dilateFilterLayout->addWidget ( dilateFilterLabel );
    dilateFilterLayout->addWidget ( d->dilateFiltersValue );
    dilateFilterLayout->addStretch ( 1 );
    d->dilateFilterWidget->setLayout ( dilateFilterLayout );

    //Erode filter widgets
    d->erodeFilterWidget = new QWidget(this);
    d->erodeFiltersValue = new QDoubleSpinBox;
    d->erodeFiltersValue->setMaximum ( 20 );
    d->erodeFiltersValue->setValue ( 5 );
    QLabel * erodeFilterLabel = new QLabel ( tr ( "Kernel size:" ) );
    QHBoxLayout * erodeFilterLayout = new QHBoxLayout;
    erodeFilterLayout->addWidget ( erodeFilterLabel );
    erodeFilterLayout->addWidget ( d->erodeFiltersValue );
    erodeFilterLayout->addStretch ( 1 );
    d->erodeFilterWidget->setLayout ( erodeFilterLayout );

    //Erode filter widgets
    d->closeFilterWidget = new QWidget(this);
    d->closeFiltersValue = new QDoubleSpinBox;
    d->closeFiltersValue->setMaximum ( 20 );
    d->closeFiltersValue->setValue ( 5 );
    QLabel * closeFilterLabel = new QLabel ( tr ( "Kernel size:" ) );
    QHBoxLayout * closeFilterLayout = new QHBoxLayout;
    closeFilterLayout->addWidget ( closeFilterLabel );
    closeFilterLayout->addWidget ( d->closeFiltersValue );
    closeFilterLayout->addStretch ( 1 );
    d->closeFilterWidget->setLayout ( closeFilterLayout );

    // Run button:
    QPushButton *runButton = new QPushButton ( tr ( "Run" ) );
    runButton->setFocusPolicy ( Qt::NoFocus );
    runButton->setToolTip(tr("Launch the selected filter"));

    // Principal layout:
    QWidget *widget = new QWidget ( this );

    d->progression_stack = new medProgressionStack ( widget );

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget ( d->filters );
    layout->addLayout ( dataTypeLayout );
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
    layout->addWidget ( d->dilateFilterWidget );
    layout->addWidget ( d->erodeFilterWidget );
    layout->addWidget ( d->closeFilterWidget );
    layout->addWidget ( runButton );
    layout->addWidget ( d->progression_stack );
    layout->addStretch ( 1 );

    this->onFiltersActivated ( 0 );
    widget->setLayout ( layout );

    // Main toolbox:
    this->setTitle ( tr ( "ITK Basic Filters" ) );
    this->addWidget ( widget );

    // Add about plugin
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "itkFiltersPlugin" );
    setAboutPluginButton ( plugin );
    setAboutPluginVisibility ( true );

    connect ( runButton, SIGNAL ( clicked() ), this, SLOT ( run() ) );

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
    return factory->registerToolBox<itkFiltersToolBox> (
                "itkBasicFilters",
                "ITK Basic Filters",
                "ITK filters, processing examples",
                QStringList()<<"filtering"/*,
                createitkFiltersToolBox */);
}


dtkAbstractData* itkFiltersToolBox::processOutput()
{
    if ( !d->process )
        return NULL;

    return d->process->output();
}

void itkFiltersToolBox::clear()
{
    qDebug() << "Clear itk filters toolbox";

    d->dataTypeValue->setText ( "Unknown" );
    d->addFiltersValue->setMaximum ( 100.0 );
    d->subtractFiltersValue->setMaximum ( 100.0 );

    d->intensityMinimumValue->setMinimum ( 0 );
    d->intensityMinimumValue->setMaximum ( 255 );
    d->intensityMinimumValue->setValue ( 0 );

    d->intensityMaximumValue->setMinimum ( 0 );
    d->intensityMaximumValue->setMaximum ( 255 );
    d->intensityMaximumValue->setValue ( 255 );

    d->intensityOutputMinimumValue->setMinimum ( 0 );
    d->intensityOutputMinimumValue->setMaximum ( 255 );
    d->intensityOutputMinimumValue->setValue ( 0 );

    d->intensityOutputMaximumValue->setMinimum ( 0 );
    d->intensityOutputMaximumValue->setMaximum ( 255 );
    d->intensityOutputMaximumValue->setValue ( 255 );
}

void itkFiltersToolBox::update ( dtkAbstractView* view )
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
            d->addFiltersValue->setMaximum ( std::numeric_limits<char>::max() );
            d->subtractFiltersValue->setMaximum ( std::numeric_limits<char>::max() );

            d->intensityMinimumValue->setMinimum ( std::numeric_limits<char>::min() );
            d->intensityMinimumValue->setMaximum ( std::numeric_limits<char>::max() );
            d->intensityMinimumValue->setValue ( std::numeric_limits<char>::min() );

            d->intensityMaximumValue->setMinimum ( std::numeric_limits<char>::min() );
            d->intensityMaximumValue->setMaximum ( std::numeric_limits<char>::max() );
            d->intensityMaximumValue->setValue ( std::numeric_limits<char>::max() );


            d->intensityOutputMinimumValue->setMinimum ( std::numeric_limits<char>::min() );
            d->intensityOutputMinimumValue->setMaximum ( std::numeric_limits<char>::max() );
            d->intensityOutputMinimumValue->setValue ( std::numeric_limits<char>::min() );

            d->intensityOutputMaximumValue->setMinimum ( std::numeric_limits<char>::min() );
            d->intensityOutputMaximumValue->setMaximum ( std::numeric_limits<char>::max() );
            d->intensityOutputMaximumValue->setValue ( std::numeric_limits<char>::max() );

        }
        else if ( identifier == "itkDataImageUChar3" )
        {
            d->dataTypeValue->setText ( "Unsigned char" );
            d->addFiltersValue->setMaximum ( std::numeric_limits<unsigned char>::max() );
            d->subtractFiltersValue->setMaximum ( std::numeric_limits<unsigned char>::max() );


            d->intensityMinimumValue->setMinimum ( std::numeric_limits<unsigned char>::min() );
            d->intensityMinimumValue->setMaximum ( std::numeric_limits<unsigned char>::max() );
            d->intensityMinimumValue->setValue ( std::numeric_limits<unsigned char>::min() );

            d->intensityMaximumValue->setMinimum ( std::numeric_limits<unsigned char>::min() );
            d->intensityMaximumValue->setMaximum ( std::numeric_limits<unsigned char>::max() );
            d->intensityMaximumValue->setValue ( std::numeric_limits<unsigned char>::max() );

            d->intensityOutputMinimumValue->setMinimum ( std::numeric_limits<unsigned char>::min() );
            d->intensityOutputMinimumValue->setMaximum ( std::numeric_limits<unsigned char>::max() );
            d->intensityOutputMinimumValue->setValue ( std::numeric_limits<unsigned char>::min() );

            d->intensityOutputMaximumValue->setMinimum ( std::numeric_limits<unsigned char>::min() );
            d->intensityOutputMaximumValue->setMaximum ( std::numeric_limits<unsigned char>::max() );
            d->intensityOutputMaximumValue->setValue ( std::numeric_limits<unsigned char>::max() );
        }
        else if ( identifier == "itkDataImageShort3" )
        {
            d->dataTypeValue->setText ( "Short" );
            d->addFiltersValue->setMaximum ( std::numeric_limits<short>::max() );
            d->subtractFiltersValue->setMaximum ( std::numeric_limits<short>::max() );

            d->intensityMinimumValue->setMinimum ( std::numeric_limits<short>::min() );
            d->intensityMinimumValue->setMaximum ( std::numeric_limits<short>::max() );
            d->intensityMinimumValue->setValue ( std::numeric_limits<short>::min() );

            d->intensityMaximumValue->setMinimum ( std::numeric_limits<short>::min() );
            d->intensityMaximumValue->setMaximum ( std::numeric_limits<short>::max() );
            d->intensityMaximumValue->setValue ( std::numeric_limits<short>::max() );

            d->intensityOutputMinimumValue->setMinimum ( std::numeric_limits<short>::min() );
            d->intensityOutputMinimumValue->setMaximum ( std::numeric_limits<short>::max() );
            d->intensityOutputMinimumValue->setValue ( std::numeric_limits<short>::min() );

            d->intensityOutputMaximumValue->setMinimum ( std::numeric_limits<short>::min() );
            d->intensityOutputMaximumValue->setMaximum ( std::numeric_limits<short>::max() );
            d->intensityOutputMaximumValue->setValue ( std::numeric_limits<short>::max() );
        }
        else if ( identifier == "itkDataImageUShort3" )
        {
            d->dataTypeValue->setText ( "Unsigned short" );
            d->addFiltersValue->setMaximum ( std::numeric_limits<unsigned short>::max() );
            d->subtractFiltersValue->setMaximum ( std::numeric_limits<unsigned short>::max() );

            d->intensityMinimumValue->setMinimum ( std::numeric_limits<unsigned short>::min() );
            d->intensityMinimumValue->setMaximum ( std::numeric_limits<unsigned short>::max() );
            d->intensityMinimumValue->setValue ( std::numeric_limits<unsigned short>::min() );

            d->intensityMaximumValue->setMinimum ( std::numeric_limits<unsigned short>::min() );
            d->intensityMaximumValue->setMaximum ( std::numeric_limits<unsigned short>::max() );
            d->intensityMaximumValue->setValue ( std::numeric_limits<unsigned short>::max() );

            d->intensityOutputMinimumValue->setMinimum ( std::numeric_limits<unsigned short>::min() );
            d->intensityOutputMinimumValue->setMaximum ( std::numeric_limits<unsigned short>::max() );
            d->intensityOutputMinimumValue->setValue ( std::numeric_limits<unsigned short>::min() );

            d->intensityOutputMaximumValue->setMinimum ( std::numeric_limits<unsigned short>::min() );
            d->intensityOutputMaximumValue->setMaximum ( std::numeric_limits<unsigned short>::max() );
            d->intensityOutputMaximumValue->setValue ( std::numeric_limits<unsigned short>::max() );
        }
        else if ( identifier == "itkDataImageInt3" )
        {
            d->dataTypeValue->setText ( "Int" );
            d->addFiltersValue->setMaximum ( std::numeric_limits<int>::max() );
            d->subtractFiltersValue->setMaximum ( std::numeric_limits<int>::max() );

            d->intensityMinimumValue->setMinimum ( std::numeric_limits<int>::min() );
            d->intensityMinimumValue->setMaximum ( std::numeric_limits<int>::max() );
            d->intensityMinimumValue->setValue ( std::numeric_limits<int>::min() );

            d->intensityMaximumValue->setMinimum ( std::numeric_limits<int>::min() );
            d->intensityMaximumValue->setMaximum ( std::numeric_limits<int>::max() );
            d->intensityMaximumValue->setValue ( std::numeric_limits<int>::max() );

            d->intensityOutputMinimumValue->setMinimum ( std::numeric_limits<int>::min() );
            d->intensityOutputMinimumValue->setMaximum ( std::numeric_limits<int>::max() );
            d->intensityOutputMinimumValue->setValue ( std::numeric_limits<int>::min() );

            d->intensityOutputMaximumValue->setMinimum ( std::numeric_limits<int>::min() );
            d->intensityOutputMaximumValue->setMaximum ( std::numeric_limits<int>::max() );
            d->intensityOutputMaximumValue->setValue ( std::numeric_limits<int>::max() );
        }
        else if ( identifier == "itkDataImageUInt3" )
        {
            d->dataTypeValue->setText ( "Unsigned int" );
            d->addFiltersValue->setMaximum ( std::numeric_limits<unsigned int>::max() );
            d->subtractFiltersValue->setMaximum ( std::numeric_limits<unsigned int>::max() );

            d->intensityMinimumValue->setMinimum ( std::numeric_limits<unsigned int>::min() );
            d->intensityMinimumValue->setMaximum ( std::numeric_limits<unsigned int>::max() );
            d->intensityMinimumValue->setValue ( std::numeric_limits<unsigned int>::min() );

            d->intensityMaximumValue->setMinimum ( std::numeric_limits<unsigned int>::min() );
            d->intensityMaximumValue->setMaximum ( std::numeric_limits<unsigned int>::max() );
            d->intensityMaximumValue->setValue ( std::numeric_limits<unsigned int>::max() );

            d->intensityOutputMinimumValue->setMinimum ( std::numeric_limits<unsigned int>::min() );
            d->intensityOutputMinimumValue->setMaximum ( std::numeric_limits<unsigned int>::max() );
            d->intensityOutputMinimumValue->setValue ( std::numeric_limits<unsigned int>::min() );

            d->intensityOutputMaximumValue->setMinimum ( std::numeric_limits<unsigned int>::min() );
            d->intensityOutputMaximumValue->setMaximum ( std::numeric_limits<unsigned int>::max() );
            d->intensityOutputMaximumValue->setValue ( std::numeric_limits<unsigned int>::max() );
        }
        else if ( identifier == "itkDataImageLong3" )
        {
            d->dataTypeValue->setText ( "Long" );
            d->addFiltersValue->setMaximum ( std::numeric_limits<long>::max() );
            d->subtractFiltersValue->setValue ( std::numeric_limits<long>::max() );

            d->intensityMinimumValue->setMinimum ( std::numeric_limits<long>::min() );
            d->intensityMinimumValue->setMaximum ( std::numeric_limits<long>::max() );
            d->intensityMinimumValue->setValue ( std::numeric_limits<long>::min() );

            d->intensityMaximumValue->setMinimum ( std::numeric_limits<long>::min() );
            d->intensityMaximumValue->setMaximum ( std::numeric_limits<long>::max() );
            d->intensityMaximumValue->setValue ( std::numeric_limits<long>::max() );

            d->intensityOutputMinimumValue->setMinimum ( std::numeric_limits<long>::min() );
            d->intensityOutputMinimumValue->setMaximum ( std::numeric_limits<long>::max() );
            d->intensityOutputMinimumValue->setValue ( std::numeric_limits<long>::min() );

            d->intensityOutputMaximumValue->setMinimum ( std::numeric_limits<long>::min() );
            d->intensityOutputMaximumValue->setMaximum ( std::numeric_limits<long>::max() );
            d->intensityOutputMaximumValue->setValue ( std::numeric_limits<long>::max() );
        }
        else if ( identifier== "itkDataImageULong3" )
        {
            d->dataTypeValue->setText ( "Unsigned long" );
            d->addFiltersValue->setMaximum ( std::numeric_limits<unsigned long>::max() );
            d->subtractFiltersValue->setMaximum ( std::numeric_limits<unsigned long>::max() );

            d->intensityMinimumValue->setMinimum ( std::numeric_limits<unsigned long>::min() );
            d->intensityMinimumValue->setMaximum ( std::numeric_limits<unsigned long>::max() );
            d->intensityMinimumValue->setValue ( std::numeric_limits<unsigned long>::min() );

            d->intensityMaximumValue->setMinimum ( std::numeric_limits<unsigned long>::min() );
            d->intensityMaximumValue->setMaximum ( std::numeric_limits<unsigned long>::max() );
            d->intensityMaximumValue->setValue ( std::numeric_limits<unsigned long>::max() );

            d->intensityOutputMinimumValue->setMinimum ( std::numeric_limits<unsigned long>::min() );
            d->intensityOutputMinimumValue->setMaximum ( std::numeric_limits<unsigned long>::max() );
            d->intensityOutputMinimumValue->setValue ( std::numeric_limits<unsigned long>::min() );

            d->intensityOutputMaximumValue->setMinimum ( std::numeric_limits<unsigned long>::min() );
            d->intensityOutputMaximumValue->setMaximum ( std::numeric_limits<unsigned long>::max() );
            d->intensityOutputMaximumValue->setValue ( std::numeric_limits<unsigned long>::max() );
        }
        else if ( identifier == "itkDataImageFloat3" )
        {
            d->dataTypeValue->setText ( "Float" );
            d->addFiltersValue->setMaximum ( std::numeric_limits<float>::max() );
            d->subtractFiltersValue->setMaximum ( std::numeric_limits<float>::max() );

            d->intensityMinimumValue->setMinimum ( std::numeric_limits<float>::min() );
            d->intensityMinimumValue->setMaximum ( std::numeric_limits<float>::max() );
            d->intensityMinimumValue->setValue ( std::numeric_limits<float>::min() );

            d->intensityMaximumValue->setMinimum ( std::numeric_limits<float>::min() );
            d->intensityMaximumValue->setMaximum ( std::numeric_limits<float>::max() );
            d->intensityMaximumValue->setValue ( std::numeric_limits<float>::max() );

            d->intensityOutputMinimumValue->setMinimum ( std::numeric_limits<float>::min() );
            d->intensityOutputMinimumValue->setMaximum ( std::numeric_limits<float>::max() );
            d->intensityOutputMinimumValue->setValue ( std::numeric_limits<float>::min() );

            d->intensityOutputMaximumValue->setMinimum ( std::numeric_limits<float>::min() );
            d->intensityOutputMaximumValue->setMaximum ( std::numeric_limits<float>::max() );
            d->intensityOutputMaximumValue->setValue ( std::numeric_limits<float>::max() );
        }
        else if ( identifier == "itkDataImageDouble3" )
        {
            d->dataTypeValue->setText ( "Double" );
            d->addFiltersValue->setMaximum ( std::numeric_limits<double>::max() );
            d->subtractFiltersValue->setMaximum ( std::numeric_limits<double>::max() );

            d->intensityMinimumValue->setMinimum ( std::numeric_limits<double>::min() );
            d->intensityMinimumValue->setMaximum ( std::numeric_limits<double>::max() );
            d->intensityMinimumValue->setValue ( std::numeric_limits<double>::min() );

            d->intensityMaximumValue->setMinimum ( std::numeric_limits<double>::min() );
            d->intensityMaximumValue->setMaximum ( std::numeric_limits<double>::max() );
            d->intensityMaximumValue->setValue ( std::numeric_limits<double>::max() );

            d->intensityOutputMinimumValue->setMinimum ( std::numeric_limits<double>::min() );
            d->intensityOutputMinimumValue->setMaximum ( std::numeric_limits<double>::max() );
            d->intensityOutputMinimumValue->setValue ( std::numeric_limits<double>::min() );

            d->intensityOutputMaximumValue->setMinimum ( std::numeric_limits<double>::min() );
            d->intensityOutputMaximumValue->setMaximum ( std::numeric_limits<double>::max() );
            d->intensityOutputMaximumValue->setValue ( std::numeric_limits<double>::max() );
        }
        else
        {
            qWarning() << "Error : pixel type not yet implemented ("
            << identifier
            << ")";
        }
    }
}

void itkFiltersToolBox::setupItkAddProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkAddProcess" );
    
    if (!d->process)
        return;

    d->process->setInput ( this->parentToolBox()->data() );
    d->process->setParameter ( d->addFiltersValue->value(), 0 );
}

void itkFiltersToolBox::setupItkSubtractProcess()
{
    qDebug() << "Subtract parameter (run) : " << d->subtractFiltersValue->value();
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkSubtractProcess" );
    
    if (!d->process)
        return;

    d->process->setInput ( this->parentToolBox()->data() );
    d->process->setParameter ( d->subtractFiltersValue->value(), 0 );
}

void itkFiltersToolBox::setupItkMultiplyProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkMultiplyProcess" );
    
    if (!d->process)
        return;
    
    d->process->setInput ( this->parentToolBox()->data() );
    d->process->setParameter ( d->multiplyFiltersValue->value(), 0 );
}

void itkFiltersToolBox::setupItkDivideProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkDivideProcess" );
    
    if (!d->process)
        return;
    
    d->process->setInput ( this->parentToolBox()->data() );
    d->process->setParameter ( d->divideFiltersValue->value(), 0 );
}

void itkFiltersToolBox::setupItkGaussianProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkGaussianProcess" );
    
    if (!d->process)
        return;
    
    d->process->setInput ( this->parentToolBox()->data() );
    d->process->setParameter ( d->gaussianFiltersValue->value(), 0);
}

void itkFiltersToolBox::setupItkMedianProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkMedianProcess" );
    
    if (!d->process)
        return;
    
    d->process->setInput ( this->parentToolBox()->data() );
}

void itkFiltersToolBox::setupItkNormalizeProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkNormalizeProcess" );
    
    if (!d->process)
        return;
    
    d->process->setInput ( this->parentToolBox()->data() );
}

void itkFiltersToolBox::setupItkInvertProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkInvertProcess" );
    
    if (!d->process)
        return;
    
    d->process->setInput ( this->parentToolBox()->data() );
}

void itkFiltersToolBox::setupItkShrinkProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkShrinkProcess" );
    
    if (!d->process)
        return;
    
    d->process->setInput ( this->parentToolBox()->data() );
    d->process->setParameter ( ( double ) d->shrink0Value->value(), 0 );
    d->process->setParameter ( ( double ) d->shrink1Value->value(), 1 );
    d->process->setParameter ( ( double ) d->shrink2Value->value(), 2 );
}

void itkFiltersToolBox::setupItkWindowingProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkWindowingProcess" );
    
    if (!d->process)
        return;
    
    d->process->setInput ( this->parentToolBox()->data() );
    d->process->setParameter ( d->intensityMinimumValue->value(), 0);
    d->process->setParameter ( d->intensityMaximumValue->value(), 1);
    d->process->setParameter ( d->intensityOutputMinimumValue->value(), 2 );
    d->process->setParameter ( d->intensityOutputMaximumValue->value(), 3 );
}

void itkFiltersToolBox::setupItkDilateProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkDilateProcess" );

    if (!d->process)
        return;
    
    d->process->setInput ( this->parentToolBox()->data() );
    d->process->setParameter ( d->dilateFiltersValue->value(), 0 );
}

void itkFiltersToolBox::setupItkErodeProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkErodeProcess" );

    if (!d->process)
        return;
    
    d->process->setInput ( this->parentToolBox()->data() );
    d->process->setParameter ( d->erodeFiltersValue->value(), 0 );
}

void itkFiltersToolBox::setupItkCloseProcess()
{
    d->process = dtkAbstractProcessFactory::instance()->createSmartPointer ( "itkCloseProcess" );

    if (!d->process)
        return;
    
    d->process->setInput ( this->parentToolBox()->data() );
    d->process->setParameter ( d->closeFiltersValue->value(), 0 );
}

void itkFiltersToolBox::run ( void )
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
    case 10: // dilate filter
        this->setupItkDilateProcess();
        break;
    case 11: // erode filter
        this->setupItkErodeProcess();
        break;
    case 12: // erode filter
        this->setupItkCloseProcess();
        break;
    }

    if (! d->process)
        return;

    medRunnableProcess *runProcess = new medRunnableProcess;
    runProcess->setProcess ( d->process );

    d->progression_stack->addJobItem ( runProcess, tr ( "Progress:" ) );

    connect ( runProcess, SIGNAL ( success ( QObject* ) ),  this, SIGNAL ( success () ) );
    connect ( runProcess, SIGNAL ( failure ( QObject* ) ),  this, SIGNAL ( failure () ) );

    medJobManager::instance()->registerJobItem ( runProcess );
    QThreadPool::globalInstance()->start ( dynamic_cast<QRunnable*> ( runProcess ) );
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
    d->dilateFilterWidget->hide();
    d->erodeFilterWidget->hide();
    d->closeFilterWidget->hide();

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
        d->dilateFilterWidget->show();
        break;
    case 11:
        d->erodeFilterWidget->show();
        break;
    case 12:
        d->closeFilterWidget->show();
        break;
    default:
        d->addFilterWidget->show();
    }
}

dtkPlugin* itkFiltersToolBox::plugin()
{
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "itkFiltersPlugin" );
    return plugin;
}


medToolBox *createitkFiltersToolBox ( QWidget *parent )
{
    return new itkFiltersToolBox ( parent );
}
