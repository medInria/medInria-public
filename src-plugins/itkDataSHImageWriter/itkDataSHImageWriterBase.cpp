#include "itkDataSHImageWriterBase.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkImageFileWriter.h>
#include <itkImage.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkVectorImage.h>
#include <itkVector.h>

itkDataSHImageWriterBase::itkDataSHImageWriterBase(void) : dtkAbstractDataWriter()
{
    this->io = 0;
}

itkDataSHImageWriterBase::~itkDataSHImageWriterBase(void)
{
}

QStringList itkDataSHImageWriterBase::handled(void) const
{
    return QStringList() << "itkDataSHImageDouble3"
                         << "itkDataSHImageFloat3";
}

QStringList itkDataSHImageWriterBase::s_handled(void)
{
    return QStringList() << "itkDataSHImageDouble3"
                         << "itkDataSHImageFloat3";
}

bool itkDataSHImageWriterBase::canWrite(const QString& path)
{
    if (this->io.IsNull())
        return false;

    return this->io->CanWriteFile ( path.toAscii().constData() );
}

bool itkDataSHImageWriterBase::write(const QString& path)
{
	 if (!this->data())
		 return false;

	 if (this->io.IsNull())
		 return false;

	if (dtkAbstractData *dtkdata = this->data() ) {

                if(dtkdata->identifier()=="itkDataSHImageFloat3") {

		    float dummy = 0;
		    write(path, dummy);
		}

                else if(dtkdata->identifier()=="itkDataSHImageDouble3") {

		    double dummy = 0;
		    write(path, dummy);
		}

		else {
			qWarning() << "Unrecognized pixel type";
			return false;
		}
	}

	return true;
}

template <class PixelType>
bool itkDataSHImageWriterBase::write(const QString& path, PixelType dummyArgument)
{
    typedef typename itk::VectorImage<PixelType, 3>     SHImageType;

    typedef typename SHImageType::Pointer SHImageTypePointer;
    SHImageTypePointer image = dynamic_cast< SHImageType* >( (itk::Object*)(this->data()->output()) );

    typedef typename itk::ImageFileWriter<SHImageType>::Pointer ImageFileWriterPointer;
    ImageFileWriterPointer myWriter = itk::ImageFileWriter<SHImageType>::New();
    myWriter->SetFileName(path.toAscii().constData());
    myWriter->SetInput(/*mySH*/image);
    try {
        myWriter->Write();
    }
    catch(itk::ExceptionObject &e) {
        qDebug() << e.GetDescription();
        return false;
    }

    return true;
}
