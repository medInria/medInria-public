#include "itkDataImageWriterBase.h"

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkLog.h>

#include <itkImage.h>
#include <itkImageFileWriter.h>
#include <itkRGBPixel.h>

itkDataImageWriterBase::itkDataImageWriterBase()
{
    this->io = 0;
}

itkDataImageWriterBase::~itkDataImageWriterBase()
{
}

QStringList itkDataImageWriterBase::handled(void) const
{
    return QStringList() << "itkDataImageDouble3"
			 << "itkDataImageFloat3"
			 << "itkDataImageFloat4"
	<< "itkDataImageULong3"
			 << "itkDataImageLong3"
			 << "itkDataImageUInt3"
			 << "itkDataImageInt3"
			 << "itkDataImageUShort3"
			 << "itkDataImageUShort4"
			 << "itkDataImageShort3"
			 << "itkDataImageShort4"	
			 << "itkDataImageUChar3"
			 << "itkDataImageChar3"
			 << "itkDataImageRGB3";
}

QStringList itkDataImageWriterBase::s_handled(void)
{
    return QStringList() << "itkDataImageDouble3"
			 << "itkDataImageFloat3"
			 << "itkDataImageFloat4"	
			 << "itkDataImageULong3"
			 << "itkDataImageLong3"
			 << "itkDataImageUInt3"
			 << "itkDataImageInt3"
			 << "itkDataImageUShort3"
			 << "itkDataImageUShort4"
			 << "itkDataImageShort3"
			 << "itkDataImageShort4"	
			 << "itkDataImageUChar3"
			 << "itkDataImageChar3"
			 << "itkDataImageRGB3";
}

bool itkDataImageWriterBase::canWrite(const QString& path)
{
    if (this->io.IsNull())
        return false;

    return this->io->CanWriteFile ( path.toAscii().constData() );
}

