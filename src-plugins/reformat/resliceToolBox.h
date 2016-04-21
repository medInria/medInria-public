#pragma once

#include <medSegmentationAbstractToolBox.h>
#include <medAbstractWorkspace.h>
#include "reformatPluginExport.h"

class resliceToolBoxPrivate;

class REFORMATPLUGIN_EXPORT resliceToolBox : public medSegmentationAbstractToolBox
{
    Q_OBJECT

    MED_TOOLBOX_INTERFACE("resliceToolBox","used to reslice an image", << "reformat")
        
public:
    resliceToolBox(QWidget *parentToolBox = 0);
    ~resliceToolBox();
    static bool registered();
    dtkPlugin* plugin();
    void setWorkspace(medAbstractWorkspace * workspace);
    medAbstractData *processOutput();
    void changeButtonValue(QString buttonName, double value);

private:
    resliceToolBoxPrivate *d;

public slots:
    void startReformat();
    void stopReformat();
    void updateView();
    void displayInfoOnCurrentView();
    void saveReformatedImage();
    void generateReformatedImage();
    void switchSpacingAndDimension(const QString &value);
};
