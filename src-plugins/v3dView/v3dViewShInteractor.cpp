#include "v3dViewShInteractor.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>

#include <vtkSphericalHarmonicManager.h>
#include <vtkStructuredPoints.h>

#include "v3dView.h"

#include <itkSphericalHarmonicITKToVTKFilter.h>

#include <itkImage.h>
#include <itkVectorImage.h>

typedef itk::VectorImage<float, 3>    ShImageTypeFloat;
typedef ShImageTypeFloat::PixelType ShTypeFloat;
typedef ShImageTypeFloat::Pointer ShImagePointerFloat;

typedef itk::VectorImage<double, 3>    ShImageTypeDouble;
typedef ShImageTypeDouble::PixelType ShTypeDouble;
typedef ShImageTypeDouble::Pointer ShImagePointerDouble;

class v3dViewShInteractorPrivate
{
public:
    dtkAbstractData        *data;
    v3dView                *view;
    vtkSphericalHarmonicManager       *manager;

    // the filters will convert from itk Sh image format to vtkStructuredPoint (format handled by the Sh manager)
    itk::SphericalHarmonicITKToVTKFilter<ShImageTypeFloat>::Pointer filterFloat;
    ShImagePointerFloat      datasetFloat;

    itk::SphericalHarmonicITKToVTKFilter<ShImageTypeDouble>::Pointer filterDouble;
    ShImagePointerDouble      datasetDouble;
};

v3dViewShInteractor::v3dViewShInteractor(): medShAbstractViewInteractor(), d(new v3dViewShInteractorPrivate)
{
    d->data    = 0;
    d->view    = 0;
    d->manager = vtkSphericalHarmonicManager::New();

    d->datasetFloat = 0;
    d->filterFloat = 0;

    d->datasetDouble = 0;
    d->filterDouble = 0;

    this->addProperty("TesselationType", QStringList() << "Icosahedron" << "Dodecahedron"
                      << "Octahedron" << "Hexahedron" << "Tetrahedron");
    this->addProperty("FlipX", QStringList() << "true" << "false");
    this->addProperty("FlipY", QStringList() << "true" << "false");
    this->addProperty("FlipZ", QStringList() << "true" << "false");
    this->addProperty("ColorGlyphs", QStringList() << "true" << "false");

    this->addProperty("Normalization", QStringList() << "true" << "false");
    this->addProperty("TesselationBasis", QStringList() << "SHMatrix"
                      << "SHMatrixMaxThesis" << "SHMatrixTournier" << "SHMatrixRshBasis");

    // set default properties
    d->manager->SetTesselationType(0);
    d->manager->SetTesselationBasis(0);
    d->manager->Normalization(false);
    this->setProperty("TesselationType", "Icosahedron");
    this->setProperty("TesselationBasis", "SHMatrix");
}

v3dViewShInteractor::~v3dViewShInteractor()
{
    this->disable();
    d->manager->Delete();

    delete d;
    d = 0;
}

QString v3dViewShInteractor::description(void) const
{
    return tr("Interactor displaying 3D Spherical Harmonics (ODF ADC obtained from dmri)");
}

QString v3dViewShInteractor::identifier() const
{
    return "v3dViewShInteractor";
}

QStringList v3dViewShInteractor::handled(void) const
{
    return QStringList () << "v3dView";
}

bool v3dViewShInteractor::registered(void)
{
    return dtkAbstractViewFactory::instance()->registerViewInteractorType("v3dViewShInteractor", QStringList() << "v3dView", createV3dViewShInteractor);
}

