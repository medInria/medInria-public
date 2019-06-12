
#include "medBinaryOperatorBase.h"

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <itkAndImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkOrImageFilter.h>
#include <itkXorImageFilter.h>

#include <medAbstractDataFactory.h>
#include <medMetaDataKeys.h>
#include <medUtilities.h>

template <class ImageType> itk::Image<unsigned char, 3>::Pointer castToUChar3(medAbstractData* input)
{
    typedef itk::CastImageFilter< ImageType, itk::Image<unsigned char, 3> > CasterType;
    typename CasterType::Pointer caster = CasterType::New();
    caster->SetInput(dynamic_cast< ImageType*>((itk::Object*)(input->data())));
    caster->Update();
    typename itk::Image<unsigned char, 3>::Pointer output = caster->GetOutput();
    return output;
}


medBinaryOperatorBase::medBinaryOperatorBase() : dtkAbstractProcess()
{
    m_inputA = nullptr;
    m_inputB = nullptr;
    m_output = nullptr;
}

medBinaryOperatorBase::~medBinaryOperatorBase()
{
    m_inputA = nullptr;
    m_inputB = nullptr;
    m_output = nullptr;
}

void medBinaryOperatorBase::setInput(medAbstractData *data, int channel)
{
    if (channel == 0)
    {
        m_inputA = data;
    }

    if (channel == 1)
    {        
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
        //TODO: this is used in music to display dedicated error messages
        //to users, according to the error type. Here, the pixel type is wrong.
        //else
        //{
            //res = medAbstractProcess::PIXEL_TYPE;
        //}
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
        //TODO: this is used in music to display dedicated error messages
        //to users, according to the error type. Here, the pixel type is wrong.
//        else
//        {
//            res = medAbstractProcess::PIXEL_TYPE;
//        }
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
        imageA = castToUChar3<ImageType>(m_inputA);
    }
    else
    {
        imageA = dynamic_cast< ImageTypeOutput*>((itk::Object*)(m_inputA->data()));
    }

    if (m_inputB->identifier() != "itkDataImageUChar3")
    {
        imageB = castToUChar3<ImageType2>(m_inputB);
    }
    else
    {
        imageB = dynamic_cast< ImageTypeOutput*>((itk::Object*)(m_inputB->data()));
    }
    
    if (imageA->GetLargestPossibleRegion().GetSize() != imageB->GetLargestPossibleRegion().GetSize())
    {
        //TODO music3 error messages
        //return medAbstractProcess::DATA_SIZE;
        return DTK_FAILURE;
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
        assert(false);
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

    m_output = medAbstractDataFactory::instance()->createSmartPointer ( "itkDataImageUChar3" );
    m_output->setData(filter->GetOutput());

    QString derivedDescription = description() + " " + m_inputB->metadata(medMetaDataKeys::SeriesDescription.key());
    medUtilities::setDerivedMetaData(m_output, m_inputA, derivedDescription);

    return DTK_SUCCEED;
}

medAbstractData * medBinaryOperatorBase::output()
{
    return m_output;
}
