/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkViewVtkFibersInteractor.h>

#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkFiberDataSetManager.h>
#include <vtkImageView.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkLimitFibersToVOI.h>
#include <vtkPointData.h>
#include <vtkLookupTableManager.h>
#include <vtkFiberDataSet.h>
#include <vtkMatrix4x4.h>
#include <vtkLimitFibersToROI.h>
#include <vtkIsosurfaceManager.h>
#include <vtkCellArray.h>

#include <itkImage.h>
#include <itkImageToVTKImageFilter.h>
#include <itkFiberBundleStatisticsCalculator.h>
#include <itkCastImageFilter.h>

#include <medMessageController.h>
#include <medDataManager.h>
#include <medMetaDataKeys.h>
#include <medViewFactory.h>
#include <medAbstractData.h>
#include <medAbstractImageView.h>
#include <medVtkViewBackend.h>
#include <medAbstractDataFactory.h>
#include <medAbstractDbController.h>
#include <medBoolParameter.h>
#include <medBoolGroupParameter.h>
#include <medTriggerParameter.h>
#include <medDoubleParameter.h>
#include <medStringListParameter.h>
#include <medIntParameter.h>
#include <medDataIndexParameter.h>

#include <QInputDialog>
#include <QColorDialog>

#ifdef WIN32
    #define isFinite(double) _finite(double)
#else
    #define isFinite(double) std::isfinite(double)
#endif


class medVtkViewVtkFibersInteractorPrivate
{
public:
    template <class T> void setROI (medAbstractData *data);

    medAbstractData        *data;
    medAbstractImageView   *view;
    medAbstractData        *projectionData;
    vtkSmartPointer<vtkFiberDataSetManager> manager;
    vtkSmartPointer<vtkIsosurfaceManager>   roiManager;
    vtkSmartPointer<vtkFiberDataSet> dataset;
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;
    vtkRenderWindow *render;

    vtkSmartPointer <vtkActor> actor;
    vtkSmartPointer <vtkProperty> opacityProperty;

    QWidget *toolboxWidget;
    QPointer<QWidget> bundleToolboxWidget;

    QMap<QString, double> meanLengthList;
    QMap<QString, double> minLengthList;
    QMap<QString, double> maxLengthList;
    QMap<QString, double> varLengthList;

    QList<medAbstractParameter*> parameters;

    medStringListParameter *colorFiberParameter;
    medBoolParameter *gpuParameter;
    medBoolGroupParameter *shapesDisplayedGroupParameter;
    medBoolParameter *displayPolylinesParameter;
    medBoolParameter *displayRibbonsParameter;
    medBoolParameter *displayTubesParameter;
    medIntParameter *radiusParameter;

    medDataIndexParameter *roiParameter;
    QComboBox    *roiComboBox;
    QMap <int,int> roiLabels;
    QUuid roiImportUuid;

    medBoolParameter *andParameter;
    medBoolParameter *notParameter;
    medBoolParameter *nullParameter;

    medTriggerParameter *saveParameter;
    medTriggerParameter *validateParameter;
    medTriggerParameter *resetParameter;
    medBoolParameter *addParameter;
    medTriggerParameter *tagParameter;
    medBoolGroupParameter *bundleOperationGroupParameter;
    medDoubleParameter* opacityParam;

    medBoolParameter *showAllBundleParameter;
    medBoolParameter *showBundleBox;

    QStandardItemModel *bundlingModel;

    QTreeView *bundlingList;

    medIntParameter *slicingParameter;
};

template <class T>
void medVtkViewVtkFibersInteractorPrivate::setROI (medAbstractData *data)
{
    if (!data)
        return;

    typedef itk::Image<T, 3> InputROIType;
    typedef itk::Image<unsigned char, 3> ROIType;

    typename InputROIType::Pointer tmpROIImage = dynamic_cast<InputROIType*>(static_cast<itk::Object*>(data->data()));

    if (tmpROIImage.IsNull())
        return;

    typedef itk::CastImageFilter <InputROIType,ROIType> CastFilterType;
    typename CastFilterType::Pointer castFilter = CastFilterType::New();
    castFilter->SetInput(tmpROIImage);
    castFilter->Update();

    typename ROIType::Pointer roiImage = castFilter->GetOutput();
    roiImage->DisconnectPipeline();

    QList <int> labels;
    typedef itk::ImageRegionConstIterator <ROIType> ROIITeratorType;
    ROIITeratorType roiItr(roiImage,roiImage->GetLargestPossibleRegion());

    while (!roiItr.IsAtEnd())
    {
        if ((roiItr.Get() != 0)&&(!labels.contains(roiItr.Get())))
            labels.append(roiItr.Get());

        ++roiItr;
    }

    typename itk::ImageToVTKImageFilter<ROIType>::Pointer converter = itk::ImageToVTKImageFilter<ROIType>::New();
    converter->SetReferenceCount(2);
    converter->SetInput(roiImage);
    converter->Update();

    typename ROIType::DirectionType directions = roiImage->GetDirection();
    vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
    matrix->Identity();
    for (int i=0; i<3; i++)
        for (int j=0; j<3; j++)
            matrix->SetElement (i, j, directions (i,j));

    manager->Reset();
    manager->GetROILimiter()->SetMaskImage (converter->GetOutput());
    manager->GetROILimiter()->SetDirectionMatrix (matrix);

    typename ROIType::PointType origin = roiImage->GetOrigin();
    vtkMatrix4x4 *matrix2 = vtkMatrix4x4::New();
    matrix2->Identity();
    for (int i=0; i<3; i++)
        for (int j=0; j<3; j++)
            matrix2->SetElement (i, j, directions (i,j));
    double v_origin[4], v_origin2[4];
    for (int i=0; i<3; i++)
        v_origin[i] = origin[i];
    v_origin[3] = 1.0;
    matrix->MultiplyPoint (v_origin, v_origin2);
    for (int i=0; i<3; i++)
        matrix2->SetElement (i, 3, v_origin[i]-v_origin2[i]);

    roiManager->SetInput (converter->GetOutput());
    roiManager->SetDirectionMatrix (matrix2);

    // manager->GetROILimiter()->Update();
    roiManager->GenerateData();

    qSort(labels);
    roiLabels.clear();

    if (roiComboBox)
    {
        roiComboBox->blockSignals(true);
        roiComboBox->clear();
        unsigned int i = 0;
        foreach (int label, labels)
        {
            roiLabels[i] = label;
            roiComboBox->addItem("ROI " + QString::number(label));
            ++i;
        }

        roiComboBox->blockSignals(false);
        roiComboBox->setCurrentIndex(0);
    }

    matrix->Delete();
    matrix2->Delete();

    view->render();
}

