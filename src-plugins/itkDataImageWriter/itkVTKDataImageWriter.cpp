#include <itkVTKDataImageWriter.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkVTKImageIO.h>

const char itkVTKDataImageWriter::ID[] = "itkVTKDataImageWriter";

itkVTKDataImageWriter::itkVTKDataImageWriter(): itkDataImageWriterBase() {
    this->io = itk::VTKImageIO::New();
}

itkVTKDataImageWriter::~itkVTKDataImageWriter() { }

QStringList itkVTKDataImageWriter::handled() const {
    return s_handled();
}

QStringList itkVTKDataImageWriter::s_handled() {
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
            << "itkDataImageRGB3" << "itkDataImageRGBA3";
}

bool itkVTKDataImageWriter::registered() {
    return dtkAbstractDataFactory::instance()->registerDataWriterType(ID,s_handled(),createItkVTKDataImageWriter);
}

QString itkVTKDataImageWriter::identifier() const {
    return ID;
}

QString itkVTKDataImageWriter::description() const {
    return "Writer VTK images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataWriter *createItkVTKDataImageWriter() {
    return new itkVTKDataImageWriter;
}

