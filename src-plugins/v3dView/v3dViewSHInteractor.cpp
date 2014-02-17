/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <v3dViewSHInteractor.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>

#include <vtkSphericalHarmonicManager.h>
#include <vtkStructuredPoints.h>

#include <v3dView.h>
#include <medVtkView.h>
#include <medParameter.h>

#include <itkSphericalHarmonicITKToVTKFilter.h>

#include <itkImage.h>
#include <itkVectorImage.h>

class v3dViewSHInteractorPrivate {
public:

    dtkAbstractData*             data;
    v3dView*                     view;
    vtkSphericalHarmonicManager* manager;
    double                       imageBounds[6];

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

        manager->Update();
        data = d;
    }
};

v3dViewSHInteractor::v3dViewSHInteractor(): medAbstractVtkViewInteractor(),d(new v3dViewSHInteractorPrivate) {

    d->data    = 0;
    d->view    = 0;
    d->manager = vtkSphericalHarmonicManager::New();

    d->filterFloat = 0;
    d->filterDouble = 0;

    //  Set default properties
    d->manager->SetTesselationType(0);
    d->manager->SetTesselationBasis(0);
    d->manager->Normalization(false);

    for (int i=0; i<6; i++)
        d->imageBounds[i] = 0;
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
    return QStringList() << v3dView::s_identifier() << medVtkView::s_identifier();
}

bool v3dViewSHInteractor::isDataTypeHandled(QString dataType) const
{
    if (dataType.startsWith( "itkDataSHImage"))
        return true;

    return false;
}


bool v3dViewSHInteractor::registered() {
    return dtkAbstractViewFactory::instance()->registerViewInteractorType("v3dViewSHInteractor",
                                                                          QStringList () << v3dView::s_identifier() << medVtkView::s_identifier(),
                                                                          createV3dViewSHInteractor);
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
    {
        qDebug() << "Unrecognized SH data type: " << identifier;
        return;
    }

    if(d->view->layersCount() == 0)
    {
        int imSize[3];
        imageSize(imSize);
        computeBounds();
        d->view->changeBounds(d->imageBounds, imSize);
    }

    setupParameters(data);
}

