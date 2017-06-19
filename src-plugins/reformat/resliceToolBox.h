#pragma once

#include <medAbstractSelectableToolBox.h>
#include <medAbstractWorkspace.h>
#include "reformatPluginExport.h"

class resliceToolBoxPrivate;

class REFORMATPLUGIN_EXPORT resliceToolBox : public medAbstractSelectableToolBox
{
    Q_OBJECT

    MED_TOOLBOX_INTERFACE("Reslice",
                          "Used to reslice an image", << "Reformat")
        
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

protected:
    virtual void clear();
};
