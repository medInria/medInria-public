#include <itkNrrdDataTensorImageReader.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkNrrdImageIO.h>

const char itkNrrdDataTensorImageReader::ID[] = "itkNrrdDataTensorImageReader";

itkNrrdDataTensorImageReader::itkNrrdDataTensorImageReader(): itkDataTensorImageReaderBase() {
    this->io = itk::NrrdImageIO::New();
}

itkNrrdDataTensorImageReader::~itkNrrdDataTensorImageReader() { }

bool itkNrrdDataTensorImageReader::registered() {
  return dtkAbstractDataFactory::instance()->registerDataReaderType(ID, itkDataTensorImageReaderBase::s_handled(),
								    createItkNrrdDataTensorImageReader);
}

QString itkNrrdDataTensorImageReader::identifier() const {
    return ID;
}

QString itkNrrdDataTensorImageReader::description() const {
    return "Reader for Nrrd tensor images";
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractDataReader *createItkNrrdDataTensorImageReader() {
    return new itkNrrdDataTensorImageReader;
}

