/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkViewItkSHImageInteractor.h>

#include <QVTKWidget.h>

#include <vtkSphericalHarmonicManager.h>
#include <vtkStructuredPoints.h>

#include <vtkImageActor.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>

#include <medAbstractData.h>
#include <medAbstractParameter.h>
#include <medStringListParameter.h>
#include <medIntParameter.h>
#include <medBoolParameter.h>
#include <medDoubleParameter.h>
#include <medAbstractImageView.h>
#include <medViewFactory.h>
#include <medVtkViewBackend.h>

#include <itkSphericalHarmonicITKToVTKFilter.h>

#include <itkImage.h>
#include <itkVectorImage.h>

class medVtkViewItkSHImageInteractorPrivate {
public:

    medAbstractData* data;
    medAbstractImageView *view;
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;
    vtkRenderWindow *render;
    vtkMatrix4x4 *orientationMatrix;

    QList <medAbstractParameter*> parameters;
    vtkSphericalHarmonicManager* manager;
    double                       imageBounds[6];

    int minorScaling;
    int majorScalingExponent;

    medIntParameter *slicingParameter;

    typedef vtkSmartPointer <vtkProperty>  PropertySmartPointer;
    PropertySmartPointer actorProperty;

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

        itk::ImageBase<3>::DirectionType directions = dataset->GetDirection();
        itk::ImageBase<3>::PointType origin = dataset->GetOrigin();
        orientationMatrix = vtkMatrix4x4::New();
        orientationMatrix->Identity();
        for (int i=0; i<3; i++)
            for (int j=0; j<3; j++)
                orientationMatrix->SetElement (i, j, directions (i,j));
        double v_origin[4], v_origin2[4];
        for (int i=0; i<3; i++)
            v_origin[i] = origin[i];
        v_origin[3] = 1.0;
        orientationMatrix->MultiplyPoint (v_origin, v_origin2);
        for (int i=0; i<3; i++)
            orientationMatrix->SetElement (i, 3, v_origin[i]-v_origin2[i]);

        double v_spacing[3];
        for (int i=0; i<3; i++)
            v_spacing[i] = dataset->GetSpacing()[i];

        //  We need to call this function because GetOutput() just returns the input

        manager->SetInput(filter->GetVTKSphericalHarmonic());
        manager->SetDirectionMatrix(filter->GetDirectionMatrix());

        manager->ResetPosition();

        const int number = dataset->GetNumberOfComponentsPerPixel();
        const int Order  = -1.5+std::sqrt((float)(0.25+2*number));
        manager->SetOrder(Order);

        manager->Update();

        data = d;

        if (view)
        {
            int dim[3];
            manager->GetSphericalHarmonicDimensions(dim);
            view2d->SetInput(manager->GetSHVisuManagerAxial()->GetActor(), view->layer(data), orientationMatrix, dim, v_spacing, v_origin);
            view2d->SetInput(manager->GetSHVisuManagerSagittal()->GetActor(), view->layer(data), orientationMatrix, dim, v_spacing, v_origin);
            view2d->SetInput(manager->GetSHVisuManagerCoronal()->GetActor(), view->layer(data), orientationMatrix, dim, v_spacing, v_origin);
        }
    }
};

