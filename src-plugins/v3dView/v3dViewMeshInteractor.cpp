/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "v3dViewMeshInteractor.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkLog/dtkLog.h>
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
#include <vtkPolyDataManager.h>
#include <v3dView.h>
#include <vtkImageFromBoundsSource.h>
#include <vtkImageData.h>
#include <vtkImageActor.h>
#include <vtkLookupTableManager.h>
#include <vtkLookupTable.h>
#include <vtkMetaDataSet.h>
#include <vtkMetaDataSetSequence.h>
#include <vtkDataArrayCollection.h>

#include <vector>

typedef QPair<vtkSmartPointer<vtkLookupTable>, QString> LutPair;

class v3dViewMeshInteractorPrivate
{
public:
    typedef vtkSmartPointer <vtkActor>  ActorSmartPointer;
    typedef vtkSmartPointer <vtkProperty>  PropertySmartPointer;
    typedef vtkSmartPointer <vtkDataArray>  AttributeSmartPointer;

    v3dView * view;
    QList<vtkMetaDataSet*> dataList;
    QList<ActorSmartPointer> actor2dList;
    QList<ActorSmartPointer> actor3dList;
    QList<PropertySmartPointer> actorPropertyList;
    QList<LutPair> lutList;
    QList<AttributeSmartPointer> attributeList;
    double imageBounds[6];
};


v3dViewMeshInteractor::v3dViewMeshInteractor(): medMeshAbstractViewInteractor(), d(new v3dViewMeshInteractorPrivate)
{
    for (int i=0; i<6; i++)
        d->imageBounds[i] = 0;
    d->view = NULL;
}


v3dViewMeshInteractor::~v3dViewMeshInteractor()
{
    this->disable();

    delete d;
    d = 0;
}


QString v3dViewMeshInteractor::description() const
{
    return tr("Interactor displaying Meshes");
}


QString v3dViewMeshInteractor::identifier() const
{
    return "v3dViewMeshInteractor";
}


QStringList v3dViewMeshInteractor::handled() const
{
    return QStringList () << v3dView::s_identifier();
}


bool v3dViewMeshInteractor::registered()
{
    return dtkAbstractViewFactory::instance()->registerViewInteractorType("v3dViewMeshInteractor", QStringList() << v3dView::s_identifier(), createV3dViewMeshInteractor);
}


void v3dViewMeshInteractor::setData(dtkAbstractData *data)
{
    if(data->identifier() == "vtkDataMesh4D" || data->identifier() == "vtkDataMesh")
    {
        vtkMetaDataSet * mesh = dynamic_cast<vtkMetaDataSet*>((vtkDataObject *)(data->data()));
        vtkPointSet * pointSet = vtkPointSet::SafeDownCast (mesh->GetDataSet());

        if(!d->view->hasImage())
            changeBounds(pointSet);

        d->dataList.append(mesh);
        d->lutList.append(LutPair(NULL, "Default"));
        d->attributeList.append(NULL);
        updatePipeline(d->dataList.size()-1);
    }
}


void v3dViewMeshInteractor::setView(dtkAbstractView *view)
{
    if (v3dView *v3dview = dynamic_cast<v3dView*>(view) )
        d->view = v3dview;
}


void v3dViewMeshInteractor::enable()
{
    dtkWarn() << "enabling v3dViewMeshInteractor";
    if (this->enabled())
        return;
    updatePipeline ();
    dtkAbstractViewInteractor::enable();
}


void v3dViewMeshInteractor::disable()
{
    if (!this->enabled())
        return;

    if (d->view)
    {
        for (int i = 0; i < d->dataList.size(); ++i)
        {
            if(vtkPointSet * pointSet = vtkPointSet::SafeDownCast(d->dataList[i]->GetDataSet()))
            {
                d->view->view3d()->RemoveDataSet(pointSet);
                d->view->view2d()->RemoveDataSet(pointSet);
            }
        }

        if(!d->view->hasImage())
            d->view->view2d()->RemoveLayer(0);

        // MAYBE TODO d->actor2dList[d->currentLayer]->Delete();
        // TODO        d->view->view3D ()->RemoveDataset ();
    }
    dtkAbstractViewInteractor::disable();
}


bool v3dViewMeshInteractor::isAutoEnabledWith ( dtkAbstractData * data )
{
    if ( data->identifier() == "vtkDataMesh" )
    {
        this->enable ();
        return true;
    }
    return false;
}


void v3dViewMeshInteractor::setOpacity(int meshLayer, double value)
{
    d->actorPropertyList[meshLayer]->SetOpacity(value);
    d->view->update();
}


double v3dViewMeshInteractor::opacity(int meshLayer) const
{
    return d->actorPropertyList[meshLayer]->GetOpacity();
}


