/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkViewVtkMeshInteractor.h>

#include <QVTKWidget.h>

#include <vtkCamera.h>
#include <vtkActor.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkImageView.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkPointSet.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkBoundingBox.h>
#include <vtkImageFromBoundsSource.h>
#include <vtkImageData.h>
#include <vtkImageActor.h>
#include <vtkLookupTableManager.h>
#include <vtkLookupTable.h>
#include <vtkMetaDataSet.h>
#include <vtkMetaDataSetSequence.h>
#include <vtkDataArrayCollection.h>

#include <dtkLog/dtkLog.h>

#include <medAbstractData.h>
#include <medAbstractParameter.h>
#include <medBoolParameter.h>
#include <medDoubleParameter.h>
#include <medStringListParameter.h>
#include <medAbstractData.h>
#include <medViewFactory.h>
#include <medAbstractImageView.h>
#include <medIntParameter.h>
#include <medVtkViewBackend.h>

#include <vector>

typedef QPair<vtkSmartPointer<vtkLookupTable>, QString> LutPair;

class medVtkViewVtkMeshInteractorPrivate
{
public:
    typedef vtkSmartPointer <vtkActor>  ActorSmartPointer;
    typedef vtkSmartPointer <vtkProperty>  PropertySmartPointer;
    typedef vtkSmartPointer <vtkDataArray>  AttributeSmartPointer;

    // views
    medAbstractImageView *view;
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;
    medAbstractData *data;

    vtkMetaDataSet* metaDataSet;
    ActorSmartPointer actor2d;
    ActorSmartPointer actor3d;
    PropertySmartPointer actorProperty;
    LutPair lut;
    AttributeSmartPointer attribute;
    double imageBounds[6];

    medStringListParameter *attributesParam;
    medStringListParameter *LUTParam;
    medBoolParameter *edgeVisibleParam;
    medStringListParameter *colorParam;
    medStringListParameter *renderingParam;

    QList <medAbstractParameter*> parameters;

    medIntParameter *slicingParameter;
};


medVtkViewVtkMeshInteractor::medVtkViewVtkMeshInteractor(medAbstractView *parent):
    medAbstractImageViewInteractor(parent), d(new medVtkViewVtkMeshInteractorPrivate)
{
    d->view = dynamic_cast<medAbstractImageView*>(parent);

    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;

    for (int i=0; i<6; i++)
        d->imageBounds[i] = 0;

    d->attributesParam = NULL;
    d->LUTParam = NULL;
    d->edgeVisibleParam = NULL;
    d->colorParam = NULL;
    d->renderingParam = NULL;
    d->slicingParameter = NULL;
}


medVtkViewVtkMeshInteractor::~medVtkViewVtkMeshInteractor()
{
    delete d;
    d = NULL;
}


QString medVtkViewVtkMeshInteractor::description() const
{
    return tr("Interactor displaying Meshes");
}


QString medVtkViewVtkMeshInteractor::identifier() const
{
    return "medVtkViewVtkMeshInteractor";
}


QStringList medVtkViewVtkMeshInteractor::handled() const
{
    return medVtkViewVtkMeshInteractor::dataHandled();
}

QStringList medVtkViewVtkMeshInteractor::dataHandled()
{
    QStringList d = QStringList() << "medVtkMeshData";
    return  d;
}

bool medVtkViewVtkMeshInteractor::registered()
{
    medViewFactory *factory = medViewFactory::instance();
    return factory->registerInteractor<medVtkViewVtkMeshInteractor>("medVtkViewVtkMeshInteractor",
                                                                  QStringList () << "medVtkView" <<
                                                                  medVtkViewVtkMeshInteractor::dataHandled());
}


void medVtkViewVtkMeshInteractor::setInputData(medAbstractData *data)
{
    medAbstractInteractor::setInputData(data);

    vtkMetaDataSet * mesh = dynamic_cast<vtkMetaDataSet*>((vtkDataObject *)(data->data()));

    d->metaDataSet = mesh;
    d->lut = LutPair(NULL, "Default");

    updatePipeline();

    setupParameters();
}

