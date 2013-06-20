#include <v3dViewSHInteractor.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>

#include <vtkSphericalHarmonicManager.h>
#include <vtkStructuredPoints.h>

#include <v3dView.h>

#include <itkSphericalHarmonicITKToVTKFilter.h>

#include <itkImage.h>
#include <itkVectorImage.h>

class v3dViewSHInteractorPrivate {
public:

    dtkAbstractData*             data;
    v3dView*                     view;
    vtkSphericalHarmonicManager* manager;

    //  The filters will convert from itk SH image format to vtkStructuredPoint (format handled by the SH manager)

    itk::SphericalHarmonicITKToVTKFilter<itk::VectorImage<float,3> >::Pointer  filterFloat;
    itk::SphericalHarmonicITKToVTKFilter<itk::VectorImage<double,3> >::Pointer filterDouble;

    template <typename SH_IMAGE>
    void setVTKFilter(dtkAbstractData* d,typename itk::SphericalHarmonicITKToVTKFilter<SH_IMAGE>::Pointer& filter) {

        SH_IMAGE* dataset = static_cast<SH_IMAGE*>(d->data());

        if (filter)
            filter->Delete();

        filter = itk::SphericalHarmonicITKToVTKFilter<SH_IMAGE>::New();

        filter->SetInput(dataset);

        // typename SH_IMAGE::DirectionType directions = dataset->GetDirection();
        // typename SH_IMAGE::PointType i_origin = dataset->GetOrigin();

        //  This line generates the vtkSHs, otherwise is not generated, even if the next filter
        //  in the pipeline is connected and Update() is called

        filter->Update();

        //  We need to call this function because GetOutput() just returns the input

        manager->SetInput(filter->GetVTKSphericalHarmonic());
        manager->SetMatrixT(filter->GetDirectionMatrix());

        //TODO:JGG This has to be changed the order has to be a property saved inside the image

        const int number = dataset->GetNumberOfComponentsPerPixel();
        const int Order  = -1.5+std::sqrt((float)(0.25+2*number));
        manager->SetOrder(Order);

        // TODO this should not be here once the toolbox is coded
        //            manager->ResetPosition();

        manager->Update();
        data = d;
    }
};

v3dViewSHInteractor::v3dViewSHInteractor(): medSHAbstractViewInteractor(),d(new v3dViewSHInteractorPrivate) {

    d->data    = 0;
    d->view    = 0;
    d->manager = vtkSphericalHarmonicManager::New();

    d->filterFloat = 0;
    d->filterDouble = 0;

    addProperty("TesselationType",QStringList() << "Icosahedron" << "Dodecahedron" << "Octahedron" << "Hexahedron" << "Tetrahedron");
    addProperty("FlipX",QStringList() << "true" << "false");
    addProperty("FlipY",QStringList() << "true" << "false");
    addProperty("FlipZ",QStringList() << "true" << "false");
    addProperty("ColorGlyphs",QStringList() << "true" << "false");

    addProperty("Normalization",QStringList() << "true" << "false");
    addProperty("TesselationBasis",QStringList() << "SHMatrix" << "SHMatrixMaxThesis" << "SHMatrixTournier" << "SHMatrixRshBasis");

    //  Set default properties

    d->manager->SetTesselationType(0);
    d->manager->SetTesselationBasis(0);
    d->manager->Normalization(false);
    setProperty("TesselationType","Icosahedron");
    setProperty("TesselationBasis","SHMatrix");
}

v3dViewSHInteractor::~v3dViewSHInteractor() {
    disable();
    d->manager->Delete();
    delete d;
    d = 0;
}

QString v3dViewSHInteractor::description() const {
    return tr("Interactor displaying 3D Spherical Harmonics (ODF ADC obtained from dmri)");
}

QString v3dViewSHInteractor::identifier() const {
    return "v3dViewSHInteractor";
}

QStringList v3dViewSHInteractor::handled() const {
    return QStringList() << "v3dView";
}

bool v3dViewSHInteractor::registered() {
    return dtkAbstractViewFactory::instance()->registerViewInteractorType("v3dViewSHInteractor",QStringList() << "v3dView",createV3dViewSHInteractor);
}

void v3dViewSHInteractor::setData(dtkAbstractData *data) {

    if (!data || !data->data())
        return;

    //  Two itk SH image formats are supported
    //  we need to convert them to vtkStructuredPoints so it's understood by the SH manager

    const QString& identifier = data->identifier();
    if (identifier=="itkDataSHImageFloat3")
        d->setVTKFilter<itk::VectorImage<float,3> >(data,d->filterFloat);
    else if (identifier=="itkDataSHImageDouble3")
        d->setVTKFilter<itk::VectorImage<double,3> >(data,d->filterDouble);
    else
        qDebug() << "Unrecognized SH data type: " << identifier;
}

dtkAbstractData *v3dViewSHInteractor::data() {
    return d->data;
}

void v3dViewSHInteractor::setView(dtkAbstractView *view) {
    if (v3dView *v3dview = dynamic_cast<v3dView*>(view)) {
        d->view = v3dview;
        // be careful not to forget setting the same renderer for the interactor and the view
        // otherwise a new renderer is created
        d->manager->SetRenderWindowInteractor(d->view->interactor(),d->view->renderer3d());
        d->view->renderer2d()->AddActor(d->manager->GetSHVisuManagerAxial()->GetActor());
        d->view->renderer2d()->AddActor(d->manager->GetSHVisuManagerSagittal()->GetActor());
        d->view->renderer2d()->AddActor(d->manager->GetSHVisuManagerCoronal()->GetActor());
    }
}