void v3dViewMeshInteractor::setVisibility(int meshLayer, bool visible)
{
    int v = (visible) ? 1 : 0;
    d->actor2dList[meshLayer]->SetVisibility(v);
    d->actor3dList[meshLayer]->SetVisibility(v);
    d->view->update();
}


bool v3dViewMeshInteractor::visibility(int meshLayer) const
{
    return (d->actor2dList[meshLayer]->GetVisibility() == 1);
}


void v3dViewMeshInteractor::setEdgeVisibility(int meshLayer, bool visible)
{
    int v = (visible) ? 1 : 0;
    d->actorPropertyList[meshLayer]->SetEdgeVisibility(v);
    d->view->update();
}


bool v3dViewMeshInteractor::edgeVisibility(int meshLayer) const
{
    return (d->actorPropertyList[meshLayer]->GetEdgeVisibility() == 1);
}


void v3dViewMeshInteractor::setColor(int meshLayer, QColor color)
{
    if( ! color.isValid())
        color.setRgb(0,0,0);

    double r,g,b;
    color.getRgbF(&r, &g, &b);
    d->actorPropertyList[meshLayer]->SetColor(r, g, b);
    d->view->update();
}


QColor v3dViewMeshInteractor::color(int meshLayer) const
{
    double r,g,b;
    d->actorPropertyList[meshLayer]->GetColor(r, g, b);
    return QColor::fromRgbF(r, b, g);
}


void v3dViewMeshInteractor::setRenderingType(int meshLayer, const QString & type)
{
    QString value = type.toLower();
    if (value == "wireframe")
        d->actorPropertyList[meshLayer]->SetRepresentationToWireframe ();
    else if (value == "surface")
        d->actorPropertyList[meshLayer]->SetRepresentationToSurface ();
    else if (value == "points")
        d->actorPropertyList[meshLayer]->SetRepresentationToPoints ();
    d->view->update();
}


QString v3dViewMeshInteractor::renderingType(int meshLayer) const
{
    return QString::fromStdString(d->actorPropertyList[meshLayer]->GetRepresentationAsString()).toLower();
}


void v3dViewMeshInteractor::setAttribute(int meshLayer, const QString & attributeName)
{
    vtkPointSet * pointSet = vtkPointSet::SafeDownCast(d->dataList[meshLayer]->GetDataSet());
    if ( ! pointSet )
        return;

    vtkDataSetAttributes * attributes = NULL;
    vtkMapper * mapper2d = d->actor2dList[meshLayer]->GetMapper();
    vtkMapper * mapper3d = d->actor3dList[meshLayer]->GetMapper();
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
        d->attributeList[meshLayer] = attributes->GetArray(qPrintable(attributeName));
        attributes->SetActiveScalars(qPrintable(attributeName));

        mapper2d->SelectColorArray(qPrintable(attributeName));
        mapper3d->SelectColorArray(qPrintable(attributeName));

        this->setLut(meshLayer, d->lutList[meshLayer].first);

        mapper2d->SetScalarVisibility(1);
        mapper3d->SetScalarVisibility(1);
    }
    else
    {
        d->attributeList[meshLayer] = NULL;
        mapper2d->SetScalarVisibility(0);
        mapper3d->SetScalarVisibility(0);
    }

    d->view->update();
}


QString v3dViewMeshInteractor::attribute(int meshLayer) const
{
    return QString::fromUtf8(d->actor2dList[meshLayer]->GetMapper()->GetArrayName());
}


QStringList v3dViewMeshInteractor::getAllAttributes(int meshLayer) const
{
    vtkMetaDataSet * metadataset = d->dataList[meshLayer];

    if (!metadataset || (metadataset->GetType() != vtkMetaDataSet::VTK_META_SURFACE_MESH &&
                         metadataset->GetType() != vtkMetaDataSet::VTK_META_VOLUME_MESH))
        return QStringList();

    QStringList nameList;

    for (int i = 0; i < metadataset->GetDataSet()->GetPointData()->GetNumberOfArrays(); i++)
    {
        if (metadataset->GetDataSet()->GetPointData()->GetArray(i)->GetNumberOfComponents() == 1)
            nameList << QString::fromUtf8(metadataset->GetDataSet()->GetPointData()->GetArrayName(i));
    }

    for (int i = 0;  i < metadataset->GetDataSet()->GetCellData()->GetNumberOfArrays(); i++)
    {
        if (metadataset->GetDataSet()->GetCellData()->GetArray(i)->GetNumberOfComponents() == 1)
            nameList << QString::fromUtf8(metadataset->GetDataSet()->GetCellData()->GetArrayName(i));
    }

    return nameList;
}


void v3dViewMeshInteractor::setLut(int meshLayer, const QString & lutName)
{
    vtkLookupTable * lut = vtkLookupTableManager::GetLookupTable(lutName.toStdString());

    if ( ! d->attributeList[meshLayer])
     return;

    d->lutList[meshLayer] = LutPair(lut, lutName);
    this->setLut(meshLayer, lut);
    d->view->update();
}


