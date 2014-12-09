/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medToolBox.h>
#include <medCoreExport.h>
#include <medJobItem.h>

class medAbstractRegistrationProcess;
class medAbstractImageView;
class medAbstractImageData;
class medDataIndex;
class medRegistrationSelectorToolBoxPrivate;

class MEDCORE_EXPORT medRegistrationSelectorToolBox : public medToolBox
{
    Q_OBJECT

public:
     medRegistrationSelectorToolBox(QWidget *parent = 0);
    ~medRegistrationSelectorToolBox();

    medAbstractData *fixedData();
    medAbstractData *movingData();

    medAbstractRegistrationProcess * process();
    void setProcess(medAbstractRegistrationProcess *process);

    medAbstractRegistrationProcess * undoRedoProcess();
    void setUndoRedoProcess(medAbstractRegistrationProcess *proc);

    QString getNameOfCurrentAlgorithm();

    bool setFixedData(medAbstractData* data);
    bool setMovingData(medAbstractData* data);

    enum typeOfOperation { algorithm, undo, redo, reset };

signals:
    void showError (const QString&,unsigned int timeout);
    void showInfo(const QString&,unsigned int timeout);
    void movingDataRegistered(medAbstractData *output);

public slots:

    void changeCurrentToolBox(int index);
    void clear();
    void onSaveTrans();
    void handleOutput(typeOfOperation type=algorithm,QString algoName="");
    void enableSelectorToolBox(bool enable = true);
    void onJobAdded(medJobItem* item, QString jobName);

private:
    medRegistrationSelectorToolBoxPrivate *d;
};