medVtkViewVtkFibersInteractor::medVtkViewVtkFibersInteractor(medAbstractView *parent): medAbstractImageViewInteractor(parent),
    d(new medVtkViewVtkFibersInteractorPrivate)
{
    d->data    = NULL;
    d->dataset = NULL;
    d->view    = dynamic_cast<medAbstractImageView*>(parent);
    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;

    d->render = backend->renWin;

    d->manager = vtkFiberDataSetManager::New();
    d->manager->SetHelpMessageVisibility(0);
    d->roiManager = vtkIsosurfaceManager::New();

    vtkLookupTable* lut = vtkLookupTableManager::GetSpectrumLookupTable();
    d->manager->SetLookupTable(lut);
    d->manager->SetRenderWindowInteractor(d->render->GetInteractor());
    d->roiManager->SetRenderWindowInteractor(d->render->GetInteractor());

    lut->Delete();

    d->toolboxWidget = NULL;
    d->bundleToolboxWidget = NULL;

    d->colorFiberParameter = new medStringListParameter("colorFiberParameter", this);
    d->colorFiberParameter->setToolTip(tr("Choose the coloring method of the fibers."));
    d->colorFiberParameter->getLabel()->setText(tr("Color fibers by:"));
    d->colorFiberParameter->addItem("Local orientation");
    d->colorFiberParameter->addItem("Global orientation");
    d->parameters << d->colorFiberParameter;

    d->gpuParameter = new medBoolParameter("gpuFiberParameter", this);
    d->gpuParameter->setToolTip(tr("Select to use GPU hardware acceleration when possible."));
    d->gpuParameter->setText(tr("Use hardware acceleration"));
    d->gpuParameter->getCheckBox()->setEnabled(false);
    d->parameters << d->gpuParameter;

    d->shapesDisplayedGroupParameter = new medBoolGroupParameter("shapesDisplayedGroupParameter", this);
    d->parameters << d->shapesDisplayedGroupParameter;

    d->displayPolylinesParameter = new medBoolParameter("displayPolylinesParameter", this);
    d->displayPolylinesParameter->setToolTip(tr("Use polylines to draw the fibers."));
    d->displayPolylinesParameter->setText(tr("Display fibers as polylines"));
    d->displayPolylinesParameter->getLabel()->hide();
    d->shapesDisplayedGroupParameter->addParameter(d->displayPolylinesParameter);
    d->displayRibbonsParameter = new medBoolParameter("displayRibbonsParameter", this);
    d->displayRibbonsParameter->setToolTip(tr("Use ribbons to draw the fibers."));
    d->displayRibbonsParameter->setText(tr("Display fibers as ribbons"));
    d->displayRibbonsParameter->getLabel()->hide();
    d->shapesDisplayedGroupParameter->addParameter(d->displayRibbonsParameter);
    d->displayTubesParameter = new medBoolParameter("displayTubesParameter", this);
    d->displayTubesParameter->setToolTip(tr("Use tubes to draw the fibers."));
    d->displayTubesParameter->setText(tr("Display fibers as tubes"));
    d->displayTubesParameter->getLabel()->hide();
    d->shapesDisplayedGroupParameter->addParameter(d->displayTubesParameter);

    d->displayPolylinesParameter->setValue(true);

    d->radiusParameter = new medIntParameter("fibersRadiusParameter", this);
    d->radiusParameter->setToolTip(tr("Increase of decrease the radius of the fibers (except if there are being drawn with polylines)."));
    d->radiusParameter->getLabel()->setText(tr("Fibers radius:"));
    d->radiusParameter->setRange(1, 10);

    connect (d->colorFiberParameter, SIGNAL(valueChanged(QString)), this, SLOT(setFiberColorMode(QString)));
    connect (d->gpuParameter, SIGNAL(valueChanged(bool)), this, SLOT(activateGPU(bool)));
    connect (d->displayPolylinesParameter, SIGNAL(valueChanged(bool)), this, SLOT(selectLineMode (bool)));
    connect (d->displayRibbonsParameter, SIGNAL(valueChanged(bool)), this, SLOT(selectRibbonMode(bool)));
    connect (d->displayTubesParameter, SIGNAL(valueChanged(bool)), this, SLOT(selectTubeMode(bool)));
    connect (d->radiusParameter, SIGNAL(valueChanged(int)),  this, SLOT(setRadius(int)));

    //--Bundling
    d->andParameter = new medBoolParameter("andFiberParameter", this);
    d->andParameter->setToolTip(tr("If \"AND\" is selected fibers will need to overlap with this ROI to be displayed."));
    d->andParameter->setText("AND");
    d->andParameter->setValue(true);
    d->andParameter->getLabel()->hide();

    d->notParameter = new medBoolParameter("notFiberParameter", this);
    d->notParameter->setToolTip(tr("If \"NOT\" is selected fibers overlapping with this ROI will not be displayed."));
    d->notParameter->setText("NOT");
    d->notParameter->getLabel()->hide();

    d->nullParameter = new medBoolParameter("nullFiberParameter", this);
    d->nullParameter->setToolTip(tr("If \"NULL\" is selected this ROI won't be taken into account."));
    d->nullParameter->setText("NULL");
    d->nullParameter->getLabel()->hide();

    d->bundleOperationGroupParameter = new medBoolGroupParameter("bundleOperationGroupParameter", this);
    d->bundleOperationGroupParameter->addParameter(d->andParameter);
    d->bundleOperationGroupParameter->addParameter(d->notParameter);
    d->bundleOperationGroupParameter->addParameter(d->nullParameter);

    d->tagParameter = new medTriggerParameter("tagFiberParameter", this);
    d->tagParameter->setToolTip(tr("Tag the currently shown bundle to let medInria memorize it and another, new bundling box, will appear.\nThis new box will also isolate fibers but will also consider the previously \"tagged\" fibers."));
    d->tagParameter->setButtonText("Tag");

    d->addParameter = new medBoolParameter("addFiberParameter", this);
    d->addParameter->setToolTip(tr("Select to either include the fibers that overlap with the bundling box, or to include the ones that do not overlap."));
    d->addParameter->setText("Add");
    d->addParameter->setValue(true);

    d->resetParameter = new medTriggerParameter("resetFiberParameter", this);
    d->resetParameter->setToolTip(tr("Reset all previously tagged bundling boxes."));
    d->resetParameter->setButtonText("Reset");

    d->validateParameter = new medTriggerParameter("validateFiberParameter", this);
    d->validateParameter->setToolTip(tr("Save the current shown bundle and show useful information about it."));
    d->validateParameter->setButtonText("Validate");

    d->saveParameter = new medTriggerParameter("saveFiberParameter", this);
    d->saveParameter->setToolTip(tr("Save all bundles to database"));
    d->saveParameter->setButtonText("Save");

    d->showAllBundleParameter = new medBoolParameter("showAllBundleFiberParameter", this);
    d->showAllBundleParameter->setValue(true);
    d->showAllBundleParameter->setToolTip(tr("Uncheck if you do not want the previously validated bundles to be displayed."));
    d->showAllBundleParameter->setText(tr("Show all bundles"));
    d->parameters << d->showAllBundleParameter;

    d->showBundleBox = new medBoolParameter("showBundleFiberBox", this);
    d->showBundleBox->setToolTip(tr("Select to activate and show the fiber bundling box on the screen."));
    d->showBundleBox->setText("Activate bundling box");
    d->showBundleBox->setValue(false);

    d->bundlingModel = new QStandardItemModel(0, 1, this);
    d->bundlingModel->setHeaderData(0, Qt::Horizontal, tr("Fiber bundles"));

    d->opacityParam = new medDoubleParameter("Opacity", this);
    d->opacityParam->setRange(0,1);
    d->opacityParam->setSingleStep(0.01);
    d->opacityParam->setValue(1);
    d->parameters << d->opacityParam;

    connect(d->opacityParam, SIGNAL(valueChanged(double)), this, SLOT(setOpacity(double)));

    connect (d->validateParameter, SIGNAL(triggered()), this, SLOT(validateBundling()));
    connect (d->showBundleBox, SIGNAL(valueChanged(bool)), this, SLOT(setBoxVisibility (bool)));
    connect (d->addParameter, SIGNAL(valueChanged(bool)), this, SLOT(setBoxBooleanOperation(bool)));
    connect (d->showAllBundleParameter, SIGNAL(valueChanged(bool)), this, SLOT(setAllBundlesVisibility (bool)));
    connect (d->tagParameter, SIGNAL(triggered()),this, SLOT(tagSelection()));
    connect (d->resetParameter, SIGNAL(triggered()), this, SLOT(resetSelection()));
    connect (d->saveParameter, SIGNAL(triggered()), this, SLOT(saveBundlesInDataBase()));

    connect (d->andParameter, SIGNAL(valueChanged(bool)), this, SLOT(setRoiAddOperation(bool)));
    connect (d->notParameter, SIGNAL(valueChanged(bool)), this, SLOT(setRoiNotOperation(bool)));
    connect (d->nullParameter, SIGNAL(valueChanged(bool)), this, SLOT(setRoiNullOperation(bool)));

    connect (d->bundlingModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(changeBundlingItem(QStandardItem*)));
    connect(d->view, SIGNAL(orientationChanged()), this, SLOT(updateWidgets()));


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

    d->slicingParameter = new medIntParameter("Slicing", this);
    connect(d->slicingParameter, SIGNAL(valueChanged(int)), this, SLOT(moveToSlice(int)));
    connect(d->view->positionBeingViewedParameter(), SIGNAL(valueChanged(QVector3D)), this, SLOT(updateSlicingParam()));
}

