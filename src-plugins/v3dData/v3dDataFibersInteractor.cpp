/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <v3dDataFibersInteractor.h>

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

#include <medDropSite.h>

#include <medImageFileLoader.h>

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

#include <QInputDialog>
#include <QColorDialog>

#ifdef WIN32
    #define isFinite(double) _finite(double)
#else
    #define isFinite(double) std::isfinite(double)
#endif


class v3dDataFibersInteractorPrivate
{
public:
    medAbstractData        *data;
    medAbstractImageView   *view;
    medAbstractData        *projectionData;
    vtkSmartPointer<vtkFiberDataSetManager> manager;
    vtkSmartPointer<vtkIsosurfaceManager>   roiManager;
    vtkSmartPointer<vtkFiberDataSet> dataset;
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;
    vtkRenderer *renderer2d;
    vtkRenderer *renderer3d;
    vtkRenderWindow *render;

    vtkSmartPointer <vtkActor> actor;
    vtkSmartPointer <vtkProperty> opacityProperty;

    QImage thumbnail;
    QWidget *toolboxWidget;
    QWidget *layerWidget;
    QWidget *toolbarWidget;
    QWidget *bundleToolboxWidget;

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

    medDropSite *dropOrOpenRoi;
    QComboBox    *roiComboBox;

    medBoolParameter *andParameter;
    medBoolParameter *notParameter;
    medBoolParameter *nullParameter;

    medTriggerParameter *saveParameter;
    medTriggerParameter *validateParameter;
    medTriggerParameter *resetParameter;
    medBoolParameter *addParameter;
    medTriggerParameter *tagParameter;
    medBoolGroupParameter *bundleOperationtGroupParameter;
    medDoubleParameter* opacityParam;

    medBoolParameter *showAllBundleParameter;
    medBoolParameter *showBundleBox;

    QStandardItemModel *bundlingModel;

    QTreeView *bundlingList;
};

v3dDataFibersInteractor::v3dDataFibersInteractor(medAbstractView *parent): medAbstractImageViewInteractor(parent),
    d(new v3dDataFibersInteractorPrivate)
{
    d->data    = NULL;
    d->dataset = NULL;
    d->view    = dynamic_cast<medAbstractImageView*>(parent);
    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;

    d->renderer2d = backend->renderer2D;
    d->renderer3d = backend->renderer3D;

    d->render = backend->renWin;

    d->manager = vtkFiberDataSetManager::New();
    d->manager->SetHelpMessageVisibility(0);
    d->roiManager = vtkIsosurfaceManager::New();

    vtkLookupTable* lut = vtkLookupTableManager::GetSpectrumLookupTable();
    d->manager->SetLookupTable(lut);
    d->manager->SetRenderWindowInteractor(d->render->GetInteractor());
    d->manager->SetRenderer(d->renderer3d);

    d->roiManager->SetRenderWindowInteractor(d->render->GetInteractor());

    lut->Delete();

    d->toolboxWidget = NULL;
    d->bundleToolboxWidget = NULL;

    d->colorFiberParameter = new medStringListParameter("colorFiberParameter", this);
    d->colorFiberParameter->setToolTip(tr("Choose the coloring method of the fibers."));
    d->colorFiberParameter->getLabel()->setText(tr("Color fibers by:"));
    QStringList colorModes = QStringList() << "Local orientation" << "Global orientation" << "Fractional anisotropy";
    d->colorFiberParameter->addItems(colorModes);
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

    d->bundleOperationtGroupParameter = new medBoolGroupParameter("bundleOperationtGroupParameter", this);
    d->bundleOperationtGroupParameter->addParameter(d->andParameter);
    d->bundleOperationtGroupParameter->addParameter(d->notParameter);
    d->bundleOperationtGroupParameter->addParameter(d->nullParameter);

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

}

v3dDataFibersInteractor::~v3dDataFibersInteractor()
{
    delete d;
    d = NULL;
}

QString v3dDataFibersInteractor::description() const
{
    return tr("Interactor to help visualising Fibers");
}

QString v3dDataFibersInteractor::identifier() const
{
    return "v3dDataFibersInteractor";
}

QStringList v3dDataFibersInteractor::handled() const
{
    return v3dDataFibersInteractor::dataHandled();
}


QStringList v3dDataFibersInteractor::dataHandled()
{
    QStringList d = QStringList() << "v3dDataFibers";

    return  d;
}

bool v3dDataFibersInteractor::isDataTypeHandled(QString dataType) const
{
    if (dataType == "v3dDataFibers")
        return true;

    return false;
}