void medVtkViewVtkMeshInteractor::setupParameters()
{
    d->parameters << this->opacityParameter();

    if(!(d->metaDataSet->GetType() != vtkMetaDataSet::VTK_META_SURFACE_MESH &&
         d->metaDataSet->GetType() != vtkMetaDataSet::VTK_META_VOLUME_MESH))
    {
        d->LUTParam = new medStringListParameter("LUT", this);
        d->LUTParam->addItem("Default");
        // TODO some LUT are missing for some attribute - RDE
        connect(d->LUTParam, SIGNAL(valueChanged(QString)), this, SLOT(setLut(QString)));
        d->LUTParam->setValue("Default");
        d->parameters << d->LUTParam;

        d->attributesParam = new medStringListParameter("Attributes", this);
        QStringList nameList("Default");

        for (int i = 0; i < d->metaDataSet->GetDataSet()->GetPointData()->GetNumberOfArrays(); i++)
            nameList << QString::fromUtf8(d->metaDataSet->GetDataSet()->GetPointData()->GetArrayName(i));

        for (int i = 0;  i < d->metaDataSet->GetDataSet()->GetCellData()->GetNumberOfArrays(); i++)
            nameList << QString::fromUtf8(d->metaDataSet->GetDataSet()->GetCellData()->GetArrayName(i));

        for (int i = 0;  i < d->metaDataSet->GetDataSet()->GetFieldData()->GetNumberOfArrays(); i++)
            nameList << QString::fromUtf8(d->metaDataSet->GetDataSet()->GetFieldData()->GetArrayName(i));

        foreach(QString name, nameList)
            d->attributesParam->addItem(name);

        connect(d->attributesParam, SIGNAL(valueChanged(QString)), this, SLOT(setAttribute(QString)));
        d->attributesParam->setValue("Default");
        d->parameters << d->attributesParam;
    }

    d->edgeVisibleParam = new medBoolParameter("Edge Visible", this);
    connect(d->edgeVisibleParam, SIGNAL(valueChanged(bool)), this, SLOT(setEdgeVisibility(bool)));
    d->edgeVisibleParam->setValue(false);
    d->parameters << d->edgeVisibleParam;

    d->colorParam = new medStringListParameter("Color", this);
    QStringList colors;

    colors << "#FFFFFF";
    colors << "#808080";
    colors << "#800000";
    colors << "#804040";
    colors << "#FF8080";
    colors << "#FF0000";
    colors << "#FFFF80";
    colors << "#FFFF00";
    colors << "#FF8040";
    colors << "#FF8000";
    colors << "#80FF80";
    colors << "#80FF00";
    colors << "#00FF00";
    colors << "#80FFFF";
    colors << "#00FFFF";
    colors << "#004080";
    colors << "#0000FF";
    colors << "#0080FF";
    colors << "#0080C0";

    foreach(QString color, colors)
        d->colorParam->addItem(color, medStringListParameter::createIconFromColor(color));

    connect(d->colorParam, SIGNAL(valueChanged(QString)), this, SLOT(setColor(QString)));
    d->colorParam->setValue("#FFFFFF");
    d->parameters << d->colorParam;

    d->renderingParam = new medStringListParameter("Rendering", this);
    d->renderingParam->addItem("WireFrame");
    d->renderingParam->addItem("Surface");
    d->renderingParam->addItem("Points");
    connect(d->renderingParam, SIGNAL(valueChanged(QString)), this, SLOT(setRenderingType(QString)));
    d->renderingParam->setValue("Surface");
    d->parameters << d->renderingParam;

    d->slicingParameter = new medIntParameter("Slicing", this);
    connect(d->slicingParameter, SIGNAL(valueChanged(int)), this, SLOT(moveToSlice(int)));
    connect(d->view->positionBeingViewedParameter(), SIGNAL(valueChanged(QVector3D)), this, SLOT(updateSlicingParam()));

    d->parameters << this->visibilityParameter();

    this->updateWidgets();
}

void medVtkViewVtkMeshInteractor::setOpacity(double value)
{
    d->actorProperty->SetOpacity(value);

    d->view->render();
}

void medVtkViewVtkMeshInteractor::setVisibility(bool visible)
{
    int v = (visible) ? 1 : 0;
    d->actor2d->SetVisibility(v);
    d->actor3d->SetVisibility(v);

    d->view->render();
}

void medVtkViewVtkMeshInteractor::setEdgeVisibility(bool visible)
{
    int v = (visible) ? 1 : 0;
    d->actorProperty->SetEdgeVisibility(v);

    d->view->render();
}


bool medVtkViewVtkMeshInteractor::edgeVisibility() const
{
    return (d->actorProperty->GetEdgeVisibility() == 1);
}


void medVtkViewVtkMeshInteractor::setColor(QColor color)
{
    if( ! color.isValid())
        color.setRgb(0,0,0);

    double r,g,b;
    color.getRgbF(&r, &g, &b);
    d->actorProperty->SetColor(r, g, b);

    d->view->render();
}

void medVtkViewVtkMeshInteractor::setColor(const QString &color)
{
    setColor(QColor(color));
}

