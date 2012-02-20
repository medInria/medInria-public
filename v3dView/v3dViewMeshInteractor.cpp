#include "v3dViewMeshInteractor.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkLog.h>
#include <vtkCamera.h>
#include <vtkActor.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkImageView.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkPointSet.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkBoundingBox.h>
#include <vtkPolyDataManager.h>
#include "v3dView.h"
#include "vtkImageFromBoundsSource.h"
#include <vtkImageData.h>
#include <QInputDialog>
#include <QColorDialog>
#include <vtkImageActor.h>
#include <vtkLookupTableManager.h>
#include <vtkLookupTable.h>
#include <vtkMetaDataSetSequence.h>
#include <vtkDataArrayCollection.h>

class v3dViewMeshInteractorPrivate
{
public:
    typedef vtkSmartPointer <vtkActor>  ActorSmartPointer;
    typedef vtkSmartPointer <vtkProperty>  PropertySmartPointer;

    QList<dtkAbstractData*> dataList;
    v3dView          *view;
    QList<ActorSmartPointer> actor2dList;
    QList<ActorSmartPointer> actor3dList;
    QList<PropertySmartPointer> actorPropertyList;
    QList<double> opacityList;
    QList<bool> visibilityList;
    QList<bool> edgeVisibilityList;
    QList<QString> colorList;
    QList<QString> renderingList;
    QList<QString> attributeList;
    QList<QString> lutList;
    int currentLayer;
    bool isMeshOnly;
    double ImageBounds[6];
    bool isImageOutBounded;
};

v3dViewMeshInteractor::v3dViewMeshInteractor(): medMeshAbstractViewInteractor(), d(new v3dViewMeshInteractorPrivate)
{
    for (int i=0; i<6; i++)
        d->ImageBounds[i]=0;
    //d->data = NULL;
    d->view = NULL;
    d->currentLayer = 0;
    d->isMeshOnly = false;
    d->isImageOutBounded = false;

    // addProperty here
    this->addProperty("Visibility", QStringList() << "true" << "false");
    this->addProperty("ShowEdges", QStringList() << "true" << "false");
    this->addProperty("RenderingMode", QStringList() << "surface" << "wireframe" << "points" );
    this->addProperty("OpacityMode", QStringList() << "");
    this->addProperty("LUTMode", QStringList() << "Default" << "Black & White" << "Black & White Inversed" << "Spectrum" << "Hot Metal" << "Hot Green"
                      << "Hot Iron" << "GE" << "Flow" << "Loni" << "Loni 2" << "Asymmetry" << "P-Value" << "Red Black Alpha"
                      << "Green Black Alpha" << "Blue Black Alpha" << "Muscles & Bones" << "Bones" << "Red Vessels"
                      << "Cardiac" << "Gray Rainbow" << "Stern" << "Black Body");
    this->addProperty("ColorMode", QStringList() << "#FFFFFF");
}

v3dViewMeshInteractor::~v3dViewMeshInteractor()
{
    this->disable();

    delete d;
    d = 0;
}

QString v3dViewMeshInteractor::description(void) const
{
    return tr("Interactor displaying Meshes");
}

QString v3dViewMeshInteractor::identifier() const
{
    return "v3dViewMeshInteractor";
}

QStringList v3dViewMeshInteractor::handled(void) const
{
    return QStringList () << "v3dView";
}

bool v3dViewMeshInteractor::registered(void)
{
    return dtkAbstractViewFactory::instance()->registerViewInteractorType("v3dViewMeshInteractor", QStringList() << "v3dView", createV3dViewMeshInteractor);
}

void v3dViewMeshInteractor::setData(dtkAbstractData *data)
{
    if (d->dataList.contains (data))
    {
        return;
    }

    if(data->identifier() == "vtkDataMesh4D")
    {
        vtkMetaDataSetSequence *sequence = dynamic_cast<vtkMetaDataSetSequence *>((vtkDataObject *)(data->data()));
        vtkPointSet *pointSet = vtkPointSet::SafeDownCast (sequence->GetDataSet());

        if(!d->view->hasImage())
        {
            changeBounds(pointSet);
        }

        d->dataList.append(data);
        d->opacityList.append(1.0);
        d->visibilityList.append(true);
        d->edgeVisibilityList.append(false);
        d->colorList.append(QString("#FFFFFF"));
        d->renderingList.append("Surface");
        d->attributeList.append("Solid");
        d->lutList.append("Default");
        updatePipeline(d->dataList.size()-1);
    }
    else if (vtkPointSet *pointSet = dynamic_cast<vtkPointSet *>((vtkDataObject *)(data->data()))) {

        if(!d->view->hasImage())
        {
            changeBounds(pointSet);
        }

        d->dataList.append(data);
        d->opacityList.append(1.0);
        d->visibilityList.append(true);
        d->edgeVisibilityList.append(false);
        d->colorList.append(QString("#FFFFFF"));
        d->renderingList.append("Surface");
        d->attributeList.append("Solid");
        d->lutList.append("Default");
        updatePipeline(d->dataList.size()-1);
    }
}

