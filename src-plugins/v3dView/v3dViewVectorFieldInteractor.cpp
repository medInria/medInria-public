#include "v3dViewVectorFieldInteractor.h"

#include "medVtkView.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medAbstractDataImage.h>
#include <medAbstractDataTypedImage.h>

#include <vtkPointData.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkAssignAttribute.h>
#include <vtkVectorManager.h>

#include <itkVector.h>
#include <itkImage.h>
#include <itkImageToVTKImageFilter.h>


const int ImageDimension = 3;
typedef itk::Image< itk::Vector< float, 3 >,  ImageDimension >   VectorFloatImageType;
typedef itk::Image< itk::Vector< double, 3 >,  ImageDimension >   VectorDoubleImageType;

typedef itk::ImageToVTKImageFilter< VectorFloatImageType > FloatFilterType;
typedef itk::ImageToVTKImageFilter< VectorDoubleImageType > DoubleFilterType;

class v3dViewVectorFieldInteractorPrivate
{
public:
    dtkSmartPointer<dtkAbstractData> data;
    dtkSmartPointer<v3dView>         view;
    VectorFloatImageType::Pointer    floatData;
    VectorDoubleImageType::Pointer   doubleData;
    FloatFilterType::Pointer         floatFilter;
    DoubleFilterType::Pointer        doubleFilter;

    vtkVectorManager                 *manager;
};

v3dViewVectorFieldInteractor::v3dViewVectorFieldInteractor(): d(new v3dViewVectorFieldInteractorPrivate)
{
    d->data = NULL;
    d->view = NULL;
    d->manager = vtkVectorManager::New();
}


v3dViewVectorFieldInteractor::~v3dViewVectorFieldInteractor()
{
    d->manager->Delete();
}

QString v3dViewVectorFieldInteractor::description() const
{
    return tr("Interactor helping display vector fields");
}

QString v3dViewVectorFieldInteractor::identifier() const
{
    return "v3dViewVectorFieldInteractor";
}

QStringList v3dViewVectorFieldInteractor::handled() const
{
    return QStringList () << v3dView::s_identifier() << medVtkView::s_identifier();
}

bool v3dViewVectorFieldInteractor::isDataTypeHandled(QString dataType) const
{
    if (dataType.startsWith("itkDataImageVectorFloat3") || dataType.startsWith("itkDataImageVectorDouble3"))
        return true;

    return false;
}

bool v3dViewVectorFieldInteractor::registered()
{
    return dtkAbstractViewFactory::instance()->registerViewInteractorType("v3dViewVectorFieldInteractor",
                                                                          QStringList () << v3dView::s_identifier() << medVtkView::s_identifier(),
                                                                          createV3dViewVectorFieldInteractor);
}

void v3dViewVectorFieldInteractor::setData(dtkAbstractData *data)
{
    if (!data)
        return;

    QString identifier = data->identifier();

    d->data = data;

    vtkMatrix4x4 *mat = vtkMatrix4x4::New(); //start with identity matrix

    if( identifier.compare("itkDataImageVectorFloat3") == 0 )
    {
        typedef    itk::Vector< float, 3 >    InputPixelType;
        typedef itk::Image< InputPixelType,  ImageDimension >   ImageType;

        d->floatData = static_cast<ImageType* >(data->data());

        ImageType::DirectionType direction = d->floatData->GetDirection();
        ImageType::PointType i_origin =  d->floatData->GetOrigin();

        for (int i=0; i<3; i++)
            for (int k=0; k<3; k++)
                mat->SetElement(i,k, direction(i,k));

        double v_origin[4], v_origin2[4];
        for (int i=0; i<3; i++)
          v_origin[i] = i_origin[i];
        v_origin[3] = 1.0;
        mat->MultiplyPoint (v_origin, v_origin2);
        for (int i=0; i<3; i++)
          mat->SetElement (i, 3, v_origin[i]-v_origin2[i]);

        d->floatFilter = FloatFilterType::New();
        d->floatFilter->SetInput( d->floatData );
        d->floatFilter->Update();

        vtkImageData *vtkImage = d->floatFilter->GetOutput();

        d->manager->SetInput(vtkImage);
    }

    else if( identifier.compare("itkDataImageVectorDouble3") == 0 )
    {
        typedef    itk::Vector< double, 3 >    InputPixelType;
        typedef itk::Image< InputPixelType,  ImageDimension >   ImageType;

        d->doubleData = static_cast<ImageType* >(data->data());

        ImageType::DirectionType direction = d->doubleData->GetDirection();
        ImageType::PointType i_origin =  d->doubleData->GetOrigin();

        for (int i=0; i<3; i++)
            for (int k=0; k<3; k++)
                mat->SetElement(i,k, direction(i,k));

        double v_origin[4], v_origin2[4];
        for (int i=0; i<3; i++)
          v_origin[i] = i_origin[i];
        v_origin[3] = 1.0;
        mat->MultiplyPoint (v_origin, v_origin2);
        for (int i=0; i<3; i++)
          mat->SetElement (i, 3, v_origin[i]-v_origin2[i]);

        d->doubleFilter = DoubleFilterType::New();
        d->doubleFilter->SetInput( d->doubleData );
        d->doubleFilter->Update();

        vtkImageData *vtkImage = d->doubleFilter->GetOutput();

        d->manager->SetInput(vtkImage);
    }

    else return;

    d->manager->SetDirectionMatrix(mat);
    d->manager->ResetPosition();
    d->manager->Update();
}


