#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <iterativeClosestPointPluginExport.h>

#include <medAbstractSelectableToolBox.h>

class iterativeClosestPointToolBoxPrivate;

/*!
 * \brief Toolbox to match two meshes using the ICP (Iterative Closest Point) algorithm.
 */
class ITERATIVECLOSESTPOINTPLUGIN_EXPORT iterativeClosestPointToolBox : public medAbstractSelectableToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Iterative Closest Point",
                          "Used to match two meshes using the ICP algorithm.",
                          <<"Meshing")
    
public:
    iterativeClosestPointToolBox(QWidget *parent = nullptr);
    ~iterativeClosestPointToolBox();

    static bool registered();
    medAbstractData* processOutput();
    dtkPlugin* plugin();
    void addLayer(unsigned int layer);
    
public slots:
    void run();
    virtual void updateView();
    void resetComboBoxes();
    void displayFRE();
    
protected slots:
    void displayOutput();
    void onExportTransferMatrixCheckBoxToggled(bool toggle);
    void onbCheckMeanDistanceCheckBoxToggled(bool toggle);
    void editTransferMatrixPath(QString newPath);
    void hideFRE();
    void showFRE(double &mean, double &stdDev, double &median);

private:
    iterativeClosestPointToolBoxPrivate *d;
};
