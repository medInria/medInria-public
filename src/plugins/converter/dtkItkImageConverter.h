// Version: $Id$
//
//

// Commentary:
//
//

// Change Log:
//
//

// Code:
#pragma once

#include <QObject>


class dtkImage;
#include "itkImage.h"
#include <dtkItkPixelTypeTrait.h>

// /////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////

template < typename Pixel, int dim > class dtkPixelClassDataExtractor
{
public:
	static typename Pixel::PixelType* getData(typename itk::Image<typename dtkItkPixelTypeTrait<Pixel>::itkPixelType,dim>::Pointer image)
	{
        return (typename Pixel::PixelType*)(image->GetBufferPointer()->GetDataPointer());
	}
};

// /////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////

template < typename Pixel, int dim > class dtkPixelRawDataExtractor
{
public:
	static typename Pixel::PixelType* getData(typename itk::Image<typename dtkItkPixelTypeTrait<Pixel>::itkPixelType, dim>::Pointer image)
	{
        return (typename Pixel::PixelType*)(image->GetBufferPointer());
	}
};

// /////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////

template < typename Pixel, int dim > class dtkItkImageConverterHandler
{
    typedef itk::Image<typename dtkItkPixelTypeTrait<Pixel>::itkPixelType, dim> ItkImageType;
public:
    static void convertToNative  (dtkImage* source, typename ItkImageType::Pointer target);
    static void convertFromNative(typename ItkImageType::Pointer source, dtkImage* target);
};

// /////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////

template < typename Pixel, int dim > class dtkItkImageConverter
{
public:
    typedef itk::Image<typename dtkItkPixelTypeTrait<Pixel>::itkPixelType, dim> ItkImageType;

    static void convertToNative  (dtkImage *source, typename ItkImageType::Pointer target);
    static void convertFromNative(typename ItkImageType::Pointer source, dtkImage *target);
};

// /////////////////////////////////////////////////////////////////

typedef itk::Image<typename dtkItkPixelTypeTrait< dtkScalarPixel<unsigned  char> >::itkPixelType, 3>  ItkImageUChar3;
typedef itk::Image<typename dtkItkPixelTypeTrait< dtkScalarPixel<          char> >::itkPixelType, 3>   ItkImageChar3;
typedef itk::Image<typename dtkItkPixelTypeTrait< dtkScalarPixel<unsigned short> >::itkPixelType, 3> ItkImageUShort3;
typedef itk::Image<typename dtkItkPixelTypeTrait< dtkScalarPixel<         short> >::itkPixelType, 3>  ItkImageShort3;
typedef itk::Image<typename dtkItkPixelTypeTrait< dtkScalarPixel<unsigned   int> >::itkPixelType, 3>   ItkImageUInt3;
typedef itk::Image<typename dtkItkPixelTypeTrait< dtkScalarPixel<           int> >::itkPixelType, 3>    ItkImageInt3;
typedef itk::Image<typename dtkItkPixelTypeTrait< dtkScalarPixel<unsigned  long> >::itkPixelType, 3>  ItkImageULong3;
typedef itk::Image<typename dtkItkPixelTypeTrait< dtkScalarPixel<          long> >::itkPixelType, 3>   ItkImageLong3;
typedef itk::Image<typename dtkItkPixelTypeTrait< dtkScalarPixel<         float> >::itkPixelType, 3>  ItkImageFloat3;
typedef itk::Image<typename dtkItkPixelTypeTrait< dtkScalarPixel<        double> >::itkPixelType, 3> ItkImageDouble3;

// /////////////////////////////////////////////////////////////////

#include "dtkItkImageConverter.tpp"

//
