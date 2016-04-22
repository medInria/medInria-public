
#include "medBinaryOperatorBase.h"

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <medMetaDataKeys.h>
#include <medAbstractDataFactory.h>
#include <medUtilities.h>

#include <itkCastImageFilter.h>
#include <itkAndImageFilter.h>
#include <itkOrImageFilter.h>
#include <itkXorImageFilter.h>


medBinaryOperatorBase::medBinaryOperatorBase() : medAbstractProcess()
{
    m_inputA = NULL;
    m_inputB = NULL;
    m_output = NULL;
}

medBinaryOperatorBase::~medBinaryOperatorBase()
{
    m_inputA = NULL;
    m_inputB = NULL;
    m_output = NULL;
}

void medBinaryOperatorBase::setInput ( medAbstractData *data, int channel)
{
    if ( channel == 0)
    {
        m_inputA = data;
    }

    if ( channel == 1 )
    {        
        m_output = medAbstractDataFactory::instance()->createSmartPointer ( "itkDataImageUChar3" );
        m_inputB = data;
    }
}

int medBinaryOperatorBase::update()
{
    int res = DTK_FAILURE;

    if (m_inputA)
    {
        QString id = m_inputA->identifier();
        
        if ( id == "itkDataImageChar3" )
        {
            res = run< itk::Image <char,3> >();
        }
        else if ( id == "itkDataImageUChar3" )
        {
            res = run< itk::Image <unsigned char,3> >();
        }
        else if ( id == "itkDataImageShort3" )
        {
            res = run< itk::Image <short,3> >();
        }
        else if ( id == "itkDataImageUShort3" )
        {
            res = run< itk::Image <unsigned short,3> >();
        }
        else if ( id == "itkDataImageInt3" )
        {
            res = run< itk::Image <int,3> >();
        }
        else if ( id == "itkDataImageUInt3" )
        {
            res = run< itk::Image <unsigned int,3> >();
        }
        else if ( id == "itkDataImageLong3" )
        {
            res = run< itk::Image <long,3> >();
        }
        else if ( id== "itkDataImageULong3" )
        {
            res = run< itk::Image <unsigned long,3> >();
        }
        else if ( id == "itkDataImageFloat3" )
        {
            res = run< itk::Image <float,3> >();
        }
        else if ( id == "itkDataImageDouble3" )
        {
            res = run< itk::Image <double,3> >();
        }
        else
        {
            res = medAbstractProcess::PIXEL_TYPE;
        }
    }
    return res;
}

template <class ImageType> int medBinaryOperatorBase::run()
{
    int res = DTK_FAILURE;

    if (m_inputB)
    {
        QString id = m_inputB->identifier();
        
        if ( id == "itkDataImageChar3" )
        {
            res = runProcess< ImageType, itk::Image <char,3> >();
        }
        else if ( id == "itkDataImageUChar3" )
        {
            res = runProcess< ImageType, itk::Image <unsigned char,3> >();
        }
        else if ( id == "itkDataImageShort3" )
        {
            res = runProcess< ImageType, itk::Image <short,3> >();
        }
        else if ( id == "itkDataImageUShort3" )
        {
            res = runProcess< ImageType, itk::Image <unsigned short,3> >();
        }
        else if ( id == "itkDataImageInt3" )
        {
            res = runProcess< ImageType, itk::Image <int,3> >();
        }
        else if ( id == "itkDataImageUInt3" )
        {
            res = runProcess< ImageType, itk::Image <unsigned int,3> >();
        }
        else if ( id == "itkDataImageLong3" )
        {
            res = runProcess< ImageType, itk::Image <long,3> >();
        }
        else if ( id== "itkDataImageULong3" )
        {
            res = runProcess< ImageType, itk::Image <unsigned long,3> >();
        }
        else if ( id == "itkDataImageFloat3" )
        {
            res = runProcess< ImageType, itk::Image <float,3> >();
        }
        else if ( id == "itkDataImageDouble3" )
        {
            res = runProcess< ImageType, itk::Image <double,3> >();
        }
        else
        {
            res = medAbstractProcess::PIXEL_TYPE;
        }
    }
    return res;
}

template <class ImageType, class ImageType2> int medBinaryOperatorBase::runProcess()
{
    if ( !m_inputA->data() || !m_inputB->data())
    {
        return DTK_FAILURE;
    }

    typedef itk::Image<unsigned char, 3> ImageTypeOutput;
    ImageTypeOutput::Pointer imageA;
    ImageTypeOutput::Pointer imageB;

    if (m_inputA->identifier() != "itkDataImageUChar3")
    {
        typename ImageType::Pointer imA = dynamic_cast< ImageType*>((itk::Object*)(m_inputA->data()));
        typedef itk::CastImageFilter< ImageType, ImageTypeOutput > CasterType;
        typename CasterType::Pointer caster = CasterType::New();
        caster->SetInput(imA);
        caster->Update();
        imageA = caster->GetOutput();
    }
    else
    {
        imageA = dynamic_cast< ImageTypeOutput*>((itk::Object*)(m_inputA->data()));
    }

    if (m_inputB->identifier() != "itkDataImageUChar3")
    {
        typename ImageType2::Pointer imB = dynamic_cast< ImageType2*>((itk::Object*)(m_inputB->data()));
        typedef itk::CastImageFilter< ImageType2, ImageTypeOutput > CasterType;
        typename CasterType::Pointer caster = CasterType::New();
        caster->SetInput(imB);
        caster->Update();
        imageB = caster->GetOutput();
    }
    else
    {
        imageB = dynamic_cast< ImageTypeOutput*>((itk::Object*)(m_inputB->data()));
    }
    
    if (imageA->GetLargestPossibleRegion().GetSize() != imageB->GetLargestPossibleRegion().GetSize())
    {
        return medAbstractProcess::DATA_SIZE;
    }

    typedef itk::InPlaceImageFilter< ImageTypeOutput, ImageTypeOutput >  FilterType;
    typename FilterType::Pointer filter;
    
    if(description() == "AND")
    {
        typedef itk::AndImageFilter <ImageTypeOutput, ImageTypeOutput, ImageTypeOutput> AndImageFilterType;
        filter = AndImageFilterType::New();
    }

    else if(description() == "OR")
    {
        typedef itk::OrImageFilter <ImageTypeOutput, ImageTypeOutput, ImageTypeOutput> OrImageFilterType;
        filter = OrImageFilterType::New();
    }

    else if(description() == "XOR")
    {
        typedef itk::XorImageFilter <ImageTypeOutput, ImageTypeOutput, ImageTypeOutput> XorImageFilterType;
        filter = XorImageFilterType::New();
    }

    else
    {
        qDebug()<<__FILE__<<":"<<__LINE__<<", Wrong binary operator:"<<description();
        return DTK_FAILURE;
    }

    filter->SetInput(0, imageA);
    filter->SetInput(1, imageB);

    try
    {
        filter->Update();
    }
    catch( itk::ExceptionObject & err )
    {
        std::cerr << "ExceptionObject caught in "<< metaObject()->className() << std::endl;
        std::cerr << err << std::endl;
        return DTK_FAILURE;
    }

    m_output->setData(filter->GetOutput());

    QString derivedDescription = description() + " " + m_inputB->metadata(medMetaDataKeys::SeriesDescription.key());
    medUtilities::setDerivedMetaData(m_output, m_inputA, derivedDescription);

    return DTK_SUCCEED;
}        

medAbstractData * medBinaryOperatorBase::output()
{
    return ( m_output );
}
