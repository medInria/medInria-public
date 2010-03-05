#include "v3dViewFuseInteractor.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataImage.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>

#include "v3dView.h"

#include "vtkImageFuse.h"

#include "itkImageToVTKImageFilter.h"
#include "itkImage.h"

class v3dViewFuseInteractorPrivate
{
public:
  dtkAbstractData *data1;
  dtkAbstractData *data2;

  dtkAbstractData *output;

  v3dView *view;
  
  vtkImageFuse* fuse;
  
};

v3dViewFuseInteractor::v3dViewFuseInteractor(): dtkAbstractViewInteractor(), d(new v3dViewFuseInteractorPrivate)
{
    d->data1 = 0;
    d->data2 = 0;
    d->view = 0;
    
    d->fuse = vtkImageFuse::New();
    d->fuse->UseClampOff();
    d->fuse->UseColorMapsOff();
    d->fuse->SetFuseMode ( vtkImageFuse::FUSEMODE_BLEND );
    d->fuse->SetBlendAlpha (0.5);
    d->fuse->SetCheckerAlpha (0.06);
    
    d->output = dtkAbstractDataFactory::instance()->create ("v3dDataImage");
    if (d->output)
        d->output->setData ( d->fuse->GetOutput() );


    this->addProperty("FusionStyle", QStringList() << "blend" << "checkerboard");
}

v3dViewFuseInteractor::~v3dViewFuseInteractor()
{
    d->fuse->Delete();
}


QString v3dViewFuseInteractor::description(void) const
{
    return "v3dViewFuseInteractor";
}

QStringList v3dViewFuseInteractor::handled(void) const
{
    return QStringList () << "v3dView";
}

bool v3dViewFuseInteractor::registered(void)
{
    return dtkAbstractViewFactory::instance()->registerViewInteractorType("v3dViewFuseInteractor", QStringList() << "v3dView", createV3dViewFuseInteractor);
}

void v3dViewFuseInteractor::onPropertySet (QString key, QString value)
{
    if (key=="FusionStyle")
        this->onFusionStylePropertySet (value);

    if (d->view)
        d->view->update();
}

void v3dViewFuseInteractor::onFusionStylePropertySet (QString value)
{
    if (value=="blend")
        d->fuse->SetFuseMode ( vtkImageFuse::FUSEMODE_BLEND );

    if (value=="checkerboard")
        d->fuse->SetFuseMode ( vtkImageFuse::FUSEMODE_GRID );
}

void v3dViewFuseInteractor::onBlendAlphaValueSet (int value)
{
    d->fuse->SetBlendAlpha ((double)value/100.0);
    if (d->view)
        d->view->update();
}

void v3dViewFuseInteractor::onCheckerboardDivisionCountValueSet (int value)
{
    d->fuse->SetCheckerAlpha ((double)value/100.0);
    if (d->view)
        d->view->update();
}