QColor medVtkViewVtkMeshInteractor::color() const
{
    double r,g,b;
    d->actorProperty->GetColor(r, g, b);
    return QColor::fromRgbF(r, b, g);
}


void medVtkViewVtkMeshInteractor::setRenderingType(const QString & type)
{
    QString value = type.toLower();
    if (value == "wireframe")
        d->actorProperty->SetRepresentationToWireframe ();
    else if (value == "surface")
        d->actorProperty->SetRepresentationToSurface ();
    else if (value == "points")
        d->actorProperty->SetRepresentationToPoints ();

    d->view->render();
}


QString medVtkViewVtkMeshInteractor::renderingType() const
{
    return QString::fromStdString(d->actorProperty->GetRepresentationAsString()).toLower();
}


void medVtkViewVtkMeshInteractor::setAttribute(const QString & attributeName)
{
    vtkPointSet * pointSet = vtkPointSet::SafeDownCast(d->metaDataSet->GetDataSet());
    if ( ! pointSet )
        return;

    vtkDataSetAttributes * attributes = NULL;
    vtkMapper * mapper2d = d->actor2d->GetMapper();
    vtkMapper * mapper3d = d->actor3d->GetMapper();
    if (pointSet->GetPointData()->HasArray(qPrintable(attributeName)))
    {
        attributes = pointSet->GetPointData();
        mapper2d->SetScalarModeToUsePointFieldData();
        mapper3d->SetScalarModeToUsePointFieldData();
    }
    else if (pointSet->GetCellData()->HasArray(qPrintable(attributeName)))
    {
        attributes = pointSet->GetCellData();
        mapper2d->SetScalarModeToUseCellFieldData();
        mapper3d->SetScalarModeToUseCellFieldData();
    }

    if (attributes)
    {

        if(d->colorParam)
            d->colorParam->hide();
        if(d->LUTParam)
        {
            QStringList luts;
            const std::vector<std::string> & vec = vtkLookupTableManager::GetAvailableLookupTables();
            for(std::vector<std::string>::const_iterator it = vec.begin(); it < vec.end(); ++it)
                luts.append(QString::fromStdString(*it));
            d->LUTParam->clear();
            foreach(QString lut, luts)
                d->LUTParam->addItem(lut);

            d->attribute = attributes->GetArray(qPrintable(attributeName));
            attributes->SetActiveScalars(qPrintable(attributeName));

            d->LUTParam->show();
        }

        mapper2d->SelectColorArray(qPrintable(attributeName));
        mapper3d->SelectColorArray(qPrintable(attributeName));

        this->setLut(d->lut.first);

        mapper2d->SetScalarVisibility(1);
        mapper3d->SetScalarVisibility(1);
    }
    else
    {
        if(d->LUTParam)
            d->LUTParam->hide();
        if(d->colorParam)
            d->colorParam->show();

        d->attribute = NULL;
        mapper2d->SetScalarVisibility(0);
        mapper3d->SetScalarVisibility(0);
    }
    d->view->render();
}


QString medVtkViewVtkMeshInteractor::attribute() const
{
    return QString::fromUtf8(d->actor2d->GetMapper()->GetArrayName());
}


void medVtkViewVtkMeshInteractor::setLut(const QString & lutName)
{
    vtkLookupTable * lut = NULL;

    if (lutName != "Default")
        lut = vtkLookupTableManager::GetLookupTable(lutName.toStdString());

    if ( ! d->attribute)
     return;

    d->lut = LutPair(lut, lutName);
    this->setLut(lut);

    d->view->render();
}


QString medVtkViewVtkMeshInteractor::lut() const
{
    return d->lut.second;
}


void medVtkViewVtkMeshInteractor::updatePipeline ()
{
    if (d->view && d->metaDataSet )
    {
        if(vtkPointSet * pointSet = vtkPointSet::SafeDownCast (d->metaDataSet->GetDataSet()))
        {
            d->actor2d = d->view2d->AddDataSet(pointSet);
            d->actor3d = d->view3d->AddDataSet(pointSet);

            d->actorProperty = medVtkViewVtkMeshInteractorPrivate::PropertySmartPointer::New();
            d->actor2d->SetProperty( d->actorProperty );
            d->actor3d->SetProperty( d->actorProperty );

            d->view2d->UpdateBounds(pointSet->GetBounds(), d->view->layer(this->inputData()));
        }
    }
    d->view3d->ResetCamera();
}


