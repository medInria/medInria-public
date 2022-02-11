%ignore med::pipeline::PipelineWorkspace::importFinished;
%ignore med::pipeline::PipelineWorkspace::exceptionAnnounced;
%ignore medAbstractWorkspace::layerSelectionChanged;

%{
#include "medPipelineInstance.h"
#include "medPipelineWorkspace.h"
#include "medTabbedViewContainers.h"
%}

%include "medPipelineWorkspace.h"

%forwardFunction(setSelectionToolBoxVisibility, PipelineWorkspace.instance())
%forwardFunction(setMouseInteractionToolBoxVisibility, PipelineWorkspace.instance())
%forwardFunction(setViewSettingsToolBoxVisibility, PipelineWorkspace.instance())
%forwardFunction(setLayerSettingsToolBoxVisibility, PipelineWorkspace.instance())
%forwardFunction(setDynamicToolBoxVisibilities, PipelineWorkspace.instance())
%forwardFunction(removeToolBox, PipelineWorkspace.instance())
%forwardFunction(setVolumeRendering, PipelineWorkspace.instance())
%forwardFunction(setRenderingParameters, PipelineWorkspace.instance())
%forwardFunction(setColorRange, PipelineWorkspace.instance())
%forwardFunction(setLookUpTable, PipelineWorkspace.instance())

// Forward workspace methods as global functions
%inline
%{
    med::pipeline::PipelineWorkspace* getWorkspace()
    {
        return med::pipeline::PipelineWorkspace::instance();
    }
    
    medViewContainer* addDataToView(dtkSmartPointer<medAbstractData> data, int containerIndex = 0, int tabIndex = 0)
    {
        return med::pipeline::PipelineWorkspace::instance()->addData(data, containerIndex, tabIndex);
    }

    medToolBox* addToolBox(QString identifier)
    {
        return med::pipeline::PipelineWorkspace::instance()->addToolBox(identifier);
    }

    medToolBox* getToolBox(QString identifier)
    {
        return med::pipeline::PipelineWorkspace::instance()->getToolBox(identifier);
    }

    dtkSmartPointer<medAbstractData> getToolBoxOutput(QString identifier)
    {
        return med::pipeline::PipelineWorkspace::instance()->getToolBoxOutput(identifier);
    }

    dtkSmartPointer<medAbstractData> getToolBoxOutput(medToolBox* toolBox)
    {
        return med::pipeline::PipelineWorkspace::instance()->getToolBoxOutput(toolBox);
    }

    void closeTab(int index)
    {
        med::pipeline::PipelineWorkspace::instance()->tabbedViewContainers()->closeTab(index);
    }

%}
