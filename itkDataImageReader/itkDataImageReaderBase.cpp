#include "medItk.h"

#include "itkDataImageReaderBase.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkImageFileReader.h>
#include <itkRGBPixel.h>
#include <itkObjectFactoryBase.h>
//#include "itkDataImageReaderCommand.h"


itkDataImageReaderBase::itkDataImageReaderBase(void) : dtkAbstractDataReader()
{
	this->io = 0;
}


itkDataImageReaderBase::~itkDataImageReaderBase(void)
{
}



bool itkDataImageReaderBase::canRead (QString path)
{
	if (!this->io.IsNull())
		return this->io->CanReadFile ( path.toAscii().constData() );
	return false;
}


void itkDataImageReaderBase::readInformation (QString path)
{
	if (this->io.IsNull())
		return;
	
    this->io->SetFileName ( path.toAscii().constData() );
    try {
		this->io->ReadImageInformation();
    }
    catch (itk::ExceptionObject &e) {
		qDebug() << e.GetDescription();
		return;
    }
    
	
    dtkAbstractData* dtkdata = this->data();
    
    
    if (!dtkdata) {
		
		if (this->io->GetPixelType() == itk::ImageIOBase::SCALAR ) {
			
			switch (this->io->GetComponentType()) {
					
				case itk::ImageIOBase::UCHAR:
					dtkdata = dtkAbstractDataFactory::instance()->create ("itkDataImageUChar3");
					if (dtkdata)
						this->setData ( dtkdata );
					break;
					
				case itk::ImageIOBase::CHAR:
					dtkdata = dtkAbstractDataFactory::instance()->create ("itkDataImageChar3");
					if (dtkdata)
						this->setData ( dtkdata );
					break;
					
				case itk::ImageIOBase::USHORT:
				{
					dtkdata = dtkAbstractDataFactory::instance()->create ("itkDataImageUShort3");
					if (dtkdata) 
						this->setData ( dtkdata );
					break;
				}
					
				case itk::ImageIOBase::SHORT:
				{
					dtkdata = dtkAbstractDataFactory::instance()->create ("itkDataImageShort3");
					if (dtkdata)
						this->setData ( dtkdata );
				}
					break;
					
				case itk::ImageIOBase::UINT:
					dtkdata = dtkAbstractDataFactory::instance()->create ("itkDataImageUInt3");
					if (dtkdata)
						this->setData ( dtkdata );
					break;
					
				case itk::ImageIOBase::INT:
					dtkdata = dtkAbstractDataFactory::instance()->create ("itkDataImageInt3");
					if (dtkdata)
						this->setData ( dtkdata );
					break;
					
				case itk::ImageIOBase::ULONG:
					dtkdata = dtkAbstractDataFactory::instance()->create ("itkDataImageULong3");
					if (dtkdata)
						this->setData ( dtkdata );
					break;
					
				case itk::ImageIOBase::LONG:
					dtkdata = dtkAbstractDataFactory::instance()->create ("itkDataImageLong3");
					if (dtkdata)
						this->setData ( dtkdata );
					break;
					
				case itk::ImageIOBase::FLOAT:
					dtkdata = dtkAbstractDataFactory::instance()->create ("itkDataImageFloat3");
					if (dtkdata)
						this->setData ( dtkdata );
					break;
					
				case itk::ImageIOBase::DOUBLE:
					dtkdata = dtkAbstractDataFactory::instance()->create ("itkDataImageDouble3");
					if (dtkdata)
						this->setData ( dtkdata );
					break;
					
				default:
					qDebug() << "Unrecognized component type";
					return;
			}
			
		}
		else if ( this->io->GetPixelType()==itk::ImageIOBase::RGB ) {
			
			switch (this->io->GetComponentType()) {
					
				case itk::ImageIOBase::UCHAR:
					dtkdata = dtkAbstractDataFactory::instance()->create ("itkDataImageRGB3");
					
					if (dtkdata)
						this->setData ( dtkdata );
					break;
					
				default:
					qDebug() << "Unrecognized component type";
					return;
			}
		}
		else {
			qDebug() << "Unsupported pixel type";
			return;
		}
    }
	
	
    if (dtkdata) {
		/*
		QStringList patientName;
		QStringList studyName;
		QStringList seriesName;
		QStringList filePaths;
		
		patientName << this->io->GetPatientName().c_str();
		studyName << this->io->GetStudyDescription().c_str();
		seriesName << this->io->GetSeriesDescription().c_str();
		
		for (unsigned int i=0; i<this->io->GetOrderedFileNames().size(); i++ )
			filePaths << this->io->GetOrderedFileNames()[i].c_str();
		
		if (!dtkdata->hasMetaData ( tr ("PatientName") ))
			dtkdata->addMetaData ( "PatientName", patientName );
		else
			dtkdata->setMetaData ( "PatientName", patientName );
		
		if (!dtkdata->hasMetaData ( tr ("StudyDescription") ))
			dtkdata->addMetaData ( "StudyDescription", studyName );
		else
			dtkdata->setMetaData ( "StudyDescription", studyName );
		
		if (!dtkdata->hasMetaData ( tr ("SeriesDescription") ))
			dtkdata->addMetaData ( "SeriesDescription", seriesName );
		else
			dtkdata->setMetaData ( "SeriesDescription", seriesName );
		*/
		dtkdata->addMetaData ("FilePath", QStringList() << path);
		
    }
}


