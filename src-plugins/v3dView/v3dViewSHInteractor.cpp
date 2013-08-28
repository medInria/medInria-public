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

v3dViewSHInteractor::v3dViewSHInteractor(): medSHAbstractViewInteractor(),d(new v3dViewSHInteractorPrivate) {

    d->data    = 0;
    d->view    = 0;
    d->manager = vtkSphericalHarmonicManager::New();

    d->filterFloat = 0;
    d->filterDouble = 0;

    //  Set default properties
    d->manager->SetTesselationType(0);
    d->manager->SetTesselationBasis(0);
    d->manager->Normalization(false);

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


void v3dViewSHInteractor::setTesselationType(TesselationType tesselationType) {
    d->manager->SetTesselationType(tesselationType);
}

void v3dViewSHInteractor::setTesselationBasis(TesselationBasis tesselationBasis) {
    d->manager->SetTesselationBasis(tesselationBasis);
}

void v3dViewSHInteractor::setSampleRate(int sampleRate) {
    d->manager->SetSampleRate(sampleRate,sampleRate,sampleRate);
}

void v3dViewSHInteractor::setGlyphResolution(int glyphResolution) {
    d->manager->SetGlyphResolution(glyphResolution);
}

void v3dViewSHInteractor::setScaling(double scale) {
    d->manager->SetGlyphScale((float)scale);
}

void v3dViewSHInteractor::setXSlice(int xSlice) {
    int dims[3];
    d->manager->GetCurrentPosition(dims);
    dims[0]=xSlice;
    d->manager->SetCurrentPosition((int*)dims);
}

void v3dViewSHInteractor::setYSlice(int ySlice) {
    int dims[3];
    d->manager->GetCurrentPosition(dims);
    dims[1]=ySlice;
    d->manager->SetCurrentPosition((int*)dims);
}

void v3dViewSHInteractor::setZSlice(int zSlice) {
    int dims[3];
    d->manager->GetCurrentPosition(dims);
    dims[2]=zSlice;
    d->manager->SetCurrentPosition((int*)dims);
}

void v3dViewSHInteractor::setShowAxial(bool show) {
    const int visible = show ? 1 : 0;
    d->manager->SetAxialSliceVisibility(visible);
}

void v3dViewSHInteractor::setShowCoronal(bool show) {
    const int visible = show ? 1 : 0;
    d->manager->SetCoronalSliceVisibility(visible);
}

void v3dViewSHInteractor::setShowSagittal(bool show) {
    const int visible = show ? 1 : 0;
    d->manager->SetSagittalSliceVisibility(visible);
}

void v3dViewSHInteractor::setFlipX(const bool flipX) {
    d->manager->FlipX(flipX);
}

void v3dViewSHInteractor::setFlipY(const bool flipY) {
    d->manager->FlipY(flipY);
}

void v3dViewSHInteractor::setFlipZ(const bool flipZ) {
    d->manager->FlipZ(flipZ);
}

void v3dViewSHInteractor::setColorGlyphs(const bool ColorGlyph) {
    d->manager->ColorGlyphs(ColorGlyph);
}

void v3dViewSHInteractor::setNormalization(const bool Norma) {
    d->manager->Normalization(Norma);
}

void v3dViewSHInteractor::setPosition(const QVector3D& position,bool propagate) {
    d->manager->SetCurrentPosition(position.x(),position.y(),position.z());
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createV3dViewSHInteractor() {
    return new v3dViewSHInteractor;
}