bool itkDataImageWriterBase::write(const QString& path)
{
    if (!this->data())
        return false;

    if (this->io.IsNull())
        return false;

    if (dtkAbstractData *dtkdata = this->data() ) {

        if(dtkdata->description()=="itkDataImageUChar3") {
	  itk::Image<unsigned char, 3>::Pointer image = dynamic_cast< itk::Image<unsigned char, 3>* >( (itk::Object*)(this->data()->output()) );
	  if (image.IsNull())
	      return false;
	  itk::ImageFileWriter < itk::Image<unsigned char, 3> >::Pointer writer = itk::ImageFileWriter < itk::Image<unsigned char, 3> >::New();
	  writer->SetImageIO ( this->io );
	  writer->SetFileName ( path.toAscii().constData() );
	  writer->SetInput ( image );
	  try {
	    writer->Update();
	  }
	  catch(itk::ExceptionObject &e) {
	    qDebug() << e.GetDescription();
	    return false;
	  }
	}

	else if(dtkdata->description()=="itkDataImageChar3") {
	    itk::Image<char, 3>::Pointer image = dynamic_cast< itk::Image<char, 3>* >( (itk::Object*)(this->data()->output()) );
	    if (image.IsNull())
	        return false;
	    itk::ImageFileWriter < itk::Image<char, 3> >::Pointer writer = itk::ImageFileWriter < itk::Image<char, 3> >::New();
	    writer->SetImageIO ( this->io );
	    writer->SetFileName ( path.toAscii().constData() );
	    writer->SetInput ( image );
	    try {
	        writer->Update();
	    }
	    catch(itk::ExceptionObject &e) {
	        qDebug() << e.GetDescription();
	        return false;
	    }
	}

	else if(dtkdata->description()=="itkDataImageUShort3") {
	    itk::Image<unsigned short, 3>::Pointer image = dynamic_cast< itk::Image<unsigned short, 3>* >( (itk::Object*)(this->data()->output()) );
	    if (image.IsNull())
	        return false;
	    itk::ImageFileWriter < itk::Image<unsigned short, 3> >::Pointer writer = itk::ImageFileWriter < itk::Image<unsigned short, 3> >::New();
	    writer->SetImageIO ( this->io );
	    writer->SetFileName ( path.toAscii().constData() );
	    writer->SetInput ( image );
	    try {
	        writer->Update();
	    }
	    catch(itk::ExceptionObject &e) {
	        qDebug() << e.GetDescription();
	        return false;
	    }
	}


	else if(dtkdata->description()=="itkDataImageUShort4") {
	    itk::Image<unsigned short, 4>::Pointer image = dynamic_cast< itk::Image<unsigned short, 4>* >( (itk::Object*)(this->data()->output()) );
	    if (image.IsNull())
	        return false;
	    itk::ImageFileWriter < itk::Image<unsigned short, 4> >::Pointer writer = itk::ImageFileWriter < itk::Image<unsigned short, 4> >::New();
	    writer->SetImageIO ( this->io );
	    writer->SetFileName ( path.toAscii().constData() );
	    writer->SetInput ( image );
	    try {
	        writer->Update();
	    }
	    catch(itk::ExceptionObject &e) {
	        qDebug() << e.GetDescription();
	        return false;
	    }
	}

	else if(dtkdata->description()=="itkDataImageShort3") {
	    itk::Image<short, 3>::Pointer image = dynamic_cast< itk::Image<short, 3>* >( (itk::Object*)(this->data()->output()) );
	    if (image.IsNull())
	        return false;
	    itk::ImageFileWriter < itk::Image<short, 3> >::Pointer writer = itk::ImageFileWriter < itk::Image<short, 3> >::New();
	    writer->SetImageIO ( this->io );
	    writer->SetFileName ( path.toAscii().constData() );
	    writer->SetInput ( image );
	    try {
	        writer->Update();
	    }
	    catch(itk::ExceptionObject &e) {
	        qDebug() << e.GetDescription();
	        return false;
	    }
	}

	else if(dtkdata->description()=="itkDataImageShort4") {
	    itk::Image<short, 4>::Pointer image = dynamic_cast< itk::Image<short, 4>* >( (itk::Object*)(this->data()->output()) );
	    if (image.IsNull())
	        return false;
	    itk::ImageFileWriter < itk::Image<short, 4> >::Pointer writer = itk::ImageFileWriter < itk::Image<short, 4> >::New();
	    writer->SetImageIO ( this->io );
	    writer->SetFileName ( path.toAscii().constData() );
	    writer->SetInput ( image );
	    try {
	        writer->Update();
	    }
	    catch(itk::ExceptionObject &e) {
	        qDebug() << e.GetDescription();
	        return false;
	    }
	}

	else if(dtkdata->description()=="itkDataImageUInt3") {
	    itk::Image<unsigned int, 3>::Pointer image = dynamic_cast< itk::Image<unsigned int, 3>* >( (itk::Object*)(this->data()->output()) );
	    if (image.IsNull())
	        return false;
	    itk::ImageFileWriter < itk::Image<unsigned int, 3> >::Pointer writer = itk::ImageFileWriter < itk::Image<unsigned int, 3> >::New();
	    writer->SetImageIO ( this->io );
	    writer->SetFileName ( path.toAscii().constData() );
	    writer->SetInput ( image );
	    try {
	        writer->Update();
	    }
	    catch(itk::ExceptionObject &e) {
	        qDebug() << e.GetDescription();
	        return false;
	    }
	}

	else if(dtkdata->description()=="itkDataImageInt3") {
	    itk::Image<int, 3>::Pointer image = dynamic_cast< itk::Image<int, 3>* >( (itk::Object*)(this->data()->output()) );
	    if (image.IsNull())
	        return false;
	    itk::ImageFileWriter < itk::Image<int, 3> >::Pointer writer = itk::ImageFileWriter < itk::Image<int, 3> >::New();
	    writer->SetImageIO ( this->io );
	    writer->SetFileName ( path.toAscii().constData() );
	    writer->SetInput ( image );
	    try {
	        writer->Update();
	    }
	    catch(itk::ExceptionObject &e) {
	        qDebug() << e.GetDescription();
	        return false;
	    }
	}

	else if(dtkdata->description()=="itkDataImageULong3") {
	    itk::Image<unsigned long, 3>::Pointer image = dynamic_cast< itk::Image<unsigned long, 3>* >( (itk::Object*)(this->data()->output()) );
	    if (image.IsNull())
	        return false;
	    itk::ImageFileWriter < itk::Image<unsigned long, 3> >::Pointer writer = itk::ImageFileWriter < itk::Image<unsigned long, 3> >::New();
	    writer->SetImageIO ( this->io );
	    writer->SetFileName ( path.toAscii().constData() );
	    writer->SetInput ( image );
	    try {
	        writer->Update();
	    }
	    catch(itk::ExceptionObject &e) {
	        qDebug() << e.GetDescription();
	        return false;
	    }
	}

	else if(dtkdata->description()=="itkDataImageLong3") {
	    itk::Image<long, 3>::Pointer image = dynamic_cast< itk::Image<long, 3>* >( (itk::Object*)(this->data()->output()) );
	    if (image.IsNull())
	        return false;
	    itk::ImageFileWriter < itk::Image<long, 3> >::Pointer writer = itk::ImageFileWriter < itk::Image<long, 3> >::New();
	    writer->SetImageIO ( this->io );
	    writer->SetFileName ( path.toAscii().constData() );
	    writer->SetInput ( image );
	    try {
	        writer->Update();
	    }
	    catch(itk::ExceptionObject &e) {
	        qDebug() << e.GetDescription();
	        return false;
	    }
	}

	else if(dtkdata->description()=="itkDataImageFloat3") {
	    itk::Image<float, 3>::Pointer image = dynamic_cast< itk::Image<float, 3>* >( (itk::Object*)(this->data()->output()) );
	    if (image.IsNull())
	        return false;
	    itk::ImageFileWriter < itk::Image<float, 3> >::Pointer writer = itk::ImageFileWriter < itk::Image<float, 3> >::New();
	    writer->SetImageIO ( this->io );
	    writer->SetFileName ( path.toAscii().constData() );
	    writer->SetInput ( image );
	    try {
	        writer->Update();
	    }
	    catch(itk::ExceptionObject &e) {
	        qDebug() << e.GetDescription();
	        return false;
	    }
	}
	
	else if(dtkdata->description()=="itkDataImageFloat4") {
	    itk::Image<float, 4>::Pointer image = dynamic_cast< itk::Image<float, 4>* >( (itk::Object*)(this->data()->output()) );
	    if (image.IsNull())
	        return false;
	    itk::ImageFileWriter < itk::Image<float, 4> >::Pointer writer = itk::ImageFileWriter < itk::Image<float, 4> >::New();
	    writer->SetImageIO ( this->io );
	    writer->SetFileName ( path.toAscii().constData() );
	    writer->SetInput ( image );
	    try {
	        writer->Update();
	    }
	    catch(itk::ExceptionObject &e) {
	        qDebug() << e.GetDescription();
	        return false;
	    }
	}

	else if(dtkdata->description()=="itkDataImageDouble3") {
	    itk::Image<double, 3>::Pointer image = dynamic_cast< itk::Image<double, 3>* >( (itk::Object*)(this->data()->output()) );
	    if (image.IsNull())
	        return false;
	    itk::ImageFileWriter < itk::Image<double, 3> >::Pointer writer = itk::ImageFileWriter < itk::Image<double, 3> >::New();
	    writer->SetImageIO ( this->io );
	    writer->SetFileName ( path.toAscii().constData() );
	    writer->SetInput ( image );
	    try {
	        writer->Update();
	    }
	    catch(itk::ExceptionObject &e) {
	        qDebug() << e.GetDescription();
	        return false;
	    }
	}

	else if(dtkdata->description()=="itkDataImageRGB3") {
	    itk::Image< itk::RGBPixel<unsigned char>, 3>::Pointer image = dynamic_cast< itk::Image< itk::RGBPixel<unsigned char>, 3>* >( (itk::Object*)(this->data()->output()) );
	    if (image.IsNull())
	        return false;
	    itk::ImageFileWriter < itk::Image< itk::RGBPixel<unsigned char>, 3> >::Pointer writer = itk::ImageFileWriter < itk::Image< itk::RGBPixel<unsigned char>, 3> >::New();
	    writer->SetImageIO ( this->io );
	    writer->SetFileName ( path.toAscii().constData() );
	    writer->SetInput ( image );
	    try {
	        writer->Update();
	    }
	    catch(itk::ExceptionObject &e) {
	        qDebug() << e.GetDescription();
	        return false;
	    }
	}

         else {
	     qWarning() << "Unrecognized pixel type";
	     return false;
	 }
      
    }
    
    
    return true;
}