bool itkDataImageReaderBase::read (QString path)
{
	if (this->io.IsNull())
		return false;
	
	this->readInformation ( path );
	
	if (this->io->GetNumberOfDimensions() != 3) {
		qDebug() << "Only 3D images are supported for now (required: " << this->io->GetNumberOfDimensions() << ")";
		return false;
	}
	
	qDebug() << "Can read with: " << this->description();
/*	
	itk::DataImageReaderCommand::Pointer command = itk::DataImageReaderCommand::New();
	command->SetDCMTKDataImageReader ( this );
	this->io->AddObserver ( itk::ProgressEvent(), command);
*/	
	
	if (dtkAbstractData *dtkdata = this->data() ) {
		
		if (dtkdata->description()=="itkDataImageUChar3") {
			itk::ImageFileReader< itk::Image<unsigned char, 3> >::Pointer ucharReader = itk::ImageFileReader< itk::Image<unsigned char, 3> >::New();
			ucharReader->SetImageIO ( this->io );
			ucharReader->SetFileName ( path.toAscii().constData() );
			if( dtkAbstractData* dtkdata = this->data() )
				dtkdata->setData ( ucharReader->GetOutput() );
			try {
				ucharReader->Update();
			}
			catch (itk::ExceptionObject &e) {
				qDebug() << e.GetDescription();
				return false;
			}
		}
		
		else if (dtkdata->description()=="itkDataImageChar3") {
			itk::ImageFileReader< itk::Image<char, 3> >::Pointer charReader = itk::ImageFileReader< itk::Image<char, 3> >::New();
			charReader->SetImageIO ( this->io );
			charReader->SetFileName ( path.toAscii().constData() );
			if( dtkAbstractData* dtkdata = this->data() )
				dtkdata->setData ( charReader->GetOutput() );
			try {
				charReader->Update();
			}
			catch (itk::ExceptionObject &e) {
				qDebug() << e.GetDescription();
				return false;
			}
		}
		
		else if (dtkdata->description()=="itkDataImageUShort3") {
			itk::ImageFileReader< itk::Image<unsigned short, 3> >::Pointer ushortReader = itk::ImageFileReader< itk::Image<unsigned short, 3> >::New();
			ushortReader->SetImageIO ( this->io );
			ushortReader->SetFileName ( path.toAscii().constData() );
			if( dtkAbstractData* dtkdata = this->data() )
				dtkdata->setData ( ushortReader->GetOutput() );
			try {
				ushortReader->Update();
			}
			catch (itk::ExceptionObject &e) {
				qDebug() << e.GetDescription();
				return false;
			}
		}
		
		else if (dtkdata->description()=="itkDataImageShort3") {
			itk::ImageFileReader< itk::Image<short, 3> >::Pointer shortReader = itk::ImageFileReader< itk::Image<short, 3> >::New();
			shortReader->SetImageIO ( this->io );
			shortReader->SetFileName ( path.toAscii().constData() );
			if( dtkAbstractData* dtkdata = this->data() )
				dtkdata->setData ( shortReader->GetOutput() );
			try {
				shortReader->Update();
			}
			catch (itk::ExceptionObject &e) {
				qDebug() << e.GetDescription();
				return false;
			}
		}
		
		else if (dtkdata->description()=="itkDataImageUInt3") {
			itk::ImageFileReader< itk::Image<unsigned int, 3> >::Pointer uintReader = itk::ImageFileReader< itk::Image<unsigned int, 3> >::New();
			uintReader->SetImageIO ( this->io );
			uintReader->SetFileName ( path.toAscii().constData() );
			if( dtkAbstractData* dtkdata = this->data() )
				dtkdata->setData ( uintReader->GetOutput() );
			try {
				uintReader->Update();
			}
			catch (itk::ExceptionObject &e) {
				qDebug() << e.GetDescription();
				return false;
			}
		}
		
		else if (dtkdata->description()=="itkDataImageInt3") {
			itk::ImageFileReader< itk::Image<int, 3> >::Pointer intReader = itk::ImageFileReader< itk::Image<int, 3> >::New();
			intReader->SetImageIO ( this->io );
			intReader->SetFileName ( path.toAscii().constData() );
			if( dtkAbstractData* dtkdata = this->data() )
				dtkdata->setData ( intReader->GetOutput() );
			try {
				intReader->Update();
			}
			catch (itk::ExceptionObject &e) {
				qDebug() << e.GetDescription();
				return false;
			}
		}
		
		else if (dtkdata->description()=="itkDataImageULong3") {
			itk::ImageFileReader< itk::Image<unsigned long, 3> >::Pointer ulongReader = itk::ImageFileReader< itk::Image<unsigned long, 3> >::New();
			ulongReader->SetImageIO ( this->io );
			ulongReader->SetFileName ( path.toAscii().constData() );
			if( dtkAbstractData* dtkdata = this->data() )
				dtkdata->setData ( ulongReader->GetOutput() );
			try {
				ulongReader->Update();
			}
			catch (itk::ExceptionObject &e) {
				qDebug() << e.GetDescription();
				return false;
			}
		}
		
		else if (dtkdata->description()=="itkDataImageLong3") {
			itk::ImageFileReader< itk::Image<long, 3> >::Pointer longReader = itk::ImageFileReader< itk::Image<long, 3> >::New();
			longReader->SetImageIO ( this->io );
			longReader->SetFileName ( path.toAscii().constData() );
			if( dtkAbstractData* dtkdata = this->data() )
				dtkdata->setData ( longReader->GetOutput() );
			try {
				longReader->Update();
			}
			catch (itk::ExceptionObject &e) {
				qDebug() << e.GetDescription();
				return false;
			}
		}
		
		else if (dtkdata->description()=="itkDataImageFloat3") {
			itk::ImageFileReader< itk::Image<float, 3> >::Pointer floatReader = itk::ImageFileReader< itk::Image<float, 3> >::New();
			floatReader->SetImageIO ( this->io );
			floatReader->SetFileName ( path.toAscii().constData() );
			if( dtkAbstractData* dtkdata = this->data() )
				dtkdata->setData ( floatReader->GetOutput() );
			try {
				floatReader->Update();
			}
			catch (itk::ExceptionObject &e) {
				qDebug() << e.GetDescription();
				return false;
			}
		}
		
		else if (dtkdata->description()=="itkDataImageDouble3") {
			itk::ImageFileReader< itk::Image<double, 3> >::Pointer doubleReader = itk::ImageFileReader< itk::Image<double, 3> >::New();
			doubleReader->SetImageIO ( this->io );
			doubleReader->SetFileName ( path.toAscii().constData() );
			if( dtkAbstractData* dtkdata = this->data() )
				dtkdata->setData ( doubleReader->GetOutput() );
			try {
				doubleReader->Update();
			}
			catch (itk::ExceptionObject &e) {
				qDebug() << e.GetDescription();
				return false;
			}
		}
		
		else if (dtkdata->description()=="itkDataImageRGB3") {
			itk::ImageFileReader< itk::Image<itk::RGBPixel<unsigned char>, 3> >::Pointer rgbReader = itk::ImageFileReader< itk::Image<itk::RGBPixel<unsigned char>, 3> >::New();
			rgbReader->SetImageIO ( this->io );
			rgbReader->SetFileName ( path.toAscii().constData() );
			
			if( dtkAbstractData* dtkdata = this->data() )
				dtkdata->setData ( rgbReader->GetOutput() );
			try {
				rgbReader->Update();
			}
			catch (itk::ExceptionObject &e) {
				qDebug() << e.GetDescription();
				return false;
			}
		}
		else {
			qDebug() << "Unrecognized pixel type";
			return false;
		}
		
	}
	/*
	if (this->io->GetPixelType()==itk::ImageIOBase::SCALAR) {
		
		switch (this->io->GetComponentType()) {
				
			case itk::ImageIOBase::UCHAR:
			{
				itk::ImageFileReader< itk::Image<unsigned char, 3> >::Pointer ucharReader = itk::ImageFileReader< itk::Image<unsigned char, 3> >::New();
				ucharReader->SetImageIO ( this->io );
				ucharReader->SetFileName ( path.toAscii().constData() );
				if( dtkAbstractData* dtkdata = this->data() )
					dtkdata->setData ( ucharReader->GetOutput() );
				try {
					ucharReader->Update();
				}
				catch (itk::ExceptionObject &e) {
					qDebug() << e.GetDescription();
					return false;
				}
				break;
			}
			case itk::ImageIOBase::CHAR:
			{
				itk::ImageFileReader< itk::Image<char, 3> >::Pointer charReader = itk::ImageFileReader< itk::Image<char, 3> >::New();
				charReader->SetImageIO ( this->io );
				charReader->SetFileName ( path.toAscii().constData() );
				if( dtkAbstractData* dtkdata = this->data() )
					dtkdata->setData ( charReader->GetOutput() );
				try {
					charReader->Update();
				}
				catch (itk::ExceptionObject &e) {
					qDebug() << e.GetDescription();
					return false;
				}
				break;
			}
			case itk::ImageIOBase::USHORT:
			{
				itk::ImageFileReader< itk::Image<unsigned short, 3> >::Pointer ushortReader = itk::ImageFileReader< itk::Image<unsigned short, 3> >::New();
				ushortReader->SetImageIO ( this->io );
				ushortReader->SetFileName ( path.toAscii().constData() );
				if( dtkAbstractData* dtkdata = this->data() )
					dtkdata->setData ( ushortReader->GetOutput() );
				try {
					ushortReader->Update();
				}
				catch (itk::ExceptionObject &e) {
					qDebug() << e.GetDescription();
					return false;
				}
				break;
			}
			case itk::ImageIOBase::SHORT:
			{
				itk::ImageFileReader< itk::Image<short, 3> >::Pointer shortReader = itk::ImageFileReader< itk::Image<short, 3> >::New();
				shortReader->SetImageIO ( this->io );
				shortReader->SetFileName ( path.toAscii().constData() );
				if( dtkAbstractData* dtkdata = this->data() )
					dtkdata->setData ( shortReader->GetOutput() );
				try {
					shortReader->Update();
				}
				catch (itk::ExceptionObject &e) {
					qDebug() << e.GetDescription();
					return false;
				}
				break;
			}
			case itk::ImageIOBase::UINT:
			{
				itk::ImageFileReader< itk::Image<unsigned int, 3> >::Pointer uintReader = itk::ImageFileReader< itk::Image<unsigned int, 3> >::New();
				uintReader->SetImageIO ( this->io );
				uintReader->SetFileName ( path.toAscii().constData() );
				if( dtkAbstractData* dtkdata = this->data() )
					dtkdata->setData ( uintReader->GetOutput() );
				try {
					uintReader->Update();
				}
				catch (itk::ExceptionObject &e) {
					qDebug() << e.GetDescription();
					return false;
				}
				break;
			}
			case itk::ImageIOBase::INT:
			{
				itk::ImageFileReader< itk::Image<int, 3> >::Pointer intReader = itk::ImageFileReader< itk::Image<int, 3> >::New();
				intReader->SetImageIO ( this->io );
				intReader->SetFileName ( path.toAscii().constData() );
				if( dtkAbstractData* dtkdata = this->data() )
					dtkdata->setData ( intReader->GetOutput() );
				try {
					intReader->Update();
				}
				catch (itk::ExceptionObject &e) {
					qDebug() << e.GetDescription();
					return false;
				}
				break;
			}
			case itk::ImageIOBase::ULONG:
			{
				itk::ImageFileReader< itk::Image<unsigned long, 3> >::Pointer ulongReader = itk::ImageFileReader< itk::Image<unsigned long, 3> >::New();
				ulongReader->SetImageIO ( this->io );
				ulongReader->SetFileName ( path.toAscii().constData() );
				if( dtkAbstractData* dtkdata = this->data() )
					dtkdata->setData ( ulongReader->GetOutput() );
				try {
					ulongReader->Update();
				}
				catch (itk::ExceptionObject &e) {
					qDebug() << e.GetDescription();
					return false;
				}
				break;
			}
			case itk::ImageIOBase::LONG:
			{
				itk::ImageFileReader< itk::Image<long, 3> >::Pointer longReader = itk::ImageFileReader< itk::Image<long, 3> >::New();
				longReader->SetImageIO ( this->io );
				longReader->SetFileName ( path.toAscii().constData() );
				if( dtkAbstractData* dtkdata = this->data() )
					dtkdata->setData ( longReader->GetOutput() );
				try {
					longReader->Update();
				}
				catch (itk::ExceptionObject &e) {
					qDebug() << e.GetDescription();
					return false;
				}
				break;
			}
			case itk::ImageIOBase::FLOAT:
			{
				itk::ImageFileReader< itk::Image<float, 3> >::Pointer floatReader = itk::ImageFileReader< itk::Image<float, 3> >::New();
				floatReader->SetImageIO ( this->io );
				floatReader->SetFileName ( path.toAscii().constData() );
				if( dtkAbstractData* dtkdata = this->data() )
					dtkdata->setData ( floatReader->GetOutput() );
				try {
					floatReader->Update();
				}
				catch (itk::ExceptionObject &e) {
					qDebug() << e.GetDescription();
					return false;
				}
				break;
			}
			case itk::ImageIOBase::DOUBLE:
			{
				itk::ImageFileReader< itk::Image<double, 3> >::Pointer doubleReader = itk::ImageFileReader< itk::Image<double, 3> >::New();
				doubleReader->SetImageIO ( this->io );
				doubleReader->SetFileName ( path.toAscii().constData() );
				if( dtkAbstractData* dtkdata = this->data() )
					dtkdata->setData ( doubleReader->GetOutput() );
				try {
					doubleReader->Update();
				}
				catch (itk::ExceptionObject &e) {
					qDebug() << e.GetDescription();
					return false;
				}
				break;
			}
			default:
			{
				qDebug() << "Unrecognized component type";
				return false;
			}
		}
	}
	else if (this->io->GetPixelType()==itk::ImageIOBase::RGB) {
		
		switch (this->io->GetComponentType()) {
				
			case itk::ImageIOBase::UCHAR:
			{
				itk::ImageFileReader< itk::Image<itk::RGBPixel<unsigned char>, 3> >::Pointer rgbReader = itk::ImageFileReader< itk::Image<itk::RGBPixel<unsigned char>, 3> >::New();
				rgbReader->SetImageIO ( this->io );
				rgbReader->SetFileName ( path.toAscii().constData() );
				
				if( dtkAbstractData* dtkdata = this->data() )
					dtkdata->setData ( rgbReader->GetOutput() );
				try {
					rgbReader->Update();
				}
				catch (itk::ExceptionObject &e) {
					qDebug() << e.GetDescription();
					return false;
				}
				break;
			}
			default:
			{
				qDebug() << "Unrecognized component type";
				return false;
			}
		}
	}
	else {
		qDebug() << "Unrecognized pixel type";
		return false;
	}
	
	*/
	
	//this->io->RemoveAllObservers ();
	
	return true;
	
}


void itkDataImageReaderBase::setProgress (int value)
{
    emit progressed (value);
}

