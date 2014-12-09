/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTtkTensorImageDataReaderBase.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <itkTensor.h>
#include <itkImageFileReader.h>
#include <itkObjectFactoryBase.h>
#include <itkImageRegionConstIteratorWithIndex.h>
#include <itkImageRegionIteratorWithIndex.h>

// /////////////////////////////////////////////////////////////////
// itkDataTensorImageReader
// /////////////////////////////////////////////////////////////////

medTtkTensorImageDataReaderBase::medTtkTensorImageDataReaderBase() : dtkAbstractDataReader()
{
    this->io = 0;
}

medTtkTensorImageDataReaderBase::~medTtkTensorImageDataReaderBase()
{
}

QStringList medTtkTensorImageDataReaderBase::handled() const
{
    return QStringList() << "medTtkDouble3TensorImageData"
			 << "medTtkFloat3TensorImageData";
}

QStringList medTtkTensorImageDataReaderBase::s_handled()
{
    return QStringList() << "medTtkDouble3TensorImageData"
			 << "medTtkFloat3TensorImageData";
}

bool medTtkTensorImageDataReaderBase::canRead (const QStringList &paths)
{
    if (paths.count())
        return this->canRead (paths[0]);
    return false;
}

bool medTtkTensorImageDataReaderBase::canRead (const QString &path)
{
    if (!this->io.IsNull()) {
        if (!this->io->CanReadFile ( path.toAscii().constData() ))
            return false;

        this->io->SetFileName (path.toAscii().constData());
        try {
            this->io->ReadImageInformation();
        }
        catch (itk::ExceptionObject &e) {
            qDebug() << e.GetDescription();
            return false;
        }

        if (this->io->GetNumberOfComponents()!=6 && this->io->GetNumberOfComponents()!=9)
            return false;

        return true;
    }
    return false;
}

bool medTtkTensorImageDataReaderBase::readInformation (const QStringList &paths)
{
    if (paths.count())
        return this->readInformation (paths[0]);
    
    return false;
}

bool medTtkTensorImageDataReaderBase::readInformation (const QString &path)
{
    if (this->io.IsNull())
        return false;
    
    this->io->SetFileName ( path.toAscii().constData() );
    try {
        this->io->ReadImageInformation();
    }
    catch (itk::ExceptionObject &e) {
        qDebug() << e.GetDescription();
	return false;
    }
    
    medAbstractData *medData = dynamic_cast<medAbstractData*>(this->data());

    if (!medData) {
      
        switch (this->io->GetComponentType()) {

	    case itk::ImageIOBase::FLOAT:
            medData = medAbstractDataFactory::instance()->create ("medTtkFloat3TensorImageData");
		if (medData)
		    this->setData ( medData );
		break;
		  
	    case itk::ImageIOBase::DOUBLE:
            medData = medAbstractDataFactory::instance()->create ("medTtkDouble3TensorImageData");
		if (medData)
		    this->setData ( medData );
		break;
		  
	    default:
	        qDebug() << "Unsupported component type";
		return false;
	}
    }

    if (medData) {
        medData->addMetaData ("FilePath", QStringList() << path);
    }
    
    return true;
}

bool medTtkTensorImageDataReaderBase::read (const QStringList &paths)
{
    if (paths.count())
        return this->read (paths[0]);
    return false;
}

