#ifndef _med_Itk_h_
#define _med_Itk_h_


#include <itkImage.h>
#include <itkImageFileWriter.h>
#include <itkOrientedImage.h>
#include <itkRGBPixel.h>
#include <itkPoint.h>
#include <itkVector.h>

#include "medItkExport.h"

namespace itk
{
  extern template class MEDITK_EXPORT Vector<double, 2>;
  extern template class MEDITK_EXPORT Vector<double, 3>;
  extern template class MEDITK_EXPORT Vector<unsigned char, 3>;

  extern template class MEDITK_EXPORT Point<double, 2>;
  extern template class MEDITK_EXPORT Point<double, 3>;
  extern template class MEDITK_EXPORT Point<double, 4>;

  extern template class MEDITK_EXPORT RGBPixel<unsigned char>;
  
  extern template class MEDITK_EXPORT ImageBase<3>;
  extern template class MEDITK_EXPORT ImageBase<4>;

  extern template class MEDITK_EXPORT Image <double, 3>;
  extern template class MEDITK_EXPORT Image <float, 3>;
  extern template class MEDITK_EXPORT Image <long, 3>;
  extern template class MEDITK_EXPORT Image <unsigned long, 3>;
  extern template class MEDITK_EXPORT Image <int, 3>;
  extern template class MEDITK_EXPORT Image <unsigned int, 3>;
  extern template class MEDITK_EXPORT Image <short, 3>;
  extern template class MEDITK_EXPORT Image <unsigned short, 3>;
  extern template class MEDITK_EXPORT Image <char, 3>;
  extern template class MEDITK_EXPORT Image <unsigned char, 3>;

  extern template class MEDITK_EXPORT Image <double, 4>;
  extern template class MEDITK_EXPORT Image <float, 4>;
  extern template class MEDITK_EXPORT Image <long, 4>;
  extern template class MEDITK_EXPORT Image <unsigned long, 4>;
  extern template class MEDITK_EXPORT Image <int, 4>;
  extern template class MEDITK_EXPORT Image <unsigned int, 4>;
  extern template class MEDITK_EXPORT Image <short, 4>;
  extern template class MEDITK_EXPORT Image <unsigned short, 4>;
  extern template class MEDITK_EXPORT Image <char, 4>;
  extern template class MEDITK_EXPORT Image <unsigned char, 4>;
	
  extern template class MEDITK_EXPORT Image <RGBPixel<unsigned char> , 3>;
  extern template class MEDITK_EXPORT Image <Vector<unsigned char, 3> , 3>;

  extern template class MEDITK_EXPORT Image <RGBPixel<unsigned char> , 4>;
  extern template class MEDITK_EXPORT Image <Vector<unsigned char, 3> , 4>;

  extern template class MEDITK_EXPORT OrientedImage <short, 3>;
  extern template class MEDITK_EXPORT OrientedImage <short, 4>;

  extern template class MEDITK_EXPORT ImageFileWriter< Image <Vector<unsigned char, 3> , 3> >;
  extern template class MEDITK_EXPORT ImageFileWriter< Image <RGBPixel<unsigned char> , 3> >;
	
  extern template class MEDITK_EXPORT ImageFileWriter< Image <Vector<unsigned char, 3> , 4> >;
  extern template class MEDITK_EXPORT ImageFileWriter< Image <RGBPixel<unsigned char> , 4> >;
}

#endif
