/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medAbstractDataFactory.h>
#include <medAbstractProcessLegacy.h>
#include <medDataManager.h>
#include <meshManipulationToolBox.h>
#include <medPluginManager.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxBody.h>
#include <medToolBoxFactory.h>
#include <medUtilities.h>
#include <medUtilitiesVTK.h>
#include <medVtkViewBackend.h>

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>
#include <dtkCoreSupport/dtkSmartPointer.h>

#include <vtkActor.h>
#include <vtkBoxWidget.h>
#include <vtkCommand.h>
#include <vtkTransform.h>
#include <vtkMetaDataSet.h>
#include <vtkMetaDataSetSequence.h>
#include <vtkPointSet.h>
#include <vtkTransformFilter.h>

class ManipulationCallback : public vtkCommand
{
public:
    static ManipulationCallback* New()
    {
        return new ManipulationCallback;
    }

    virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
        vtkSmartPointer<vtkTransform> t = vtkSmartPointer<vtkTransform>::New();
        vtkBoxWidget *widget = reinterpret_cast<vtkBoxWidget*>(caller);
        widget->GetTransform(t);

        for(vtkMetaDataSet *metadata : datasets)
        {
            for(unsigned int i = 0; i < metadata->GetNumberOfActors(); i++)
            {
                metadata->GetActor(i)->SetUserTransform(t);
            }
        }
    }

    void addDataSet(vtkMetaDataSet *dataset)
    {
        if (!datasets.contains(dataset))
        {
            datasets.append(dataset);
        }
    }

    void clearDataSet()
    {
       datasets.clear();
    }

private:
    QList<vtkMetaDataSet*> datasets;
};

meshManipulationToolBox::meshManipulationToolBox(QWidget *parent)
    : medAbstractSelectableToolBox(parent)
{
    QWidget *w = new QWidget(this);
    this->addWidget(w);
    w->setLayout(new QVBoxLayout);
    
    QLabel *explanation = new QLabel("Drop one or multiple meshes in the view.\n\n"
                                     "Scaling: hold <right-click> on the bounding box\n\n"
                                     "Rotation: hold <left-click> on the sides of the bounding box\n\n"
                                     "Translation: hold <mouse-wheel button>, or <left-click> on the central sphere of the bounding box", this);
    explanation->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    explanation->setWordWrap(true);
    explanation->setStyleSheet("font: italic");
    w->layout()->addWidget(explanation);

    // scaling
    _scalingCheckBox = new QCheckBox(tr("Enable scaling"));
    _scalingCheckBox->setChecked(true);
    w->layout()->addWidget(_scalingCheckBox);

    // rotation
    _rotationCheckBox = new QCheckBox(tr("Enable rotation"));
    _rotationCheckBox->setChecked(true);
    w->layout()->addWidget(_rotationCheckBox);

    // translation
    _translationCheckBox = new QCheckBox(tr("Enable translation"));
    _translationCheckBox->setChecked(true);
    w->layout()->addWidget(_translationCheckBox);

    _saveButton = new QPushButton("Save");
    _saveButton->setToolTip("Current transform will be applied to all selected layers.");
    _saveButton->setEnabled(false);
    w->layout()->addWidget(_saveButton);

    _cancelButton = new QPushButton("Cancel");
    _cancelButton->setEnabled(false);
    w->layout()->addWidget(_cancelButton);

    _exportButton = new QPushButton("Export matrix");
    _exportButton->setEnabled(false);
    w->layout()->addWidget(_exportButton);

    _importButton = new QPushButton("Import matrix");
    _importButton->setEnabled(false);
    w->layout()->addWidget(_importButton);

    connect(_scalingCheckBox,     SIGNAL(toggled(bool)), this, SLOT(enableScaling(bool)));
    connect(_rotationCheckBox,    SIGNAL(toggled(bool)), this, SLOT(enableRotation(bool)));
    connect(_translationCheckBox, SIGNAL(toggled(bool)), this, SLOT(enableTranslation(bool)));
    connect(_saveButton,   SIGNAL(clicked()), this, SLOT(save()));
    connect(_cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(_exportButton, SIGNAL(clicked()), this, SLOT(exportTransform()));
    connect(_importButton, SIGNAL(clicked()), this, SLOT(importTransform()));

    _boxWidget = vtkSmartPointer<vtkBoxWidget>::New();
    _callback = vtkSmartPointer<ManipulationCallback>::New();
    _boxWidget->AddObserver(vtkCommand::InteractionEvent, _callback);
}

bool meshManipulationToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<meshManipulationToolBox>();
}

