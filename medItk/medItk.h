#ifndef _dtk_Itk_h_
#define _dtk_Itk_h_


#include <itkImage.h>
#include <itkImageFileWriter.h>
#include <itkOrientedImage.h>
#include <itkRGBPixel.h>
#include <itkPoint.h>
#include <itkVector.h>

#include "medItkExport.h"

namespace itk
{
  extern template MEDITK_EXPORT class Vector<double, 2>;
  extern template MEDITK_EXPORT class Vector<double, 3>;
  extern template MEDITK_EXPORT class Vector<unsigned char, 3>;

  extern template MEDITK_EXPORT class Point<double, 2>;
  extern template MEDITK_EXPORT class Point<double, 3>;

  extern template MEDITK_EXPORT class RGBPixel<unsigned char>;
  
  extern template MEDITK_EXPORT class ImageBase<3>;

  extern template MEDITK_EXPORT class Image <double, 3>;
  extern template MEDITK_EXPORT class Image <float, 3>;
  extern template MEDITK_EXPORT class Image <long, 3>;
  extern template MEDITK_EXPORT class Image <unsigned long, 3>;
  extern template MEDITK_EXPORT class Image <int, 3>;
  extern template MEDITK_EXPORT class Image <unsigned int, 3>;
  extern template MEDITK_EXPORT class Image <short, 3>;
  extern template MEDITK_EXPORT class Image <unsigned short, 3>;
  extern template MEDITK_EXPORT class Image <char, 3>;
  extern template MEDITK_EXPORT class Image <unsigned char, 3>;
  
  extern template MEDITK_EXPORT class Image <RGBPixel<unsigned char> , 3>;
  extern template MEDITK_EXPORT class Image <Vector<unsigned char, 3> , 3>;
  
  extern template MEDITK_EXPORT class OrientedImage <short, 3>;

  extern template MEDITK_EXPORT class ImageFileWriter< Image <Vector<unsigned char, 3> , 3> >;
  extern template MEDITK_EXPORT class ImageFileWriter< Image <RGBPixel<unsigned char> , 3> >;
}

#endif
