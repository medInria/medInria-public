/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <manualRegistrationPluginExport.h>

#include <medAbstractSelectableToolBox.h>
#include <medTabbedViewContainers.h>

class manualRegistrationToolBoxPrivate;

/*! \brief Toolbox to register two images manually.
 *
 * This toolbox has several named widgets which can be accessed in python pipelines:\n\n
 * "startManualRegistrationButton" : QPushButton\n
 * "transformType" : medComboBox\n
 * "computeRegistrationButton" : QPushButton\n
 * "resetButton" : QPushButton
 */
class MANUALREGISTRATIONPLUGIN_EXPORT manualRegistrationToolBox : public medAbstractSelectableToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Manual Registration",
                          "Register two images manually.",
                          <<"Registration")

public:
    manualRegistrationToolBox(QWidget *parent = nullptr);
    ~manualRegistrationToolBox();
    
    medAbstractData* processOutput();
    
    static bool registered();
    dtkPlugin * plugin() override;

    void updateGUI(int left,int right);

protected slots:
    void updateView();    
    void synchroniseMovingFuseView();

public slots:
    void startManualRegistration();
    void stopManualRegistration();
    void computeRegistration();
    void reset();
    void retrieveProcessOutputAndUpdateViews();

protected:
    virtual void clear();

private:
    void displayButtons(bool);
    void constructContainers(medTabbedViewContainers *);
    void setDisableComputeResetButtons(bool);
    void setDisableResetButton(bool);

    manualRegistrationToolBoxPrivate *d;
};


