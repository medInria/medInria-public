#include "v3dViewPublicFuseInteractor.h"

#include <dtkCore/dtkLog.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataImage.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>

#include "v3dViewPublic.h"

#include "itkVTKImageToImageFilter.h"
#include "itkImage.h"
#include "itkCheckerBoardImageFilter.h"
#include "itkWeightedAddImageFilter.h"
#include "itkCastImageFilter.h"

class v3dViewPublicFuseInteractorPrivate
{
public:

  typedef short PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  typedef itk::WeightedAddImageFilter<ImageType, ImageType, ImageType>  BlendFilterType;
  typedef itk::CheckerBoardImageFilter<ImageType>                       CheckerboardFilterType;
  
  dtkAbstractDataImage *data1;
  dtkAbstractDataImage *data2;

  dtkAbstractData *output;

  v3dViewPublic *view;

  double alpha;
  int    pattern;
  
  BlendFilterType::Pointer        blender;
  CheckerboardFilterType::Pointer checkerboarder;
  
};

v3dViewPublicFuseInteractor::v3dViewPublicFuseInteractor(): dtkAbstractViewInteractor(), d(new v3dViewPublicFuseInteractorPrivate)
{
    d->data1 = dynamic_cast<dtkAbstractDataImage*>( dtkAbstractDataFactory::instance()->create("itkDataImageShort3") );
    d->data2 = dynamic_cast<dtkAbstractDataImage*>( dtkAbstractDataFactory::instance()->create("itkDataImageShort3") );
    d->view = 0;

    d->alpha = 0.5;
    d->pattern = 50;
    
    d->blender        = 0; 
    d->checkerboarder = 0;
    
    d->output = dtkAbstractDataFactory::instance()->create("itkDataImageShort3");

    this->addProperty ("FusionStyle", QStringList() << "blend" << "checkerboard");
    this->setProperty ("FusionStyle", "blend");
}

v3dViewPublicFuseInteractor::~v3dViewPublicFuseInteractor()
{
    // delete d->output;
    delete d;
    d = 0;
}


QString v3dViewPublicFuseInteractor::description(void) const
{
    return "v3dViewFuseInteractor";
}

QStringList v3dViewPublicFuseInteractor::handled(void) const
{
    return QStringList () << "v3dViewPublic";
}

bool v3dViewPublicFuseInteractor::registered(void)
{
    return dtkAbstractViewFactory::instance()->registerViewInteractorType("v3dViewFuseInteractor", QStringList() << "v3dViewPublic", createV3dViewPublicFuseInteractor);
}

void v3dViewPublicFuseInteractor::onPropertySet (const QString& key, const QString& value)
{
    if (key=="FusionStyle")
        this->onFusionStylePropertySet (value);

    //if (d->view)
      //  d->view->update();
}

void v3dViewPublicFuseInteractor::onFusionStylePropertySet (const QString& value)
{
    if (value=="blend")
        if (!d->blender.IsNull())
	    d->output->setData( d->blender->GetOutput() );

    if (value=="checkerboard")
        if (!d->checkerboarder.IsNull())
	    d->output->setData( d->checkerboarder->GetOutput() );
      
     if (d->view && d->data1 && d->data1->data() &&
	 d->data2 && d->data2->data() )
         d->view->setData ( d->output );
}

void v3dViewPublicFuseInteractor::onBlendAlphaValueSet (int value)
{
    this->onBlendAlphaValueSet ( (double)value/100.0 );
}

void v3dViewPublicFuseInteractor::onBlendAlphaValueSet (double value)
{
    d->alpha = value;
    if (!d->blender.IsNull()) {
        d->blender->SetAlpha (value);
	if (d->view)
	    d->view->update();
    }
}

void v3dViewPublicFuseInteractor::onCheckerboardDivisionCountValueSet (int value)
{
    if (d->data1) {
      if ( value>d->data1->xDimension() )
	value = d->data1->xDimension();
      
      if ( value>d->data1->yDimension() )
	value = d->data1->yDimension();
      
      if ( value>d->data1->zDimension() )
	value = d->data1->zDimension();  
    }

    d->pattern = value;

    if (!d->checkerboarder.IsNull()) {
      
        v3dViewPublicFuseInteractorPrivate::CheckerboardFilterType::PatternArrayType array;
	for (int i=0; i<3; i++)
	    array[i] = value;

	d->checkerboarder->SetCheckerPattern (array);
    
	if (d->view)
	    d->view->update();
    }
}

