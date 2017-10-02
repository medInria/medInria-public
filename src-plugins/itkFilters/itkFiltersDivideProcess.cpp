/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkFiltersDivideProcess.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <itkDivideImageFilter.h>
#include <itkImage.h>

#include <medUtilities.h>

class itkFiltersDivideProcessPrivate
{
public:
    double divideFactor;
};

const double itkFiltersDivideProcess::defaultDivideFactor = 2.0;

//-------------------------------------------------------------------------------------------

itkFiltersDivideProcess::itkFiltersDivideProcess(itkFiltersDivideProcess *parent) 
    : itkFiltersProcessBase(parent), d(new itkFiltersDivideProcessPrivate)
{  
    d->divideFactor = defaultDivideFactor;
}

itkFiltersDivideProcess::itkFiltersDivideProcess(const itkFiltersDivideProcess& other)
     : itkFiltersProcessBase(other), d(other.d)
{
}

//-------------------------------------------------------------------------------------------

itkFiltersDivideProcess::~itkFiltersDivideProcess( void )
{
    delete d;
}

//-------------------------------------------------------------------------------------------

bool itkFiltersDivideProcess::registered( void )
{
    return dtkAbstractProcessFactory::instance()->registerProcessType("itkDivideProcess", createitkFiltersDivideProcess);
}

//-------------------------------------------------------------------------------------------

QString itkFiltersDivideProcess::description() const
{
    return tr("ITK divide by constant filter");
}

//-------------------------------------------------------------------------------------------

void itkFiltersDivideProcess::setParameter(double data, int channel)
{
    if (channel != 0)
        return;
    
    d->divideFactor = data;
}

//-------------------------------------------------------------------------------------------

int itkFiltersDivideProcess::tryUpdate()
{
    int res = DTK_FAILURE;
    
    if ( getInputData() )
    {
        QString id = getInputData()->identifier();

        if ( id == "itkDataImageChar3" )
        {
            res = updateProcess<char>();
        }
        else if ( id == "itkDataImageUChar3" )
        {
            res = updateProcess<unsigned char>();
        }
        else if ( id == "itkDataImageShort3" )
        {
            res = updateProcess<short>();
        }
        else if ( id == "itkDataImageUShort3" )
        {
            res = updateProcess<unsigned short>();
        }
        else if ( id == "itkDataImageInt3" )
        {
            res = updateProcess<int>();
        }
        else if ( id == "itkDataImageUInt3" )
        {
            res = updateProcess<unsigned int>();
        }
        else if ( id == "itkDataImageLong3" )
        {
            res = updateProcess<long>();
        }
        else if ( id== "itkDataImageULong3" )
        {
            res = updateProcess<unsigned long>();
        }
        else if ( id == "itkDataImageFloat3" )
        {
            res = updateProcess<float>();
        }
        else if ( id == "itkDataImageDouble3" )
        {
            res = updateProcess<double>();
        }
        else
        {
            res = medAbstractProcess::PIXEL_TYPE;
        }
    }

    return res;
}

template <class PixelType> int itkFiltersDivideProcess::updateProcess()
{
    typedef itk::Image< PixelType, 3 > ImageType;
    typedef itk::DivideImageFilter< ImageType, itk::Image<double, ImageType::ImageDimension>, ImageType >  DivideFilterType;
    typename DivideFilterType::Pointer divideFilter = DivideFilterType::New();

    divideFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( getInputData()->data() ) ) );
    divideFilter->SetConstant ( d->divideFactor );

    itk::CStyleCommand::Pointer callback = itk::CStyleCommand::New();
    callback->SetClientData ( ( void * ) this );
    callback->SetCallback ( itkFiltersProcessBase::eventCallback );
    divideFilter->AddObserver ( itk::ProgressEvent(), callback );

    divideFilter->Update();

    getOutputData()->setData ( divideFilter->GetOutput() );

    QString newSeriesDescription = "divide filter " + QString::number(d->divideFactor);
    medUtilities::setDerivedMetaData(getOutputData(), getInputData(), newSeriesDescription);

    return DTK_SUCCEED;
}

// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractProcess * createitkFiltersDivideProcess ( void )
{
    return new itkFiltersDivideProcess;
}
