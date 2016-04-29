/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCore/dtkSignalBlocker.h>

#include <medAbstractLayeredView.h>
#include <medAbstractMeshData.h>
#include <medClutToolBox.h>
#include <medDataManager.h>
#include <medSliderSpinboxPair.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medViewContainer.h>
#include <medVtkViewBackend.h>

#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkDataSet.h>
#include <vtkDataSetCollection.h>
#include <vtkImageView3D.h>
#include <vtkLookupTableManager.h>
#include <vtkMapper.h>
#include <vtkMetaDataSet.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkPointSet.h>
#include <vtkPointData.h>
#include <vtkScalarBarActor.h>

class medClutToolBoxPrivate
{
public:
    medAbstractLayeredView *view;
    QPushButton * export_B;
    medDoubleSliderSpinboxPair * minRange;
    medDoubleSliderSpinboxPair * maxRange;
    QStringList listOfAttributes;
    medComboBox * attributeBox;
    medComboBox * lutBox;
    QStringList luts;
    QHash<medAbstractView*,vtkLookupTable*>* viewAndLut;
    QHash<medAbstractView*,int> * attributeChosen;
    QHash<medAbstractView*,double*> * rangeView;
    QHash<medAbstractView*,vtkDataSet*> * viewDataSet;
};

medClutToolBox::medClutToolBox(QWidget *parent) :
medToolBox(parent), d(new medClutToolBoxPrivate)
{
    this->setTitle("Clut Editor ToolBox");
    QWidget * body = new QWidget(this);
    QVBoxLayout * layoutBody = new QVBoxLayout();
    body->setLayout(layoutBody);
    this->addWidget(body);
    
    QHBoxLayout * layoutAttribute = new QHBoxLayout();
    QLabel * labelAttribute = new QLabel("Attributes : ");
    d->attributeBox = new medComboBox(this);
    d->attributeBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    connect(d->attributeBox, SIGNAL(activated (int)), this, SLOT(changeAttribute(int)));

    d->lutBox = new medComboBox(this);
    d->lutBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    connect(d->lutBox, SIGNAL(activated (int)), this, SLOT(changeLut(int)));

    // get all luts
    if( d->luts.isEmpty() )
    {
        const std::vector<std::string> & vec = vtkLookupTableManager::GetAvailableLookupTables();
        for(std::vector<std::string>::const_iterator it = vec.begin(); it < vec.end(); ++it)
            d->luts.append(QString::fromStdString(*it));
    }
    
    foreach (QString lut,d->luts)
    {
        d->lutBox->addItem(lut);
    }
    //
    d->minRange = new medDoubleSliderSpinboxPair(this);
    d->minRange->setStep(0.2);
    connect(d->minRange,SIGNAL(valueChanged(double)),this,SLOT(updateRange()));
    d->maxRange = new medDoubleSliderSpinboxPair(this);
    d->maxRange->setStep(0.2);
    connect(d->maxRange,SIGNAL(valueChanged(double)),this,SLOT(updateRange()));
    d->export_B  = new QPushButton("Export with Lut",this);
    connect(d->export_B,SIGNAL(clicked()),this,SLOT(exportMeshWithLUT()));

    layoutAttribute->addWidget(labelAttribute);
    layoutAttribute->addWidget(d->attributeBox);
    layoutBody->addLayout(layoutAttribute);
    layoutBody->addWidget(d->lutBox);
    layoutBody->addWidget(d->minRange);
    layoutBody->addWidget(d->maxRange);
    layoutBody->addWidget(d->export_B);

    d->view = 0;
    d->viewAndLut = new QHash<medAbstractView*,vtkLookupTable*>();
    d->attributeChosen = new QHash<medAbstractView*,int>();
    d->rangeView = new QHash<medAbstractView*,double *>();
    d->viewDataSet = new QHash<medAbstractView*,vtkDataSet*>();
}

medClutToolBox::~medClutToolBox(void)
{
    delete d;
    d = NULL;
}