void v3dViewFuseInteractor::setData(dtkAbstractData *data, int channel)
{
    if (!data || channel<0 || channel>1)
        return;

    if (data->description()==tr("itkDataImageChar3")) {
      typedef char ScalarType;
      typedef itk::Image<ScalarType, 3> ImageType;
      typedef itk::ImageToVTKImageFilter<ImageType> ConverterType;
      ConverterType::Pointer converter = ConverterType::New();
      converter->SetInput ( dynamic_cast<ImageType*>( (itk::Object*)(data->data()) ) );
      converter->Update();
      converter->SetReferenceCount (2);
      
      d->fuse->SetInput (channel, converter->GetOutput() );
    }
    else if (data->description()==tr("itkDataImageUChar3")) {
      typedef unsigned char ScalarType;
      typedef itk::Image<ScalarType, 3> ImageType;
      typedef itk::ImageToVTKImageFilter<ImageType> ConverterType;
      ConverterType::Pointer converter = ConverterType::New();
      converter->SetInput ( dynamic_cast<ImageType*>( (itk::Object*)(data->data()) ) );
      converter->Update();
      converter->SetReferenceCount (2);
      
      d->fuse->SetInput (channel, converter->GetOutput() );
    }

    else if (data->description()==tr("itkDataImageShort3")) {
      typedef short ScalarType;
      typedef itk::Image<ScalarType, 3> ImageType;
      typedef itk::ImageToVTKImageFilter<ImageType> ConverterType;
      ConverterType::Pointer converter = ConverterType::New();
      converter->SetInput ( dynamic_cast<ImageType*>( (itk::Object*)(data->data()) ) );
      converter->Update();
      converter->SetReferenceCount (2);
      
      d->fuse->SetInput (channel, converter->GetOutput() );
    }

    else if (data->description()==tr("itkDataImageUShort3")) {
      typedef unsigned short ScalarType;
      typedef itk::Image<ScalarType, 3> ImageType;
      typedef itk::ImageToVTKImageFilter<ImageType> ConverterType;
      ConverterType::Pointer converter = ConverterType::New();
      converter->SetInput ( dynamic_cast<ImageType*>( (itk::Object*)(data->data()) ) );
      converter->Update();
      converter->SetReferenceCount (2);
      
      d->fuse->SetInput (channel, converter->GetOutput() );
    }

    else if (data->description()==tr("itkDataImageInt3")) {
      typedef int ScalarType;
      typedef itk::Image<ScalarType, 3> ImageType;
      typedef itk::ImageToVTKImageFilter<ImageType> ConverterType;
      ConverterType::Pointer converter = ConverterType::New();
      converter->SetInput ( dynamic_cast<ImageType*>( (itk::Object*)(data->data()) ) );
      converter->Update();
      converter->SetReferenceCount (2);
      
      d->fuse->SetInput (channel, converter->GetOutput() );
    }

    else if (data->description()==tr("itkDataImageUInt3")) {
      typedef unsigned int ScalarType;
      typedef itk::Image<ScalarType, 3> ImageType;
      typedef itk::ImageToVTKImageFilter<ImageType> ConverterType;
      ConverterType::Pointer converter = ConverterType::New();
      converter->SetInput ( dynamic_cast<ImageType*>( (itk::Object*)(data->data()) ) );
      converter->Update();
      converter->SetReferenceCount (2);
      
      d->fuse->SetInput (channel, converter->GetOutput() );
    }

    else if (data->description()==tr("itkDataImageLong3")) {
      typedef long ScalarType;
      typedef itk::Image<ScalarType, 3> ImageType;
      typedef itk::ImageToVTKImageFilter<ImageType> ConverterType;
      ConverterType::Pointer converter = ConverterType::New();
      converter->SetInput ( dynamic_cast<ImageType*>( (itk::Object*)(data->data()) ) );
      converter->Update();
      converter->SetReferenceCount (2);
      
      d->fuse->SetInput (channel, converter->GetOutput() );
    }

    else if (data->description()==tr("itkDataImageULong3")) {
      typedef unsigned long ScalarType;
      typedef itk::Image<ScalarType, 3> ImageType;
      typedef itk::ImageToVTKImageFilter<ImageType> ConverterType;
      ConverterType::Pointer converter = ConverterType::New();
      converter->SetInput ( dynamic_cast<ImageType*>( (itk::Object*)(data->data()) ) );
      converter->Update();
      converter->SetReferenceCount (2);
      
      d->fuse->SetInput (channel, converter->GetOutput() );
    }

    else if (data->description()==tr("itkDataImageFloat3")) {
      typedef float ScalarType;
      typedef itk::Image<ScalarType, 3> ImageType;
      typedef itk::ImageToVTKImageFilter<ImageType> ConverterType;
      ConverterType::Pointer converter = ConverterType::New();
      converter->SetInput ( dynamic_cast<ImageType*>( (itk::Object*)(data->data()) ) );
      converter->Update();
      converter->SetReferenceCount (2);
      
      d->fuse->SetInput (channel, converter->GetOutput() );
    }

    else if (data->description()==tr("itkDataImageDouble3")) {
      typedef double ScalarType;
      typedef itk::Image<ScalarType, 3> ImageType;
      typedef itk::ImageToVTKImageFilter<ImageType> ConverterType;
      ConverterType::Pointer converter = ConverterType::New();
      converter->SetInput ( dynamic_cast<ImageType*>( (itk::Object*)(data->data()) ) );
      converter->Update();
      converter->SetReferenceCount (2);
      
      d->fuse->SetInput (channel, converter->GetOutput() );
    }
    else if (data->description()==tr("vtkDataImage")) {
      d->fuse->SetInput (channel, dynamic_cast<vtkImageData*>( (vtkObject*)(data->data()) ) );
    }
    else
      return;
    
    if (channel==0)
        d->data1 = data;
    if (channel==1)
        d->data2 = data;    
    
    if (d->data1 && d->data2 && d->view) {
        d->fuse->Update();
	//d->output->setData (d->fuse->GetOutput());
	
	if (d->data1->hasMetaData ("PatientName") && d->data2->hasMetaData ("PatientName"))
	    d->output->addMetaData ("PatientName",
				    tr("Fusion - ") +
				    d->data1->metaDataValues("PatientName")[0] + "\n" +
				    d->data2->metaDataValues("PatientName")[0]);
	
        d->view->setData ( d->output );
    }
}

void v3dViewFuseInteractor::setView(dtkAbstractView *view)
{
    if (v3dView *v3dview = dynamic_cast<v3dView*>(view) ) {
        d->view = v3dview;
    }
}

void v3dViewFuseInteractor::enable(void)
{
    if (this->enabled())
        return;
	
    if (d->view && d->data1 && d->data2) {
        d->view->setData ( d->output );
    }
    dtkAbstractViewInteractor::enable();
}

void v3dViewFuseInteractor::disable(void)
{
    if (!this->enabled())
        return;
    dtkAbstractViewInteractor::disable();
}


// /////////////////////////////////////////////////////////////////
// Type instanciation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createV3dViewFuseInteractor(void)
{
    return new v3dViewFuseInteractor;
}