bool v3dViewMeshInteractor::isMeshOnly()
{
    return d->isMeshOnly;
}

void v3dViewMeshInteractor::setView(dtkAbstractView *view)
{
    if (v3dView *v3dview = dynamic_cast<v3dView*>(view) ) {
        d->view = v3dview;
    }
}


bool v3dViewMeshInteractor::isAutoEnabledWith ( dtkAbstractData * data )
{
    if ( data->identifier() == "vtkDataMesh" ) {
        this->enable ();
        return true;
    }
    return false;
}

void v3dViewMeshInteractor::enable(void)
{
    dtkWarning() << "enabling v3dViewMeshInteractor" ;
    if (this->enabled())
        return;
    updatePipeline ();
    dtkAbstractViewInteractor::enable();
}


void v3dViewMeshInteractor::disable(void)
{
    if (!this->enabled())
        return;
    if (d->view) {
        for (int i = 0; i < d->dataList.size(); ++i) {
            if (d->view && !d->dataList.isEmpty() ) {
                if(vtkPointSet *pointset = dynamic_cast<vtkPointSet*>((vtkObject *)(d->dataList[i]->data()))) {
                    d->view->view3d()->RemoveDataSet (pointset);
                    d->view->view2d()->RemoveDataSet(pointset);
                }
                else if (vtkMetaDataSetSequence *sequence = dynamic_cast<vtkMetaDataSetSequence *>((vtkDataObject *)(d->dataList[i]->data()))){
                    if(vtkPointSet *pointSet4D = dynamic_cast<vtkPointSet*> (vtkPointSet::SafeDownCast (sequence->GetDataSet())))
                    {
                        d->view->view3d()->RemoveDataSet (pointSet4D);
                        d->view->view2d()->RemoveDataSet(pointSet4D);
                    }
                }
            }
        }

        if(!d->view->hasImage())
            d->view->view2d()->RemoveLayer(0);

        // MAYBE TODO d->actor2dList[d->currentLayer]->Delete();d->actorPropertyList
        // TODO        d->view->view3D ()->RemoveDataset ();
    }
    dtkAbstractViewInteractor::disable();
}

void v3dViewMeshInteractor::onPropertySet(const QString& key, const QString& value)
{
    if (key.contains("Visibility"))
        this->onVisibilityPropertySet (value);

    if (key.contains("ShowEdges"))
        this->onEdgeVisibilityPropertySet (value);

    if (key.contains("RenderingMode"))
        this->onRenderingModePropertySet (value);

    if (key.contains("LUTMode"))
        this->onLUTModePropertySet (value);

    if (d->view)
        d->view->update();
}

void v3dViewMeshInteractor::setLayer(int meshLayer)
{
    d->currentLayer = meshLayer;
}

int v3dViewMeshInteractor::meshLayer(void)
{
    return d->currentLayer;
}

void v3dViewMeshInteractor::onVisibilityPropertySet (const QString& value)
{
    if (value=="true") {
        d->actor2dList[d->currentLayer]->SetVisibility(1);
        d->actor3dList[d->currentLayer]->SetVisibility(1);
        d->visibilityList[d->currentLayer] = true;
    } else {
        d->actor2dList[d->currentLayer]->SetVisibility(0);
        d->actor3dList[d->currentLayer]->SetVisibility(0);
        d->visibilityList[d->currentLayer] = false;
    }
}

void v3dViewMeshInteractor::onColorPropertySet (const QColor& color)
{
    int r,g,b=0;
    color.getRgb(&r,&g,&b);
    d->actorPropertyList[d->currentLayer]->SetColor((double)r/(255.0),(double)g/(255.0),(double)b/(255.0));
    d->colorList[d->currentLayer] = color.name().toUpper();
    d->view->update();
}
void v3dViewMeshInteractor::onEdgeVisibilityPropertySet (const QString& value)
{
    if (value=="true")
    {
        d->actorPropertyList[d->currentLayer]->SetEdgeVisibility (1);
        d->edgeVisibilityList[d->currentLayer] = true;
    }
    else
    {
        d->actorPropertyList[d->currentLayer]->SetEdgeVisibility (0);
        d->edgeVisibilityList[d->currentLayer] = false;
    }
}
void v3dViewMeshInteractor::onRenderingModePropertySet (const QString& value)
{
    if (value=="wireframe") {
        d->actorPropertyList[d->currentLayer]->SetRepresentationToWireframe ();
        d->renderingList[d->currentLayer] = "Wire frame";
    } else if (value=="surface") {
        d->actorPropertyList[d->currentLayer]->SetRepresentationToSurface ();
        d->renderingList[d->currentLayer] = "Surface";
    } else if (value=="points") {
        d->actorPropertyList[d->currentLayer]->SetRepresentationToPoints ();
        d->renderingList[d->currentLayer] = "Points";
    }
}

