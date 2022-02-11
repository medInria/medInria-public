#pragma once

#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medAbstractData.h>
#include <medAbstractLayeredView.h>
#include <medAbstractWorkspaceLegacy.h>

#include "medPipelineConstants.h"
#include "medPipelineExport.h"
#include "medPipelineTypes.h"

namespace med::pipeline
{

class PipelineChooserToolBox;
class PipelineControlToolBox;
class PipelineInstance;
class WorkspacePrivate;
    
class MEDPIPELINE_EXPORT PipelineWorkspace : public medAbstractWorkspaceLegacy
{
    Q_OBJECT
    MED_WORKSPACE_INTERFACE("Pipelines",
                            "Workspace to use pipelines",
                            "Clinical")

signals:
    void importFinished();
    void exceptionAnnounced(QString title, QString message);

public:
    static bool isUsable();
    static bool registered();

    static void displayError(QString title, QString message);

    static PipelineWorkspace* instance();

    PipelineWorkspace(QWidget* parent = 0);
    virtual ~PipelineWorkspace();

    void setupTabbedViewContainer() override;
    void clear();

    void runPipeline(dtkSmartPointer<PipelineInstance> pipeline);
    void stopPipeline();

    void setToolBoxVisibility(QString identifier, bool visible);
    void setToolBoxVisibility(medToolBox* toolBox, bool visible);

    medToolBox* addToolBox(QString identifier);
    using medAbstractWorkspaceLegacy::addToolBox;

    template <class T> T* addToolBox()
    {
        return static_cast<T*>(addToolBox(T::staticIdentifier()));
    }

    medToolBox* insertToolBox(int index, QString identifier);
    using medAbstractWorkspaceLegacy::insertToolBox;

    template <class T> T* insertToolBox(int index)
    {
        return static_cast<T*>(insertToolBox(index, T::staticIdentifier()));
    }

    void removeToolBox(QString identifier);
    using medAbstractWorkspaceLegacy::removeToolBox;

    medToolBox* getToolBox(QString identifier);
    template <class T> T* getToolBox()
    {
        return static_cast<T*>(getToolBox(T::staticIdentifier()));
    }

    dtkSmartPointer<medAbstractData> getToolBoxOutput(QString identifier);
    dtkSmartPointer<medAbstractData> getToolBoxOutput(medToolBox* toolBox);

    bool hasDroppedResults();
    dtkSmartPointer<medAbstractData> getDroppedResult();
    dtkSmartPointer<medAbstractData> getDroppedResultOrToolBoxOutput(QString identifier);
    QList<dtkSmartPointer<medAbstractData> > getDroppedResults();

    medAbstractLayeredView* getView(int containerIndex = 0, int tabIndex = 0);
    medViewContainer* getContainer(int containerIndex = 0, int tabIndex = 0);
    dtkSmartPointer<medAbstractData> getLayerData(int layerIndex = 0, int containerIndex = 0, int tabIndex = 0);

    medViewContainer* addData(dtkSmartPointer<medAbstractData> data, int containerIndex = 0, int tabIndex = 0);

    void setWindowLevel(double window, double level, int layerIndex = 0, int containerIndex = 0, int tabIndex = 0);
    void setRenderingParameters(med::pipeline::RenderingParameters renderingParameters, int layerIndex, int containerIndex, int tabIndex);

    void secureImport(dtkSmartPointer<medAbstractData> data);

    void setVolumeRendering(int containerIndex = 0, int tabIndex = 0);
    void setCoronalRendering(int containerIndex = 0, int tabIndex = 0);
    void setSagittalRendering(int containerIndex = 0, int tabIndex = 0);

    void displayException(QString title, std::exception& e);

    /**
     * @brief setColorRange is used to change the min and max intensities of the Layer Settings toolbox
     * @param double value of the minimum intensity
     * @param double value of the maximum intensity
     * @param int container index
     * @param int tab index
     */
    void setColorRange(double minimumValue = 0.0, double maximumValue = 1.0, int containerIndex = 0, int tabIndex = 0);
    void setLookUpTable(QString lookupTable = "Default", int containerIndex = 0, int tabIndex = 0);

public slots:
    virtual void open(const medDataIndex& index);

private slots:
    void importCurrentData(const medDataIndex& index);
    void updateImportedDataIndex(const medDataIndex& index, QUuid importId);
    void displayExceptionBox(QString title, QString message);

private:
    WorkspacePrivate *d;
};

} // namespace med::pipeline