void v3dViewPublicFuseInteractor::onCheckerboardDivisionCountValueSet (double value)
{
    this->onCheckerboardDivisionCountValueSet ( (int)(value*100.0) );
}

void v3dViewPublicFuseInteractor::setData(dtkAbstractData *data, int channel)
{
    if (!data || channel<0 || channel>1 || !data->data())
        return;
        
    if (data->description()==tr("itkDataImageChar3")) {
      typedef char ScalarType;
      typedef itk::Image<ScalarType, 3> ImageType;

      typedef itk::CastImageFilter<ImageType, v3dViewPublicFuseInteractorPrivate::ImageType> CasterType;

      CasterType::Pointer caster = CasterType::New();
      caster->SetInput ( dynamic_cast<ImageType*>( (itk::Object*)(data->data()) ) );
      try {
	caster->Update();
      }
      catch (itk::ExceptionObject &e) {
	qDebug() << e.GetDescription();
	return;
      }

      if (channel==0)
          d->data1->setData ( caster->GetOutput() );
      else if (channel==1)
          d->data2->setData ( caster->GetOutput() );
    }
    else if (data->description()==tr("itkDataImageUChar3")) {
      typedef unsigned char ScalarType;
      typedef itk::Image<ScalarType, 3> ImageType;

      typedef itk::CastImageFilter<ImageType, v3dViewPublicFuseInteractorPrivate::ImageType> CasterType;

      CasterType::Pointer caster = CasterType::New();
      caster->SetInput ( dynamic_cast<ImageType*>( (itk::Object*)(data->data()) ) );
      try {
	caster->Update();
      }
      catch (itk::ExceptionObject &e) {
	qDebug() << e.GetDescription();
	return;
      }

      if (channel==0)
          d->data1->setData ( caster->GetOutput() );
      else if (channel==1)
          d->data2->setData ( caster->GetOutput() );
    }

    else if (data->description()==tr("itkDataImageShort3")) {
      typedef short ScalarType;
      typedef itk::Image<ScalarType, 3> ImageType;

      typedef itk::CastImageFilter<ImageType, v3dViewPublicFuseInteractorPrivate::ImageType> CasterType;

      CasterType::Pointer caster = CasterType::New();
      caster->SetInput ( dynamic_cast<ImageType*>( (itk::Object*)(data->data()) ) );
      try {
	caster->Update();
      }
      catch (itk::ExceptionObject &e) {
	qDebug() << e.GetDescription();
	return;
      }      
      
      if (channel==0)
          d->data1->setData ( caster->GetOutput() );
      else if (channel==1)
          d->data2->setData ( caster->GetOutput() );
    }

    else if (data->description()==tr("itkDataImageUShort3")) {
      typedef unsigned short ScalarType;
      typedef itk::Image<ScalarType, 3> ImageType;

      typedef itk::CastImageFilter<ImageType, v3dViewPublicFuseInteractorPrivate::ImageType> CasterType;

      CasterType::Pointer caster = CasterType::New();
      caster->SetInput ( dynamic_cast<ImageType*>( (itk::Object*)(data->data()) ) );
      try {
	caster->Update();
      }
      catch (itk::ExceptionObject &e) {
	qDebug() << e.GetDescription();
	return;
      }
      
      if (channel==0)
          d->data1->setData ( caster->GetOutput() );
      else if (channel==1)
          d->data2->setData ( caster->GetOutput() );
    }

    else if (data->description()==tr("itkDataImageInt3")) {
      typedef int ScalarType;
      typedef itk::Image<ScalarType, 3> ImageType;

      typedef itk::CastImageFilter<ImageType, v3dViewPublicFuseInteractorPrivate::ImageType> CasterType;

      CasterType::Pointer caster = CasterType::New();
      caster->SetInput ( dynamic_cast<ImageType*>( (itk::Object*)(data->data()) ) );
      try {
	caster->Update();
      }
      catch (itk::ExceptionObject &e) {
	qDebug() << e.GetDescription();
	return;
      }
      
      if (channel==0)
          d->data1->setData ( caster->GetOutput() );
      else if (channel==1)
          d->data2->setData ( caster->GetOutput() );
    }

    else if (data->description()==tr("itkDataImageUInt3")) {
      typedef unsigned int ScalarType;
      typedef itk::Image<ScalarType, 3> ImageType;

      typedef itk::CastImageFilter<ImageType, v3dViewPublicFuseInteractorPrivate::ImageType> CasterType;

      CasterType::Pointer caster = CasterType::New();
      caster->SetInput ( dynamic_cast<ImageType*>( (itk::Object*)(data->data()) ) );
      try {
	caster->Update();
      }
      catch (itk::ExceptionObject &e) {
	qDebug() << e.GetDescription();
	return;
      }
      
      if (channel==0)
          d->data1->setData ( caster->GetOutput() );
      else if (channel==1)
          d->data2->setData ( caster->GetOutput() );
    }

    else if (data->description()==tr("itkDataImageLong3")) {
      typedef long ScalarType;
      typedef itk::Image<ScalarType, 3> ImageType;

      typedef itk::CastImageFilter<ImageType, v3dViewPublicFuseInteractorPrivate::ImageType> CasterType;

      CasterType::Pointer caster = CasterType::New();
      caster->SetInput ( dynamic_cast<ImageType*>( (itk::Object*)(data->data()) ) );
      try {
	caster->Update();
      }
      catch (itk::ExceptionObject &e) {
	qDebug() << e.GetDescription();
	return;
      }
      
      if (channel==0)
          d->data1->setData ( caster->GetOutput() );
      else if (channel==1)
          d->data2->setData ( caster->GetOutput() );
    }

    else if (data->description()==tr("itkDataImageULong3")) {
      typedef unsigned long ScalarType;
      typedef itk::Image<ScalarType, 3> ImageType;

      typedef itk::CastImageFilter<ImageType, v3dViewPublicFuseInteractorPrivate::ImageType> CasterType;

      CasterType::Pointer caster = CasterType::New();
      caster->SetInput ( dynamic_cast<ImageType*>( (itk::Object*)(data->data()) ) );
      try {
	caster->Update();
      }
      catch (itk::ExceptionObject &e) {
	qDebug() << e.GetDescription();
	return;
      }
      
      if (channel==0)
          d->data1->setData ( caster->GetOutput() );
      else if (channel==1)
          d->data2->setData ( caster->GetOutput() );
    }

    else if (data->description()==tr("itkDataImageFloat3")) {
      typedef float ScalarType;
      typedef itk::Image<ScalarType, 3> ImageType;

      typedef itk::CastImageFilter<ImageType, v3dViewPublicFuseInteractorPrivate::ImageType> CasterType;

      CasterType::Pointer caster = CasterType::New();
      caster->SetInput ( dynamic_cast<ImageType*>( (itk::Object*)(data->data()) ) );
      try {
	caster->Update();
      }
      catch (itk::ExceptionObject &e) {
	qDebug() << e.GetDescription();
	return;
      }
      
      if (channel==0)
          d->data1->setData ( caster->GetOutput() );
      else if (channel==1)
          d->data2->setData ( caster->GetOutput() );
    }

    else if (data->description()==tr("itkDataImageDouble3")) {
      typedef double ScalarType;
      typedef itk::Image<ScalarType, 3> ImageType;

      typedef itk::CastImageFilter<ImageType, v3dViewPublicFuseInteractorPrivate::ImageType> CasterType;

      CasterType::Pointer caster = CasterType::New();
      caster->SetInput ( dynamic_cast<ImageType*>( (itk::Object*)(data->data()) ) );
      try {
	caster->Update();
      }
      catch (itk::ExceptionObject &e) {
	qDebug() << e.GetDescription();
	return;
      }
      
      if (channel==0)
          d->data1->setData ( caster->GetOutput() );
      else if (channel==1)
          d->data2->setData ( caster->GetOutput() );
    }
    else if (data->description()==tr("v3dDataImage")) {
      typedef itk::VTKImageToImageFilter<v3dViewPublicFuseInteractorPrivate::ImageType> ConverterType;
      ConverterType::Pointer converter = ConverterType::New();
      
      converter->SetInput ( dynamic_cast<vtkImageData*>( (vtkObject*)(data->data()) ) );
      try {
	converter->Update();
      }
      catch (itk::ExceptionObject &e) {
	qDebug() << e.GetDescription();
	return;
      }
      
      if (channel==0)
          d->data1->setData ( converter->GetOutput() );
      else if (channel==1)
          d->data2->setData ( converter->GetOutput() );
    }
    else {
        qDebug() << "Unsupported image type for fusion: type=" << data->description();
        return;
    }

    
    if (d->data1 && d->data1->data() &&
	d->data2 && d->data2->data() &&
	d->view) {
      /*
      if (d->data1->xDimension()!=d->data2->xDimension() ||
	  d->data1->yDimension()!=d->data2->yDimension() ||
	  d->data1->zDimension()!=d->data2->zDimension()) {
	dtkWarning() << "Dimensions mismatch";
	return;
      }
      */

        v3dViewPublicFuseInteractorPrivate::ImageType::Pointer input1 = dynamic_cast<v3dViewPublicFuseInteractorPrivate::ImageType*>(static_cast<itk::Object*>(d->data1->data()));
	v3dViewPublicFuseInteractorPrivate::ImageType::Pointer input2 = dynamic_cast<v3dViewPublicFuseInteractorPrivate::ImageType*>(static_cast<itk::Object*>(d->data2->data()));

	d->blender = v3dViewPublicFuseInteractorPrivate::BlendFilterType::New();
	d->checkerboarder = v3dViewPublicFuseInteractorPrivate::CheckerboardFilterType::New();
	
        d->blender->SetInput1 ( input1 );
	d->blender->SetInput2 ( input2 );
	
	d->checkerboarder->SetInput1 ( input1 );
	d->checkerboarder->SetInput2 ( input2 );

	d->blender->SetAlpha ( d->alpha );
	
	v3dViewPublicFuseInteractorPrivate::CheckerboardFilterType::PatternArrayType pattern;
	for (int i=0; i<3; i++)
	  pattern[i] = d->pattern;
	
	if ((int)pattern[0]>d->data1->xDimension() )
	    pattern[0] = d->data1->xDimension();
	if ((int)pattern[1]>d->data1->yDimension() )
	    pattern[1] = d->data1->yDimension();
	if ((int)pattern[2]>d->data1->zDimension() )
	    pattern[2] = d->data1->zDimension();

	d->checkerboarder->SetCheckerPattern ( pattern );


	try {
	    d->blender->Update();
	    d->checkerboarder->Update();
	}
	catch (itk::ExceptionObject &e) {
	  qDebug() << e.GetDescription();
	  return;
	}
	
	if (d->data1->hasMetaData ("PatientName") && d->data2->hasMetaData ("PatientName"))
	    d->output->addMetaData ("PatientName",
				    tr("Fusion - ") +
				    d->data1->metaDataValues("PatientName")[0] + "\n" +
				    d->data2->metaDataValues("PatientName")[0]);

	if (this->property ("FusionStyle")=="blend")
	    d->output->setData ( d->blender->GetOutput());
	else
	    d->output->setData ( d->checkerboarder->GetOutput());
	
        d->view->setData ( d->output );
    }
}

void v3dViewPublicFuseInteractor::setView(dtkAbstractView *view)
{
    if (v3dViewPublic *v3dview = dynamic_cast<v3dViewPublic*>(view) ) {
        d->view = v3dview;
    }
}

void v3dViewPublicFuseInteractor::enable(void)
{
    if (this->enabled())
        return;
	
    if (d->view &&
	d->data1 && d->data1->data() &&
	d->data2 && d->data2->data()) {
        d->view->setData ( d->output );
    }
    dtkAbstractViewInteractor::enable();
}

void v3dViewPublicFuseInteractor::disable(void)
{
    if (!this->enabled())
        return;
    dtkAbstractViewInteractor::disable();
}


// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createV3dViewPublicFuseInteractor(void)
{
    return new v3dViewPublicFuseInteractor;
}
