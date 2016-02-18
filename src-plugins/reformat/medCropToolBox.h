#pragma once

#include <medSegmentationAbstractToolBox.h>
#include <medAbstractWorkspace.h>
#include <reformatPluginExport.h>

class medCropToolBoxPrivate;

class REFORMATPLUGIN_EXPORT medCropToolBox : public medSegmentationAbstractToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("medCropToolBox", "Image cropping",<<"reformat")

public:
    static bool registered();

    const QString applyButtonName;
    const QString saveButtonName;

    medCropToolBox(QWidget *parent = 0);
    virtual ~medCropToolBox();

    virtual dtkPlugin* plugin();
    virtual void setWorkspace(medAbstractWorkspace* workspace);
    virtual medAbstractData* processOutput();

    void handleInteractionEvent();
    void handleCameraMoveEvent();

public slots:
    void updateView();
    void applyCrop();
    void saveCrop();
    void reactToOrientationChange();
    void behaveWithBodyVisibility();
    void setEnable(bool enable);

private:
    medCropToolBoxPrivate* const d;
};