void medClutToolBox::updateView()
{
    medTabbedViewContainers * containers = this->getWorkspace()->stackedViewContainers();
    QList<medViewContainer*> containersInTabSelected = containers->containersInTab(containers->currentIndex());
    medAbstractView *view=NULL;
    for(int i=0;i<containersInTabSelected.length();i++)
    {
        if (containersInTabSelected[i]->isSelected())
        {
            view = containersInTabSelected[i]->view();
            break;
        }
    }

    if(!view)
    {
        return;
    }

    medAbstractLayeredView * medView = dynamic_cast<medAbstractLayeredView *> (view);
    if ( !medView )
        return;

    if (medView==d->view)
        return;

    d->view = medView;

    medAbstractData *data = d->view->layerData(0);
    if (!data->identifier().contains( "vtkDataMesh" ))
        return;
    
    medAbstractMeshData * dataMesh = static_cast<medAbstractMeshData*>(data);
    vtkMetaDataSet * mesh = static_cast<vtkMetaDataSet*>(dataMesh->mesh());

    if (!mesh || (mesh->GetType() != vtkMetaDataSet::VTK_META_SURFACE_MESH &&
                         mesh->GetType() != vtkMetaDataSet::VTK_META_VOLUME_MESH))
         return;

    d->listOfAttributes.clear();
    d->attributeBox->clear();

    vtkImageView3D * view3d = static_cast<medVtkViewBackend*>(d->view->backend())->view3D;

    if (view3d->GetDataSetCollection()->GetItem(0))
    {
        d->viewDataSet->insert(d->view,view3d->GetDataSetCollection()->GetItem(0)); 
    }
    else
        return;
    
    for (int i = 0; i < mesh->GetDataSet()->GetPointData()->GetNumberOfArrays(); i++)
    {
        if (mesh->GetDataSet()->GetPointData()->GetArray(i)->GetNumberOfComponents() == 1)
        {
            d->listOfAttributes << QString::fromUtf8(mesh->GetDataSet()->GetPointData()->GetArrayName(i));
            d->attributeBox->addItem("Point : "  + QString::fromUtf8(mesh->GetDataSet()->GetPointData()->GetArrayName(i)));
        }
    }

    for (int i = 0;  i < mesh->GetDataSet()->GetCellData()->GetNumberOfArrays(); i++)
    {
        if (mesh->GetDataSet()->GetCellData()->GetArray(i)->GetNumberOfComponents() == 1)
            d->listOfAttributes << QString::fromUtf8(mesh->GetDataSet()->GetCellData()->GetArrayName(i));
            d->attributeBox->addItem("Cell : "  + QString::fromUtf8(mesh->GetDataSet()->GetCellData()->GetArrayName(i)));
    }
    if (d->attributeChosen->contains(d->view))
    {
        d->attributeBox->setCurrentIndex(d->attributeChosen->value(d->view));
        changeAttribute(d->attributeChosen->value(d->view));
    }
    else
    {
        changeAttribute(0);
        d->attributeChosen->insert(d->view,0);
    }

    if (!d->viewAndLut->contains(d->view))
        changeLut(d->lutBox->currentIndex());
    // TODO : (connect) -> delete view from 3 hashes when closed.
}

bool medClutToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<medClutToolBox>();
}

void medClutToolBox::changeLut(int index)
{
   if (!d->view)
       return;
   vtkLookupTable * lut = vtkLookupTableManager::GetLookupTable(d->luts.at(index).toStdString());
    // remove the alpha channel from the LUT, it messes up the mesh
    double values[4];
    for(int i = 0; i < lut->GetNumberOfTableValues(); i++)
    {
        lut->GetTableValue(i, values);
        values[3] = 1.0;
        lut->SetTableValue(i, values);
    }
    d->viewAndLut->insert(d->view,lut);
    ApplyLut();
}

void medClutToolBox::ApplyLut()
{
    medAbstractData *data = d->view->layerData(0);
    if (!data->identifier().contains( "vtkDataMesh" ))
        return;

    medAbstractMeshData * dataMesh = static_cast<medAbstractMeshData*>(data);
    vtkMetaDataSet * mesh = static_cast<vtkMetaDataSet*>(dataMesh->mesh());

    if (!mesh || (mesh->GetType() != vtkMetaDataSet::VTK_META_SURFACE_MESH &&
        mesh->GetType() != vtkMetaDataSet::VTK_META_VOLUME_MESH))
        return;

    double * range;
    if (d->rangeView->contains(d->view))
        range = d->rangeView->value(d->view);
    else
        range = mesh->GetCurrentScalarRange();

    vtkImageView2D * view2d = static_cast<medVtkViewBackend*>(d->view->backend())->view2D;
    vtkImageView3D * view3d = static_cast<medVtkViewBackend*>(d->view->backend())->view3D;

    vtkActor * actor2d = static_cast<vtkActor*>(view2d->FindDataSetActor(d->viewDataSet->value(d->view)));
    vtkActor * actor3d = static_cast<vtkActor*>(view3d->FindDataSetActor(d->viewDataSet->value(d->view)));
    vtkMapper * mapper2d = actor2d->GetMapper();
    vtkMapper * mapper3d = actor3d->GetMapper();
    mapper3d->InterpolateScalarsBeforeMappingOn();
    mapper2d->InterpolateScalarsBeforeMappingOn();

    mapper2d->SetLookupTable(d->viewAndLut->value(d->view));
    mapper2d->UseLookupTableScalarRangeOn();
    mapper3d->SetLookupTable(d->viewAndLut->value(d->view));
    mapper3d->UseLookupTableScalarRangeOn();
    view3d->GetScalarBar()->SetLookupTable(d->viewAndLut->value(d->view));
    view2d->GetScalarBar()->SetLookupTable(d->viewAndLut->value(d->view));
    updateRange(range[0],range[1]);
}