void v3dViewSHInteractor::setupParameters(dtkAbstractData *data)
{
    QStringList tesselationTypeList;
    tesselationTypeList << "Icosahedron" << "Octahedron" << "Tetrahedron";
    medListParameter *tesselationTypeParam = new medListParameter("tesselationType", data);
    tesselationTypeParam->setValues(tesselationTypeList);

    //  Combobox to control the spherical Harmonics basis

    QStringList tesselationBasisList;
    tesselationBasisList << "SHMatrix" << "SHMatrixMaxThesis" << "SHMatrixTournier" << "SHMatrixRshBasis";
    medListParameter * tesselationBasisParam = new medListParameter("tesselationBasis", data);
    tesselationBasisParam->setValues(tesselationBasisList);

    //  Control sample rate

    medIntParameter *sampleRateParam = new medIntParameter("sampleRate", data);
    sampleRateParam->setMinimum(1);
    sampleRateParam->setMaximum(10);
    sampleRateParam->setValue(1);


    //  flipX, flipY, flipZ and Enhance checkboxes

    medBooleanParameter *flipXParam = new medBooleanParameter("FlipX", data);
    medBooleanParameter *flipYParam = new medBooleanParameter("FlipY", data);
    medBooleanParameter *flipZParam = new medBooleanParameter("FlipZ", data);
    flipZParam->setValue(true);

    medBooleanParameter *enhanceParam = new medBooleanParameter("Enhance", data);


    //  Control glyph resolution

    medIntParameter *glyphResolutionParam = new medIntParameter("glyphResolution", data);
    glyphResolutionParam->setMinimum(0);
    glyphResolutionParam->setMaximum(10);
    glyphResolutionParam->setValue(2);


    //  We need to calculate one single number for the scale, out of the minor and major scales
    //  scale = minor*10^(major)

    //  Minor scaling

    medIntParameter *minorScalingParam = new medIntParameter("minorScaling", data);
    minorScalingParam->setMinimum(0);
    minorScalingParam->setMaximum(9);
    minorScalingParam->setValue(3);


    //  Major scaling

    medIntParameter *majorScalingParam = new medIntParameter("majorScaling", data);
    majorScalingParam->setMinimum(-10);
    majorScalingParam->setMaximum(10);
    majorScalingParam->setValue(0);


    //  Hide or show axial, coronal, and sagittal

    medBooleanParameter *showAxialParam = new medBooleanParameter("ShowAxial", data);
    showAxialParam->setValue(true);
    medBooleanParameter *showCoronalParam = new medBooleanParameter("ShowCoronal", data);
    showCoronalParam->setValue(true);
    medBooleanParameter *showSagittalParam = new medBooleanParameter("ShowSagittal", data);
    showSagittalParam->setValue(true);


    parameters.insert(data, tesselationTypeParam);
    parameters.insert(data, tesselationBasisParam);
    parameters.insert(data, sampleRateParam);
    parameters.insert(data, flipXParam);
    parameters.insert(data, flipYParam);
    parameters.insert(data, flipZParam);
    parameters.insert(data, enhanceParam);
    parameters.insert(data, glyphResolutionParam);
    parameters.insert(data, minorScalingParam);
    parameters.insert(data, majorScalingParam);
    parameters.insert(data, showAxialParam);
    parameters.insert(data, showCoronalParam);
    parameters.insert(data, showSagittalParam);

    connect(tesselationTypeParam, SIGNAL(valueChanged(int)), this, SLOT(setTesselationType(TesselationType)));
    connect(tesselationBasisParam, SIGNAL(valueChanged(int)), this, SLOT(setTesselationBasis(TesselationBasis)));
    connect(sampleRateParam, SIGNAL(valueChanged(int)), this, SLOT(setSampleRate(int)));
    connect(flipXParam, SIGNAL(valueChanged(bool)), this, SLOT(setFlipX(bool)));
    connect(flipYParam, SIGNAL(valueChanged(bool)), this, SLOT(setFlipY(bool)));
    connect(flipZParam, SIGNAL(valueChanged(bool)), this, SLOT(setFlipZ(bool)));
    connect(enhanceParam, SIGNAL(valueChanged(bool)), this, SLOT(setNormalization(bool)));
    connect(glyphResolutionParam, SIGNAL(valueChanged(int)), this, SLOT(setGlyphResolution(int)));
    //connect(minorScalingParam, SIGNAL(valueChanged(int)), this, SLOT(set(TesselationType)));
    //connect(majorScalingParam, SIGNAL(valueChanged(int)), this, SLOT(setTesselationType(TesselationType)));
    connect(showAxialParam, SIGNAL(valueChanged(bool)), this, SLOT(setShowAxial(bool)));
    connect(showCoronalParam, SIGNAL(valueChanged(bool)), this, SLOT(setShowCoronal(bool)));
    connect(showSagittalParam, SIGNAL(valueChanged(bool)), this, SLOT(setShowSagittal(bool)));

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

        connect(d->view, SIGNAL(positionChanged(const QVector3D&,bool)),
                this,    SLOT(setPosition(const QVector3D&,bool)));
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


void v3dViewSHInteractor::setTesselationType(TesselationType tesselationType) {
    d->manager->SetTesselationType(tesselationType);
    d->view->update();
}

void v3dViewSHInteractor::setTesselationBasis(TesselationBasis tesselationBasis) {
    d->manager->SetTesselationBasis(tesselationBasis);
    d->view->update();
}

void v3dViewSHInteractor::setSampleRate(int sampleRate) {
    d->manager->SetSampleRate(sampleRate,sampleRate,sampleRate);
    d->view->update();
}

void v3dViewSHInteractor::setGlyphResolution(int glyphResolution) {
    d->manager->SetGlyphResolution(glyphResolution);
    d->view->update();
}

void v3dViewSHInteractor::setScaling(double scale) {
    d->manager->SetGlyphScale((float)scale);
    d->view->update();
}

void v3dViewSHInteractor::setXSlice(int xSlice) {
    int dims[3];
    d->manager->GetCurrentPosition(dims);
    dims[0]=xSlice;
    d->manager->SetCurrentPosition((int*)dims);
    d->view->update();
}

void v3dViewSHInteractor::setYSlice(int ySlice) {
    int dims[3];
    d->manager->GetCurrentPosition(dims);
    dims[1]=ySlice;
    d->manager->SetCurrentPosition((int*)dims);
    d->view->update();
}

void v3dViewSHInteractor::setZSlice(int zSlice) {
    int dims[3];
    d->manager->GetCurrentPosition(dims);
    dims[2]=zSlice;
    d->manager->SetCurrentPosition((int*)dims);
    d->view->update();
}

void v3dViewSHInteractor::setShowAxial(bool show) {
    const int visible = show ? 1 : 0;
    d->manager->SetAxialSliceVisibility(visible);
    d->view->update();
}

void v3dViewSHInteractor::setShowCoronal(bool show) {
    const int visible = show ? 1 : 0;
    d->manager->SetCoronalSliceVisibility(visible);
    d->view->update();
}

void v3dViewSHInteractor::setShowSagittal(bool show) {
    const int visible = show ? 1 : 0;
    d->manager->SetSagittalSliceVisibility(visible);
    d->view->update();
}

void v3dViewSHInteractor::setFlipX(const bool flipX) {
    d->manager->FlipX(flipX);
    d->view->update();
}

void v3dViewSHInteractor::setFlipY(const bool flipY) {
    d->manager->FlipY(flipY);
    d->view->update();
}

void v3dViewSHInteractor::setFlipZ(const bool flipZ) {
    d->manager->FlipZ(flipZ);
    d->view->update();
}

void v3dViewSHInteractor::setColorGlyphs(const bool ColorGlyph) {
    d->manager->ColorGlyphs(ColorGlyph);
    d->view->update();
}

void v3dViewSHInteractor::setNormalization(const bool Norma) {
    d->manager->Normalization(Norma);
    d->view->update();
}

void v3dViewSHInteractor::setPosition(const QVector3D& position,bool propagate) {
    d->manager->SetCurrentPosition(position.x(),position.y(),position.z());
    d->view->update();
}

void v3dViewSHInteractor::setOpacity(dtkAbstractData * /*data*/, double /*opacity*/)
{
    //TODO
}

double v3dViewSHInteractor::opacity(dtkAbstractData * /*data*/) const
{
    //TODO
    return 100;
}

void v3dViewSHInteractor::setVisible(dtkAbstractData * /*data*/, bool /*visible*/)
{
    //TODO
}

bool v3dViewSHInteractor::isVisible(dtkAbstractData * /*data*/) const
{
    //TODO
    return true;
}

void v3dViewSHInteractor::computeBounds()
{
    d->manager->GetSHVisuManagerAxial()->GetActor()->GetBounds(d->imageBounds);

    updateBounds(d->manager->GetSHVisuManagerSagittal()->GetActor()->GetBounds());
    updateBounds(d->manager->GetSHVisuManagerCoronal()->GetActor()->GetBounds());

    // these bounds are used by vtkImageFromBoundsSource to generate a background image in case there is none
    // vtkImageFromBoundsSource output image size is actually [boundsXMax-boundXMin]...,
    // so we need to increase bounds by +1 to have the correct image size
    d->imageBounds[0] = round(d->imageBounds[0]);
    d->imageBounds[1] = round(d->imageBounds[1])+1;
    d->imageBounds[2] = round(d->imageBounds[2]);
    d->imageBounds[3] = round(d->imageBounds[3])+1;
    d->imageBounds[4] = round(d->imageBounds[4]);
    d->imageBounds[5] = round(d->imageBounds[5])+1;
}

void v3dViewSHInteractor::updateBounds(const double bounds[])
{
    for (int i=0; i<6; i=i+2)
    {
        if (bounds[i] < d->imageBounds[i])
        {
            d->imageBounds[i]=bounds[i];
        }
    }
    for (int i=1; i<6; i=i+2)
    {
        if (bounds[i] > d->imageBounds[i])
        {
            d->imageBounds[i]=bounds[i];
        }
    }
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createV3dViewSHInteractor() {
    return new v3dViewSHInteractor;
}
