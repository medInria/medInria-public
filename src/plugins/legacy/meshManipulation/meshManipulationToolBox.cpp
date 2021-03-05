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
#include <medMessageController.h>
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
#include <vtkFieldData.h>
#include <vtkMatrix4x4.h>
#include <vtkMetaDataSet.h>
#include <vtkMetaDataSetSequence.h>
#include <vtkPointSet.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>

class ManipulationCallback : public vtkCommand
{
public:
    static ManipulationCallback* New()
    {
        return new ManipulationCallback;
    }

    virtual void Execute(vtkObject *caller, unsigned long eventId, void*)
    {
        vtkBoxWidget *widget = reinterpret_cast<vtkBoxWidget*>(caller);
        vtkSmartPointer<vtkTransform> t = vtkSmartPointer<vtkTransform>::New();
        widget->GetTransform(t);

        if (eventId == vtkCommand::InteractionEvent)
        {
            for(vtkMetaDataSet *metadata : datasets)
            {
                for(unsigned int i = 0; i < metadata->GetNumberOfActors(); i++)
                {
                    metadata->GetActor(i)->SetUserTransform(t);
                }
            }
        }
        else if(eventId == vtkCommand::EndInteractionEvent)
        {
            // save the transformation in the
            toolbox->addTransformationMatrixToUndoStack(t->GetMatrix());
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

    void applyExternalMatrix(vtkSmartPointer<vtkMatrix4x4> matrix)
    {
        vtkSmartPointer<vtkTransform> t = vtkSmartPointer<vtkTransform>::New();
        t->SetMatrix(matrix);

        for(vtkMetaDataSet *metadata : datasets)
        {
            for(unsigned int i = 0; i < metadata->GetNumberOfActors(); i++)
            {
                metadata->GetActor(i)->SetUserTransform(t);
            }
        }

        boxWidget->SetTransform(t);
    }

public:
    QList<vtkMetaDataSet*> datasets;
    meshManipulationToolBox* toolbox = nullptr;
    vtkBoxWidget* boxWidget = nullptr;
};

meshManipulationToolBox::meshManipulationToolBox(QWidget *parent)
    : medAbstractSelectableToolBox(parent)
{
    initializeUndoStack();

    QWidget *w = new QWidget(this);
    this->addWidget(w);
    QVBoxLayout* toolboxLayout = new QVBoxLayout();
    w->setLayout(toolboxLayout);
    
    QLabel *explanation = new QLabel("Select one or multiple layers to manually adjust.\n\n"
                                     "Scaling: hold <right-click> on the bounding box\n"
                                     "Rotation: hold <left-click> on the sides of the bounding box\n"
                                     "Translation: hold <mouse-wheel button>, or <left-click> on the central sphere of the bounding box\n\n", this);
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

    QHBoxLayout* undoLayout = new QHBoxLayout();

    _undoButton = new QPushButton("Undo", this);
    _undoButton->setShortcut(QKeySequence("Ctrl+Z"));
    _redoButton = new QPushButton("Redo", this);
    _redoButton->setShortcut(QKeySequence("Ctrl+Shift+Z"));

    _cancelButton = new QPushButton("Cancel");
    _cancelButton->setEnabled(false);
    undoLayout->addWidget(_undoButton);
    undoLayout->addWidget(_redoButton);
    undoLayout->addWidget(_cancelButton);

    toolboxLayout->addLayout(undoLayout);

    QLabel* registrationListText = new QLabel("Imported transformation matrices");
    registrationListText->setStyleSheet("QLabel{color:#ED6639;}");
    toolboxLayout->addWidget(registrationListText);

    _availableMatricesWidget = new QListWidget(this);
    _availableMatricesWidget->setToolTip("Lists all the imported registration matrices.");
    _availableMatricesWidget->setObjectName("availableMatricesWidget");
    _availableMatricesWidget->setMaximumHeight(96);
    _availableMatricesWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    toolboxLayout->addWidget(_availableMatricesWidget);

    QHBoxLayout* applyLayout = new QHBoxLayout();
    _applyButton = new QPushButton("Apply", this);
    _applyButton->setToolTip("Apply selected transformation.");

    _applyInverseButton = new QPushButton("Apply inverse", this);
    _applyInverseButton->setToolTip("Apply the inverse transformation");

    applyLayout->addWidget(_applyButton);
    applyLayout->addWidget(_applyInverseButton);
    toolboxLayout->addLayout(applyLayout);

    QHBoxLayout* exportLayout = new QHBoxLayout();
    _exportButton = new QPushButton("Export matrix");
    _exportButton->setEnabled(false);
    exportLayout->addWidget(_exportButton);

    _importButton = new QPushButton("Import matrix");
    _importButton->setEnabled(false);
    exportLayout->addWidget(_importButton);

    toolboxLayout->addLayout(exportLayout);

    connect(_scalingCheckBox,     SIGNAL(toggled(bool)), this, SLOT(enableScaling(bool)));
    connect(_rotationCheckBox,    SIGNAL(toggled(bool)), this, SLOT(enableRotation(bool)));
    connect(_translationCheckBox, SIGNAL(toggled(bool)), this, SLOT(enableTranslation(bool)));
    connect(_saveButton,   SIGNAL(clicked()), this, SLOT(save()));
    connect(_cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(_exportButton, SIGNAL(clicked()), this, SLOT(exportTransform()));
    connect(_importButton, SIGNAL(clicked()), this, SLOT(importTransform()));
    connect(_undoButton, &QPushButton::clicked, this, &meshManipulationToolBox::undo);
    connect(_redoButton, &QPushButton::clicked, this, &meshManipulationToolBox::redo);
    connect(_applyButton, &QPushButton::clicked, this, &meshManipulationToolBox::applySelectedTransformation);
    connect(_applyInverseButton, &QPushButton::clicked, this, &meshManipulationToolBox::applySelectedTransformationInverted);

    _boxWidget = vtkSmartPointer<vtkBoxWidget>::New();
    _callback = vtkSmartPointer<ManipulationCallback>::New();
    _callback->toolbox = this;
    _callback->boxWidget = _boxWidget;
    _boxWidget->AddObserver(vtkCommand::EndInteractionEvent, _callback);
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

            retrieveRegistrationMatricesFromLayers();
            handleLayerChange();
        }
    }
}

void meshManipulationToolBox::retrieveRegistrationMatricesFromLayers()
{
    _availableMatricesWidget->clear();
    _registrationMatrices.clear();

    unsigned int nbLayers = _view->layersCount();
    for (unsigned int i = 0; i < nbLayers; ++i)
    {
        medAbstractData* abstractData = _view->layerData(i);
        QString identifier = abstractData->identifier();
        if (identifier.contains("vtkDataMesh") || identifier.contains("EPMap"))
        {
            // check if the vtk dataset has matrices stored
            vtkMetaDataSet* metaData = static_cast<vtkMetaDataSet*>(abstractData->data());
            vtkFieldData* fieldData = metaData->GetDataSet()->GetFieldData();
            for (int j = 0; j < fieldData->GetNumberOfArrays(); ++j)
            {
                vtkDataArray* array = fieldData->GetArray(j);
                if (arrayHoldsRegistrationMatrix(array))
                {
                    QVector<double> matrixAsVector = retrieveMatrixFromArray(array);

                    QString arrayName(array->GetName());
                    tryAddingRegistrationMatrix(matrixAsVector, arrayName);
                }
            }
        }
    }
}

bool meshManipulationToolBox::arrayHoldsRegistrationMatrix(vtkDataArray* array)
{
    bool isNumeric = static_cast<bool>(array->IsNumeric());
    bool has1Component = (array->GetNumberOfComponents() == 1);
    bool has16Tuples = (array->GetNumberOfTuples() == 16);

    return isNumeric && has1Component && has16Tuples;
}

QVector<double> meshManipulationToolBox::retrieveMatrixFromArray(vtkDataArray* array)
{
    constexpr int stride = 16;
    QVector<double> flatMatrix(stride);
    // this code assumes row major ordering
    for (int i = 0; i < stride; ++i)
    {
        flatMatrix[i] = array->GetTuple1(i);
    }

    return flatMatrix;
}

void meshManipulationToolBox::tryAddingRegistrationMatrix(const QVector<double>& matrixAsVector,
                                                          QString registrationName)
{
    auto essentiallyEqual = [](double x1, double x2)
    {
        return std::fabs(x2 - x1) < 1e-15;
    };

    QString matrixName = registrationName;
    if (_registrationMatrices.contains(matrixName))
    {
        // already a registration matrix with this name, check if
        // its the same matrix
        bool sameRegistration = true;
        auto& oldMatrix = _registrationMatrices[matrixName];
        for (int i = 0; i < 16; ++i)
        {
            if (!essentiallyEqual(oldMatrix[i], matrixAsVector[i]))
            {
                // not the same transformation, exit early
                sameRegistration = false;
                break;
            }
        }

        if (sameRegistration)
        {
            return;
        }
        else
        {
            // assign a new name
            int index = 1;
            QString suffix = "_" + QString::number(index);
            while (_registrationMatrices.contains(matrixName + suffix))
            {
                suffix  = "_" + QString::number(++index);
            }
            matrixName += suffix;
        }
    }

    _registrationMatrices[matrixName] = matrixAsVector;
    _availableMatricesWidget->addItem(matrixName);
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
    reset();

    // Check if current layer is a mesh
    if (!_view.isNull() && _view->layersCount() > 0)
    {
        // Retrieve currently selected layers
        QList<int> selectedLayerIndices = this->getWorkspace()->getSelectedLayerIndices();

        _selectedData.clear();
        for (int layerIndex: selectedLayerIndices)
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

        // Give box size and position throught bounds: take all the
        // selected data into account
        double min = std::numeric_limits<double>::lowest();
        double max = std::numeric_limits<double>::max();
        // xmin, xmax, ymin, ymax, zmin, zmax
        double bounds[6] = {max, min, max, min, max, min};
        for (auto* data : _selectedData)
        {
            vtkDataSet *dataset = (static_cast<vtkMetaDataSet*>(data->data()))->GetDataSet();
            vtkPointSet *pointset = dynamic_cast<vtkPointSet*>(dataset);
            double* localBounds = pointset->GetBounds();
            for (int i = 0; i < 6; i += 2)
            {
                // minima
                if (localBounds[i] < bounds[i])
                {
                    bounds[i] = localBounds[i];
                }
                // maxima
                if (localBounds[i + 1] > bounds[i+1])
                {
                    bounds[i + 1] = localBounds[i + 1];
                }
            }
        }

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

    initializeUndoStack();
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
    auto showError = [=](QString errorMessage, QFile& f)
    {
        qDebug() << metaObject()->className() << "::" << errorMessage;
        medMessageController::instance()->showError(errorMessage, 3000);
    };

    QString filePath = QFileDialog::getOpenFileName(this, "Import the matrix file");
    if (!filePath.isEmpty())
    {
        QFile f(filePath);
        if (!f.open(QIODevice::ReadOnly))
        {
            qDebug() << "Can't open file" << filePath;
        }
        else
        {
            QTextStream stream(&f);

            int nbLinesRead = 0;
            QVector<double> m(16);
            while (!stream.atEnd())
            {
                QString line = stream.readLine();
                if (line[0] == '#')
                {
                    // skip commentary
                    continue;
                }

                std::stringstream tokenizer(line.toStdString());
                int i = 0;
                for (; i < 4 && tokenizer.good(); ++i)
                {
                    tokenizer >> m[nbLinesRead * 4 + i];
                }

                if (i != 4)
                {
                    // the line does not contain 4 values
                    showError("Matrix has wrong formatting", f);
                    f.close();
                    return;
                }

                nbLinesRead++;
            }

            if (nbLinesRead == 4)
            {
                tryAddingRegistrationMatrix(m, QFileInfo(filePath).baseName());
            }
            else
            {
                showError("Not a 4x4 matrix.", f);
            }

            f.close();
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

void meshManipulationToolBox::initializeUndoStack()
{
    _undoStack.clear();
    std::array<double, 16> identity = {1.0, 0.0, 0.0, 0.0,
                                       0.0, 1.0, 0.0, 0.0,
                                       0.0, 0.0, 1.0, 0.0,
                                       0.0, 0.0, 0.0, 1.0};
    _undoStack.push_back(identity);
    _undoStackPos = _undoStack.end();
}

void meshManipulationToolBox::addTransformationMatrixToUndoStack(vtkMatrix4x4* matrix)
{
    // remove any transformation that are after the current position
    if (_undoStackPos != _undoStack.end())
    {
         // there are elements that need to be removed
        _undoStack.erase(_undoStackPos, _undoStack.end());
        _undoStackPos = _undoStack.end();
    }

    // allow a maximum of 64 transformations in the undo stack
    if (_undoStack.size() >= 64)
    {
        _undoStack.pop_front();
    }

    std::array<double, 16> matrixAsArray;
    std::memcpy(matrixAsArray.data(), matrix->GetData(), 16 * sizeof(double));
    _undoStack.insert(_undoStackPos, matrixAsArray);
}

void meshManipulationToolBox::undo()
{
    auto pos = _undoStackPos;
    if (_undoStackPos != _undoStack.begin())
    {
        --pos;
        if(pos != _undoStack.begin())
        {
            _undoStackPos--;
            pos--;
        }
        auto matrix = vtkSmartPointer<vtkMatrix4x4>::New();
        auto& matrixAsVector = *pos;
        std::memcpy(matrix->GetData(), matrixAsVector.data(), 16 * sizeof(double));
        _callback->applyExternalMatrix(matrix);
    }

    if (_view)
    {
        _view->render();
    }
}

void meshManipulationToolBox::redo()
{
    if (_undoStackPos != _undoStack.end())
    {
        auto& matrixAsVector = *_undoStackPos;
        _undoStackPos++;
        auto matrix = vtkSmartPointer<vtkMatrix4x4>::New();
        std::memcpy(matrix->GetData(), matrixAsVector.data(), 16 * sizeof(double));
        _callback->applyExternalMatrix(matrix);
    }

    if (_view)
    {
        _view->render();
    }
}

void meshManipulationToolBox::cancel()
{
    reset();
    setupModificationBox();
}

void meshManipulationToolBox::applySelectedTransformation()
{
    auto matrix = vtkSmartPointer<vtkMatrix4x4>::New();
    if (buildMatrixFromSelectedTransform(matrix))
    {
        applyTransformationMatrix(matrix);
    }
}

void meshManipulationToolBox::applySelectedTransformationInverted()
{
    auto matrix = vtkSmartPointer<vtkMatrix4x4>::New();
    if (buildMatrixFromSelectedTransform(matrix))
    {
        matrix->Invert();
        applyTransformationMatrix(matrix);
    }
}

void meshManipulationToolBox::applyTransformationMatrix(vtkSmartPointer<vtkMatrix4x4> matrix)
{
    addTransformationMatrixToUndoStack(matrix);
    _callback->applyExternalMatrix(matrix);
    if (_view)
    {
        _view->render();
    }
}

bool meshManipulationToolBox::buildMatrixFromSelectedTransform(vtkSmartPointer<vtkMatrix4x4> matrix)
{
    auto selectedItems = _availableMatricesWidget->selectedItems();
    if (selectedItems.size())
    {
        // only one item can be selected so this is safe
        auto* item = selectedItems.first();
        QString name = item->text();
        if (_registrationMatrices.contains(name))
        {
            QVector<double>& matrixAsVector = _registrationMatrices[name];
            std::memcpy(matrix->GetData(), matrixAsVector.data(), 16 * sizeof(double));
            return true;
        }
    }

    return false;
}
