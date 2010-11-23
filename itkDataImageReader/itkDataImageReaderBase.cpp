#include "itkDataImageReaderBase.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include <itkImageFileReader.h>
#include <itkRGBPixel.h>
#include <itkObjectFactoryBase.h>

itkDataImageReaderBase::itkDataImageReaderBase(void) : dtkAbstractDataReader()
{
  this->io = 0;
}


itkDataImageReaderBase::~itkDataImageReaderBase(void)
{
}


QStringList itkDataImageReaderBase::handled(void) const
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

QStringList itkDataImageReaderBase::s_handled(void)
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

bool itkDataImageReaderBase::canRead (const QString& path)
{
  if (!this->io.IsNull())
    return this->io->CanReadFile ( path.toAscii().constData() );
  return false;
}

bool itkDataImageReaderBase::canRead (const QStringList& paths)
{
  if (!paths.count())
    return false;
  return this->canRead ( paths[0].toAscii().constData() );
}

void itkDataImageReaderBase::readInformation (const QString& path)
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
	      if ( this->io->GetNumberOfDimensions()<=3 )
		dtkdata = dtkAbstractDataFactory::instance()->create ("itkDataImageUShort3");
	      else if ( this->io->GetNumberOfDimensions()==4 )
		dtkdata = dtkAbstractDataFactory::instance()->create ("itkDataImageUShort4");
		    
	      if (dtkdata) 
		this->setData ( dtkdata );
	      break;
	    }
		  
	  case itk::ImageIOBase::SHORT:
	    {
	      if ( this->io->GetNumberOfDimensions()<=3 )
		dtkdata = dtkAbstractDataFactory::instance()->create ("itkDataImageShort3");
	      else if ( this->io->GetNumberOfDimensions()==4 )
		dtkdata = dtkAbstractDataFactory::instance()->create ("itkDataImageShort4");
		        
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
		if ( this->io->GetNumberOfDimensions()<=3 )
	        dtkdata = dtkAbstractDataFactory::instance()->create ("itkDataImageFloat3");
		else if ( this->io->GetNumberOfDimensions()==4 )
			dtkdata = dtkAbstractDataFactory::instance()->create ("itkDataImageFloat4");
			  
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


void itkDataImageReaderBase::readInformation (const QStringList& paths)
{
  if (!paths.count())
    return;
  this->readInformation ( paths[0].toAscii().constData() );
}


bool itkDataImageReaderBase::read (const QString& path)
{
  if (this->io.IsNull())
    return false;

  this->setProgress (0);
	
  this->readInformation ( path );

  this->setProgress (50);
	
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
      ucharReader->SetUseStreaming(true);
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
      charReader->SetUseStreaming(true);
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
      ushortReader->SetUseStreaming(true);
      dtkdata->setData ( ushortReader->GetOutput() );
      try {
	ushortReader->Update();
      }
      catch (itk::ExceptionObject &e) {
	qDebug() << e.GetDescription();
	return false;
      }
    }

    else if (dtkdata->description()=="itkDataImageUShort4") {
      itk::ImageFileReader< itk::Image<unsigned short, 4> >::Pointer ushortReader = itk::ImageFileReader< itk::Image<unsigned short, 4> >::New();
      ushortReader->SetImageIO ( this->io );
      ushortReader->SetFileName ( path.toAscii().constData() );
      ushortReader->SetUseStreaming(true);
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
      shortReader->SetUseStreaming(true);
      dtkdata->setData ( shortReader->GetOutput() );
      try {
	shortReader->Update();
      }
      catch (itk::ExceptionObject &e) {
	qDebug() << e.GetDescription();
	return false;
      }
    }

    else if (dtkdata->description()=="itkDataImageShort4") {
      itk::ImageFileReader< itk::Image<short, 4> >::Pointer shortReader = itk::ImageFileReader< itk::Image<short, 4> >::New();
      shortReader->SetImageIO ( this->io );
      shortReader->SetFileName ( path.toAscii().constData() );
      shortReader->SetUseStreaming(true);
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
      uintReader->SetUseStreaming(true);
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
      intReader->SetUseStreaming(true);
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
      ulongReader->SetUseStreaming(true);
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
      longReader->SetUseStreaming(true);
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
      floatReader->SetUseStreaming(true);
      dtkdata->setData ( floatReader->GetOutput() );
      try {
	floatReader->Update();
      }
      catch (itk::ExceptionObject &e) {
	qDebug() << e.GetDescription();
	return false;
      }
    }
	  
	else if (dtkdata->description()=="itkDataImageFloat4") {
	  itk::ImageFileReader< itk::Image<float, 4> >::Pointer floatReader = itk::ImageFileReader< itk::Image<float, 4> >::New();
		floatReader->SetImageIO ( this->io );
		floatReader->SetFileName ( path.toAscii().constData() );
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
      doubleReader->SetUseStreaming(true);
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
      rgbReader->SetUseStreaming(true);
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
      qWarning() << "Unrecognized pixel type";
      return false;
    }
		
  }

  this->setProgress (100);

  //this->io->RemoveAllObservers ();
	
  return true;
	
}

bool itkDataImageReaderBase::read (const QStringList& paths)
{
  if (!paths.count())
    return false;
  return this->read ( paths[0].toAscii().constData() );
}

void itkDataImageReaderBase::setProgress (int value)
{
  emit progressed (value);
}
