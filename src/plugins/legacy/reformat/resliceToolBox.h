#pragma once
/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2020. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include <medAbstractSelectableToolBox.h>
#include <reformatPluginExport.h>

class resliceToolBoxPrivate;

/*! \brief Toolbox to reslice a volume.
 *
 * This toolbox has several named widgets which can be accessed in python pipelines:\n\n
 * "startReformatButton" : QPushButton\n
 * "stopReformatButton" : QPushButton\n
 * "saveImageButton" : QPushButton\n
 * "bySpacingOrDimension" : QComboBox\n
 * "SpacingOrSizeX" : medDoubleParameterL\n
 * "SpacingOrSizeY" : medDoubleParameterL\n
 * "SpacingOrSizeZ" : medDoubleParameterL\n
 * "helpBegin" : QLabel
 */
class REFORMATPLUGIN_EXPORT resliceToolBox : public medAbstractSelectableToolBox
{
    Q_OBJECT

    MED_TOOLBOX_INTERFACE("Reslice",
                          "Used to reslice an image",
                          << "Reformat")

public:
    resliceToolBox(QWidget *parentToolBox = nullptr);
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