bool v3dDataFibersInteractor::registered()
{
    medViewFactory *factory = medViewFactory::instance();
    factory->registerInteractor<v3dDataFibersInteractor>("v3dDataFibersInteractor",
                                                         QStringList () << "medVtkView" <<
                                                         v3dDataFibersInteractor::dataHandled());
    return true;
}



void v3dDataFibersInteractor::setData(medAbstractData *data)
{
    if (!data)
        return;

    if(data->identifier() != "v3dDataFibers")
        return;


    if (vtkFiberDataSet *dataset = static_cast<vtkFiberDataSet *>(data->data()))
    {
        d->dataset = dataset;
        d->manager->SetInput(d->dataset);
        if (!data->hasMetaData("BundleList"))
            data->addMetaData("BundleList", QStringList());
        if (!data->hasMetaData("BundleColorList"))
            data->addMetaData("BundleColorList", QStringList());

        d->actor = d->manager->GetOutput();
        d->opacityProperty = vtkSmartPointer <vtkProperty>::New();
        d->actor->SetProperty(d->opacityProperty);

        d->renderer2d->AddActor(d->actor);
        d->renderer3d->AddActor(d->actor);


        d->data = data;
    }
}

void v3dDataFibersInteractor::changeBundlingItem(QStandardItem *item)
{
    QString itemOldName = item->data(Qt::UserRole+1).toString();

    if (itemOldName != item->text())
    {
        this->changeBundleName(itemOldName,item->text());
        item->setData(item->text(),Qt::UserRole+1);
    }

    this->setBundleVisibility(item->text(), item->checkState());
}

void v3dDataFibersInteractor::changeBundleName(QString oldName, QString newName)
{
    if (!d->dataset)
        return;

    d->dataset->ChangeBundleName(oldName.toStdString(), newName.toStdString());
}

void v3dDataFibersInteractor::setVisibility(bool visible)
{
    d->manager->SetVisibility(visible);
}

void v3dDataFibersInteractor::setBoxVisibility(bool visible)
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

void v3dDataFibersInteractor::selectLineMode(bool value)
{
    if(value)
        this->setRenderingMode(v3dDataFibersInteractor::Lines);
}

void v3dDataFibersInteractor::selectRibbonMode(bool value)
{
    if(value)
        this->setRenderingMode(v3dDataFibersInteractor::Ribbons);
}

void v3dDataFibersInteractor::selectTubeMode(bool value)
{
    if(value)
        this->setRenderingMode(v3dDataFibersInteractor::Tubes);
}

void v3dDataFibersInteractor::setRenderingMode(RenderingMode mode)
{
    switch(mode)
    {
        case v3dDataFibersInteractor::Lines:
            d->manager->SetRenderingModeToPolyLines();
            break;

        case v3dDataFibersInteractor::Ribbons:
            d->manager->SetRenderingModeToRibbons();
            break;

        case v3dDataFibersInteractor::Tubes:
            d->manager->SetRenderingModeToTubes();
            break;
        default:
            qDebug() << "v3dDataFibersInteractor: unknown rendering mode";
    }
    d->render->Render();
}

void v3dDataFibersInteractor::activateGPU(bool activate)
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

void v3dDataFibersInteractor::setFiberColorMode(QString mode)
{
    if (mode == "Local orientation")
        this->setColorMode(v3dDataFibersInteractor::Local);
    else if (mode == "Global orientation")
        this->setColorMode(v3dDataFibersInteractor::Global);
    else if (mode == "Fractional anisotropy")
        this->setColorMode(v3dDataFibersInteractor::FA);
 }

void v3dDataFibersInteractor::setColorMode(ColorMode mode)
{
    switch(mode)
    {
        case v3dDataFibersInteractor::Local:
            d->manager->SetColorModeToLocalFiberOrientation();
            break;

        case v3dDataFibersInteractor::Global:
            d->manager->SetColorModelToGlobalFiberOrientation();
            break;

        case v3dDataFibersInteractor::FA:
            d->manager->SetColorModeToLocalFiberOrientation();
            for (int i=0; i<d->manager->GetNumberOfPointArrays(); i++)
            {
                if (d->manager->GetPointArrayName (i))
                {
                    if (strcmp ( d->manager->GetPointArrayName (i), "FA") == 0)
                    {
                        d->manager->SetColorModeToPointArray (i);
                        break;
                    }
                }
            }
            break;

        default:
            qDebug() << "v3dDataFibersInteractor: unknown color mode";
    }
    d->render->Render();
}

void v3dDataFibersInteractor::setBoxBooleanOperation(bool value)
{
    if (value)
        this->setBoxBooleanOperation(v3dDataFibersInteractor::Plus);
    else
        this->setBoxBooleanOperation(v3dDataFibersInteractor::Minus);
}