void v3dViewMeshInteractor::setOpacity (double value)
{
    d->actorPropertyList[d->currentLayer]->SetOpacity(value);
    d->opacityList[d->currentLayer] = value;
}
double v3dViewMeshInteractor::opacity (int meshLayer)
{
    return d->opacityList.at(meshLayer);
}
bool v3dViewMeshInteractor::visibility (int meshLayer)
{
    return d->visibilityList.at(meshLayer);
}
bool v3dViewMeshInteractor::edgeVisibility (int meshLayer)
{
    return d->edgeVisibilityList.at(meshLayer);
}
QString* v3dViewMeshInteractor::color (int meshLayer)
{
    return &(d->colorList[meshLayer]);
}
QString* v3dViewMeshInteractor::renderingType (int meshLayer)
{
    return &(d->renderingList[meshLayer]);
}
QString* v3dViewMeshInteractor::attribute (int meshLayer)
{
    return &(d->attributeList[meshLayer]);
}
void v3dViewMeshInteractor::setAttribute (const QString& attribute, int meshLayer)
{
    d->attributeList[meshLayer] = attribute;
}
QString* v3dViewMeshInteractor::lut (int meshLayer)
{
    return &(d->lutList[meshLayer]);
}
char* v3dViewMeshInteractor::getLUTQuery (int meshLayer)
{
    if(vtkPointSet *pointSet = dynamic_cast<vtkPointSet *>((vtkDataObject *)(d->dataList[meshLayer]->data())))
    {
        vtkDataArray * dataArray = pointSet->GetPointData()->GetArray(0);
        if (!dataArray)
            return NULL;

        return dataArray->GetName();
    }
    return NULL;
}
void v3dViewMeshInteractor::onLUTModePropertySet (const QString& value)
{

    if(vtkPointSet *pointSet = dynamic_cast<vtkPointSet *>((vtkDataObject *)(d->dataList[d->currentLayer]->data())))
    {

        this->setScalarVisibility(true);
        /*vtkDataArrayCollection *collection = vtkDataArrayCollection::New();
        int i;
        for (i=0; i<pointSet->GetPointData()->GetNumberOfArrays(); i++)
        {
            collection->AddItem(pointSet->GetPointData()->GetArray (i));
        }*/


        vtkDataArray * dataArray = pointSet->GetPointData()->GetArray(0);

        if (!dataArray)
            return;

        bool array_is_in_points = false;

        if (pointSet->GetPointData()->HasArray (dataArray->GetName()))
            array_is_in_points = true;

        double min = 0, max = 0;

        vtkDataArray* junk;
        vtkDataSetAttributes* attributes;
        if (array_is_in_points)
        {
            junk = pointSet->GetPointData()->GetArray (dataArray->GetName());
            attributes = pointSet->GetPointData();
        }
        else
        {
            //junk = pointSet->GetCellData()->GetArray (dataArray->GetName());
            //attributes = pointSet->GetCellData();
        }

        if (!junk)
            return;

        if (min > junk->GetRange()[0])
            min = junk->GetRange()[0];
        if (max < junk->GetRange()[1])
            max = junk->GetRange()[1];

        vtkLookupTable* lut = vtkLookupTableManager::GetLookupTable(value.toStdString());
        if (lut)
        {
            lut->SetRange (min, max);
            junk->SetLookupTable (lut);
        }

        attributes->SetActiveScalars(dataArray->GetName());

        if (!d->actor2dList[d->currentLayer])
            return;


        if (!array_is_in_points)
        {
            d->actor2dList[d->currentLayer]->GetMapper ()->SetScalarModeToUseCellFieldData();
            d->actor3dList[d->currentLayer]->GetMapper ()->SetScalarModeToUseCellFieldData();
        }
        else
        {
            d->actor2dList[d->currentLayer]->GetMapper ()->SetScalarModeToUsePointFieldData();
            d->actor3dList[d->currentLayer]->GetMapper ()->SetScalarModeToUsePointFieldData();
        }
        if (lut)
        {
            d->actor2dList[d->currentLayer]->GetMapper ()->SetLookupTable (lut);
            d->actor2dList[d->currentLayer]->GetMapper ()->UseLookupTableScalarRangeOn();
            d->actor3dList[d->currentLayer]->GetMapper ()->SetLookupTable (lut);
            d->actor3dList[d->currentLayer]->GetMapper ()->UseLookupTableScalarRangeOn();
        }
        d->actor2dList[d->currentLayer]->GetMapper ()->SetScalarRange (min, max);
        d->actor2dList[d->currentLayer]->GetMapper ()->SelectColorArray (dataArray->GetName());
        d->actor3dList[d->currentLayer]->GetMapper ()->SetScalarRange (min, max);
        d->actor3dList[d->currentLayer]->GetMapper ()->SelectColorArray (dataArray->GetName());
        d->lutList[d->currentLayer] = value;
    }
    d->view->update();
}
void v3dViewMeshInteractor::setScalarVisibility(bool val)
{
    vtkMapper* mapper;
    mapper = d->actor2dList[d->currentLayer]->GetMapper();
    mapper->SetScalarVisibility(val);
    mapper = d->actor3dList[d->currentLayer]->GetMapper();
    mapper->SetScalarVisibility(val);
}
// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createV3dViewMeshInteractor(void)
{
    return new v3dViewMeshInteractor;
}

