#include <itkPhilipsRECDataImageReader.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkPhilipsRECImageIO.h>

const char itkPhilipsRECDataImageReader::ID[] = "itkPhilipsRECDataImageReader";

itkPhilipsRECDataImageReader::itkPhilipsRECDataImageReader(): itkDataImageReaderBase() {
    this->io = itk::PhilipsRECImageIO::New();
}

itkPhilipsRECDataImageReader::~itkPhilipsRECDataImageReader() { }

QStringList itkPhilipsRECDataImageReader::handled() const {
    return s_handled();
}

QStringList itkPhilipsRECDataImageReader::s_handled() {
    return QStringList() << "itkDataImageChar3" << "itkDataImageChar4" << "itkDataImageUChar3" << "itkDataImageUChar4" << "itkDataImageShort3" << "itkDataImageShort4" << "itkDataImageUShort3" << "itkDataImageUShort4" << "itkDataImageInt3" << "itkDataImageInt4" << "itkDataImageUInt3" << "itkDataImageUInt4" << "itkDataImageLong3" << "itkDataImageLong4" << "itkDataImageULong3" << "itkDataImageULong4" << "itkDataImageFloat3" << "itkDataImageFloat4" << "itkDataImageDouble3" << "itkDataImageDouble4";
}

bool itkPhilipsRECDataImageReader::registered() {
    return dtkAbstractDataFactory::instance()->registerDataReaderType(ID,s_handled(),create);
}

QString itkPhilipsRECDataImageReader::identifier() const {
    return ID;
}

QString itkPhilipsRECDataImageReader::description() const {
    return "Reader for Philips REC images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader* itkPhilipsRECDataImageReader::create() {
    return new itkPhilipsRECDataImageReader;
}
