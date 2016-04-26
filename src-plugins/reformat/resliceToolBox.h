#pragma once

#include <medAbstractWorkspace.h>
#include <medReformatAbstractToolBox.h>
#include "reformatPluginExport.h"

class resliceToolBoxPrivate;

class REFORMATPLUGIN_EXPORT resliceToolBox : public medReformatAbstractToolBox
{
    Q_OBJECT

    MED_TOOLBOX_INTERFACE("Reslice","used to reslice an image", << "reformat")
        
public:
    resliceToolBox(QWidget *parentToolBox = 0);
    ~resliceToolBox();
    static bool registered();
    dtkPlugin* plugin();
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
