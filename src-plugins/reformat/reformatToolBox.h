#pragma once

#include <medSegmentationAbstractToolBox.h>
#include <medAbstractWorkspace.h>
#include "reformatPluginExport.h"

class reformatToolBoxPrivate;

class REFORMATPLUGIN_EXPORT reformatToolBox : public medSegmentationAbstractToolBox
{
    Q_OBJECT;

    MED_TOOLBOX_INTERFACE("reformatToolBox","used to reformat an image", << "reformat");
        
public:
    reformatToolBox(QWidget *parentToolBox = 0);
    ~reformatToolBox();
    static bool registered();
    dtkPlugin* plugin();
    void setWorkspace(medAbstractWorkspace * workspace);
    medAbstractData *processOutput();
    void changeButtonValue(QString buttonName, double value);

private:
    reformatToolBoxPrivate *d;

public slots:
    void startReformat();
    void stopReformat();
    void updateView();
    void actOnContainerChange(const QString&);
    void displayInfoOnCurrentView();
    void saveReformatedImage();
    void generateReformatedImage();
    void switchSpacingAndDimension(const QString &value);
};
