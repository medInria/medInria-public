/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataSHImageVtkViewInteractor.h>

#include <QVTKWidget.h>

#include <vtkSphericalHarmonicManager.h>
#include <vtkStructuredPoints.h>

#include <vtkImageActor.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkRenderer.h>

#include <medAbstractData.h>
#include <medAbstractParameter.h>
#include <medStringListParameter.h>
#include <medIntParameter.h>
#include <medBoolParameter.h>
#include <medAbstractImageView.h>
#include <medViewFactory.h>
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

    QList <medAbstractParameter*> parameters;
    vtkSphericalHarmonicManager* manager;
    double                       imageBounds[6];

    int minorScaling;
    int majorScalingExponent;

    medIntParameter *slicingParameter;

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

itkDataSHImageVtkViewInteractor::itkDataSHImageVtkViewInteractor(medAbstractView *parent):
    medAbstractImageViewInteractor(parent),
    d(new itkDataSHImageVtkViewInteractorPrivate) {

    d->data    = 0;

    d->view = dynamic_cast<medAbstractImageView*>(parent);

    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;
    d->render = backend->renWin;

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

    d->manager->SetRenderWindowInteractor(d->render->GetInteractor(), d->view3d->GetRenderer());

    connect(d->view->positionBeingViewedParameter(), SIGNAL(valueChanged(QVector3D)),
            this,    SLOT(setPosition(QVector3D)));

    d->slicingParameter = new medIntParameter("Slicing", this);

}