void medClutToolBox::updateRange()
{
    updateRange(d->minRange->value(),d->maxRange->value());
}

void medClutToolBox::updateRange(double min,double max)
{
    if (!d->view)
        return;

    medAbstractData *data = d->view->layerData(0);
    if (!data->identifier().contains( "vtkDataMesh" ))
        return;
    
    medAbstractMeshData * dataMesh = static_cast<medAbstractMeshData*>(data);
    vtkMetaDataSet * mesh = static_cast<vtkMetaDataSet*>(dataMesh->mesh());

    if (!mesh || (mesh->GetType() != vtkMetaDataSet::VTK_META_SURFACE_MESH &&
                         mesh->GetType() != vtkMetaDataSet::VTK_META_VOLUME_MESH))
         return;
    
    vtkImageView2D * view2d = static_cast<medVtkViewBackend*>(d->view->backend())->view2D;
    vtkImageView3D * view3d = static_cast<medVtkViewBackend*>(d->view->backend())->view3D;
    vtkActor * actor2d = static_cast<vtkActor*>(view2d->FindDataSetActor(d->viewDataSet->value(d->view)));
    vtkActor * actor3d = static_cast<vtkActor*>(view3d->FindDataSetActor(d->viewDataSet->value(d->view)));
    vtkMapper * mapper2d = actor2d->GetMapper();
    vtkMapper * mapper3d = actor3d->GetMapper();
    
    double * range =new double[2];
    range[0] = min;
    range[1] = max;
    
    vtkLookupTable * lut = vtkLookupTable::SafeDownCast(mapper3d->GetLookupTable());
    
    if (!lut)
        return;
    
    lut->SetRange(min,max);
    mapper2d->SetLookupTable(lut);
    mapper3d->SetLookupTable(lut);
    view3d->GetScalarBar()->SetLookupTable(lut);
    view2d->GetScalarBar()->SetLookupTable(lut);
    
    d->rangeView->insert(d->view,range);

    d->view->viewWidget()->update();
}

