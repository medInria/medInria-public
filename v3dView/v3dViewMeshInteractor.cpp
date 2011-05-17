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
#include <vtkDatasetToImageGenerator.h>
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


};

v3dViewMeshInteractor::v3dViewMeshInteractor(): medMeshAbstractViewInteractor(), d(new v3dViewMeshInteractorPrivate)
{
    //d->data = NULL;
    d->view = NULL;

    // addProperty here
    this->addProperty("Visibility", QStringList() << "true" << "false");
    this->addProperty("ShowEdges", QStringList() << "true" << "false");
    this->addProperty("RenderingMode", QStringList() << "surface" << "wireframe" << "points" );

}

v3dViewMeshInteractor::~v3dViewMeshInteractor()
{
    this->disable();

    delete d;
    d = 0;
}

QString v3dViewMeshInteractor::description(void) const
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
        return;
    if(data->description() == "vtkDataMesh4D")
    {
        vtkMetaDataSetSequence *sequence = dynamic_cast<vtkMetaDataSetSequence *>((vtkDataObject *)(data->data()));
        vtkPointSet *pointSet = vtkPointSet::SafeDownCast (sequence->GetDataSet());
        d->dataList.append(data);
        
        updatePipeline(d->dataList.size()-1);
        
    }
    else if (vtkPointSet *pointSet = dynamic_cast<vtkPointSet *>((vtkDataObject *)(data->data()))) {
        if(!d->view->data())
        {
            /*vtkDatasetToImageGenerator* imagegenerator = vtkDatasetToImageGenerator::New();
            imagegenerator->SetInput (pointSet);
            vtkImageData * image = imagegenerator->GetOutput();
            d->view->view2d()->SetInput(image);
            vtkImageActor *actor = d->view->view2d()->GetImageActor(0);
            actor->SetOpacity(0.0);	*/
        }
        Q_UNUSED( pointSet );
        d->dataList.append(data);
        updatePipeline(d->dataList.size()-1);
        
    }
}


void v3dViewMeshInteractor::setView(dtkAbstractView *view)
{
    if (v3dView *v3dview = dynamic_cast<v3dView*>(view) ) {
        d->view = v3dview;
    }
}


bool v3dViewMeshInteractor::isAutoEnabledWith ( dtkAbstractData * data )
{
    if ( data->description() == "vtkDataMesh" ) {

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

        // TODO        d->view->view3D ()->RemoveDataset ();
        // TODO        d->view->view3D ()->RemoveDataset ();
        // TODO        d->view->view3D ()->RemoveDataset ();
        // TODO        d->view->view3D ()->RemoveDataset ();
    }
    dtkAbstractViewInteractor::disable();
}

void v3dViewMeshInteractor::onPropertySet(const QString& key, const QString& value)
{
    QString meshLayer = key[key.size()-1];

    if (key.contains("Visibility"))
        this->onVisibilityPropertySet (value, meshLayer.toInt());

    if (key.contains("ShowEdges"))
        this->onEdgeVisibilityPropertySet (value, meshLayer.toInt());

    if (key.contains("RenderingMode"))
        this->onRenderingModePropertySet (value, meshLayer.toInt());

    if (key.contains("OpacityMode"))
        this->onOpacityModePropertySet (value.toDouble(), meshLayer.toInt());

    if (key.contains("LUTMode"))
        this->onLUTModePropertySet (value, meshLayer.toInt());

    if (d->view)
        d->view->update();
}


void v3dViewMeshInteractor::onVisibilityPropertySet (const QString& value, int meshLayer)
{
    if (value=="true") {
        d->actor2dList[meshLayer]->SetVisibility(1);
        d->actor3dList[meshLayer]->SetVisibility(1);
    } else {
        d->actor2dList[meshLayer]->SetVisibility(0);
        d->actor3dList[meshLayer]->SetVisibility(0);
    }
}

void v3dViewMeshInteractor::onColorPropertySet (const QColor& color, int meshLayer)
{
    int r,g,b=0;
    
    color.getRgb(&r,&g,&b);
    d->actorPropertyList[meshLayer]->SetColor((double)r/(255.0),(double)g/(255.0),(double)b/(255.0));
    d->view->update();
}
void v3dViewMeshInteractor::onEdgeVisibilityPropertySet (const QString& value, int meshLayer)
{
    if (value=="true")
        d->actorPropertyList[meshLayer]->SetEdgeVisibility (1);
    else
        d->actorPropertyList[meshLayer]->SetEdgeVisibility (0);
}

void v3dViewMeshInteractor::onRenderingModePropertySet (const QString& value, int meshLayer)
{
    if (value=="wireframe") {
        d->actorPropertyList[meshLayer]->SetRepresentationToWireframe ();
    } else if (value=="surface") {
        d->actorPropertyList[meshLayer]->SetRepresentationToSurface ();
    } else if (value=="points") {
        d->actorPropertyList[meshLayer]->SetRepresentationToPoints ();
    }

}