dtkAbstractData *v3dViewVectorFieldInteractor::data()
{
    return d->data;
}

void v3dViewVectorFieldInteractor::setView(dtkAbstractView *view)
{
    if (d->view)
    {
        disconnect(d->view, SIGNAL(positionChanged(const QVector3D&,bool)),
                   this,    SLOT(changePosition(const QVector3D&,bool)));
        d->view = 0;
    }

    if (v3dView *v3dview = qobject_cast<v3dView*>(view) )
    {
        d->view = v3dview;
        d->manager->SetRenderWindowInteractor( d->view->interactor(), d->view->renderer3d() );

        connect(d->view, SIGNAL(positionChanged(const QVector3D&,bool)),
                this,    SLOT(changePosition(const QVector3D&,bool)));

        d->view->renderer2d()->AddActor (d->manager->GetVectorVisuManagerAxial()->GetActor());
        d->view->renderer2d()->AddActor (d->manager->GetVectorVisuManagerSagittal()->GetActor());
        d->view->renderer2d()->AddActor (d->manager->GetVectorVisuManagerCoronal()->GetActor());
    }
}

dtkAbstractView *v3dViewVectorFieldInteractor::view()
{
    return d->view;
}

void v3dViewVectorFieldInteractor::enable()
{
    if (this->enabled())
        return;

    dtkAbstractViewInteractor::enable();
}

void v3dViewVectorFieldInteractor::disable()
{
    if (!this->enabled())
        return;

    dtkAbstractViewInteractor::disable();
}

void v3dViewVectorFieldInteractor::setOpacity(dtkAbstractData * /*data*/, double /*opacity*/)
{
    //TODO
}

double v3dViewVectorFieldInteractor::opacity(dtkAbstractData * /*data*/) const
{
    //TODO
    return 100;
}

void v3dViewVectorFieldInteractor::setVisible(dtkAbstractData * /*data*/, bool /*visible*/)
{
    //TODO
}

bool v3dViewVectorFieldInteractor::isVisible(dtkAbstractData * /*data*/) const
{
    //TODO
    return true;
}

void v3dViewVectorFieldInteractor::setScale(double scale)
{
    d->manager->SetGlyphScale((float)scale);
    d->view->update();
}

void v3dViewVectorFieldInteractor::setSampleRate(int sampleRate)
{
    d->manager->SetSampleRate(sampleRate,sampleRate,sampleRate);
    d->view->update();
}

void v3dViewVectorFieldInteractor::setColorMode(int mode)
{
    d->manager->SetColorMode(mode);
    d->view->update();
}

void v3dViewVectorFieldInteractor::setProjection(bool enable)
{
    d->manager->SetProjection(enable);
    d->view->update();
}

void v3dViewVectorFieldInteractor::changePosition(const QVector3D& position, bool propagate)
{
    d->manager->SetCurrentPosition(position.x(), position.y(), position.z());
    d->view->update();
}

void v3dViewVectorFieldInteractor::setShowAxial(bool show)
{
    if(show)
        d->manager->SetAxialSliceVisibility(1);
    else
        d->manager->SetAxialSliceVisibility(0);

    d->view->update();
}

void v3dViewVectorFieldInteractor::setShowCoronal(bool show)
{
    if(show)
        d->manager->SetCoronalSliceVisibility(1);
    else
        d->manager->SetCoronalSliceVisibility(0);

    d->view->update();
}

void v3dViewVectorFieldInteractor::setShowSagittal(bool show)
{
    if(show)
        d->manager->SetSagittalSliceVisibility(1);
    else
        d->manager->SetSagittalSliceVisibility(0);

    d->view->update();
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createV3dViewVectorFieldInteractor()
{
    return new v3dViewVectorFieldInteractor;
}
