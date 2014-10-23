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

    void setAvailableProcesses(QStringList);
    void setProcessToolbox(medToolBox*);
    void setUndoRedoToolbox(medToolBox*);

signals:
    void processSelected(const QString&);
    void startProcessRequested();
    void saveTransfoRequested();

public slots:
    void clear();
    void enableSelectorToolBox(bool enable = true);

private:
    medRegistrationSelectorToolBoxPrivate *d;
};