void medVtkViewVtkMeshInteractor::setLut(vtkLookupTable * lut)
{
    if ( ! d->attribute )
        return;

    if ( ! lut )
    {
        lut = d->attribute->GetLookupTable();
        d->lut = LutPair(lut, "Default");
    }

    // remove the alpha channel from the LUT, it messes up the mesh
    if (lut)
    {
        double values[4];
        for(int i = 0; i < lut->GetNumberOfTableValues(); i++)
        {
            lut->GetTableValue(i, values);
            values[3] = 1.0;
            lut->SetTableValue(i, values);
        }

        double * range = d->metaDataSet->GetCurrentScalarRange();
        lut->SetRange(range);
    }

    vtkMapper * mapper2d = d->actor2d->GetMapper();
    vtkMapper * mapper3d = d->actor3d->GetMapper();

    mapper2d->SetLookupTable(lut);
    mapper2d->UseLookupTableScalarRangeOn();
    mapper3d->SetLookupTable(lut);
    mapper3d->UseLookupTableScalarRangeOn();
}


void medVtkViewVtkMeshInteractor::removeData()
{
    if(d->metaDataSet)
    {
        if(vtkPointSet * pointSet = vtkPointSet::SafeDownCast (d->metaDataSet->GetDataSet()))
        {
            d->view2d->RemoveDataSet(pointSet);
            d->view3d->RemoveDataSet(pointSet);
            d->view->render();
        }
    }
}

void medVtkViewVtkMeshInteractor::moveToSlice  (int slice)
{
    //TODO find a way to get woorldCoordinate for slice from vtkInria.
    // instead of moving to the slice corresponding on the first layer dropped.
    if(d->view->is2D() && slice != d->view2d->GetSlice())
    {
        d->view2d->SetSlice(slice);
        d->view2d->Render();
    }
}

void medVtkViewVtkMeshInteractor::setWindowLevel (QHash<QString,QVariant>)
{
    // TODO
}

QWidget* medVtkViewVtkMeshInteractor::buildLayerWidget()
{
    QSlider *slider = this->opacityParameter()->getSlider();
    slider->setOrientation(Qt::Horizontal);
    return slider;
}

QWidget* medVtkViewVtkMeshInteractor::buildToolBoxWidget()
{
    QWidget *toolbox = new QWidget;
    QFormLayout *layout = new QFormLayout(toolbox);
    if(d->attributesParam)
        layout->addRow(d->attributesParam->getLabel(), d->attributesParam->getComboBox());

    layout->addRow(d->LUTParam->getLabel(), d->LUTParam->getComboBox());
    layout->addRow(d->edgeVisibleParam->getLabel(), d->edgeVisibleParam->getCheckBox());
    layout->addRow(d->colorParam->getLabel(), d->colorParam->getComboBox());
    layout->addRow(d->renderingParam->getLabel(), d->renderingParam->getComboBox());

    return toolbox;
}

QWidget* medVtkViewVtkMeshInteractor::buildToolBarWidget()
{
    d->slicingParameter->getSlider()->setOrientation(Qt::Horizontal);
    return d->slicingParameter->getSlider();
}

QList<medAbstractParameter*> medVtkViewVtkMeshInteractor::linkableParameters()
{
    return d->parameters;
}

QList<medBoolParameter*> medVtkViewVtkMeshInteractor::mouseInteractionParameters()
{
    // no parameters related to mouse interactions
    return QList<medBoolParameter*>();
}

void medVtkViewVtkMeshInteractor::setUpViewForThumbnail()
{

    d->view->setOrientation(medImageView::VIEW_ORIENTATION_3D);
    d->view->reset();
    d->view3d->ShowAnnotationsOff();

    //TODO find how to remove the litlle cube at the bottom left corner.
//    d->view3d->ShowActorXOff();
//    d->view3d->ShowActorYOff();
//    d->view3d->ShowActorYOff();
//    d->view3d->ShowBoxWidgetOff();
//    d->view3d->ShowPlaneWidgetOff();
//    d->view3d->ShowScalarBarOff();

}

void medVtkViewVtkMeshInteractor::updateWidgets()
{
    if(!d->view->is2D())
        d->slicingParameter->getSlider()->setEnabled(false);
    else
    {
        d->slicingParameter->getSlider()->setEnabled(true);
        this->updateSlicingParam();
    }
}


void medVtkViewVtkMeshInteractor::updateSlicingParam()
{
    if(!d->view->is2D())
        return;
    //TODO Should be set according to the real number of slice of this data and
    // not according to vtkInria (ie. first layer droped) - RDE

    d->slicingParameter->blockSignals(true);
    d->slicingParameter->setRange(d->view2d->GetSliceMin(), d->view2d->GetSliceMax());
    d->slicingParameter->blockSignals(false);

    d->slicingParameter->setValue(d->view2d->GetSlice());
}
