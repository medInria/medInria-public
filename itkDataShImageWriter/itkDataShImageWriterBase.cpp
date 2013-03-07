#include "itkDataShImageWriterBase.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkImageFileWriter.h>
#include <itkImage.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkVectorImage.h>
#include <itkVector.h>

itkDataShImageWriterBase::itkDataShImageWriterBase(void) : dtkAbstractDataWriter()
{
    this->io = 0;
}

itkDataShImageWriterBase::~itkDataShImageWriterBase(void)
{
}

QStringList itkDataShImageWriterBase::handled(void) const
{
    return QStringList() << "itkDataShImageDouble3"
                         << "itkDataShImageFloat3";
}

QStringList itkDataShImageWriterBase::s_handled(void)
{
    return QStringList() << "itkDataShImageDouble3"
                         << "itkDataShImageFloat3";
}

bool itkDataShImageWriterBase::canWrite(const QString& path)
{
    if (this->io.IsNull())
        return false;

    return this->io->CanWriteFile ( path.toAscii().constData() );
}

bool itkDataShImageWriterBase::write(const QString& path)
{
	 if (!this->data())
		 return false;

	 if (this->io.IsNull())
		 return false;

	if (dtkAbstractData *dtkdata = this->data() ) {

                if(dtkdata->identifier()=="itkDataShImageFloat3") {

		    float dummy = 0;
		    write(path, dummy);
		}

                else if(dtkdata->identifier()=="itkDataShImageDouble3") {

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
bool itkDataShImageWriterBase::write(const QString& path, PixelType dummyArgument)
{
    typedef typename itk::VectorImage<PixelType, 3>     ShImageType;

    typedef typename ShImageType::Pointer ShImageTypePointer;
    ShImageTypePointer image = dynamic_cast< ShImageType* >( (itk::Object*)(this->data()->output()) );

    typedef typename itk::ImageFileWriter<ShImageType>::Pointer ImageFileWriterPointer;
    ImageFileWriterPointer myWriter = itk::ImageFileWriter<ShImageType>::New();
    myWriter->SetFileName(path.toAscii().constData());
    myWriter->SetInput(/*mySh*/image);
    try {
        myWriter->Write();
    }
    catch(itk::ExceptionObject &e) {
        qDebug() << e.GetDescription();
        return false;
    }

    return true;
}