void v3dViewMeshInteractor::updatePipeline (unsigned int meshLayer)
{
    if (d->view && !d->dataList.isEmpty() ) {

        if(vtkPointSet *pointset = dynamic_cast<vtkPointSet*>((vtkObject *)(d->dataList[meshLayer]->data()))) {
            d->actor2dList.append(d->view->view2d ()->AddDataSet(pointset));
            d->actor3dList.append(d->view->view3d ()->AddDataSet(pointset));
            d->actorPropertyList.append(v3dViewMeshInteractorPrivate::PropertySmartPointer::New ());
            d->actor2dList[meshLayer]->SetProperty ( d->actorPropertyList[meshLayer] );
            d->actor3dList[meshLayer]->SetProperty ( d->actorPropertyList[meshLayer] );
        }
        else if (vtkMetaDataSetSequence *sequence = dynamic_cast<vtkMetaDataSetSequence *>((vtkDataObject *)(d->dataList[meshLayer]->data()))){

            if(vtkPointSet *pointSet = vtkPointSet::SafeDownCast (sequence->GetDataSet()))
            {
                d->actor2dList.append(d->view->view2d ()->AddDataSet(pointSet));
                d->actor3dList.append(d->view->view3d ()->AddDataSet(pointSet));
                d->actorPropertyList.append(v3dViewMeshInteractorPrivate::PropertySmartPointer::New ());
                d->actor2dList[meshLayer]->SetProperty ( d->actorPropertyList[meshLayer] );
                d->actor3dList[meshLayer]->SetProperty ( d->actorPropertyList[meshLayer] );
            }
        }
    }
}

void v3dViewMeshInteractor::changeBounds (vtkPointSet* pointSet)
{
    double limites[6];
    pointSet->GetBounds(limites);
    if(!d->view->dataInList(0) )
    {
        for (int i=0; i<6; i++)
        {
            d->ImageBounds[i]=limites[i];
        }
        d->isImageOutBounded = true;
    }
    else
    {
        for (int i=0; i<6; i=i+2)
        {
            if (limites[i]<d->ImageBounds[i])
            {
                d->ImageBounds[i]=limites[i];
                d->isImageOutBounded=true;
            }
        }
        for (int i=1; i<6; i=i+2)
        {
            if (limites[i]>d->ImageBounds[i])
            {
                d->ImageBounds[i]=limites[i];
                d->isImageOutBounded=true;
            }
        }
    }

    if(d->isImageOutBounded)
    {
        vtkImageFromBoundsSource* imagegenerator = vtkImageFromBoundsSource::New();
        unsigned int imSize [3]; imSize[0]=100; imSize[1]=100; imSize[2]=100;
        imagegenerator->SetOutputImageSize(imSize);
        //        vtkInformationDoubleVectorKey * origin = pointSet->ORIGIN();
        //        double *originDouble= origin->Get(pointSet->GetInformation() );
        //        imagegenerator->SetOutputImageOrigin(originDouble);
        imagegenerator->SetOutputImageBounds(d->ImageBounds);
        vtkImageData * image = imagegenerator->GetOutput();

        if(d->view->dataInList(0))
        {
            //d->view->view2d()->RemoveDataSet();
            d->view->view2d()->RemoveLayer(0);
        }

        d->view->view2d()->SetInput(image, 0);
        vtkImageActor *actor = d->view->view2d()->GetImageActor(0);
        actor->SetOpacity(0.0);
        d->isImageOutBounded=false;
        imagegenerator->Delete();
    }
}
