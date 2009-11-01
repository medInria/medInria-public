#define ITK_TEMPLATE_CXX 1

#include <itkImage.txx>
#include <itkImageBase.txx>
#include <itkImageRegion.txx>
#include <itkImageRegionIterator.txx>
#include <itkImageRegionConstIterator.txx>
#include <itkImportImageContainer.txx>
#include <itkImageFileWriter.txx>
#include <itkOrientedImage.txx>
#include <itkRGBPixel.txx>
#include <itkPoint.txx>
#include <itkVector.txx>
#include <itkFixedArray.txx>
#include <itkMatrix.txx>

#include "medItkExport.h"

namespace itk
{
  typedef Vector<unsigned char, 3> VectorUC3;
  typedef RGBPixel<unsigned char>  RGBUC;
}


  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, Vector,(double,2),D2)
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, FixedArray,(double,2),D2)
  //template MEDITK_EXPORT class Vector<double, 2>;

  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, Vector,(double,3),D3)
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, FixedArray,(double,3),D3)
  //template MEDITK_EXPORT class Vector<double, 3>;

  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, Vector,(unsigned char,3),UC3)
  //template MEDITK_EXPORT class Vector<unsigned char, 3>;
  
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, Point,(double,2),D2)
  //template MEDITK_EXPORT class Point<double, 2>;

  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, Point,(double,3),D3)
  //template MEDITK_EXPORT class Point<double, 3>;

  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, RGBPixel,(unsigned char),UC)
  //template MEDITK_EXPORT class RGBPixel<unsigned char>;

  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, ImageBase,(3),D3)  
  //template class MEDITK_EXPORT ImageBase<3>;

  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, Image,(double, 3),D3)
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, Image,(float, 3),F3)
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, Image,(long, 3),L3)
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, Image,(unsigned long, 3),ULD3)
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, Image,(int, 3),I3)
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, Image,(unsigned int, 3),UI3)
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, Image,(short, 3),S3)
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, Image,(unsigned short, 3),US3)
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, Image,(char, 3),C3)
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, Image,(unsigned char, 3),UC3)
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, Image,(RGBUC,3),RGBUC3)
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, Image,(VectorUC3,3),VECTORUC3)

  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, ImportImageContainer,(unsigned long, double),D)
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, ImportImageContainer,(unsigned long, float),F)
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, ImportImageContainer,(unsigned long, long),L)
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, ImportImageContainer,(unsigned long, unsigned long),UL)
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, ImportImageContainer,(unsigned long, int),I)
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, ImportImageContainer,(unsigned long, unsigned int),UI)
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, ImportImageContainer,(unsigned long, short),S)
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, ImportImageContainer,(unsigned long, unsigned short),US)
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, ImportImageContainer,(unsigned long, char),C)
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, ImportImageContainer,(unsigned long, unsigned char),UC)
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, ImportImageContainer,(unsigned long, RGBUC),RGBUC)
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, ImportImageContainer,(unsigned long, VectorUC3),VectorUC3)

  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, Matrix,(double, 3, 3),D33)

/*
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
  template MEDITK_EXPORT class Image <RGBPixel<unsigned char>, 3>;
  template MEDITK_EXPORT class Image <Vector<unsigned char, 3>, 3>;
  */
  ITK_EXPORT_TEMPLATE(MEDITK_EXPORT, OrientedImage, (short, 3), S3)

namespace itk
{
  template class MEDITK_EXPORT ImageFileWriter< Image <Vector<unsigned char, 3> , 3> >;
  template class MEDITK_EXPORT ImageFileWriter< Image <RGBPixel<unsigned char> , 3> >;
}
