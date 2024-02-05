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

class medCropToolBoxPrivate;

class REFORMATPLUGIN_EXPORT medCropToolBox : public medAbstractSelectableToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Cropping",
                          "Used to crop one or more datasets",
                          <<"Reformat")

public:
    static bool registered();

    const QString applyButtonName;
    const QString saveButtonName;

    medCropToolBox(QWidget *parent = nullptr);
    virtual ~medCropToolBox();

    virtual dtkPlugin* plugin();
    virtual medAbstractData* processOutput();

    void handleInteractionEvent();
    void handleCameraMoveEvent();

public slots:
    void updateView();
    void applyCrop();
    void saveCrop();
    void reactToOrientationChange();
    void setEnable(bool enable);

protected:
    void showEvent(QShowEvent *event);
    virtual void clear();
    int generateOutput();
    void enableButtons(bool wantToEnable);

private:
    medCropToolBoxPrivate* const d;
};
