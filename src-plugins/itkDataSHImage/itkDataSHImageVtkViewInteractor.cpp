/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataSHImageVtkViewInteractor.h>

#include <medAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>

#include <vtkSphericalHarmonicManager.h>
#include <vtkStructuredPoints.h>

#include <vtkImageActor.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkRenderer.h>

#include <medAbstractParameter.h>
#include <medStringListParameter.h>
#include <medIntParameter.h>
#include <medBoolParameter.h>
#include <medColorListParameter.h>
#include <medAbstractImageView.h>
#include <medImageViewFactory.h>
#include <medVtkViewBackend.h>

#include <itkSphericalHarmonicITKToVTKFilter.h>

#include <itkImage.h>
#include <itkVectorImage.h>

class itkDataSHImageVtkViewInteractorPrivate {
public:

    medAbstractData* data;
    medAbstractImageView *view;
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;
    vtkRenderWindow *render;
    vtkRenderer *renderer2d;
    vtkRenderer *renderer3d;

    QWidget *toolbox;

    QList <medAbstractParameter*> parameters;

    QImage thumbnail;

    vtkSphericalHarmonicManager* manager;
    double                       imageBounds[6];

    int minorScaling;
    int majorScalingExponent;

    //  The filters will convert from itk SH image format to vtkStructuredPoint (format handled by the SH manager)

    itk::SphericalHarmonicITKToVTKFilter<itk::VectorImage<float,3> >::Pointer  filterFloat;
    itk::SphericalHarmonicITKToVTKFilter<itk::VectorImage<double,3> >::Pointer filterDouble;