medVtkViewVtkFibersInteractor::~medVtkViewVtkFibersInteractor()
{
    delete d;
    d = NULL;
}

QString medVtkViewVtkFibersInteractor::description() const
{
    return tr("Interactor to help visualising Fibers");
}

QString medVtkViewVtkFibersInteractor::identifier() const
{
    return "medVtkViewVtkFibersInteractor";
}

QStringList medVtkViewVtkFibersInteractor::handled() const
{
    return medVtkViewVtkFibersInteractor::dataHandled();
}


QStringList medVtkViewVtkFibersInteractor::dataHandled()
{
    QStringList d = QStringList() << "medVtkFibersData";

    return  d;
}

bool medVtkViewVtkFibersInteractor::isDataTypeHandled(QString dataType) const
{
    if (dataType == "medVtkFibersData")
        return true;

    return false;
}


bool medVtkViewVtkFibersInteractor::registered()
{
    medViewFactory *factory = medViewFactory::instance();
    factory->registerInteractor<medVtkViewVtkFibersInteractor>("medVtkViewVtkFibersInteractor",
                                                         QStringList () << "medVtkView" <<
                                                         medVtkViewVtkFibersInteractor::dataHandled());
    return true;
}

void medVtkViewVtkFibersInteractor::setInputData(medAbstractData *data)
{
    medAbstractInteractor::setInputData(data);

    if (!data)
        return;

    if(data->identifier() != "medVtkFibersData")
        return;


    if (vtkFiberDataSet *dataset = static_cast<vtkFiberDataSet *>(data->data()))
    {
        d->dataset = dataset;
        d->manager->SetInput(d->dataset);

        // Update color fiber parameter
        d->colorFiberParameter->blockSignals(true);
        d->colorFiberParameter->clear();
        d->colorFiberParameter->addItem("Local orientation");
        d->colorFiberParameter->addItem("Global orientation");
        vtkPolyData *fibersData = d->dataset->GetFibers();
        unsigned int numArrays = fibersData->GetPointData()->GetNumberOfArrays();
        for (unsigned int i = 0;i < numArrays;++i)
        {
            vtkDataArray *tmpArray = fibersData->GetPointData()->GetArray(i);
            if (tmpArray->GetNumberOfComponents() > 1)
                continue;

            d->colorFiberParameter->addItem(fibersData->GetPointData()->GetArrayName(i));
        }
        d->colorFiberParameter->blockSignals(false);

        if (!data->hasMetaData("BundleList"))
            data->addMetaData("BundleList", QStringList());
        if (!data->hasMetaData("BundleColorList"))
            data->addMetaData("BundleColorList", QStringList());

        d->actor = d->manager->GetOutput();
        d->opacityProperty = vtkSmartPointer <vtkProperty>::New();
        d->actor->SetProperty(d->opacityProperty);

        d->data = data;

        d->view2d->SetInput(d->actor, d->view->layer(d->data));

        //TODO - harmonise all of this setInput methode in vtkImageView.
        d->view3d->GetRenderer()->AddActor(d->actor);
        this->updateWidgets();
    }

    d->parameters << visibilityParameter();
}