QString v3dViewMeshInteractor::lut(int meshLayer) const
{
    return d->lutList[meshLayer].second;
}


QStringList v3dViewMeshInteractor::getAllLUTs() const
{
    static QStringList luts;
    if( luts.isEmpty() )
    {
        const std::vector<std::string> & vec = vtkLookupTableManager::GetAvailableLookupTables();
        for(std::vector<std::string>::const_iterator it = vec.begin(); it < vec.end(); ++it)
            luts.append(QString::fromStdString(*it));
    }
    return luts;
}


// ---------------------------------------------------------------------------


void v3dViewMeshInteractor::updatePipeline (unsigned int meshLayer)
{
    if (d->view && !d->dataList.isEmpty() )
    {
        if(vtkPointSet * pointSet = vtkPointSet::SafeDownCast (d->dataList[meshLayer]->GetDataSet()))
        {
            d->actor2dList.append(d->view->view2d()->AddDataSet(pointSet));
            d->actor3dList.append(d->view->view3d()->AddDataSet(pointSet));

            d->actorPropertyList.append(v3dViewMeshInteractorPrivate::PropertySmartPointer::New());
            d->actor2dList[meshLayer]->SetProperty( d->actorPropertyList[meshLayer] );
            d->actor3dList[meshLayer]->SetProperty( d->actorPropertyList[meshLayer] );
        }
    }
    d->view->view3d()->ResetCamera();
}


void v3dViewMeshInteractor::changeBounds (vtkPointSet* pointSet)
{
    double bounds[6];
    bool isImageOutBounded = false;
    pointSet->GetBounds(bounds);
    if(!d->view->dataInList(0) )
    {
        for (int i=0; i<6; i++)
        {
            d->imageBounds[i]=bounds[i];
        }
        isImageOutBounded = true;
    }
    else
    {
        for (int i=0; i<6; i=i+2)
        {
            if (bounds[i] < d->imageBounds[i])
            {
                d->imageBounds[i]=bounds[i];
                isImageOutBounded=true;
            }
        }
        for (int i=1; i<6; i=i+2)
        {
            if (bounds[i] > d->imageBounds[i])
            {
                d->imageBounds[i]=bounds[i];
                isImageOutBounded=true;
            }
        }
    }

    if(isImageOutBounded)
    {
        vtkImageFromBoundsSource* imagegenerator = vtkImageFromBoundsSource::New();
        unsigned int imSize [3]; imSize[0]=100; imSize[1]=100; imSize[2]=100;
        imagegenerator->SetOutputImageSize(imSize);
        //        vtkInformationDoubleVectorKey * origin = pointSet->ORIGIN();
        //        double *originDouble= origin->Get(pointSet->GetInformation() );
        //        imagegenerator->SetOutputImageOrigin(originDouble);
        imagegenerator->SetOutputImageBounds(d->imageBounds);
        vtkImageData * image = imagegenerator->GetOutput();

        if(d->view->dataInList(0))
        {
            //d->view->view2d()->RemoveDataSet();
            d->view->view2d()->RemoveLayer(0);
        }

        d->view->view2d()->SetInput(image, 0);
        vtkImageActor *actor = d->view->view2d()->GetImageActor(0);
        actor->SetOpacity(0.0);
        isImageOutBounded=false;
        imagegenerator->Delete();
        d->view->view2d()->ResetCamera();
    }
}


void v3dViewMeshInteractor::setLut(int meshLayer, vtkLookupTable * lut)
{
    if ( ! d->attributeList[meshLayer] )
        return;

    if ( ! lut )
    {
        lut = d->attributeList[meshLayer]->GetLookupTable();
        d->lutList[meshLayer] = LutPair(lut, "Default");
        // no ? then bail, nothing to do here.
        if ( ! lut )
            return;
    }

    // remove the alpha channel from the LUT, it messes up the mesh
    double values[4];
    for(int i = 0; i < lut->GetNumberOfTableValues(); i++)
    {
      lut->GetTableValue(i, values);
      values[3] = 1.0;
      lut->SetTableValue(i, values);
    }

    double * range = d->dataList[meshLayer]->GetCurrentScalarRange();
    lut->SetRange(range);

    vtkMapper * mapper2d = d->actor2dList[meshLayer]->GetMapper();
    vtkMapper * mapper3d = d->actor3dList[meshLayer]->GetMapper();

    mapper2d->SetLookupTable(lut);
    mapper2d->UseLookupTableScalarRangeOn();
    mapper3d->SetLookupTable(lut);
    mapper3d->UseLookupTableScalarRangeOn();
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////

dtkAbstractViewInteractor *createV3dViewMeshInteractor()
{
    return new v3dViewMeshInteractor;
}
