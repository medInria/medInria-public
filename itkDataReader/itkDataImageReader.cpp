#include "itkDataImageReader.h"

#include "medItk.h"

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkLog.h>

#include <itkImage.h>
#include <itkImageFileReader.h>

#include "medItkDataImageReaderMacros.h"

medImplementItkDataImageReader (char,           3, Char3);
medImplementItkDataImageReader (unsigned char,  3, UChar3);
medImplementItkDataImageReader (short,          3, Short3);
medImplementItkDataImageReader (unsigned short, 3, UShort3);
medImplementItkDataImageReader (int,            3, Int3);
medImplementItkDataImageReader (unsigned int,   3, UInt3);
medImplementItkDataImageReader (long,           3, Long3);
medImplementItkDataImageReader (unsigned,       3, ULong3);
medImplementItkDataImageReader (float,          3, Float3);
medImplementItkDataImageReader (double,         3, Double3);





/*
class itkDataImageShort3ReaderPrivate
{
public:
};


itkDataImageShort3Reader::itkDataImageShort3Reader(void) : dtkAbstractDataReader(), d (new itkDataImageShort3ReaderPrivate)
{
}


itkDataImageShort3Reader::~itkDataImageShort3Reader(void)
{
}


bool itkDataImageShort3Reader::registered(void)
{
  return dtkAbstractDataFactory::instance()->registerDataReaderType("itkDataImageShort3Reader", QStringList() << "itkDataImageShort3", createItkDataImageShort3Reader);
}


QString itkDataImageShort3Reader::description(void) const
{
  return "itkDataImageShort3Reader";
}


QStringList itkDataImageShort3Reader::handled(void) const
{
  return QStringList() << "itkDataImageShort3";
}


bool itkDataImageShort3Reader::read(QString path)
{
  qDebug() << "itkDataImageShort3Reader::read()";
  
  typedef short        PixelType;
  const unsigned int   Dimension = 3;
  
  typedef itk::Image<PixelType, Dimension> ImageType;
  
  typedef itk::ImageFileReader<ImageType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  
  reader->SetFileName (path.toAscii().constData());
  try{
    reader->Update();
  }
  catch(itk::ExceptionObject &ex) {
    std::cerr << ex << std::endl;
    return false;
  }
  
  std::cout << "Was able to read, with ITK IO: " << reader->GetImageIO()->GetNameOfClass() << std::endl;
  
  if( dtkAbstractData *dtkdata = this->data() )
    dtkdata->setData( reader->GetOutput() );
  
  return true;
}


dtkAbstractDataReader *createItkDataImageShort3Reader(void)
{
    return new itkDataImageShort3Reader;
}
*/