void medVtkViewVtkFibersInteractor::changeBundlingItem(QStandardItem *item)
{
    QString itemOldName = item->data(Qt::UserRole+1).toString();

    if (itemOldName != item->text())
    {
        this->changeBundleName(itemOldName,item->text());
        item->setData(item->text(),Qt::UserRole+1);
    }

    this->setBundleVisibility(item->text(), item->checkState());
}

void medVtkViewVtkFibersInteractor::changeBundleName(QString oldName, QString newName)
{
    if ((!d->dataset)||(!d->manager))
        return;

    d->manager->ChangeBundleName(oldName.toStdString(), newName.toStdString());
}

void medVtkViewVtkFibersInteractor::setVisibility(bool visible)
{
    d->manager->SetVisibility(visible);
    d->view->render();
}

void medVtkViewVtkFibersInteractor::setBoxVisibility(bool visible)
{
    if (d->view && d->view->orientation() != medImageView::VIEW_ORIENTATION_3D)
    {
        medMessageController::instance()->showError("View must be in 3D mode to activate the bundling box",
                                                    3000);
        d->manager->SetBoxWidget(false);
        return;
    }

    d->manager->SetBoxWidget(visible);
}

void medVtkViewVtkFibersInteractor::selectLineMode(bool value)
{
    if(value)
        this->setRenderingMode(medVtkViewVtkFibersInteractor::Lines);
}

void medVtkViewVtkFibersInteractor::selectRibbonMode(bool value)
{
    if(value)
        this->setRenderingMode(medVtkViewVtkFibersInteractor::Ribbons);
}

void medVtkViewVtkFibersInteractor::selectTubeMode(bool value)
{
    if(value)
        this->setRenderingMode(medVtkViewVtkFibersInteractor::Tubes);
}

void medVtkViewVtkFibersInteractor::setRenderingMode(RenderingMode mode)
{
    switch(mode)
    {
        case medVtkViewVtkFibersInteractor::Lines:
            d->manager->SetRenderingModeToPolyLines();
            break;

        case medVtkViewVtkFibersInteractor::Ribbons:
            d->manager->SetRenderingModeToRibbons();
            break;

        case medVtkViewVtkFibersInteractor::Tubes:
            d->manager->SetRenderingModeToTubes();
            break;
        default:
            qDebug() << "medVtkViewVtkFibersInteractor: unknown rendering mode";
    }
    d->view->render();
}

void medVtkViewVtkFibersInteractor::activateGPU(bool activate)
{
    if (activate)
    {
        vtkFibersManager::UseHardwareShadersOn();
        d->manager->ChangeMapperToUseHardwareShaders();
    } else
    {
        vtkFibersManager::UseHardwareShadersOff();
        d->manager->ChangeMapperToDefault();
    }
}

void medVtkViewVtkFibersInteractor::setFiberColorMode(QString mode)
{
    if (mode == "Local orientation")
        d->manager->SetColorModeToLocalFiberOrientation();
    else if (mode == "Global orientation")
        d->manager->SetColorModelToGlobalFiberOrientation();
    else
    {
        d->manager->SetColorModeToLocalFiberOrientation();
        for (int i=0; i<d->manager->GetNumberOfPointArrays(); i++)
        {
            if (d->manager->GetPointArrayName (i))
            {
                QString pointArrayName = d->manager->GetPointArrayName (i);
                if (pointArrayName == mode)
                {
                    d->manager->SetColorModeToPointArray (i);
                    break;
                }
            }
        }
    }

    d->view->render();
 }

void medVtkViewVtkFibersInteractor::setBoxBooleanOperation(bool value)
{
    if (value)
        this->setBoxBooleanOperation(medVtkViewVtkFibersInteractor::Plus);
    else
        this->setBoxBooleanOperation(medVtkViewVtkFibersInteractor::Minus);
}

void medVtkViewVtkFibersInteractor::setBoxBooleanOperation(BooleanOperation op)
{
    switch(op)
    {
        case medVtkViewVtkFibersInteractor::Plus:
            d->manager->GetVOILimiter()->SetBooleanOperationToAND();
            break;

        case medVtkViewVtkFibersInteractor::Minus:
            d->manager->GetVOILimiter()->SetBooleanOperationToNOT();
            break;

        default:
            qDebug() << "medVtkViewVtkFibersInteractor: Unknown boolean operations";
    }

    d->manager->GetVOILimiter()->Modified();
    d->view->render();

}

void medVtkViewVtkFibersInteractor::tagSelection()
{
    d->manager->SwapInputOutput();
    d->view->render();
}

void medVtkViewVtkFibersInteractor::resetSelection()
{
    d->manager->Reset();
    d->view->render();
}

void medVtkViewVtkFibersInteractor::validateSelection(const QString &name, const QColor &color)
{
    if (!d->data)
        return;

    double color_d[3] = {(double)color.red()/255.0, (double)color.green()/255.0, (double)color.blue()/255.0};

    d->manager->Validate (name.toAscii().constData(), color_d);

    d->view2d->SetInput(d->manager->GetBundleActor(name.toAscii().constData()), d->view->layer(d->data));
    d->view3d->GetRenderer()->AddActor(d->manager->GetBundleActor(name.toAscii().constData()));

    d->data->addMetaData("BundleList", name);
    d->data->addMetaData("BundleColorList", color.name());

    // reset to initial navigation state
    d->manager->Reset();

    d->view->render();

}

void medVtkViewVtkFibersInteractor::saveBundlesInDataBase()
{
    if (!d->dataset)
        return;

    vtkFiberDataSet::vtkFiberBundleListType bundles = d->dataset->GetBundleList();
    vtkFiberDataSet::vtkFiberBundleListType::iterator it = bundles.begin();

    medAbstractData *tmpBundle;

    while (it!=bundles.end())
    {
        tmpBundle = medAbstractDataFactory::instance()->create("medVtkFibersData");
        if (!tmpBundle)
            return;

        vtkSmartPointer <vtkFiberDataSet> bundle = vtkFiberDataSet::New();
        bundle->SetFibers((*it).second.Bundle);

        tmpBundle->setData(bundle);

        QString newSeriesDescription = d->data->metadata ( medMetaDataKeys::SeriesDescription.key() );
        newSeriesDescription += " ";
        newSeriesDescription += (*it).first.c_str();
        tmpBundle->setMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );

        foreach ( QString metaData, d->data->metaDataList() )
        {
            if ((metaData == "BundleList")||(metaData == "BundleColorList"))
                continue;

            if (!tmpBundle->hasMetaData(metaData))
                tmpBundle->addMetaData (metaData, d->data->metaDataValues (metaData));
        }

        foreach ( QString property, d->data->propertyList() )
        tmpBundle->addProperty ( property,d->data->propertyValues ( property ) );

        QString generatedID = QUuid::createUuid().toString().replace("{","").replace("}","");
        tmpBundle->setMetaData ( medMetaDataKeys::SeriesID.key(), generatedID );

        medDataManager::instance()->importData(tmpBundle);

        ++it;
    }
}