void medClutToolBox::changeAttribute(int ind)
{
    if (ind<0 || ind>=d->listOfAttributes.size())
        return;

    if (!d->viewDataSet->contains(d->view))
        return;
    
    medAbstractData *data = d->view->layerData(0);
    if (!data->identifier().contains( "vtkDataMesh" ))
        return;
    
    medAbstractMeshData * dataMesh = static_cast<medAbstractMeshData*>(data);
    vtkMetaDataSet * mesh = static_cast<vtkMetaDataSet*>(dataMesh->mesh());

    if (!mesh || (mesh->GetType() != vtkMetaDataSet::VTK_META_SURFACE_MESH &&
                         mesh->GetType() != vtkMetaDataSet::VTK_META_VOLUME_MESH))
         return;

    vtkPointSet * pointSet = vtkPointSet::SafeDownCast(mesh->GetDataSet());

    if ( ! pointSet )
        return;

    vtkDataSetAttributes * attributes = NULL;
    bool isPoint = false;
    bool isCell = false;
    bool sameAttribute = false;

    if (pointSet->GetPointData() && pointSet->GetPointData()->HasArray(qPrintable(d->listOfAttributes[ind])))
    {
        attributes = pointSet->GetPointData();
        isPoint = true;
    }
    else if (pointSet->GetCellData() && pointSet->GetCellData()->HasArray(qPrintable(d->listOfAttributes[ind])))
    {
        attributes = pointSet->GetCellData();
        isCell = true;
    }  
    if (attributes)
    {
        attributes->SetActiveScalars(qPrintable(d->listOfAttributes[ind]));
        if (mesh->GetCurrentScalarArray()==attributes->GetArray(qPrintable(d->listOfAttributes[ind])))
            sameAttribute = true;
        else
        {
            mesh->SetCurrentActiveArray(attributes->GetArray(qPrintable(d->listOfAttributes[ind])));
        }
    }
    
    vtkImageView2D * view2d = static_cast<medVtkViewBackend*>(d->view->backend())->view2D;
    vtkImageView3D * view3d = static_cast<medVtkViewBackend*>(d->view->backend())->view3D;
    
    vtkActor * actor2d = static_cast<vtkActor*>(view2d->FindDataSetActor(d->viewDataSet->value(d->view)));
    vtkActor * actor3d = static_cast<vtkActor*>(view3d->FindDataSetActor(d->viewDataSet->value(d->view)));
    vtkMapper * mapper2d = actor2d->GetMapper();
    vtkMapper * mapper3d = actor3d->GetMapper();
    
    if (isPoint)
    {
        mapper2d->SetScalarModeToUsePointFieldData();
        mapper3d->SetScalarModeToUsePointFieldData();
    }
    else if (isCell)
    {
        mapper2d->SetScalarModeToUseCellFieldData();
        mapper3d->SetScalarModeToUseCellFieldData();
    }

    mapper2d->SelectColorArray(qPrintable(d->listOfAttributes[ind]));
    mapper2d->SetScalarVisibility(1);
    mapper3d->SelectColorArray(qPrintable(d->listOfAttributes[ind]));
    mapper3d->SetScalarVisibility(1);

    //// we set the lookuptable associated to the attribute.
    /*vtkLookupTable *lut = mesh->GetLookupTable();
    if (lut){
        mapper2d->SetLookupTable(lut);
        mapper2d->UseLookupTableScalarRangeOn();
        mapper2d->InterpolateScalarsBeforeMappingOn();
        mapper3d->SetLookupTable(lut);
        mapper3d->UseLookupTableScalarRangeOn();
        mapper3d->InterpolateScalarsBeforeMappingOn();
        view2d->GetScalarBar()->SetLookupTable(lut);
        view3d->GetScalarBar()->SetLookupTable(lut);
    }*/
    ////

    double * range = mesh->GetCurrentScalarRange();
    double step = (range[1]-range[0])/100;
    d->minRange->setStep(step);
    d->maxRange->setStep(step);
    dtkSignalBlocker dtkBlocker1 (d->minRange);
    dtkSignalBlocker dtkBlocker2 (d->maxRange);
    dtkBlocker1.blockSignals(true);
    dtkBlocker2.blockSignals(true);
    d->minRange->setMinimum(range[0]);
    d->minRange->setMaximum(range[1]);
    d->maxRange->setMinimum(range[0]);
    d->maxRange->setMaximum(range[1]);

    dtkBlocker1.blockSignals(false);
    dtkBlocker2.blockSignals(false);

    if (sameAttribute && d->rangeView->contains(d->view))
    {
        range = d->rangeView->value(d->view);
        d->minRange->setValue(range[0]);
        d->maxRange->setValue(range[1]);
    }
    else
    {
        d->minRange->setValue(range[0]);
        d->maxRange->setValue(range[1]);
    }

    
    updateRange();

    d->attributeChosen->insert(d->view,ind);
}

void medClutToolBox::exportMeshWithLUT()
{
    vtkMetaSurfaceMesh * mesh = vtkMetaSurfaceMesh::New();
    mesh->SetDataSet(d->viewDataSet->value(d->view));

    dtkSmartPointer<medAbstractData> dataToExport = medAbstractDataFactory::instance()->createSmartPointer("vtkDataMesh");
    dataToExport->setData(mesh);

    vtkPointSet * pointSet = vtkPointSet::SafeDownCast(mesh->GetDataSet());

    vtkDataSetAttributes * attributes = NULL;
    if (pointSet->GetPointData() && pointSet->GetPointData()->HasArray(qPrintable(d->listOfAttributes[d->attributeChosen->value(d->view)])))
    {
        attributes = pointSet->GetPointData();
    }
    else if (pointSet->GetCellData() && pointSet->GetCellData()->HasArray(qPrintable(d->listOfAttributes[d->attributeChosen->value(d->view)])))
    {
        attributes = pointSet->GetCellData();
    }

    if (attributes)
    {
        attributes->SetActiveScalars(qPrintable(d->listOfAttributes[d->attributeChosen->value(d->view)]));
        mesh->SetCurrentActiveArray(attributes->GetArray(qPrintable(d->listOfAttributes[d->attributeChosen->value(d->view)])));

        vtkImageView3D * view3d = static_cast<medVtkViewBackend*>(d->view->backend())->view3D;
        vtkActor * actor3d = static_cast<vtkActor*>(view3d->FindDataSetActor(d->viewDataSet->value(d->view)));
        vtkMapper * mapper3d = actor3d->GetMapper();
        vtkLookupTable * lut = static_cast<vtkLookupTable*>(mapper3d->GetLookupTable());
        if (lut)
        {
            mesh->GetCurrentScalarArray()->SetLookupTable(lut);
        }
        medDataManager::instance()->exportData(dataToExport);
    }
}