void v3dViewShInteractor::setData(dtkAbstractData *data)
{
    if (!data)
        return;

    QString identifier = data->identifier();

    // up to the moment 2 itk Sh image formats are supported
    // we need to convert them to vtkStructuredPoints so it's understood by the Sh manager
    if (identifier.compare("itkDataShImageFloat3") == 0) {
        if (ShImageTypeFloat *dataset = static_cast<ShImageTypeFloat *>(data->data())) {

            d->datasetFloat = dataset;

            d->filterFloat = itk::SphericalHarmonicITKToVTKFilter<ShImageTypeFloat>::New();

            d->filterFloat->SetInput(dataset);

//            typename ShImageTypeFloat::DirectionType directions = dataset->GetDirection();
//            typename ShImageTypeFloat::PointType i_origin = dataset->GetOrigin();

            // this line generates the vtkShs, otherwise is not generated, even if the next filter
            // in the pipeline is connected and Update() is called
            d->filterFloat->Update();

            // we need to call this function because GetOutput() just returns the input
//            vtkStructuredPoints *shs = d->filterFloat->GetVTKSphericalHarmonic();
//            vtkMatrix4x4 *matrix = d->filterFloat->GetDirectionMatrix();

            d->manager->SetInput(d->filterFloat->GetVTKSphericalHarmonic());
            d->manager->SetMatrixT( d->filterFloat->GetDirectionMatrix());
            //TODO:JGG This has to be changed the order has to be a property saved inside the image
            int number = dataset->GetNumberOfComponentsPerPixel();
            int Order = (int)(-1.5+std::sqrt((float)(0.25+2*number)));
            d->manager->SetOrder(Order);

            // TODO this should not be here once the toolbox is coded
//            d->manager->ResetPosition();

            d->manager->Update();

            d->data = data;
        }
    } else if (identifier.compare("itkDataShImageDouble3") == 0) {
        if (ShImageTypeDouble *dataset = static_cast<ShImageTypeDouble *>(data->data())) {

            d->datasetDouble = dataset;

            d->filterDouble = itk::SphericalHarmonicITKToVTKFilter<ShImageTypeDouble>::New();

            d->filterDouble->SetInput(dataset);

            // this line generates the vtkShs, otherwise is not generated, even if the next filter
            // in the pipeline is connected and Update() is called
            d->filterDouble->Update();

            // we need to call this function because GetOutput() just returns the input
//            vtkStructuredPoints* shs = d->filterDouble->GetVTKSphericalHarmonic();

//            vtkMatrix4x4 *matrix = d->filterDouble->GetDirectionMatrix();

            d->manager->SetInput(d->filterDouble->GetVTKSphericalHarmonic());
            d->manager->SetMatrixT(d->filterDouble->GetDirectionMatrix());

            //TODO: This has to be changed the order has to be a property saved inside the image
            int number = dataset->GetNumberOfComponentsPerPixel();
            int Order = (int)(-1.5+std::sqrt((float)(0.25+2*number)));
            d->manager->SetOrder(Order);

            // TODO this should not be here once the toolbox is coded
            d->manager->ResetPosition();

            d->manager->Update();

            d->data = data;
        }
    } else {
        qDebug() << "Unrecognized Sh data type: " << identifier;
    }
}

dtkAbstractData *v3dViewShInteractor::data (void)
{
    return d->data;
}

void v3dViewShInteractor::setView(dtkAbstractView *view)
{
    if (v3dView *v3dview = dynamic_cast<v3dView*>(view) ) {
        d->view = v3dview;
        // be careful not to forget setting the same renderer for the interactor and the view
        // otherwise a new renderer is created
        d->manager->SetRenderWindowInteractor( d->view->interactor(), d->view->renderer3d() );
        d->view->renderer2d()->AddActor (d->manager->GetSHVisuManagerAxial()->GetActor());
        d->view->renderer2d()->AddActor (d->manager->GetSHVisuManagerSagittal()->GetActor());
        d->view->renderer2d()->AddActor (d->manager->GetSHVisuManagerCoronal()->GetActor());
    }
}

dtkAbstractView *v3dViewShInteractor::view (void)
{
    return d->view;
}

void v3dViewShInteractor::enable(void)
{
    if (this->enabled())
        return;

    dtkAbstractViewInteractor::enable();
}

void v3dViewShInteractor::disable(void)
{
    if (!this->enabled())
        return;

    dtkAbstractViewInteractor::disable();
}


void v3dViewShInteractor::imageSize(int* imSize)
{
    d->manager->GetSphericalHarmonicDimensions(imSize);
}

void v3dViewShInteractor::onPropertySet(const QString& key, const QString& value)
{
    if (key=="TesselationType")
    {
        this->onTesselationTypePropertySet (value);
    }
    else if (key=="FlipX")
    {
        this->onFlipXPropertySet (value);
    }
    else if (key=="FlipY")
    {
        this->onFlipYPropertySet (value);
    }
    else if (key=="FlipZ")
    {
        this->onFlipZPropertySet (value);
    }
    else if (key=="ColorGlyphs")
    {
        this->ColorGlyphsPropertySet (value);
    }
    else if (key=="TesselationBasis")
    {
        this->onTesselationBasisPropertySet (value);
    }
    else if (key=="Normalization")
    {
        this->NormalizationPropertySet  (value);
    }
}

void v3dViewShInteractor::onTesselationTypePropertySet (const QString& tesselationType)
{
    if (tesselationType == "Icosahedron")
    {
        d->manager->SetTesselationType (0);
    }
    else if (tesselationType == "Octahedron")
    {
        d->manager->SetTesselationType (2);
    }
    else if (tesselationType == "Tetrahedron")
    {
        d->manager->SetTesselationType (4);
    }
}