void medVtkViewVtkFibersInteractor::bundleImageStatistics (const QString &bundleName,
                                                        QMap <QString, double> &mean,
                                                        QMap <QString, double> &min,
                                                        QMap <QString, double> &max,
                                                        QMap <QString, double> &var)
{
    vtkPolyData *bundleData = d->dataset->GetBundle(bundleName.toAscii().constData()).Bundle;

    if (!bundleData)
        return;

    if (bundleData->GetPointData()->HasArray("Tensors"))
    {
        // Specific TTK case
        itk::FiberBundleStatisticsCalculator::Pointer statCalculator = itk::FiberBundleStatisticsCalculator::New();
        statCalculator->SetInput (bundleData);

        try
        {
            statCalculator->Compute();
        }
        catch(itk::ExceptionObject &e)
        {
            qDebug() << e.GetDescription();
            return;
        }

        double meanVal, minVal, maxVal, varVal;
        statCalculator->GetADCStatistics(meanVal, minVal, maxVal, varVal);
        mean["ADC"] = meanVal;
        min["ADC"] = minVal;
        max["ADC"] = maxVal;
        var["ADC"] = varVal;

        statCalculator->GetFAStatistics(meanVal, minVal, maxVal, varVal);
        mean["FA"] = meanVal;
        min["FA"] = minVal;
        max["FA"] = maxVal;
        var["FA"] = varVal;
    }

    vtkPointData *bundlePointData = bundleData->GetPointData();
    unsigned int numberOfArrays = bundlePointData->GetNumberOfArrays();
    vtkPoints* points = bundleData->GetPoints();
    vtkCellArray* lines = bundleData->GetLines();
    lines->InitTraversal();

    for (unsigned int i = 0;i < numberOfArrays;++i)
    {
        // To do, explore fibers one by one, compute mean value on each fiber, then deduce all stats
        // Put them inside maps
        QString arrayName = bundlePointData->GetArrayName(i);
        vtkDataArray *imageCoefficients = bundlePointData->GetArray(i);
        unsigned int numComponents = imageCoefficients->GetNumberOfComponents();

        if (numComponents != 1)
            continue;

        vtkIdType  npts  = 0;
        vtkIdType* ptids = 0;
        vtkIdType test = lines->GetNextCell (npts, ptids);

        double sumData = 0;
        double squareSumData = 0;
        double minValue = HUGE_VAL;
        double maxValue = - HUGE_VAL;
        unsigned int numberOfLines = 0;

        // go over lines, each cell is a line
        while (test)
        {
            double meanFiberData = 0;

            for (int k=0; k<npts; k++)
            {
                double pt[3];
                points->GetPoint (ptids[k], pt);

                double tupleValue = 0;
                imageCoefficients->GetTuple(ptids[k], &tupleValue);

                if (k == 0)
                {
                    minValue = tupleValue;
                    maxValue = tupleValue;
                }

                if (minValue > tupleValue)
                    minValue = tupleValue;

                if (maxValue < tupleValue)
                    maxValue = tupleValue;

                meanFiberData += tupleValue;
            }

            meanFiberData /= npts;

            if (minValue > meanFiberData)
                minValue = meanFiberData;

            if (maxValue < meanFiberData)
                maxValue = meanFiberData;

            sumData += meanFiberData;
            squareSumData += meanFiberData * meanFiberData;
            ++numberOfLines;

            test = lines->GetNextCell (npts, ptids);
        }

        if (isFinite (sumData / numberOfLines))
        {
            mean[arrayName] = sumData / numberOfLines;
            min[arrayName] = minValue;
            max[arrayName] = maxValue;
            var[arrayName] = 0;
            if (numberOfLines > 1)
                var[arrayName] = (squareSumData - sumData * sumData / numberOfLines) / (numberOfLines - 1.0);
        }
    }
}

void medVtkViewVtkFibersInteractor::computeBundleLengthStatistics (const QString &name,
                                                                double &mean,
                                                                double &min,
                                                                double &max,
                                                                double &var)
{
    vtkPolyData *bundleData = d->dataset->GetBundle(name.toAscii().constData()).Bundle;

    if (!bundleData)
        return;

    vtkPoints* points = bundleData->GetPoints();
    vtkCellArray* lines = bundleData->GetLines();

    lines->InitTraversal();
    vtkIdType  npts  = 0;
    vtkIdType* ptids = 0;
    vtkIdType test = lines->GetNextCell (npts, ptids);

    double sumData = 0;
    double squareSumData = 0;
    double minValue = HUGE_VAL;
    double maxValue = - HUGE_VAL;
    unsigned int numberOfLines = 0;

    // go over lines, each cell is a line
    while (test)
    {
        double lengthFiber = 0;

        for (int k=1; k<npts; k++)
        {
            double pt1[3];
            double pt2[3];
            points->GetPoint (ptids[k-1], pt1);
            points->GetPoint (ptids[k], pt2);

            double normDiff = 0;
            for (unsigned int l = 0;l < 3;++l)
                normDiff += (pt2[l] - pt1[l])*(pt2[l] - pt1[l]);

            lengthFiber += sqrt(normDiff);
        }

        if (minValue > lengthFiber)
            minValue = lengthFiber;

        if (maxValue < lengthFiber)
            maxValue = lengthFiber;

        sumData += lengthFiber;
        squareSumData += lengthFiber * lengthFiber;
        ++numberOfLines;

        test = lines->GetNextCell (npts, ptids);
    }

    if (isFinite (sumData / numberOfLines))
    {
        mean = sumData / numberOfLines;
        min = minValue;
        max = maxValue;
        var = 0;
        if (numberOfLines > 1)
            var = (squareSumData - sumData * sumData / numberOfLines) / (numberOfLines - 1.0);
    }
    else
    {
        mean = 0;
        min = 0;
        max = 0;
        var = 0;
    }
}