dtkAbstractView *v3dViewSHInteractor::view() {
    return d->view;
}

void v3dViewSHInteractor::enable() {
    if (enabled())
        return;

    dtkAbstractViewInteractor::enable();
}

void v3dViewSHInteractor::disable() {
    if (!enabled())
        return;

    dtkAbstractViewInteractor::disable();
}


void v3dViewSHInteractor::imageSize(int* imSize) {
    d->manager->GetSphericalHarmonicDimensions(imSize);
}

void v3dViewSHInteractor::onPropertySet(const QString& key,const QString& value) {
    if (key=="TesselationType")
        onTesselationTypePropertySet(value);
    else if (key=="FlipX")
        onFlipXPropertySet(value);
    else if (key=="FlipY")
        onFlipYPropertySet(value);
    else if (key=="FlipZ")
        onFlipZPropertySet(value);
    else if (key=="ColorGlyphs")
        ColorGlyphsPropertySet(value);
    else if (key=="TesselationBasis")
        onTesselationBasisPropertySet(value);
    else if (key=="Normalization")
        NormalizationPropertySet(value);
}

void v3dViewSHInteractor::onTesselationTypePropertySet(const QString& tesselationType) {
    if (tesselationType == "Icosahedron")
        d->manager->SetTesselationType(0);
    else if (tesselationType == "Octahedron")
        d->manager->SetTesselationType(2);
    else if (tesselationType == "Tetrahedron")
        d->manager->SetTesselationType(4);
}

void v3dViewSHInteractor::onTesselationBasisPropertySet(const QString& tesselationBasis) {
    if (tesselationBasis == "SHMatrix")
        d->manager->SetTesselationBasis(0);
    else if (tesselationBasis == "SHMatrixMaxThesis")
        d->manager->SetTesselationBasis(1);
    else if (tesselationBasis == "SHMatrixTournier")
        d->manager->SetTesselationBasis(2);
    else if (tesselationBasis == "SHMatrixRshBasis")
        d->manager->SetTesselationBasis(3);
}

void v3dViewSHInteractor::onSampleRatePropertySet(int sampleRate) {
    d->manager->SetSampleRate(sampleRate,sampleRate,sampleRate);
}

//void v3dViewSHInteractor::onEigenVectorPropertySet(int eigenVector)
//{
//    // we need to substract 1 because the manager receives an index
////    d->manager->SetColorModeToEigenvector(eigenVector-1);
//}

void v3dViewSHInteractor::onGlyphResolutionPropertySet(int glyphResolution) {
    d->manager->SetGlyphResolution(glyphResolution);
}

//void v3dViewSHInteractor::onReverseBackgroundColorPropertySet(bool isWhite)
//{
//    if (!d->view)
//        return;

//    if(isWhite)
//        d->view->setBackgroundColor(1.0,1.0,1.0);
//    else
//        d->view->setBackgroundColor(0.0,0.0,0.0);
//}

void v3dViewSHInteractor::onScalingPropertySet(double scale) {
    d->manager->SetGlyphScale((float)scale);
}

void v3dViewSHInteractor::onXSlicePropertySet(int xSlice) {
    int dims[3];
    d->manager->GetCurrentPosition(dims);
    dims[0]=xSlice;
    d->manager->SetCurrentPosition((int*)dims);
}

void v3dViewSHInteractor::onYSlicePropertySet(int ySlice) {
    int dims[3];
    d->manager->GetCurrentPosition(dims);
    dims[1]=ySlice;
    d->manager->SetCurrentPosition((int*)dims);
}

void v3dViewSHInteractor::onZSlicePropertySet(int zSlice) {
    int dims[3];
    d->manager->GetCurrentPosition(dims);
    dims[2]=zSlice;
    d->manager->SetCurrentPosition((int*)dims);
}

void v3dViewSHInteractor::onHideShowAxialPropertySet(bool show) {
    const int visible = show ? 1 : 0;
    d->manager->SetAxialSliceVisibility(visible);
}

void v3dViewSHInteractor::onHideShowCoronalPropertySet(bool show) {
    const int visible = show ? 1 : 0;
    d->manager->SetCoronalSliceVisibility(visible);
}

void v3dViewSHInteractor::onHideShowSagittalPropertySet(bool show) {
    const int visible = show ? 1 : 0;
    d->manager->SetSagittalSliceVisibility(visible);
}

void v3dViewSHInteractor::onFlipXPropertySet(const QString& flipX) {
    const bool flip = (flipX=="true");
    d->manager->FlipX(flip);
}

void v3dViewSHInteractor::onFlipYPropertySet(const QString& flipY) {
    const bool flip = (flipY=="true");
    d->manager->FlipY(flip);
}

void v3dViewSHInteractor::onFlipZPropertySet(const QString& flipZ) {
    const bool flip = (flipZ=="true");
    d->manager->FlipZ(flip);
}

void v3dViewSHInteractor::ColorGlyphsPropertySet(const QString& ColorGlyph) {
    const bool colorize = (ColorGlyph=="true");
    d->manager->ColorGlyphs(colorize);
}

void v3dViewSHInteractor::NormalizationPropertySet(const QString& Norma) {
    const bool normalize = (Norma=="true");
    d->manager->Normalization(normalize);
}

void v3dViewSHInteractor::onPositionChanged(const QVector3D& position,bool propagate) {
    d->manager->SetCurrentPosition(position.x(),position.y(),position.z());
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createV3dViewSHInteractor() {
    return new v3dViewSHInteractor;
}