void v3dDataFibersInteractor::setBoxBooleanOperation(BooleanOperation op)
{
    switch(op)
    {
        case v3dDataFibersInteractor::Plus:
            d->manager->GetVOILimiter()->SetBooleanOperationToAND();
            break;

        case v3dDataFibersInteractor::Minus:
            d->manager->GetVOILimiter()->SetBooleanOperationToNOT();
            break;

        default:
            qDebug() << "v3dDataFibersInteractor: Unknown boolean operations";
    }

    d->manager->GetVOILimiter()->Modified();
    d->render->Render();

}

void v3dDataFibersInteractor::tagSelection()
{
    d->manager->SwapInputOutput();
    d->render->Render();
}

void v3dDataFibersInteractor::resetSelection()
{
    d->manager->Reset();
    d->render->Render();
}

void v3dDataFibersInteractor::validateSelection(const QString &name, const QColor &color)
{
    if (!d->data)
        return;

    double color_d[3] = {(double)color.red()/255.0, (double)color.green()/255.0, (double)color.blue()/255.0};

    d->manager->Validate (name.toAscii().constData(), color_d);

    d->renderer2d->AddActor (d->manager->GetBundleActor(name.toAscii().constData()));

    d->data->addMetaData("BundleList", name);
    d->data->addMetaData("BundleColorList", color.name());

    // reset to initial navigation state
    d->manager->Reset();

    d->render->Render();

}

void v3dDataFibersInteractor::saveBundlesInDataBase()
{
    if (!d->dataset)
        return;

    vtkFiberDataSet::vtkFiberBundleListType bundles = d->dataset->GetBundleList();
    vtkFiberDataSet::vtkFiberBundleListType::iterator it = bundles.begin();

    dtkSmartPointer <medAbstractData> tmpBundle;

    while (it!=bundles.end())
    {
        tmpBundle = medAbstractDataFactory::instance()->createSmartPointer("v3dDataFibers");
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

        QString uuid = QUuid::createUuid().toString();
        medDataManager::instance()->importNonPersistent (tmpBundle, uuid);

        ++it;
    }
}

void v3dDataFibersInteractor::bundleImageStatistics (const QString &bundleName,
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

    for (unsigned int i = 0;i < numberOfArrays;++i)
    {
        // To do, explore fibers one by one, compute mean value on each fiber, then deduce all stats
        // Put them inside maps
        QString arrayName = bundlePointData->GetArrayName(i);
        vtkDataArray *imageCoefficients = bundlePointData->GetArray(i);
        unsigned int tupleSize = imageCoefficients->GetElementComponentSize();
        if (tupleSize != 1)
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
            var[arrayName] = (squareSumData - sumData * sumData / numberOfLines) / (numberOfLines - 1.0);
        }
    }
}

void v3dDataFibersInteractor::computeBundleLengthStatistics (const QString &name,
                                                            double &mean,
                                                            double &min,
                                                            double &max,
                                                            double &var)
{
    itk::FiberBundleStatisticsCalculator::Pointer statCalculator = itk::FiberBundleStatisticsCalculator::New();
    statCalculator->SetInput (d->dataset->GetBundle (name.toAscii().constData()).Bundle);
    try
    {
        statCalculator->Compute();
    }
    catch(itk::ExceptionObject &e)
    {
        qDebug() << e.GetDescription();
        mean = 0.0;
        min  = 0.0;
        max  = 0.0;
        var  = 0.0;
        return;
    }

    statCalculator->GetLengthStatistics(mean, min, max, var);
}