itkDataSHImageVtkViewInteractor::~itkDataSHImageVtkViewInteractor() {

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

bool itkDataSHImageVtkViewInteractor::registered()
{
    medViewFactory *factory = medViewFactory::instance();
    factory->registerInteractor<itkDataSHImageVtkViewInteractor>("itkDataSHImageVtkViewInteractor",
                                                                     QStringList () << "medVtkView" <<
                                                                     itkDataSHImageVtkViewInteractor::dataHandled());
    return true;
}

void itkDataSHImageVtkViewInteractor::setInputData(medAbstractData *data)
{
    medAbstractInteractor::setInputData(data);

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

    int dim[3];
    d->manager->GetSphericalHarmonicDimensions(dim);
    d->view2d->SetInput(d->manager->GetSHVisuManagerAxial()->GetActor(), d->view->layer(d->data), dim);
    d->view2d->SetInput(d->manager->GetSHVisuManagerSagittal()->GetActor(), d->view->layer(d->data), dim);
    d->view2d->SetInput(d->manager->GetSHVisuManagerCoronal()->GetActor(), d->view->layer(d->data), dim);

    setupParameters();
}

void itkDataSHImageVtkViewInteractor::removeData()
{
    d->view2d->RemoveLayer(d->view->layer(this->inputData()));
    d->manager->Delete();
}

void itkDataSHImageVtkViewInteractor::setupParameters()
{
    QStringList tesselationTypeList;
    tesselationTypeList << "Icosahedron" << "Octahedron" << "Tetrahedron";
    medStringListParameter *tesselationTypeParam = new medStringListParameter("Tesselation Type", this);
    tesselationTypeParam->addItems(tesselationTypeList);

    //  Combobox to control the spherical Harmonics basis

    QStringList tesselationBasisList;
    tesselationBasisList << "SHMatrix" << "SHMatrixMaxThesis" << "SHMatrixTournier" << "SHMatrixRshBasis";
    medStringListParameter * tesselationBasisParam = new medStringListParameter("Tesselation Basis", this);
    tesselationBasisParam->addItems(tesselationBasisList);

    //  Control sample rate

    medIntParameter *sampleRateParam = new medIntParameter("Sample Rate", this);
    sampleRateParam->setRange(1,10);
    sampleRateParam->setValue(1);


    //  flipX, flipY, flipZ and Enhance checkboxes

    medBoolParameter *flipXParam = new medBoolParameter("FlipX", this);
    medBoolParameter *flipYParam = new medBoolParameter("FlipY", this);
    medBoolParameter *flipZParam = new medBoolParameter("FlipZ", this);
    flipZParam->setValue(true);

    medBoolParameter *enhanceParam = new medBoolParameter("Enhance", this);

    //  Control glyph resolution

    medIntParameter *glyphResolutionParam = new medIntParameter("Resolution", this);
    glyphResolutionParam->setRange(0,10);
    glyphResolutionParam->setValue(2);


    //  We need to calculate one single number for the scale, out of the minor and major scales
    //  scale = minor*10^(major)

    //  Minor scaling

    medIntParameter *minorScalingParam = new medIntParameter("Scale", this);
    minorScalingParam->setRange(0,9);
    minorScalingParam->setValue(3);


    //  Major scaling

    medIntParameter *majorScalingParam = new medIntParameter("x10^", this);
    majorScalingParam->setRange(-10,10);
    majorScalingParam->setValue(0);

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

    if(d->view->layer(d->data) == 0)
    {
        switch(d->view2d->GetViewOrientation())
        {
        case vtkImageView2D::VIEW_ORIENTATION_AXIAL:
            d->view->setOrientation(medImageView::VIEW_ORIENTATION_AXIAL);
            break;
        case vtkImageView2D::VIEW_ORIENTATION_SAGITTAL:
            d->view->setOrientation(medImageView::VIEW_ORIENTATION_SAGITTAL);
            break;
        case vtkImageView2D::VIEW_ORIENTATION_CORONAL:
            d->view->setOrientation(medImageView::VIEW_ORIENTATION_CORONAL);
            break;
        }
    }

    connect(d->slicingParameter, SIGNAL(valueChanged(int)), this, SLOT(moveToSlice(int)));
    connect(d->view->positionBeingViewedParameter(), SIGNAL(valueChanged(QVector3D)), this, SLOT(updateSlicingParam()));
    this->updateWidgets();

}

void itkDataSHImageVtkViewInteractor::setWindowLevel(QHash<QString,QVariant>)
{
    //TODO
}

void itkDataSHImageVtkViewInteractor::setOpacity(double opacity)
{
    //TODO
}

void itkDataSHImageVtkViewInteractor::setVisibility(bool visibility)
{
    int v = (visibility) ? 1 : 0;

    d->manager->GetSHVisuManagerAxial()->GetActor()->SetVisibility(v);
    d->manager->GetSHVisuManagerSagittal()->GetActor()->SetVisibility(v);
    d->manager->GetSHVisuManagerCoronal()->GetActor()->SetVisibility(v);

    this->update();
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

void itkDataSHImageVtkViewInteractor::setUpViewForThumbnail()
{
    d->view2d->Reset();
    d->view2d->SetBackground(0.0, 0.0, 0.0);
    d->view2d->CursorFollowMouseOff();
    d->view2d->ShowImageAxisOff();
    d->view2d->ShowScalarBarOff();
    d->view2d->ShowAnnotationsOff();
    d->view2d->ShowRulerWidgetOff();
}

void itkDataSHImageVtkViewInteractor::moveToSlice(int slice)
{
    //TODO find a way to get woorldCoordinate for slice from vtkInria.
    // instead of moving to the slice corresponding on the first layer dropped.
    if(d->view->is2D() && slice != d->view2d->GetSlice())
    {
        d->view2d->SetSlice(slice);
        d->view2d->Render();
    }
}

QWidget* itkDataSHImageVtkViewInteractor::buildLayerWidget()
{
    return new QWidget;
}

QWidget* itkDataSHImageVtkViewInteractor::buildToolBoxWidget()
{
    QWidget *toolbox = new QWidget;
    QFormLayout *layout = new QFormLayout(toolbox);
    foreach(medAbstractParameter *parameter, d->parameters)
        layout->addRow(parameter->getLabel(), parameter->getWidget());

    return toolbox;
}

QWidget* itkDataSHImageVtkViewInteractor::buildToolBarWidget()
{
    d->slicingParameter->getSlider()->setOrientation(Qt::Horizontal);
    return d->slicingParameter->getSlider();
}

QList<medAbstractParameter*> itkDataSHImageVtkViewInteractor::linkableParameters()
{
    QList <medAbstractParameter*> linkableParams = d->parameters;
    linkableParams << this->visibilityParameter();
    return linkableParams;
}

QList<medBoolParameter*> itkDataSHImageVtkViewInteractor::mouseInteractionParameters()
{
    // no parameters related to mouse interactions
    return QList<medBoolParameter*>();
}

void itkDataSHImageVtkViewInteractor::update()
{
    d->view->render();
}

void itkDataSHImageVtkViewInteractor::updateWidgets()
{
    if(!d->view->is2D())
        d->slicingParameter->getSlider()->setEnabled(false);
    else
    {
        d->slicingParameter->getSlider()->setEnabled(true);
        this->updateSlicingParam();
    }
}

void itkDataSHImageVtkViewInteractor::updateSlicingParam()
{
    if(!d->view->is2D())
        return;

    //TODO Should be set according to the real number of slice of this data and
    // not according to vtkInria (ie. first layer droped) - RDE
    // slice orientation may differ from view orientation. Adapt slider range accordingly.
//    int orientationId = d->view2d->GetSliceOrientation();
//    int dim[3];
//    d->manager->GetSphericalHarmonicDimensions(dim);

//    if (orientationId==vtkImageView2D::SLICE_ORIENTATION_XY)
//        d->slicingParameter->setRange(0, dim[2] - 1);
//    else if (orientationId==vtkImageView2D::SLICE_ORIENTATION_XZ)
//        d->slicingParameter->setRange (0, dim[1] - 1);
//    else if (orientationId==vtkImageView2D::SLICE_ORIENTATION_YZ)
//        d->slicingParameter->setRange (0, dim[0] - 1);

    d->slicingParameter->blockSignals(true);
    d->slicingParameter->setRange(d->view2d->GetSliceMin(), d->view2d->GetSliceMax());
    d->slicingParameter->blockSignals(false);

    d->slicingParameter->setValue(d->view2d->GetSlice());
}

QString itkDataSHImageVtkViewInteractor::name() const
{
    return "itkDataSHImageVtkViewInteractor";
}
