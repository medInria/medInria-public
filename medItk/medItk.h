#ifndef _dtk_Itk_h_
#define _dtk_Itk_h_


#include <itkImage.h>
#include <itkImageFileWriter.h>
#include <itkOrientedImage.h>
#include <itkRGBPixel.h>
#include <itkPoint.h>
#include <itkVector.h>


namespace itk
{
  extern template class Vector<double, 2>;
  extern template class Vector<double, 3>;
  extern template class Vector<unsigned char, 3>;

  extern template class Point<double, 2>;
  extern template class Point<double, 3>;

  extern template class RGBPixel<unsigned char>;
  
  extern template class ImageBase<3>;

  extern template class Image <double, 3>;
  extern template class Image <float, 3>;
  extern template class Image <long, 3>;
  extern template class Image <unsigned long, 3>;
  extern template class Image <int, 3>;
  extern template class Image <unsigned int, 3>;
  extern template class Image <short, 3>;
  extern template class Image <unsigned short, 3>;
  extern template class Image <char, 3>;
  extern template class Image <unsigned char, 3>;
  
  extern template class Image <RGBPixel<unsigned char> , 3>;
  extern template class Image <Vector<unsigned char, 3> , 3>;
  
  extern template class OrientedImage <short, 3>;

  extern template class ImageFileWriter< Image <Vector<unsigned char, 3> , 3> >;
  extern template class ImageFileWriter< Image <RGBPixel<unsigned char> , 3> >;
}

#endif