void v3dDataFibersInteractor::bundleLengthStatistics(const QString &name,
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

void v3dDataFibersInteractor::clearStatistics(void)
{
    d->meanLengthList.clear();
    d->minLengthList.clear();
    d->maxLengthList.clear();
    d->varLengthList.clear();
}

void v3dDataFibersInteractor::setProjection(const QString& value)
{
    if (!d->view)
        return;

    if (value=="true") {
        d->view2d->AddDataSet( d->manager->GetCallbackOutput() );
    }
}

void v3dDataFibersInteractor::setRadius (int value)
{
    d->manager->SetRadius (value);
    d->render->Render();
}

void v3dDataFibersInteractor::setROI(medAbstractData *data)
{
    if (!data)
        return;

    if (data->identifier()!="itkDataImageUChar3")
        return;

    typedef itk::Image<unsigned char, 3> ROIType;

    ROIType::Pointer roiImage = static_cast<ROIType*>(data->data());

    if (!roiImage.IsNull())
    {
        itk::ImageToVTKImageFilter<ROIType>::Pointer converter = itk::ImageToVTKImageFilter<ROIType>::New();
        converter->SetReferenceCount(2);
        converter->SetInput(roiImage);
        converter->Update();

        ROIType::DirectionType directions = roiImage->GetDirection();
        vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
        matrix->Identity();
        for (int i=0; i<3; i++)
            for (int j=0; j<3; j++)
                matrix->SetElement (i, j, directions (i,j));

        d->manager->Reset();
        d->manager->GetROILimiter()->SetMaskImage (converter->GetOutput());
        d->manager->GetROILimiter()->SetDirectionMatrix (matrix);

        ROIType::PointType origin = roiImage->GetOrigin();
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

        d->roiManager->SetInput (converter->GetOutput());
        d->roiManager->SetDirectionMatrix (matrix2);

        // d->manager->GetROILimiter()->Update();
        d->roiManager->GenerateData();

        matrix->Delete();
        matrix2->Delete();
    }
    else
    {
        d->manager->GetROILimiter()->SetMaskImage (0);
        d->manager->GetROILimiter()->SetDirectionMatrix (0);

        d->roiManager->ResetData();
    }

    d->render->Render();
}

void v3dDataFibersInteractor::setRoiBoolean(int roi, int meaning)
{
    d->manager->GetROILimiter()->SetBooleanOperation (roi, meaning);

}

int v3dDataFibersInteractor::roiBoolean(int roi)
{
    return d->manager->GetROILimiter()->GetBooleanOperationVector()[roi+1];
}

void v3dDataFibersInteractor::setBundleVisibility(const QString &name, bool visibility)
{
    d->manager->SetBundleVisibility(name.toAscii().constData(), (int)visibility);
    d->render->Render();

}

bool v3dDataFibersInteractor::bundleVisibility(const QString &name) const
{
    return d->manager->GetBundleVisibility(name.toAscii().constData());
}

void v3dDataFibersInteractor::setAllBundlesVisibility(bool visibility)
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
    d->render->Render();
}


void v3dDataFibersInteractor::validateBundling()
{
    QString text = tr("Fiber bundle #") + QString::number(d->bundlingModel->rowCount()+1);

    QColor color = QColor::fromHsv(qrand()%360, 255, 210);
    this->validateSelection(text, color);
    this->addBundle(text, color);
}


void v3dDataFibersInteractor::addBundle (const QString &name, const QColor &color)
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


void v3dDataFibersInteractor::setRoiAddOperation(bool value)
{
    int roi = d->roiComboBox->currentIndex();
    if (value)
        this->setRoiBoolean(roi+1, 2);
}

void v3dDataFibersInteractor::setRoiNotOperation(bool value)
{
    int roi = d->roiComboBox->currentIndex();
    if (value)
        this->setRoiBoolean(roi+1, 1);
}

void v3dDataFibersInteractor::setRoiNullOperation(bool value)
{
    int roi = d->roiComboBox->currentIndex();
    if (value)
        this->setRoiBoolean(roi+1, 0);
}


void v3dDataFibersInteractor::importROI(const medDataIndex& index)
{
    dtkSmartPointer<medAbstractData> data = medDataManager::instance()->data(index);

    // we accept only ROIs (itkDataImageUChar3)
    // TODO try dynamic_cast of medAbstractMaskData would be better - RDE
    if (!data || data->identifier() != "itkDataImageUChar3")
        return;

    // put the thumbnail in the medDropSite as well
    // (code copied from @medDatabasePreviewItem)
    medAbstractDbController* dbc = medDataManager::instance()->controllerForDataSource(index.dataSourceId());
    QString thumbpath = dbc->metaData(index, medMetaDataKeys::ThumbnailPath);

    bool shouldSkipLoading = false;
    if ( thumbpath.isEmpty() )
    {
        // first try to get it from controller
        QImage thumbImage = dbc->thumbnail(index);
        if (!thumbImage.isNull())
        {
            d->dropOrOpenRoi->setPixmap(QPixmap::fromImage(thumbImage));
            shouldSkipLoading = true;
        }
    }

    if (!shouldSkipLoading)
    {
        medImageFileLoader *loader = new medImageFileLoader(thumbpath);
        connect(loader, SIGNAL(completed(const QImage&)), this, SLOT(setImage(const QImage&)));
        QThreadPool::globalInstance()->start(loader);
    }

    this->setROI(data);
}

void v3dDataFibersInteractor::clearRoi(void)
{
    // create dummy mask image
    medAbstractData *data = medAbstractDataFactory::instance()->create("itkDataImageUChar3");

    this->setROI(data);
    //TODO sound like a ugly hack - RDE
    data->deleteLater();

    // clear medDropSite and put text again
    d->dropOrOpenRoi->clear();
    d->dropOrOpenRoi->setText(tr("Drag-and-drop\nfrom the database\nto open a ROI."));

    d->render->Render();
}