    template <typename SH_IMAGE>
    void setVTKFilter(medAbstractData* d,typename itk::SphericalHarmonicITKToVTKFilter<SH_IMAGE>::Pointer& filter) {

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

itkDataSHImageVtkViewInteractor::itkDataSHImageVtkViewInteractor(medAbstractImageView* parent):
    medAbstractImageViewInteractor(parent),
    d(new itkDataSHImageVtkViewInteractorPrivate) {

    d->data    = 0;

    d->view = parent;

    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;
    d->render = backend->renWin;
    d->renderer2d = backend->renderer2D;
    d->renderer3d = backend->renderer3D;

    d->manager = vtkSphericalHarmonicManager::New();

    d->filterFloat = 0;
    d->filterDouble = 0;

    d->minorScaling = 1;
    d->majorScalingExponent = 0;

    //  Set default properties
    d->manager->SetTesselationType(0);
    d->manager->SetTesselationBasis(0);
    d->manager->Normalization(false);

    for (int i=0; i<6; i++)
        d->imageBounds[i] = 0;

    d->manager->SetRenderWindowInteractor(d->render->GetInteractor(),d->renderer3d);

    connect(d->view, SIGNAL(positionViewedChanged(QVector3D)),
            this,    SLOT(setPosition(QVector3D)));

    d->toolbox = NULL;
}

itkDataSHImageVtkViewInteractor::~itkDataSHImageVtkViewInteractor() {

    d->manager->Delete();
    delete d;
    d = 0;
}

QString itkDataSHImageVtkViewInteractor::description() const {
    return tr("Interactor displaying 3D Spherical Harmonics (ODF ADC obtained from dmri)");
}

QString itkDataSHImageVtkViewInteractor::identifier() const {
    return "itkDataSHImageVtkViewInteractor";
}

QStringList itkDataSHImageVtkViewInteractor::handled() const {
    return itkDataSHImageVtkViewInteractor::dataHandled();
}

QStringList itkDataSHImageVtkViewInteractor::dataHandled()
{
    QStringList datahandled = QStringList() << "itkDataSHImageDouble3" << "itkDataSHImageFloat3";
    return  datahandled;
}

bool itkDataSHImageVtkViewInteractor::registered() {
    medImageViewFactory *factory = medImageViewFactory::instance();
    factory->registerInteractor<itkDataSHImageVtkViewInteractor>("itkDataSHImageVtkViewInteractor",
                                                                     QStringList () << "medVtkView" <<
                                                                     itkDataSHImageVtkViewInteractor::dataHandled());
    return true;
}

void itkDataSHImageVtkViewInteractor::setData(medAbstractData *data) {

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

    d->renderer2d->AddActor(d->manager->GetSHVisuManagerAxial()->GetActor());
    d->renderer2d->AddActor(d->manager->GetSHVisuManagerSagittal()->GetActor());
    d->renderer2d->AddActor(d->manager->GetSHVisuManagerCoronal()->GetActor());

    if(d->view->layersCount() == 0)
    {
        int imSize[3];
        imageSize(imSize);
        computeBounds();
        //d->view->changeBounds(d->imageBounds, imSize);
    }

    setupParameters();
}

void itkDataSHImageVtkViewInteractor::removeData()
{
    d->manager->Delete();
}

void itkDataSHImageVtkViewInteractor::setupParameters()
{
    QStringList tesselationTypeList;
    tesselationTypeList << "Icosahedron" << "Octahedron" << "Tetrahedron";
    medStringListParameter *tesselationTypeParam = new medStringListParameter("Tesselation Type");
    tesselationTypeParam->addItems(tesselationTypeList);

    //  Combobox to control the spherical Harmonics basis

    QStringList tesselationBasisList;
    tesselationBasisList << "SHMatrix" << "SHMatrixMaxThesis" << "SHMatrixTournier" << "SHMatrixRshBasis";
    medStringListParameter * tesselationBasisParam = new medStringListParameter("Tesselation Basis");
    tesselationBasisParam->addItems(tesselationBasisList);

    //  Control sample rate

    medIntParameter *sampleRateParam = new medIntParameter("Sample Rate");
    sampleRateParam->setRange(1,10);
    sampleRateParam->setValue(1);


    //  flipX, flipY, flipZ and Enhance checkboxes

    medBoolParameter *flipXParam = new medBoolParameter("FlipX");
    medBoolParameter *flipYParam = new medBoolParameter("FlipY");
    medBoolParameter *flipZParam = new medBoolParameter("FlipZ");
    flipZParam->setValue(true);

    medBoolParameter *enhanceParam = new medBoolParameter("Enhance");

    //  Control glyph resolution

    medIntParameter *glyphResolutionParam = new medIntParameter("Resolution");
    glyphResolutionParam->setRange(0,10);
    glyphResolutionParam->setValue(2);


    //  We need to calculate one single number for the scale, out of the minor and major scales
    //  scale = minor*10^(major)

    //  Minor scaling

    medIntParameter *minorScalingParam = new medIntParameter("Scale");
    minorScalingParam->setRange(0,9);
    minorScalingParam->setValue(3);


    //  Major scaling

    medIntParameter *majorScalingParam = new medIntParameter("x10^");
    majorScalingParam->setRange(-10,10);
    majorScalingParam->setValue(0);


    //  Hide or show axial, coronal, and sagittal

    medBoolParameter *showAxialParam = new medBoolParameter("Show axial");
    showAxialParam->setValue(true);
    medBoolParameter *showCoronalParam = new medBoolParameter("Show coronal");
    showCoronalParam->setValue(true);
    medBoolParameter *showSagittalParam = new medBoolParameter("Show sagittal");
    showSagittalParam->setValue(true);


    d->parameters.append(tesselationTypeParam);
    d->parameters.append(tesselationBasisParam);
    d->parameters.append(sampleRateParam);
    d->parameters.append(flipXParam);
    d->parameters.append(flipYParam);
    d->parameters.append(flipZParam);
    d->parameters.append(enhanceParam);
    d->parameters.append(glyphResolutionParam);
    d->parameters.append(minorScalingParam);
    d->parameters.append(majorScalingParam);
    d->parameters.append(showAxialParam);
    d->parameters.append(showCoronalParam);
    d->parameters.append(showSagittalParam);

    connect(tesselationTypeParam, SIGNAL(valueChanged(QString)), this, SLOT(setTesselationType(QString)));
    connect(tesselationBasisParam, SIGNAL(valueChanged(QString)), this, SLOT(setTesselationBasis(QString)));
    connect(sampleRateParam, SIGNAL(valueChanged(int)), this, SLOT(setSampleRate(int)));
    connect(flipXParam, SIGNAL(valueChanged(bool)), this, SLOT(setFlipX(bool)));
    connect(flipYParam, SIGNAL(valueChanged(bool)), this, SLOT(setFlipY(bool)));
    connect(flipZParam, SIGNAL(valueChanged(bool)), this, SLOT(setFlipZ(bool)));
    connect(enhanceParam, SIGNAL(valueChanged(bool)), this, SLOT(setNormalization(bool)));
    connect(glyphResolutionParam, SIGNAL(valueChanged(int)), this, SLOT(setGlyphResolution(int)));
    connect(minorScalingParam, SIGNAL(valueChanged(int)), this, SLOT(setMinorScaling(int)));
    connect(majorScalingParam, SIGNAL(valueChanged(int)), this, SLOT(setMajorScaling(int)));
    connect(showAxialParam, SIGNAL(valueChanged(bool)), this, SLOT(setShowAxial(bool)));
    connect(showCoronalParam, SIGNAL(valueChanged(bool)), this, SLOT(setShowCoronal(bool)));
    connect(showSagittalParam, SIGNAL(valueChanged(bool)), this, SLOT(setShowSagittal(bool)));

}

void itkDataSHImageVtkViewInteractor::setWindowLevel(double &window, double &level)
{
    //TODO
}

void itkDataSHImageVtkViewInteractor::windowLevel(double &window, double &level)
{
    //TODO
}

void itkDataSHImageVtkViewInteractor::setOpacity(double opacity)
{
    //TODO
}

double itkDataSHImageVtkViewInteractor::opacity() const
{
    //TODO
    return 100;
}

void itkDataSHImageVtkViewInteractor::setVisibility(bool visibility)
{
    int v = (visibility) ? 1 : 0;

    d->manager->GetSHVisuManagerAxial()->GetActor()->SetVisibility(v);
    d->manager->GetSHVisuManagerSagittal()->GetActor()->SetVisibility(v);
    d->manager->GetSHVisuManagerCoronal()->GetActor()->SetVisibility(v);

    this->update();
}

bool itkDataSHImageVtkViewInteractor::visibility() const
{
    return (d->manager->GetSHVisuManagerAxial()->GetActor()->GetVisibility() == 1);
}

void itkDataSHImageVtkViewInteractor::imageSize(int* imSize) {
    d->manager->GetSphericalHarmonicDimensions(imSize);
}

void itkDataSHImageVtkViewInteractor::setTesselationType(QString tesselationType) {
    if(tesselationType == "Icosahedron")
      d->manager->SetTesselationType(Icosahedron);
    else if(tesselationType == "Octahedron")
        d->manager->SetTesselationType(Octahedron);
    if(tesselationType == "Tetrahedron")
      d->manager->SetTesselationType(Tetrahedron);
    update();
}

void itkDataSHImageVtkViewInteractor::setTesselationBasis(QString tesselationBasis) {
    if(tesselationBasis == "SHMatrix")
      d->manager->SetTesselationBasis(SHMatrix);
    else if(tesselationBasis == "SHMatrixMaxThesis")
      d->manager->SetTesselationBasis(SHMatrixMaxThesis);
    else if(tesselationBasis == "SHMatrixTournier")
      d->manager->SetTesselationBasis(SHMatrixTournier);
    else if(tesselationBasis == "SHMatrixRshBasis")
      d->manager->SetTesselationBasis(SHMatrixRshBasis);
    update();
}

void itkDataSHImageVtkViewInteractor::setSampleRate(int sampleRate) {
    d->manager->SetSampleRate(sampleRate,sampleRate,sampleRate);
    update();
}

void itkDataSHImageVtkViewInteractor::setGlyphResolution(int glyphResolution) {
    d->manager->SetGlyphResolution(glyphResolution);
    update();
}

void itkDataSHImageVtkViewInteractor::setScale(double scale) {
    d->manager->SetGlyphScale((float)scale);
    update();
}

void itkDataSHImageVtkViewInteractor::setMajorScaling(int majorScalingExponent)
{
     d->majorScalingExponent = majorScalingExponent;
     setScale(d->minorScaling, d->majorScalingExponent);
}

void itkDataSHImageVtkViewInteractor::setMinorScaling(int minorScaling)
{
     d->minorScaling = minorScaling;
     setScale(d->minorScaling, d->majorScalingExponent);
}

void itkDataSHImageVtkViewInteractor::setScale(int minorScale, int majorScaleExponent)
{
    double majorScale = pow(10.0, majorScaleExponent);
    double scale = majorScale * minorScale;
    setScale(scale);
}

void itkDataSHImageVtkViewInteractor::setXSlice(int xSlice) {
    int dims[3];
    d->manager->GetCurrentPosition(dims);
    dims[0]=xSlice;
    d->manager->SetCurrentPosition((int*)dims);
    update();
}

void itkDataSHImageVtkViewInteractor::setYSlice(int ySlice) {
    int dims[3];
    d->manager->GetCurrentPosition(dims);
    dims[1]=ySlice;
    d->manager->SetCurrentPosition((int*)dims);
    update();
}

void itkDataSHImageVtkViewInteractor::setZSlice(int zSlice) {
    int dims[3];
    d->manager->GetCurrentPosition(dims);
    dims[2]=zSlice;
    d->manager->SetCurrentPosition((int*)dims);
    update();
}

void itkDataSHImageVtkViewInteractor::setShowAxial(bool show) {
    const int visible = show ? 1 : 0;
    d->manager->SetAxialSliceVisibility(visible);
    update();
}

void itkDataSHImageVtkViewInteractor::setShowCoronal(bool show) {
    const int visible = show ? 1 : 0;
    d->manager->SetCoronalSliceVisibility(visible);
    update();
}

void itkDataSHImageVtkViewInteractor::setShowSagittal(bool show) {
    const int visible = show ? 1 : 0;
    d->manager->SetSagittalSliceVisibility(visible);
    update();
}

void itkDataSHImageVtkViewInteractor::setFlipX(const bool flipX) {
    d->manager->FlipX(flipX);
    update();
}

void itkDataSHImageVtkViewInteractor::setFlipY(const bool flipY) {
    d->manager->FlipY(flipY);
    update();
}

void itkDataSHImageVtkViewInteractor::setFlipZ(const bool flipZ) {
    d->manager->FlipZ(flipZ);
    update();
}

void itkDataSHImageVtkViewInteractor::setColorGlyphs(const bool ColorGlyph) {
    d->manager->ColorGlyphs(ColorGlyph);
    update();
}

void itkDataSHImageVtkViewInteractor::setNormalization(const bool Norma) {
    d->manager->Normalization(Norma);
    update();
}

void itkDataSHImageVtkViewInteractor::setPosition(const QVector3D& position) {
    d->manager->SetCurrentPosition(position.x(),position.y(),position.z());
    update();
}

void itkDataSHImageVtkViewInteractor::computeBounds()
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

void itkDataSHImageVtkViewInteractor::updateBounds(const double bounds[])
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

QImage itkDataSHImageVtkViewInteractor::generateThumbnail(const QSize &size)
{
    int w(size.width()), h(size.height());

    d->view2d->SetBackground(0.0, 0.0, 0.0);
    d->view2d->CursorFollowMouseOff();
    d->view2d->ShowImageAxisOff();
    d->view2d->ShowScalarBarOff();
    d->view2d->ShowAnnotationsOff();
    d->view2d->ShowRulerWidgetOff();

    d->render->SetOffScreenRendering(1);

    d->view->viewWidget()->resize(w,h);
    d->render->vtkRenderWindow::SetSize(w,h);
    d->view2d->Reset();
    d->view2d->Render();

    QGLWidget *glWidget = dynamic_cast<QGLWidget *>(d->view->viewWidget());
    d->thumbnail = glWidget->grabFrameBuffer();

    d->render->SetOffScreenRendering(0);

    return d->thumbnail;
}

void itkDataSHImageVtkViewInteractor::moveToSliceAtPosition(const QVector3D &position)
{
    //TODO
}

void itkDataSHImageVtkViewInteractor::moveToSlice(int slice)
{
    //TODO
}

QWidget* itkDataSHImageVtkViewInteractor::layerWidget()
{
    return new QWidget;
}

QWidget* itkDataSHImageVtkViewInteractor::toolBoxWidget()
{
    if(!d->toolbox)
    {
        d->toolbox = new QWidget;
        QFormLayout *layout = new QFormLayout(d->toolbox);
        foreach(medAbstractParameter *parameter, d->parameters)
            layout->addRow(parameter->getLabel(), parameter->getWidget());
    }
    return d->toolbox;
}

QWidget* itkDataSHImageVtkViewInteractor::toolBarWidget()
{
    return new QWidget;
}

QList<medAbstractParameter*> itkDataSHImageVtkViewInteractor::parameters()
{
    return d->parameters;
}

void itkDataSHImageVtkViewInteractor::update()
{
    d->render->Render();
}
