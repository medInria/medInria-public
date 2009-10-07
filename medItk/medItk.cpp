#include <itkImage.h>
#include <itkOrientedImage.h>
#include <itkPoint.h>
#include <itkVector.h>

#include "medItkExport.h"

namespace itk
{
  template MEDITK_EXPORT class Vector<double, 2>;
  template MEDITK_EXPORT class Vector<double, 3>;
  
  template MEDITK_EXPORT class Point<double, 2>;
  template MEDITK_EXPORT class Point<double, 3>;

  template MEDITK_EXPORT class ImageBase<3>;

  template MEDITK_EXPORT class Image <double, 3>;
  template MEDITK_EXPORT class Image <float, 3>;
  template MEDITK_EXPORT class Image <long, 3>;
  template MEDITK_EXPORT class Image <unsigned long, 3>;
  template MEDITK_EXPORT class Image <int, 3>;
  template MEDITK_EXPORT class Image <unsigned int, 3>;
  template MEDITK_EXPORT class Image <short, 3>;
  template MEDITK_EXPORT class Image <unsigned short, 3>;
  template MEDITK_EXPORT class Image <char, 3>;
  template MEDITK_EXPORT class Image <unsigned char, 3>;
  
  template MEDITK_EXPORT class OrientedImage <short, 3>;  
}
