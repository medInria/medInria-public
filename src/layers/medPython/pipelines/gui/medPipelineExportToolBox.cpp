#include "medPipelineExportToolBox.h"

#include <vtkFieldData.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <medDataManager.h>
#include <medMessageController.h>
#include <medMetaDataKeys.h>
#include <medPluginManager.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medViewContainer.h>

#include "medPipelineWorkspace.h"

namespace med::pipeline
{

class PipelineExportToolBoxPrivate
{
public:
    bool exporting;
};

bool PipelineExportToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<PipelineExportToolBox>();
}

PipelineExportToolBox::PipelineExportToolBox(QWidget* parent) : medToolBox(parent), d(new PipelineExportToolBoxPrivate())
{
    QWidget* mainWidget = new QWidget();
    addWidget(mainWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainWidget->setLayout(mainLayout);

    QPushButton* exportToNavXButton = new QPushButton(QIcon(":icons/scene.png"), "To EnSite NavX");
    exportToNavXButton->setToolTip("Export these meshes as an .xml file loadable in Abbott Laboratories mapping systems.");
    exportToNavXButton->setIconSize(QSize(40, 40));
    exportToNavXButton->setMinimumSize(150, 60);
    exportToNavXButton->setMaximumSize(150, 60);
    exportToNavXButton->setObjectName("exportToNavXButton");
    connect(exportToNavXButton, SIGNAL(clicked()), this, SLOT(exportToNavX()));

    QPushButton* exportToCartoButton = new QPushButton(QIcon(":icons/scene.png"), "To CARTO");
    exportToCartoButton->setToolTip("Export these meshes as a group of .vtk files loadable in Biosense-Webster mapping systems.");
    exportToCartoButton->setIconSize(QSize(40, 40));
    exportToCartoButton->setMinimumSize(150, 60);
    exportToCartoButton->setMaximumSize(150, 60);
    exportToCartoButton->setObjectName("exportToCartoButton");
    connect(exportToCartoButton, SIGNAL(clicked()), this, SLOT(exportToCarto()));

    QHBoxLayout* layoutLine0 = new QHBoxLayout();
    layoutLine0->addWidget(exportToNavXButton);
    layoutLine0->addWidget(exportToCartoButton);

    QPushButton* exportSceneButton = new QPushButton(QIcon(":icons/scene.png"), "To Viewer or RHYTHMIA HDx");
    exportSceneButton->setToolTip("Export these meshes as a group of .xml and .vtk files.\nThese files can be imported in the MUSICardio viewer.\n"
                                  "Vtk files can be imported in Boston Scientific mapping systems.");
    exportSceneButton->setIconSize(QSize(40, 40));
    exportSceneButton->setMinimumSize(220, 60);
    exportSceneButton->setMaximumSize(220, 60);
    exportSceneButton->setObjectName("exportSceneButton");
    connect(exportSceneButton, SIGNAL(clicked()), this, SLOT(exportScene()));

    QHBoxLayout* layoutLine1 = new QHBoxLayout();
    layoutLine1->addWidget(exportSceneButton);

    mainLayout->addLayout(layoutLine0);
    mainLayout->addLayout(layoutLine1);
}

PipelineExportToolBox::~PipelineExportToolBox(void)
{
    delete d;
}

void PipelineExportToolBox::exportToNavX()
{
    medAbstractLayeredView * view3d = dynamic_cast<medAbstractLayeredView*>(getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView());
    QList<medAbstractData*> dataList;
    if(view3d)
    {
        for (unsigned int i = 0; i < view3d->layersCount(); i++)
        {
            dataList.append(view3d->layerData(i));
        }

        QString fileName = QFileDialog::getSaveFileName(0, tr("Save NavX file"), QDir::home().absolutePath() + "/navX.xml");
        if(fileName != "")
        {

            //medDataManager::instance()->exportDataToPath(dataList, fileName, "navxDifWriter");
        }
    }
    else
    {
        medMessageController::instance()->showError(tr("Select the view containing the meshes to export"), 5000);
    }
}

void PipelineExportToolBox::exportToCarto()
{
    medAbstractLayeredView * view = dynamic_cast<medAbstractLayeredView*>(getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView());

    if(view)
    {
        // Removing step number from series description
        QList<QString> descriptions = getSeriesDescriptions(view);
        removeStepNumbersInSeriesDescription(view);

        QList<medAbstractData*> dataList;
        for (unsigned int i = 0; i < view->layersCount(); i++)
        {
            dataList.append(view->layerData(i));
        }

        QString filePath = QFileDialog::getExistingDirectory(0, tr("Select a directory to save your CARTO files"), QDir::home().absolutePath());
        if(filePath != "")
        {
            // The .vtk extension is needed to validate cartoVtkWriter::canWrite
            // If there is only 1 mesh to export, we give it its final name here
            QString currentDescription = dataList[0]->metadata(medMetaDataKeys::SeriesDescription.key());
            QString baseFileName = filePath + QString("/") + currentDescription + ".vtk";

            // Export
            //medDataManager::instance()->exportDataToPath(dataList, baseFileName, "cartoVtkWriter");

            // Wait for the export ending
            connect(medDataManager::instance(), SIGNAL(exportFinished()), this, SLOT(stopWaiting()));
            d->exporting = true;
            while (d->exporting)
            {
                QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
            }
        }

        // Put back old descriptions in file
        setSeriesDescriptions(view, descriptions);
    }
    else
    {
        medMessageController::instance()->showError(tr("Select the view containing the meshes to export"), 5000);
    }
}

void PipelineExportToolBox::stopWaiting()
{
    d->exporting = false;
}

void PipelineExportToolBox::exportScene()
{
    medAbstractLayeredView * view = dynamic_cast<medAbstractLayeredView*>(getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView());

    if(view)
    {
        // Removing step number from series description
        QList<QString> descriptions = getSeriesDescriptions(view);
        removeStepNumbersInSeriesDescription(view);

        //Export scene
        QDir workingDir(getWorkspace()->tabbedViewContainers()->getFirstSelectedContainer()->saveScene());

        //Removing field data
        QDirIterator it(workingDir.absolutePath(), QStringList() << "*.vtk", QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            it.next();
            vtkSmartPointer<vtkGenericDataObjectReader> reader =
                    vtkSmartPointer<vtkGenericDataObjectReader>::New();
            reader->SetFileName(it.filePath().toStdString().c_str());
            reader->Update();

            // All of the standard data types can be checked and obtained like this:
            if(reader->IsFilePolyData())
            {
                vtkPolyData* polyData = reader->GetPolyDataOutput();
                vtkSmartPointer<vtkFieldData> fieldData = polyData->GetFieldData();
                vtkSmartPointer<vtkFieldData> newFieldData = vtkSmartPointer<vtkFieldData>::New();

                for (int i = 0; i < fieldData->GetNumberOfArrays(); i++)
                {
                    QString arrayName = fieldData->GetArrayName(i);

                    if (!arrayName.startsWith("medMetaData::"))
                    {
                        newFieldData->AddArray(fieldData->GetAbstractArray(i));
                    }
                }

                polyData->SetFieldData(newFieldData);
                vtkMetaSurfaceMesh* mesh = vtkMetaSurfaceMesh::New();
                mesh->SetDataSet(polyData);
                mesh->Write(it.filePath().toStdString().c_str());
                mesh->Delete();
            }
        }
        // Putting back the original series descriptions
        setSeriesDescriptions(view, descriptions);
    }
}

QList<QString> PipelineExportToolBox::getSeriesDescriptions(medAbstractLayeredView* view)
{
    QList<QString> descriptions;

    for (unsigned int i = 0 ; i < view->layersCount() ; i++)
    {
        QString currentDescription = view->layerData(i)->metadata(medMetaDataKeys::SeriesDescription.key());
        descriptions.append(currentDescription);
    }

    return descriptions;
}

void PipelineExportToolBox::removeStepNumbersInSeriesDescription(medAbstractLayeredView* view)
{
    for (unsigned int i = 0 ; i < view->layersCount() ; i++)
    {
        QString currentDescription = view->layerData(i)->metadata(medMetaDataKeys::SeriesDescription.key());

        // format "XX[ -_.]name" becomes "name"
        currentDescription.remove(QRegExp(QString::fromUtf8("^[0-9]{1,}[ -_.]")));
        view->layerData(i)->setMetaData(medMetaDataKeys::SeriesDescription.key(), currentDescription);
    }
}

void PipelineExportToolBox::setSeriesDescriptions(medAbstractLayeredView* view, QList<QString> descriptions)
{
    for (unsigned int i = 0 ; i < view->layersCount() ; i++)
    {
        view->layerData(i)->setMetaData(medMetaDataKeys::SeriesDescription.key(), descriptions.at(i));
    }
}

} // namespace med::pipeline