void meshManipulationToolBox::updateView()
{
     _view = nullptr;
    if(body()->isVisible())
    {
        resetToolbox();

        medAbstractView *view = this->getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView();
        medAbstractLayeredView *view3d = qobject_cast<medAbstractLayeredView*>(view);
        if (view3d)
        {
            _view = view3d;

            connect(this->getWorkspace(), SIGNAL(layerSelectionChanged(QList<int>)),
                    this, SLOT(handleLayerChange()),
                    Qt::UniqueConnection);

            connect(_view, SIGNAL(currentLayerChanged()),
                    this, SLOT(checkLayer()),
                    Qt::UniqueConnection);

            medUtilities::switchTo3D(_view, medUtilities::MSR);

            handleLayerChange();
        }
    }
}

void meshManipulationToolBox::checkLayer()
{
    if (_view)
    {
        medAbstractData* data = _view->layerData(_view->currentLayer());
        if (!(data->identifier().contains("EPMap") || data->identifier().contains("vtkDataMesh")))
        {
            displayMessageError("This toolbox is designed to be used with 3D meshes and EP maps");
        }
    }
}

void meshManipulationToolBox::handleLayerChange()
{
    if (_callback)
    {
        _callback->clearDataSet();
    }

    // Check if current layer is a mesh
    if (!_view.isNull() && _view->layersCount() > 0)
    {
        // Retrieve currently selected layers
        QList<int> selectedLayerIndices = this->getWorkspace()->getSelectedLayerIndices();

        _selectedData.clear();
        foreach (int layerIndex, selectedLayerIndices)
        {
            medAbstractData *data = _view->layerData(layerIndex);

            if ((data->identifier().contains("vtkDataMesh") ||
                 data->identifier().contains("EPMap")))
            {
                _selectedData.append(data);
            }
        }

        setupModificationBox();
    }
}

void meshManipulationToolBox::setupModificationBox()
{
    if (_selectedData.count() && !_view.isNull())
    {
        _boxWidget->Off();

        _boxWidget->SetInteractor(static_cast<medVtkViewBackend*>(_view->backend())->view3D->GetInteractor());
        _boxWidget->SetPlaceFactor(1.25);

        // Give box size and position throught bounds
        double bounds[6] = {}; // init to zero
        medAbstractData *data = _selectedData.at(0);
        vtkDataSet *dataset = (static_cast<vtkMetaDataSet*>(data->data()))->GetDataSet();
        vtkPointSet *pointset = dynamic_cast<vtkPointSet*>(dataset);
        pointset->GetBounds(bounds);
        _boxWidget->PlaceWidget(bounds);

        // Add datasets to the callback
        for (int i = 0; i < _selectedData.count(); ++i)
        {
            vtkMetaDataSet *metaDataSet = static_cast<vtkMetaDataSet*>(_selectedData.at(i)->data());
            _callback->addDataSet(metaDataSet);
        }

        _boxWidget->SetScalingEnabled(_scalingCheckBox->isChecked());
        _boxWidget->SetRotationEnabled(_rotationCheckBox->isChecked());
        _boxWidget->SetTranslationEnabled(_translationCheckBox->isChecked());
        _boxWidget->On();

        _saveButton->setEnabled(true);
        _cancelButton->setEnabled(true);
        _exportButton->setEnabled(true);
        _importButton->setEnabled(true);
    }
}

