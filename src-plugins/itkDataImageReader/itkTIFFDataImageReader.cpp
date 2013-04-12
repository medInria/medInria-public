#include <itkTIFFDataImageReader.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkTIFFImageIO.h>

const char itkTIFFDataImageReader::ID[] = "itkTIFFDataImageReader";

itkTIFFDataImageReader::itkTIFFDataImageReader(): itkDataImageReaderBase() {
    this->io = itk::TIFFImageIO::New();
}

itkTIFFDataImageReader::~itkTIFFDataImageReader() { }

QStringList itkTIFFDataImageReader::handled() const {
    return s_handled();
}

QStringList itkTIFFDataImageReader::s_handled() {
    return QStringList()  << "itkDataImageChar3" << "itkDataImageChar4"
            << "itkDataImageUChar3" << "itkDataImageUChar4"
            << "itkDataImageShort3" << "itkDataImageShort4"
            << "itkDataImageUShort3" << "itkDataImageUShort4"
            << "itkDataImageInt3" << "itkDataImageInt4"
            << "itkDataImageUInt3" << "itkDataImageUInt4"
            << "itkDataImageLong3" << "itkDataImageLong4"
            << "itkDataImageULong3" << "itkDataImageULong4"
            << "itkDataImageFloat3" << "itkDataImageFloat4"
            << "itkDataImageDouble3" << "itkDataImageDouble4"
            << "itkDataImageRGB3" << "itkDataImageRGB3";
}

bool itkTIFFDataImageReader::registered() {
    return dtkAbstractDataFactory::instance()->registerDataReaderType(ID,s_handled(),createItkTIFFDataImageReader);
}

QString itkTIFFDataImageReader::identifier() const {
    return ID;
}

QString itkTIFFDataImageReader::description() const {
    return "Reader for TIFF images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkTIFFDataImageReader() {
    return new itkTIFFDataImageReader;
}