bool medTtkTensorImageDataReaderBase::read (const QString &path)
{
    if (this->io.IsNull())
        return false;
	
    this->readInformation ( path );
	
    qDebug() << "Read with: " << this->identifier();

    if (medAbstractData *medData = dynamic_cast<medAbstractData*>(this->data()) ) {
      
        if (medData->identifier()=="medTtkDouble3TensorImageData") {

	  if (this->io->GetNumberOfComponents()==6) {

	    typedef itk::Tensor<double, 3>    TensorType;
	    typedef itk::Image<TensorType, 3> TensorImageType;

	    typedef itk::Vector<double, 6>    VectorType;
	    typedef itk::Image<VectorType, 3> VectorImageType;

	    typedef itk::ImageFileReader<VectorImageType> ReaderType;
	    
	    VectorImageType::Pointer image = 0;
	    {
	      ReaderType::Pointer reader = ReaderType::New();
	      reader->SetImageIO (this->io);
	      reader->SetFileName ( path.toAscii().constData() );
	      try {
		reader->Update();
	      }
	      catch (itk::ExceptionObject &e) {
		qDebug() << e.GetDescription();
		return false;
	      }
	      image = reader->GetOutput(); 
	    }

	    TensorImageType::Pointer tensors = TensorImageType::New();
	    TensorImageType::RegionType region = image->GetLargestPossibleRegion();
	    tensors->SetRegions   (region);
	    tensors->SetSpacing   (image->GetSpacing());
	    tensors->SetOrigin    (image->GetOrigin());
	    tensors->SetDirection (image->GetDirection());

	    try {
	        tensors->Allocate();
	    }
	    catch (itk::ExceptionObject &e) {
	        qDebug() << e.GetDescription();
		return false;
	    }

	    itk::ImageRegionConstIteratorWithIndex<VectorImageType>  itIn (image,
									   image->GetLargestPossibleRegion());
	    itk::ImageRegionIteratorWithIndex<TensorImageType> itOut(tensors,
								     tensors->GetLargestPossibleRegion());

	    while(!itOut.IsAtEnd()) {
      
	      VectorType vec = itIn.Get();
	      TensorType tensor;
	      
	      for( unsigned int j=0; j<6; j++) {
		tensor[j] = vec[j];
	      }
      
	      itOut.Set (tensor);
	      
	      ++itOut;
	      ++itIn;
	    }

	    medData->setData (tensors);
	    
	  }
	  else if (this->io->GetNumberOfComponents()==9) {

	    typedef itk::Tensor<double, 3>    TensorType;
	    typedef itk::Image<TensorType, 3> TensorImageType;

	    typedef itk::Vector<double, 9>    VectorType;
	    typedef itk::Image<VectorType, 3> VectorImageType;

	    typedef itk::ImageFileReader<VectorImageType> ReaderType;
	    
	    VectorImageType::Pointer image = 0;
	    {
	      ReaderType::Pointer reader = ReaderType::New();
	      reader->SetImageIO (this->io);
	      reader->SetFileName ( path.toAscii().constData() );
	      try {
		reader->Update();
	      }
	      catch (itk::ExceptionObject &e) {
		qDebug() << e.GetDescription();
		return false;
	      }
	      image = reader->GetOutput(); 
	    }

	    TensorImageType::Pointer tensors = TensorImageType::New();
	    TensorImageType::RegionType region = image->GetLargestPossibleRegion();
	    tensors->SetRegions   (region);
	    tensors->SetSpacing   (image->GetSpacing());
	    tensors->SetOrigin    (image->GetOrigin());
	    tensors->SetDirection (image->GetDirection());

	    try {
	        tensors->Allocate();
	    }
	    catch (itk::ExceptionObject &e) {
	        qDebug() << e.GetDescription();
		return false;
	    }

	    itk::ImageRegionConstIteratorWithIndex<VectorImageType>  itIn (image,
									   image->GetLargestPossibleRegion());
	    itk::ImageRegionIteratorWithIndex<TensorImageType> itOut(tensors,
								     tensors->GetLargestPossibleRegion());

	    while(!itOut.IsAtEnd()) {
      
	      VectorType vec = itIn.Get();
	      TensorType tensor;
	      
	      for (unsigned int i=0; i<3; i++)
		for (unsigned int j=0; j<3; j++)
		  tensor.SetComponent (i, j, vec[i*3+j]);
      
	      itOut.Set (tensor);
	      
	      ++itOut;
	      ++itIn;
	    }

	    medData->setData (tensors);
	  }
	  else {
	      qDebug() << "Unsupported number of components";
	      return false;
	  } 
	}

	else if (medData->identifier()=="medTtkFloat3TensorImageData") {

	  if (this->io->GetNumberOfComponents()==6) {

	    typedef itk::Tensor<float, 3>     TensorType;
	    typedef itk::Image<TensorType, 3> TensorImageType;

	    typedef itk::Vector<float, 6>     VectorType;
	    typedef itk::Image<VectorType, 3> VectorImageType;

	    typedef itk::ImageFileReader<VectorImageType> ReaderType;
	    
	    VectorImageType::Pointer image = 0;
	    {
	      ReaderType::Pointer reader = ReaderType::New();
	      reader->SetImageIO (this->io);
	      reader->SetFileName ( path.toAscii().constData() );
	      try {
		reader->Update();
	      }
	      catch (itk::ExceptionObject &e) {
		qDebug() << e.GetDescription();
		return false;
	      }
	      image = reader->GetOutput(); 
	    }

	    TensorImageType::Pointer tensors = TensorImageType::New();
	    TensorImageType::RegionType region = image->GetLargestPossibleRegion();
	    tensors->SetRegions   (region);
	    tensors->SetSpacing   (image->GetSpacing());
	    tensors->SetOrigin    (image->GetOrigin());
	    tensors->SetDirection (image->GetDirection());

	    try {
	        tensors->Allocate();
	    }
	    catch (itk::ExceptionObject &e) {
	        qDebug() << e.GetDescription();
		return false;
	    }

	    itk::ImageRegionConstIteratorWithIndex<VectorImageType>  itIn (image,
									   image->GetLargestPossibleRegion());
	    itk::ImageRegionIteratorWithIndex<TensorImageType> itOut(tensors,
								     tensors->GetLargestPossibleRegion());

	    while(!itOut.IsAtEnd()) {
      
	      VectorType vec = itIn.Get();
	      TensorType tensor;
	      
	      for( unsigned int j=0; j<6; j++) {
		tensor[j] = vec[j];
	      }
      
	      itOut.Set (tensor);
	      
	      ++itOut;
	      ++itIn;
	    }
	    
	    medData->setData (tensors);
	  }
	  else if (this->io->GetNumberOfComponents()==9) {

	    typedef itk::Tensor<float, 3>     TensorType;
	    typedef itk::Image<TensorType, 3> TensorImageType;

	    typedef itk::Vector<float, 9>     VectorType;
	    typedef itk::Image<VectorType, 3> VectorImageType;

	    typedef itk::ImageFileReader<VectorImageType> ReaderType;
	    
	    VectorImageType::Pointer image = 0;
	    {
	      ReaderType::Pointer reader = ReaderType::New();
	      reader->SetImageIO (this->io);
	      reader->SetFileName ( path.toAscii().constData() );
	      try {
		reader->Update();
	      }
	      catch (itk::ExceptionObject &e) {
		qDebug() << e.GetDescription();
		return false;
	      }
	      image = reader->GetOutput(); 
	    }

	    TensorImageType::Pointer tensors = TensorImageType::New();
	    TensorImageType::RegionType region = image->GetLargestPossibleRegion();
	    tensors->SetRegions   (region);
	    tensors->SetSpacing   (image->GetSpacing());
	    tensors->SetOrigin    (image->GetOrigin());
	    tensors->SetDirection (image->GetDirection());

	    try {
	        tensors->Allocate();
	    }
	    catch (itk::ExceptionObject &e) {
	        qDebug() << e.GetDescription();
		return false;
	    }

	    itk::ImageRegionConstIteratorWithIndex<VectorImageType>  itIn (image,
									   image->GetLargestPossibleRegion());
	    itk::ImageRegionIteratorWithIndex<TensorImageType> itOut(tensors,
								     tensors->GetLargestPossibleRegion());

	    while(!itOut.IsAtEnd()) {
      
	      VectorType vec = itIn.Get();
	      TensorType tensor;
	      
	      for (unsigned int i=0; i<3; i++)
		for (unsigned int j=0; j<3; j++)
		  tensor.SetComponent (i, j, vec[i*3+j]);
      
	      itOut.Set (tensor);
	      
	      ++itOut;
	      ++itIn;
	    }

	    medData->setData (tensors);
	  }
	  else {
	      qDebug() << "Unsupported number of components";
	      return false;
	  } 
	}
	else {
	  qDebug() << "Unsupported data type";
	  return false;
	}
    }
    else {
      qDebug() << "No data set or could not create one";
      return false;
    }

    return true;
    
}