void meshManipulationToolBox::computeData()
{
    _outputs.clear();

    vtkSmartPointer<vtkTransform> t = vtkSmartPointer<vtkTransform>::New();
    _boxWidget->GetTransform(t);

    for (int i = 0; i < _selectedData.count(); ++i)
    {
        medAbstractData *data = _selectedData.at(i);
        vtkMetaDataSet *dataset   = static_cast<vtkMetaDataSet*>(data->data());

        vtkTransformFilter *transformFilter = vtkTransformFilter::New();
        dtkSmartPointer<medAbstractData> output = medAbstractDataFactory::instance()->createSmartPointer(data->identifier());
        if (data->identifier() == "vtkDataMesh")
        {
            vtkPointSet *newPointSet = transformDataSet(dataset, transformFilter, t);
            vtkMetaDataSet *metaDataset = dataset->NewInstance();
            metaDataset->SetDataSet(newPointSet);
            newPointSet->Delete();

            output->setData(metaDataset);
            metaDataset->Delete();
        }
        else if (data->identifier() == "vtkDataMesh4D")
        {
            vtkMetaDataSetSequence *seq = vtkMetaDataSetSequence::SafeDownCast(dataset);
            const std::vector<vtkMetaDataSet*>& datasetList = seq->GetMetaDataSetList();

            vtkMetaDataSetSequence *newSeq = vtkMetaDataSetSequence::New();
            for(vtkMetaDataSet *metaDataset : datasetList)
            {
                vtkPointSet *newPointSet = transformDataSet(metaDataset, transformFilter, t);
                vtkMetaDataSet *newDataset = metaDataset->NewInstance();
                newDataset->SetDataSet(newPointSet);
                newDataset->SetTime(metaDataset->GetTime());
                newPointSet->Delete();

                newSeq->AddMetaDataSet(newDataset);
                newDataset->Delete();
            }

            output->setData(newSeq);
            newSeq->Delete();
        }
        else if (data->identifier().contains("EPMap"))
        {
            // dedicated process
            dtkSmartPointer<medAbstractProcessLegacy> applyTransformProcess = dtkAbstractProcessFactory::instance()->createSmartPointer("ApplyTransformProcess");
            if (applyTransformProcess)
            {
                double matrix[16];
                vtkMatrix4x4* m = t->GetMatrix();
                std::memcpy(matrix, m->GetData(), 16 * sizeof(*matrix));

                applyTransformProcess->setInput(data);
                applyTransformProcess->setParameter(matrix, 16, 0);
                applyTransformProcess->update();
                output = applyTransformProcess->output()->clone();
            }
        }
        transformFilter->Delete();
     
        medUtilities::setDerivedMetaData(output, data, "manually modified");
        medDataManager::instance()->importData(output, false);
        _outputs.append(output.data());
    }
}

vtkPointSet* meshManipulationToolBox::transformDataSet(vtkMetaDataSet *dataset,
                                                       vtkTransformFilter *transformFilter,
                                                       vtkTransform *t)
{
    vtkPointSet *pointset = vtkPointSet::SafeDownCast(dataset->GetDataSet());
    transformFilter->SetInputData(pointset);
    transformFilter->SetTransform(t);
    transformFilter->Update();

    vtkPointSet *newPointset = pointset->NewInstance();
    newPointset->DeepCopy(transformFilter->GetOutput());

    return newPointset;
}

void meshManipulationToolBox::save()
{
    computeData();
}

medAbstractData *meshManipulationToolBox::processOutput()
{
    if (_outputs.count())
    {
        return _outputs.at(0);
    }
    return nullptr;
}

void meshManipulationToolBox::cancel()
{
    reset();
    setupModificationBox();
}

void meshManipulationToolBox::clear()
{
    medToolBox::clear();

    reset();
    if (this->getWorkspace())
    {
        disconnect(this->getWorkspace(), SIGNAL(layerSelectionChanged(QList<int>)),
                   this, SLOT(handleLayerChange()));
    }
}