void medVtkViewVtkFibersInteractor::bundleLengthStatistics(const QString &name,
                                                    double &mean,
                                                    double &min,
                                                    double &max,
                                                    double &var)
{
    if (!d->meanLengthList.contains(name)) {
        this->computeBundleLengthStatistics(name, mean, min, max, var);
        d->meanLengthList[name] = mean;
        d->minLengthList[name] = min;
        d->maxLengthList[name] = max;
        d->varLengthList[name] = var;
    }
    else {
        mean = d->meanLengthList[name];
        min  = d->minLengthList[name];
        max  = d->maxLengthList[name];
        var  = d->varLengthList[name];
    }
}

void medVtkViewVtkFibersInteractor::clearStatistics(void)
{
    d->meanLengthList.clear();
    d->minLengthList.clear();
    d->maxLengthList.clear();
    d->varLengthList.clear();
}

void medVtkViewVtkFibersInteractor::setProjection(const QString& value)
{
    if (!d->view)
        return;

    if (value=="true") {
        d->view2d->AddDataSet( d->manager->GetCallbackOutput() );
    }
}

void medVtkViewVtkFibersInteractor::setRadius (int value)
{
    d->manager->SetRadius (value);
    d->view->render();
}

void medVtkViewVtkFibersInteractor::setRoiBoolean(int roi, int meaning)
{
    d->manager->GetROILimiter()->SetBooleanOperation (roi, meaning);
    d->view->render();
}

int medVtkViewVtkFibersInteractor::roiBoolean(int roi)
{
    return d->manager->GetROILimiter()->GetBooleanOperationVector()[roi];
}

void medVtkViewVtkFibersInteractor::setBundleVisibility(const QString &name, bool visibility)
{
    d->manager->SetBundleVisibility(name.toAscii().constData(), (int)visibility);
    d->view->render();

}

bool medVtkViewVtkFibersInteractor::bundleVisibility(const QString &name) const
{
    return d->manager->GetBundleVisibility(name.toAscii().constData());
}

void medVtkViewVtkFibersInteractor::setAllBundlesVisibility(bool visibility)
{
    if (visibility)
    {
        d->manager->ShowAllBundles();
        for(int i = 0; i < d->bundlingModel->rowCount(); ++i)
            d->bundlingModel->item(i)->setCheckState(Qt::Checked);
    }
    else
    {
        d->manager->HideAllBundles();
        for(int i = 0; i < d->bundlingModel->rowCount(); ++i)
            d->bundlingModel->item(i)->setCheckState(Qt::Unchecked);
    }
    d->view->render();
}


void medVtkViewVtkFibersInteractor::validateBundling()
{
    unsigned int bundleNumber = 1;
    QString text = tr("Fiber bundle #") + QString::number(bundleNumber);

    // Handle potential deleted/renamed bundles
    while (!d->bundlingModel->findItems(text).empty())
    {
        bundleNumber++;
        text = tr("Fiber bundle #") + QString::number(bundleNumber);
    }

    QColor color = QColor::fromHsv(qrand()%360, 255, 210);
    this->validateSelection(text, color);
    this->addBundle(text, color);
}


void medVtkViewVtkFibersInteractor::addBundle (const QString &name, const QColor &color)
{
    int row = d->bundlingModel->rowCount();

    QStandardItem *item = new QStandardItem (name);
    item->setCheckable(true);
    item->setTristate(false);
    item->setEditable(true);
    item->setCheckState(Qt::Checked);

    QMap <QString, double> meanData;
    QMap <QString, double> minData;
    QMap <QString, double> maxData;
    QMap <QString, double> varData;

    double meanLength = 0.0;
    double minLength  = 0.0;
    double maxLength  = 0.0;
    double varLength  = 0.0;

    this->bundleImageStatistics(name, meanData, minData, maxData, varData);
    this->bundleLengthStatistics(name, meanLength, minLength, maxLength, varLength);

    foreach (QString key, meanData.keys())
    {
        QStandardItem *childItem1 = new QStandardItem (key + ": " + QString::number(meanData[key]));
        childItem1->setEditable(false);
        childItem1->appendRow(new QStandardItem (tr("mean: ")     + QString::number(meanData[key])));
        childItem1->appendRow(new QStandardItem (tr("variance: ") + QString::number(varData[key])));
        childItem1->appendRow(new QStandardItem (tr("min: ")      + QString::number(minData[key])));
        childItem1->appendRow(new QStandardItem (tr("max: ")      + QString::number(maxData[key])));

        item->appendRow(childItem1);
    }

    QStandardItem *childItem2 = new QStandardItem (tr("Length: ") + QString::number(meanLength));
    childItem2->setEditable(false);
    childItem2->appendRow(new QStandardItem (tr("mean: ")     + QString::number(meanLength)));
    childItem2->appendRow(new QStandardItem (tr("variance: ") + QString::number(varLength)));
    childItem2->appendRow(new QStandardItem (tr("min: ")      + QString::number(minLength)));
    childItem2->appendRow(new QStandardItem (tr("max: ")      + QString::number(maxLength)));

    item->appendRow(childItem2);
    item->setData(name,Qt::UserRole+1);

    d->bundlingModel->setItem(row, item);

    d->bundlingModel->setData(d->bundlingModel->index(row, 0, QModelIndex()),
                              true, Qt::CheckStateRole);

    d->bundlingModel->setData(d->bundlingModel->index(row, 0, QModelIndex()),
                              color, Qt::DecorationRole);

}


void medVtkViewVtkFibersInteractor::setRoiAddOperation(bool value)
{
    if (d->roiLabels.isEmpty())
        return;

    int roi = d->roiLabels[d->roiComboBox->currentIndex()];
    if (value)
        this->setRoiBoolean(roi, 2);
}

void medVtkViewVtkFibersInteractor::setRoiNotOperation(bool value)
{
    if (d->roiLabels.isEmpty())
        return;

    int roi = d->roiLabels[d->roiComboBox->currentIndex()];
    if (value)
        this->setRoiBoolean(roi, 1);
}

void medVtkViewVtkFibersInteractor::setRoiNullOperation(bool value)
{
    if (d->roiLabels.isEmpty())
        return;

    int roi = d->roiLabels[d->roiComboBox->currentIndex()];
    if (value)
        this->setRoiBoolean(roi, 0);
}