void v3dViewMeshInteractor::onOpacityModePropertySet (double value, int meshLayer)
{
    d->actorPropertyList[meshLayer]->SetOpacity(value);
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
void v3dViewMeshInteractor::onLUTModePropertySet (const QString& value, int meshLayer)
{
    if(vtkPointSet *pointSet = dynamic_cast<vtkPointSet *>((vtkDataObject *)(d->dataList[meshLayer]->data())))
    {
        /*vtkDataArrayCollection *collection = vtkDataArrayCollection::New();
        int i;
        for (i=0; i<pointSet->GetPointData()->GetNumberOfArrays(); i++)
        {
            collection->AddItem(pointSet->GetPointData()->GetArray (i));
        }*/
        
        //qDebug()<<"I : "<<pointSet->GetPointData()->GetNumberOfArrays();
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
         qDebug()<<"junk->GetSize() : "<<junk->GetSize();
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
        ////vtkActor *actor = d->actor2dList[meshLayer]; 

        if (!d->actor2dList[meshLayer])
            return;
        

        if (!array_is_in_points)
            d->actor2dList[meshLayer]->GetMapper ()->SetScalarModeToUseCellFieldData();
        else
            d->actor2dList[meshLayer]->GetMapper ()->SetScalarModeToUsePointFieldData();    

        if (lut)
        {
            qDebug()<<"d->actor2dList.size() : "<<d->actor2dList.size();
            qDebug()<<"meshLayer : "<<meshLayer;
            d->actor2dList[meshLayer]->GetMapper ()->SetLookupTable (lut);
            d->actor2dList[meshLayer]->GetMapper ()->UseLookupTableScalarRangeOn();
        }

        d->actor2dList[meshLayer]->GetMapper ()->SetScalarRange (min, max);
        d->actor2dList[meshLayer]->GetMapper ()->SelectColorArray (dataArray->GetName());

        
    }
   
   // d->view->update();
   
    
    
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

           qDebug()<<"pointset";
            
            d->actor2dList.append(d->view->view2d ()->AddDataSet(pointset));
            d->actor3dList.append(d->view->view3d ()->AddDataSet(pointset));
            //pointset->Print(std::cout);

            d->actorPropertyList.append(v3dViewMeshInteractorPrivate::PropertySmartPointer::New ());

            //d->actorPropertyList[meshLayer]->SetColor ( 1,0,1 );
            d->actor2dList[meshLayer]->SetProperty ( d->actorPropertyList[meshLayer] );
            d->actor3dList[meshLayer]->SetProperty ( d->actorPropertyList[meshLayer] );

            //d->actor2dList[meshLayer]->GetMapper ()->ScalarVisibilityOff ();
           // d->actor3dList[meshLayer]->GetMapper ()->ScalarVisibilityOff ();
        }

        /*else if(vtkMetaDataSetSequence *sequence = dynamic_cast<vtkMetaDataSetSequence *>((vtkDataObject *)(d->dataList[meshLayer]->data())))
        {
            vtkPointSet *pointSet = vtkPointSet::SafeDownCast (sequence->GetDataSet());
             qDebug()<<"vtkMetaDataSetSequence";
            d->actor2dList.append(d->view->view2d ()->AddDataSet(pointset));
            d->actor3dList.append(d->view->view3d ()->AddDataSet(pointset));
            //pointset->Print(std::cout);

            d->actorPropertyList.append(v3dViewMeshInteractorPrivate::PropertySmartPointer::New ());

            d->actorPropertyList[meshLayer]->SetColor ( 1,0,1 );
            d->actor2dList[meshLayer]->SetProperty ( d->actorPropertyList[meshLayer] );
            d->actor3dList[meshLayer]->SetProperty ( d->actorPropertyList[meshLayer] );
            qDebug()<<"d->actor2dList.size() : "<<d->actor2dList.size();
        
        }*/
        else if (vtkMetaDataSetSequence *sequence = dynamic_cast<vtkMetaDataSetSequence *>((vtkDataObject *)(d->dataList[meshLayer]->data()))){
            qDebug()<<"sequence->GetDataSet()->GetClassName() : "<<sequence->GetDataSet()->GetClassName();
            if(vtkPointSet *pointSet = vtkPointSet::SafeDownCast (sequence->GetDataSet()))
        {
            qDebug()<<"vtkMetaDataSetSequence downcasted to vtkPointSet";
            
            d->actor2dList.append(d->view->view2d ()->AddDataSet(pointSet));
            d->actor3dList.append(d->view->view3d ()->AddDataSet(pointSet));
        

            //d->view->view2d()->AddDataSet (vtkPointSet::SafeDownCast (sequence->GetDataSet()))->SetProperty(prop);
            //d->view->view3d()->AddDataSet (vtkPointSet::SafeDownCast (sequence->GetDataSet()))->SetProperty(prop);
            //pointset->Print(std::cout);

            d->actorPropertyList.append(v3dViewMeshInteractorPrivate::PropertySmartPointer::New ());

       
            d->actor2dList[meshLayer]->SetProperty ( d->actorPropertyList[meshLayer] );
            d->actor3dList[meshLayer]->SetProperty ( d->actorPropertyList[meshLayer] );
            
            //d->actor2dList[meshLayer]->GetMapper ()->ScalarVisibilityOff ();
           // d->actor3dList[meshLayer]->GetMapper ()->ScalarVisibilityOff ();
        
         }}

    }
}