medVtkViewItkSHImageInteractor::medVtkViewItkSHImageInteractor(medAbstractView *parent):
    medAbstractImageViewInteractor(parent),
    d(new medVtkViewItkSHImageInteractorPrivate) {

    d->data    = 0;

    d->view = dynamic_cast<medAbstractImageView*>(parent);

    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;
    d->render = backend->renWin;
    d->orientationMatrix = 0;

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

medVtkViewItkSHImageInteractor::~medVtkViewItkSHImageInteractor() {

    delete d;
    d = 0;
}

QString medVtkViewItkSHImageInteractor::description() const {
    return tr("Interactor displaying 3D Spherical Harmonics (ODF ADC obtained from dmri)");
}

QString medVtkViewItkSHImageInteractor::identifier() const {
    return "medVtkViewItkSHImageInteractor";
}

QStringList medVtkViewItkSHImageInteractor::handled() const {
    return medVtkViewItkSHImageInteractor::dataHandled();
}

QStringList medVtkViewItkSHImageInteractor::dataHandled()
{
    QStringList datahandled = QStringList() << "medItkDouble3SHImageData" << "medItkFloat3SHImageData";
    return  datahandled;
}

bool medVtkViewItkSHImageInteractor::registered()
{
    medViewFactory *factory = medViewFactory::instance();
    factory->registerInteractor<medVtkViewItkSHImageInteractor>("medVtkViewItkSHImageInteractor",
                                                                     QStringList () << "medVtkView" <<
                                                                     medVtkViewItkSHImageInteractor::dataHandled());
    return true;
}

void medVtkViewItkSHImageInteractor::setInputData(medAbstractData *data)
{
    medAbstractInteractor::setInputData(data);

    if (!data || !data->data())
        return;

    //  Two itk SH image formats are supported
    //  we need to convert them to vtkStructuredPoints so it's understood by the SH manager

    const QString& identifier = data->identifier();
    if (identifier=="medItkFloat3SHImageData")
        d->setVTKFilter<itk::VectorImage<float,3> >(data,d->filterFloat);
    else if (identifier=="medItkDouble3SHImageData")
        d->setVTKFilter<itk::VectorImage<double,3> >(data,d->filterDouble);
    else
    {
        qDebug() << "Unrecognized SH data type: " << identifier;
        return;
    }

    d->actorProperty = itkDataSHImageVtkViewInteractorPrivate::PropertySmartPointer::New();
    d->manager->GetSHVisuManagerAxial()->GetActor()->SetProperty( d->actorProperty );
    d->manager->GetSHVisuManagerSagittal()->GetActor()->SetProperty( d->actorProperty );
    d->manager->GetSHVisuManagerCoronal()->GetActor()->SetProperty( d->actorProperty );

    setupParameters();
}

void medVtkViewItkSHImageInteractor::removeData()
{
    d->view2d->RemoveLayer(d->view->layer(this->inputData()));
    d->manager->Delete();
}

void medVtkViewItkSHImageInteractor::setupParameters()
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

void medVtkViewItkSHImageInteractor::setWindowLevel(QHash<QString,QVariant>)
{
    //TODO
}

void medVtkViewItkSHImageInteractor::setOpacity(double opacity)
{
    d->actorProperty->SetOpacity(opacity);

    d->view->render();
}

void medVtkViewItkSHImageInteractor::setVisibility(bool visibility)
{
    int v = (visibility) ? 1 : 0;

    d->manager->GetSHVisuManagerAxial()->GetActor()->SetVisibility(v);
    d->manager->GetSHVisuManagerSagittal()->GetActor()->SetVisibility(v);
    d->manager->GetSHVisuManagerCoronal()->GetActor()->SetVisibility(v);

    this->update();
}

void medVtkViewItkSHImageInteractor::imageSize(int* imSize) {
    d->manager->GetSphericalHarmonicDimensions(imSize);
}

void medVtkViewItkSHImageInteractor::setTesselationType(QString tesselationType) {
    if(tesselationType == "Icosahedron")
      d->manager->SetTesselationType(Icosahedron);
    else if(tesselationType == "Octahedron")
        d->manager->SetTesselationType(Octahedron);
    if(tesselationType == "Tetrahedron")
      d->manager->SetTesselationType(Tetrahedron);
    update();
}

void medVtkViewItkSHImageInteractor::setTesselationBasis(QString tesselationBasis) {
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

void medVtkViewItkSHImageInteractor::setSampleRate(int sampleRate) {
    d->manager->SetSampleRate(sampleRate,sampleRate,sampleRate);
    update();
}

void medVtkViewItkSHImageInteractor::setGlyphResolution(int glyphResolution) {
    d->manager->SetGlyphResolution(glyphResolution);
    update();
}

void medVtkViewItkSHImageInteractor::setScale(double scale) {
    d->manager->SetGlyphScale((float)scale);
    update();
}

void medVtkViewItkSHImageInteractor::setMajorScaling(int majorScalingExponent)
{
     d->majorScalingExponent = majorScalingExponent;
     setScale(d->minorScaling, d->majorScalingExponent);
}

void medVtkViewItkSHImageInteractor::setMinorScaling(int minorScaling)
{
     d->minorScaling = minorScaling;
     setScale(d->minorScaling, d->majorScalingExponent);
}

void medVtkViewItkSHImageInteractor::setScale(int minorScale, int majorScaleExponent)
{
    double majorScale = pow(10.0, majorScaleExponent);
    double scale = majorScale * minorScale;
    setScale(scale);
}

void medVtkViewItkSHImageInteractor::setXSlice(int xSlice) {
    int dims[3];
    d->manager->GetCurrentPosition(dims);
    dims[0]=xSlice;
    d->manager->SetCurrentPosition((int*)dims);
    update();
}

void medVtkViewItkSHImageInteractor::setYSlice(int ySlice) {
    int dims[3];
    d->manager->GetCurrentPosition(dims);
    dims[1]=ySlice;
    d->manager->SetCurrentPosition((int*)dims);
    update();
}

void medVtkViewItkSHImageInteractor::setZSlice(int zSlice) {
    int dims[3];
    d->manager->GetCurrentPosition(dims);
    dims[2]=zSlice;
    d->manager->SetCurrentPosition((int*)dims);
    update();
}

void medVtkViewItkSHImageInteractor::setShowAxial(bool show) {
    const int visible = show ? 1 : 0;
    d->manager->SetAxialSliceVisibility(visible);
    update();
}

void medVtkViewItkSHImageInteractor::setShowCoronal(bool show) {
    const int visible = show ? 1 : 0;
    d->manager->SetCoronalSliceVisibility(visible);
    update();
}

void medVtkViewItkSHImageInteractor::setShowSagittal(bool show) {
    const int visible = show ? 1 : 0;
    d->manager->SetSagittalSliceVisibility(visible);
    update();
}

void medVtkViewItkSHImageInteractor::setFlipX(const bool flipX) {
    d->manager->FlipX(flipX);
    update();
}

void medVtkViewItkSHImageInteractor::setFlipY(const bool flipY) {
    d->manager->FlipY(flipY);
    update();
}

void medVtkViewItkSHImageInteractor::setFlipZ(const bool flipZ) {
    d->manager->FlipZ(flipZ);
    update();
}

void medVtkViewItkSHImageInteractor::setColorGlyphs(const bool ColorGlyph) {
    d->manager->ColorGlyphs(ColorGlyph);
    update();
}

void medVtkViewItkSHImageInteractor::setNormalization(const bool Norma) {
    d->manager->Normalization(Norma);
    update();
}

void medVtkViewItkSHImageInteractor::setPosition(const QVector3D& position) {
    d->manager->SetCurrentPosition(position.x(),position.y(),position.z());
    update();
}

void medVtkViewItkSHImageInteractor::setUpViewForThumbnail()
{
    d->view2d->Reset();
    d->view2d->SetBackground(0.0, 0.0, 0.0);
    d->view2d->CursorFollowMouseOff();
    d->view2d->ShowImageAxisOff();
    d->view2d->ShowScalarBarOff();
    d->view2d->ShowAnnotationsOff();
    d->view2d->ShowRulerWidgetOff();
}

void medVtkViewItkSHImageInteractor::moveToSlice(int slice)
{
    //TODO find a way to get woorldCoordinate for slice from vtkInria.
    // instead of moving to the slice corresponding on the first layer dropped.
    if(d->view->is2D() && slice != d->view2d->GetSlice())
    {
        d->view2d->SetSlice(slice);
        d->view2d->Render();
    }
}

QWidget* medVtkViewItkSHImageInteractor::buildLayerWidget()
{
    QSlider *slider = opacityParameter()->getSlider();
    slider->setOrientation(Qt::Horizontal);
    return slider;
}

QWidget* medVtkViewItkSHImageInteractor::buildToolBoxWidget()
{
    QWidget *toolbox = new QWidget;
    QFormLayout *layout = new QFormLayout(toolbox);
    foreach(medAbstractParameter *parameter, d->parameters)
        layout->addRow(parameter->getLabel(), parameter->getWidget());

    return toolbox;
}

QWidget* medVtkViewItkSHImageInteractor::buildToolBarWidget()
{
    d->slicingParameter->getSlider()->setOrientation(Qt::Horizontal);
    return d->slicingParameter->getSlider();
}

QList<medAbstractParameter*> medVtkViewItkSHImageInteractor::linkableParameters()
{
    QList <medAbstractParameter*> linkableParams = d->parameters;
    linkableParams << this->visibilityParameter() << this->opacityParameter();
    return linkableParams;
}

QList<medBoolParameter*> medVtkViewItkSHImageInteractor::mouseInteractionParameters()
{
    // no parameters related to mouse interactions
    return QList<medBoolParameter*>();
}

void medVtkViewItkSHImageInteractor::update()
{
    d->view->render();
}

void medVtkViewItkSHImageInteractor::updateWidgets()
{
    if(!d->view->is2D())
        d->slicingParameter->getSlider()->setEnabled(false);
    else
    {
        d->slicingParameter->getSlider()->setEnabled(true);
        this->updateSlicingParam();
    }
}

void medVtkViewItkSHImageInteractor::updateSlicingParam()
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