void medVtkViewVtkFibersInteractor::importROI(const medDataIndex& index)
{
    if(index == medDataIndex())
    {
        d->manager->GetROILimiter()->SetMaskImage (0);
        d->manager->GetROILimiter()->SetDirectionMatrix (0);
        d->roiManager->ResetData();
    }
    else if(index.isValidForSeries())
    {
        dtkSmartPointer<medAbstractData> data = medDataManager::instance()->retrieveData(index);

        // we accept only ROIs (medItkUChar3ImageData)
        if (!data ||
                (data->identifier() != "medItkUChar3ImageData" &&
                 data->identifier() != "medItkChar3ImageData" &&
                 data->identifier() != "medItkUShort3ImageData" &&
                 data->identifier() != "medItkShort3ImageData" &&
                 data->identifier() != "medItkUInt3ImageData" &&
                 data->identifier() != "medItkInt3ImageData" &&
                 data->identifier() != "medItkFloat3ImageData" &&
                 data->identifier() != "medItkDouble3ImageData"))
        {
            medMessageController::instance()->showError(tr("Unable to load ROI, format not supported yet"), 3000);
            return;
        }
        d->setROI<unsigned char>(data);
        d->setROI<char>(data);
        d->setROI<unsigned short>(data);
        d->setROI<short>(data);
        d->setROI<unsigned int>(data);
        d->setROI<int>(data);
        d->setROI<float>(data);
        d->setROI<double>(data);
    }
    else
        return;

    d->view->render();
}


void medVtkViewVtkFibersInteractor::clearRoi(void)
{
    // clear medDropSite and put text again
    d->roiParameter->clear();
    d->roiParameter->setText(tr("Drag-and-drop\nfrom the database\nor click here\nto open a ROI."));

    if (d->roiComboBox)
        d->roiComboBox->clear();

    d->roiLabels.clear();
}

void medVtkViewVtkFibersInteractor::selectRoi(int value)
{
    if (d->roiLabels.isEmpty())
        return;

    int boolean = this->roiBoolean (d->roiLabels[value]);
    switch (boolean)
    {
    case 2:
        d->andParameter->blockSignals(true);
        d->andParameter->setValue(true);
        d->andParameter->blockSignals(false);
        break;

    case 1:
        d->notParameter->blockSignals(true);
        d->notParameter->setValue(true);
        d->notParameter->blockSignals(false);
        break;

    case 0:
    default:
        d->nullParameter->blockSignals(true);
        d->nullParameter->setValue(true);
        d->nullParameter->blockSignals(false);
        break;
    }
}

void medVtkViewVtkFibersInteractor::setOpacity(double opacity)
{
    d->opacityProperty->SetOpacity(opacity);

    d->view->render();
}

void medVtkViewVtkFibersInteractor::setVisible(bool visible)
{
    int v = (visible) ? 1 : 0;
    d->actor->SetVisibility(v);

    d->view->render();
}

void medVtkViewVtkFibersInteractor::setWindowLevel (QHash<QString,QVariant>)
{

}


void medVtkViewVtkFibersInteractor::moveToSlice (int slice)
{
    //TODO find a way to get woorldCoordinate for slice from vtkInria.
    // instead of moving to the slice corresponding on the first layer dropped.
    if(d->view->is2D() && slice != d->view2d->GetSlice())
    {
        d->view2d->SetSlice(slice);
        d->view2d->Render();
    }
}

void medVtkViewVtkFibersInteractor::removeData()
{
    d->view2d->RemoveLayer(d->view->layer(d->data));
    d->manager->Delete();
}


QWidget* medVtkViewVtkFibersInteractor::buildToolBoxWidget()
{
    d->toolboxWidget = new QWidget;
    QVBoxLayout *toolBoxLayout = new QVBoxLayout(d->toolboxWidget);

    toolBoxLayout->addWidget(d->colorFiberParameter->getLabel());
    toolBoxLayout->addWidget(d->colorFiberParameter->getComboBox());
    toolBoxLayout->addWidget(d->gpuParameter->getCheckBox());
    toolBoxLayout->addWidget(d->shapesDisplayedGroupParameter->getRadioButtonGroup());
    toolBoxLayout->addWidget(d->radiusParameter->getLabel());
    d->radiusParameter->getSlider()->setOrientation(Qt::Horizontal);
    toolBoxLayout->addWidget(d->radiusParameter->getSlider());

    d->bundleToolboxWidget = new QWidget(d->toolboxWidget);
    QVBoxLayout *bundleToolboxLayout = new QVBoxLayout(d->bundleToolboxWidget);

    toolBoxLayout->addWidget(d->bundleToolboxWidget);

    d->roiParameter = new medDataIndexParameter("ROI", this);
    d->parameters << d->roiParameter;
    d->roiParameter->setToolTip(tr("Drag-and-drop A ROI from the database or click here."));
    d->roiParameter->setText(tr("Drag-and-drop\nfrom the database\nor click here\nto open a ROI."));

    QPushButton *clearRoiButton = new QPushButton("Clear ROI", d->toolboxWidget);
    clearRoiButton->setToolTip(tr("Clear previously loaded ROIs."));

    d->roiComboBox = new QComboBox(d->bundleToolboxWidget);
    if (!d->roiLabels.isEmpty())
    {
        QMap<int,int>::const_iterator i = d->roiLabels.constBegin();

        while (i != d->roiLabels.constEnd())
        {
            d->roiComboBox->addItem("ROI " + QString::number(i.value()));
            i++;
        }

        d->roiComboBox->setCurrentIndex(0);
    }

    d->roiComboBox->setToolTip(tr("Select a ROI to modify how its interaction with the fibers affects whether they are displayed."));

    bundleToolboxLayout->addWidget(d->roiParameter->getWidget(), 0, Qt::AlignCenter);
    bundleToolboxLayout->addWidget(clearRoiButton, 0, Qt::AlignCenter);

    connect (d->roiParameter, SIGNAL(valueChanged(const medDataIndex&)), this, SLOT(importROI(const medDataIndex&)));
    connect (clearRoiButton,   SIGNAL(clicked()), this, SLOT(clearRoi()));
    connect (d->roiComboBox,   SIGNAL(currentIndexChanged(int)), this, SLOT(selectRoi(int)));

    QHBoxLayout *roiLayout = new QHBoxLayout(d->bundleToolboxWidget);
    roiLayout->addWidget(d->roiComboBox);
    d->bundleOperationGroupParameter->setRadioButtonDirection(QBoxLayout::LeftToRight);
    roiLayout->addWidget(d->bundleOperationGroupParameter->getRadioButtonGroup());

    bundleToolboxLayout->addLayout(roiLayout);
    bundleToolboxLayout->addWidget(d->showBundleBox->getCheckBox());

    QHBoxLayout *bundlingLayout = new QHBoxLayout;
    bundlingLayout->addWidget(d->tagParameter->getPushButton());
    bundlingLayout->addWidget(d->addParameter->getPushButton());
    bundlingLayout->addWidget(d->resetParameter->getPushButton());
    bundlingLayout->addWidget(d->validateParameter->getPushButton());

    bundleToolboxLayout->addLayout(bundlingLayout);
    bundleToolboxLayout->addWidget(d->saveParameter->getPushButton());

    d->bundlingList = new QTreeView(d->toolboxWidget);
    d->bundlingList->setAlternatingRowColors(true);
    d->bundlingList->setMinimumHeight(150);
    d->bundlingList->setModel(d->bundlingModel);
    d->bundlingList->setEditTriggers(QAbstractItemView::SelectedClicked);

    d->bundlingList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(d->bundlingList,SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(bundlingListCustomContextMenu(const QPoint &)));

    bundleToolboxLayout->addWidget(d->bundlingList);
    bundleToolboxLayout->addWidget(d->showAllBundleParameter->getPushButton());

    this->updateWidgets();

    return d->toolboxWidget;
}