void v3dViewShInteractor::onTesselationBasisPropertySet (const QString& tesselationBasis)
{
    if (tesselationBasis == "SHMatrix")
    {
        d->manager->SetTesselationBasis (0);
    }
    else if (tesselationBasis == "SHMatrixMaxThesis")
    {
        d->manager->SetTesselationBasis (1);
    }
    else if (tesselationBasis == "SHMatrixTournier")
    {
        d->manager->SetTesselationBasis (2);
    }
    else if (tesselationBasis == "SHMatrixRshBasis")
    {
        d->manager->SetTesselationBasis (3);
    }
}

void v3dViewShInteractor::onSampleRatePropertySet (int sampleRate)
{
    d->manager->SetSampleRate(sampleRate, sampleRate, sampleRate);
}

//void v3dViewShInteractor::onEigenVectorPropertySet (int eigenVector)
//{
//    // we need to substract 1 because the manager receives an index
////    d->manager->SetColorModeToEigenvector(eigenVector-1);
//}

void v3dViewShInteractor::onGlyphResolutionPropertySet (int glyphResolution)
{
    d->manager->SetGlyphResolution(glyphResolution);
}

//void v3dViewShInteractor::onReverseBackgroundColorPropertySet (bool isWhite)
//{
//    if (!d->view)
//        return;

//    if(isWhite)
//        d->view->setBackgroundColor(1.0,1.0,1.0);
//    else
//        d->view->setBackgroundColor(0.0,0.0,0.0);
//}

void v3dViewShInteractor::onScalingPropertySet (double scale)
{
    d->manager->SetGlyphScale((float)scale);
}

void v3dViewShInteractor::onXSlicePropertySet (int xSlice)
{
    int dims[3];
    d->manager->GetCurrentPosition(dims);
    dims[0]=xSlice;
    d->manager->SetCurrentPosition((int*)dims);
}

void v3dViewShInteractor::onYSlicePropertySet (int ySlice)
{
    int dims[3];
    d->manager->GetCurrentPosition(dims);
    dims[1]=ySlice;
    d->manager->SetCurrentPosition((int*)dims);
}

void v3dViewShInteractor::onZSlicePropertySet (int zSlice)
{
    int dims[3];
    d->manager->GetCurrentPosition(dims);
    dims[2]=zSlice;
    d->manager->SetCurrentPosition((int*)dims);
}



void v3dViewShInteractor::onHideShowAxialPropertySet(bool show)
{
    if(show)
        d->manager->SetAxialSliceVisibility(1);
    else
        d->manager->SetAxialSliceVisibility(0);
}

void v3dViewShInteractor::onHideShowCoronalPropertySet(bool show)
{
    if(show)
        d->manager->SetCoronalSliceVisibility(1);
    else
        d->manager->SetCoronalSliceVisibility(0);
}

void v3dViewShInteractor::onHideShowSagittalPropertySet(bool show)
{
    if(show)
        d->manager->SetSagittalSliceVisibility(1);
    else
        d->manager->SetSagittalSliceVisibility(0);
}

void v3dViewShInteractor::onFlipXPropertySet (const QString& flipX)
{
    if (flipX == "true")
        d->manager->FlipX(true);
    else
        d->manager->FlipX(false);
}

void v3dViewShInteractor::onFlipYPropertySet (const QString& flipY)
{
    if (flipY == "true")
        d->manager->FlipY(true);
    else
        d->manager->FlipY(false);
}

void v3dViewShInteractor::onFlipZPropertySet (const QString& flipZ)
{
    if (flipZ == "true")
        d->manager->FlipZ(true);
    else
        d->manager->FlipZ(false);
}

void v3dViewShInteractor::ColorGlyphsPropertySet (const QString& ColorGlyph)
{
    if (ColorGlyph == "true")
        d->manager->ColorGlyphs(true);
    else
        d->manager->ColorGlyphs(false);
}

void v3dViewShInteractor::NormalizationPropertySet (const QString& Norma)
{
    if (Norma == "true")
        d->manager->Normalization(true);
    else
        d->manager->Normalization(false);
}

void v3dViewShInteractor::onPositionChanged(const QVector3D& position, bool propagate)
{
    d->manager->SetCurrentPosition(position.x(), position.y(), position.z());
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createV3dViewShInteractor(void)
{
    return new v3dViewShInteractor;
}
