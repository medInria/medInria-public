/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreLegacyExport.h>
#include <medJobItemL.h>
#include <medSelectorToolBox.h>

class medAbstractRegistrationProcess;
class medAbstractImageView;
class medAbstractImageData;
class medDataIndex;
class medRegistrationSelectorToolBoxPrivate;

class MEDCORELEGACY_EXPORT medRegistrationSelectorToolBox : public medSelectorToolBox
{
    Q_OBJECT

public:
     medRegistrationSelectorToolBox(QWidget *parent, QString name);
    ~medRegistrationSelectorToolBox();

    medAbstractData *fixedData();
    medAbstractData *movingData();

    medAbstractRegistrationProcess * process();
    void setProcess(medAbstractRegistrationProcess *process);

    medAbstractRegistrationProcess * undoRedoProcess();
    void setUndoRedoProcess(medAbstractRegistrationProcess *proc);

    QString getNameOfCurrentAlgorithm();

    void setFixedData(medAbstractData *data);
    void setMovingData(medAbstractData *data);

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

private:
    void setUndoRedoProcessInputs();
    medRegistrationSelectorToolBoxPrivate *d;
};