void v3dDataFibersInteractor::selectRoi(int value)
{
    int boolean = this->roiBoolean (value);
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

void v3dDataFibersInteractor::setOpacity(double opacity)
{
    d->opacityProperty->SetOpacity(opacity);

    d->render->Render();
}

double v3dDataFibersInteractor::opacity() const
{
    return d->opacityProperty->GetOpacity();
}

void v3dDataFibersInteractor::setVisible(bool visible)
{
    int v = (visible) ? 1 : 0;
    d->actor->SetVisibility(v);

    d->renderer2d->Render();
    d->renderer3d->Render();
    d->render->Render();
}

bool v3dDataFibersInteractor::visibility() const
{
    return (d->actor->GetVisibility() == 1);
}


void v3dDataFibersInteractor::setWindowLevel (double &window, double &level)
{

}

void v3dDataFibersInteractor::windowLevel(double &window, double &level)
{

}


void v3dDataFibersInteractor::moveToSliceAtPosition (const QVector3D &position)
{

}

void v3dDataFibersInteractor::moveToSlice (int slice)
{

}

void v3dDataFibersInteractor::removeData()
{
    d->manager->Delete();
}


QWidget* v3dDataFibersInteractor::toolBoxWidget()
{
    if(!d->toolboxWidget)
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

        d->dropOrOpenRoi = new medDropSite(d->toolboxWidget);
        d->dropOrOpenRoi->setToolTip(tr("Drag-and-drop A ROI from the database."));
        d->dropOrOpenRoi->setText(tr("Drag-and-drop\nfrom the database\nto open a ROI."));
        d->dropOrOpenRoi->setCanAutomaticallyChangeAppereance(false);

        QPushButton *clearRoiButton = new QPushButton("Clear ROI", d->toolboxWidget);
        clearRoiButton->setToolTip(tr("Clear previously loaded ROIs."));

        //TODO : what the ... why 255 ? - RDE
        d->roiComboBox = new QComboBox(d->bundleToolboxWidget);
        for (int i=0; i<255; i++)
            d->roiComboBox->addItem(tr("ROI ")+QString::number(i+1));
        d->roiComboBox->setCurrentIndex(0);
        d->roiComboBox->setToolTip(tr("Select a ROI to modify how its interaction with the fibers affects whether they are displayed."));

        bundleToolboxLayout->addWidget(d->dropOrOpenRoi, 0, Qt::AlignCenter);
        bundleToolboxLayout->addWidget(clearRoiButton, 0, Qt::AlignCenter);

        connect (d->dropOrOpenRoi, SIGNAL(objectDropped(const medDataIndex&)), this, SLOT(importROI(const medDataIndex&)));
        connect (d->dropOrOpenRoi, SIGNAL(clicked()), this, SLOT(onDropSiteClicked()));
        connect (clearRoiButton,   SIGNAL(clicked()), this, SLOT(clearRoi()));
        connect (d->roiComboBox,   SIGNAL(currentIndexChanged(int)), this, SLOT(selectRoi(int)));

        QHBoxLayout *roiLayout = new QHBoxLayout(d->bundleToolboxWidget);
        roiLayout->addWidget(d->roiComboBox);
        d->bundleOperationtGroupParameter->setRadioButtonDirection(QBoxLayout::LeftToRight);
        roiLayout->addWidget(d->bundleOperationtGroupParameter->getRadioButtonGroup());

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

        bundleToolboxLayout->addWidget(d->bundlingList);
        bundleToolboxLayout->addWidget(d->showAllBundleParameter->getPushButton());

        this->updateWidgets();
    }

    return d->toolboxWidget;
}

QWidget* v3dDataFibersInteractor::layerWidget()
{
    QSlider *slider = d->opacityParam->getSlider();
    slider->setOrientation(Qt::Horizontal);
    return slider;
}


QWidget* v3dDataFibersInteractor::toolBarWidget()
{
    return NULL;
}


QImage v3dDataFibersInteractor::generateThumbnail(const QSize &size)
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

QList<medAbstractParameter*> v3dDataFibersInteractor::parameters()
{
    return d->parameters;
}

void v3dDataFibersInteractor::updateWidgets()
{
    if(!d->toolboxWidget)
        return;

    if(d->view->orientation() == medImageView::VIEW_ORIENTATION_3D)
        d->bundleToolboxWidget->show();
    else
        d->bundleToolboxWidget->hide();
}
