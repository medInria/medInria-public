#pragma once

#include "dtkPixelImpl.h"
#include "itkRGBPixel.h"
#include "itkRGBAPixel.h"

template < typename PixelType> class dtkItkPixelTypeTrait;

template < typename StorageType >
class dtkItkPixelTypeTrait< dtkScalarPixel<StorageType> >
{
public:
    typedef StorageType itkPixelType;
};

template < typename StorageType >
class dtkItkPixelTypeTrait< dtkRGBPixel<StorageType> >
{
public:
    typedef itk::RGBPixel<StorageType> itkPixelType;
};

template < typename StorageType >
class dtkItkPixelTypeTrait< dtkRGBAPixel<StorageType> >
{
public:
    typedef itk::RGBAPixel<StorageType> itkPixelType;
};
