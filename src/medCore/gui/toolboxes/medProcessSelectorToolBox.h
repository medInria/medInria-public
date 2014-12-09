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
#include <medAbstractJob.h>

class medProcessSelectorToolBoxPrivate;

class MEDCORE_EXPORT medProcessSelectorToolBox : public medToolBox
{
    Q_OBJECT

public:
     medProcessSelectorToolBox(QWidget *parent = 0);
    ~medProcessSelectorToolBox();

    void setAvailableProcesses(QStringList);
    void setProcessToolbox(medToolBox*);

signals:
    void processSelected(const QString&);

public slots:
    void clear();
    void enableSelectorToolBox(bool enable = true);

private:
    medProcessSelectorToolBoxPrivate *d;
};