void meshManipulationToolBox::reset()
{
    _outputs.clear();
    if (_callback)
    {
        _callback->clearDataSet();
    }

    // iterate on all layers and reset the transform
    if (!_view.isNull() && _view->layersCount())
    {
        vtkSmartPointer<vtkTransform> t_id = vtkSmartPointer<vtkTransform>::New();
        for (unsigned int i = 0; i < _view->layersCount(); ++i)
        {
            medAbstractData *data = _view->layerData(i);
            if ((data->identifier().contains("vtkDataMesh") ||
                 data->identifier().contains("EPMap")))
            {
                vtkMetaDataSet *dataset = static_cast<vtkMetaDataSet*>(data->data());
                for(unsigned int j = 0; j < dataset->GetNumberOfActors(); j++)
                {
                    dataset->GetActor(j)->SetUserTransform(t_id);
                }
            }
        }
        if (_boxWidget && _boxWidget->GetInteractor())
        {
            _boxWidget->Off();
        }
    }
}

void meshManipulationToolBox::resetToolbox()
{
    reset();

    _cancelButton->setEnabled(false);
    _saveButton->setEnabled(false);
    _exportButton->setEnabled(false);
    _importButton->setEnabled(false);
}

void meshManipulationToolBox::exportTransform()
{
    if (_boxWidget && _boxWidget->GetEnabled())
    {
        vtkSmartPointer<vtkTransform> t = vtkSmartPointer<vtkTransform>::New();
        _boxWidget->GetTransform(t);

        vtkSmartPointer<vtkMatrix4x4> m = vtkSmartPointer<vtkMatrix4x4>::New();
        m->DeepCopy(t->GetMatrix());

        QByteArray matrixStr;
        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                matrixStr += QByteArray::number(m->GetElement(i, j)) + "\t";
            }
            matrixStr += "\n";
        }

        QString filePath = QFileDialog::getSaveFileName(this, tr("Export the transformation matrix in txt"),
                                                        "/home/transformation.txt",
                                                        tr("Text files (*.txt)"));
        if (!filePath.isEmpty())
        {
            QFile f(filePath);
            if (!f.open(QIODevice::WriteOnly))
            {
                qDebug() << "Can't open file" << filePath;
                return;
            }

            f.write(matrixStr);
            f.close();

            qDebug() << "Done exporting tranform!";
        }
    }
}

void meshManipulationToolBox::importTransform()
{
    QString filePath = QFileDialog::getOpenFileName(0, "Import the matrix file");
    if (!filePath.isEmpty())
    {
        QFile f(filePath);
        if (!f.open(QIODevice::ReadOnly))
        {
            qDebug() << "Can't open file" << filePath;
        }
        else
        {
            QByteArray matrixStr = f.readAll();
            f.close();

            vtkSmartPointer<vtkMatrix4x4> m = vtkSmartPointer<vtkMatrix4x4>::New();
            int i = 0, j = 0;
            for(QByteArray line : matrixStr.split('\n'))
            {
                for(QByteArray num : line.split('\t'))
                {
                    m->SetElement(i, j, num.toDouble());
                    j++;
                }
                i++;j = 0;
            }

            vtkSmartPointer<vtkTransform> t = vtkSmartPointer<vtkTransform>::New();
            t->SetMatrix(m);

            _boxWidget->SetTransform(t);
            _boxWidget->InvokeEvent(vtkCommand::InteractionEvent);

            medAbstractView *view = this->getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView();
            view->render();
        }
    }
}

dtkPlugin* meshManipulationToolBox::plugin()
{
    medPluginManager *pm = medPluginManager::instance();
    dtkPlugin *plugin = pm->plugin("Mesh Manipulation");
    return plugin;
}

void meshManipulationToolBox::enableScaling(bool state)
{
    _boxWidget->SetScalingEnabled(static_cast<int>(state));
}

void meshManipulationToolBox::enableRotation(bool state)
{
    _boxWidget->SetRotationEnabled(static_cast<int>(state));
}

void meshManipulationToolBox::enableTranslation(bool state)
{
    _boxWidget->SetTranslationEnabled(static_cast<int>(state));
}