void medVtkViewVtkFibersInteractor::bundlingListCustomContextMenu(const QPoint &point)
{
    QMenu *menu = new QMenu;

    QAction *saveAction = new QAction(tr("Save"), this);
    saveAction->setIcon(QIcon(":icons/save.png"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveCurrentBundle()));
    menu->addAction(saveAction);

    QAction *removeAction = new QAction(tr("Remove"), this);
    removeAction->setIcon(QIcon(":icons/cross.svg"));
    connect(removeAction, SIGNAL(triggered()), this, SLOT(removeCurrentBundle()));
    menu->addAction(removeAction);

    menu->exec(QCursor::pos());
}

void medVtkViewVtkFibersInteractor::saveCurrentBundle()
{
    QModelIndex index = d->bundlingList->currentIndex();
    unsigned int dataIndex = index.row();

    if (!d->dataset)
        return;

    vtkFiberDataSet::vtkFiberBundleListType bundles = d->dataset->GetBundleList();
    vtkFiberDataSet::vtkFiberBundleListType::iterator it = bundles.begin();

    unsigned int i = 0;
    while (i < dataIndex)
    {
        i++;
        it++;
    }

    medAbstractData *savedBundle = medAbstractDataFactory::instance()->create("medVtkFibersData");
    if (!savedBundle)
        return;

    vtkSmartPointer <vtkFiberDataSet> bundle = vtkFiberDataSet::New();
    bundle->SetFibers((*it).second.Bundle);

    savedBundle->setData(bundle);

    QString newSeriesDescription = d->data->metadata ( medMetaDataKeys::SeriesDescription.key() );
    newSeriesDescription += " ";
    newSeriesDescription += (*it).first.c_str();
    savedBundle->setMetaData ( medMetaDataKeys::SeriesDescription.key(), newSeriesDescription );

    foreach ( QString metaData, d->data->metaDataList() )
    {
        if ((metaData == "BundleList")||(metaData == "BundleColorList"))
            continue;

        if (!savedBundle->hasMetaData(metaData))
            savedBundle->addMetaData (metaData, d->data->metaDataValues (metaData));
    }

    foreach ( QString property, d->data->propertyList() )
        savedBundle->addProperty ( property,d->data->propertyValues ( property ) );

    QString generatedID = QUuid::createUuid().toString().replace("{","").replace("}","");
    savedBundle->setMetaData ( medMetaDataKeys::SeriesID.key(), generatedID );

    medDataManager::instance()->importData(savedBundle);
}

void medVtkViewVtkFibersInteractor::removeCurrentBundle()
{
    QModelIndex index = d->bundlingList->currentIndex();
    QString bundleName = d->bundlingModel->item(index.row())->data(Qt::UserRole+1).toString();

    d->view2d->RemoveLayerActor(d->manager->GetBundleActor(bundleName.toAscii().constData()),d->view->layer(d->data));
    d->view3d->GetRenderer()->RemoveActor(d->manager->GetBundleActor(bundleName.toAscii().constData()));

    d->manager->RemoveBundle(bundleName.toAscii().constData());

    // TO DO : better handle bundle list: how to remove metadata from object ?
    //d->data->addMetaData("BundleList", name);
    //d->data->addMetaData("BundleColorList", color.name());

    // reset to initial navigation state
    d->manager->Reset();
    d->view->render();

    d->bundlingModel->removeRow(index.row());
}

QWidget* medVtkViewVtkFibersInteractor::buildLayerWidget()
{
    QSlider *slider = d->opacityParam->getSlider();
    slider->setOrientation(Qt::Horizontal);
    return slider;
}


QWidget* medVtkViewVtkFibersInteractor::buildToolBarWidget()
{
    d->slicingParameter->getSlider()->setOrientation(Qt::Horizontal);
    return d->slicingParameter->getSlider();
}


void medVtkViewVtkFibersInteractor::setUpViewForThumbnail()
{
    d->view->setOrientation(medImageView::VIEW_ORIENTATION_3D);
    d->view->reset();
    d->view3d->ShowAnnotationsOff();
}

QList<medAbstractParameter*> medVtkViewVtkFibersInteractor::linkableParameters()
{
    return d->parameters;
}

QList<medBoolParameter*> medVtkViewVtkFibersInteractor::mouseInteractionParameters()
{
    // no parameters related to mouse interactions
    return QList<medBoolParameter*>();
}

void medVtkViewVtkFibersInteractor::updateWidgets()
{
    if(!d->view->is2D())
    {
        if(!d->bundleToolboxWidget.isNull())
            d->bundleToolboxWidget->show();
        d->slicingParameter->getSlider()->setEnabled(false);
    }
    else
    {
        if(!d->bundleToolboxWidget.isNull())
            d->bundleToolboxWidget->hide();
        d->slicingParameter->getSlider()->setEnabled(true);
        this->updateSlicingParam();
    }
}

void medVtkViewVtkFibersInteractor::updateSlicingParam()
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
