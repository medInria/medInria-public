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

class medAbstractSelectableToolBox;
class medSelectorToolBoxPrivate;

class MEDCORE_EXPORT medSelectorToolBox : public medToolBox
{
    Q_OBJECT

public:
    medSelectorToolBox(QWidget *parent, QString tlbxId);
    ~medSelectorToolBox();

    medAbstractData* data();

    medAbstractSelectableToolBox* currentToolBox();

signals:
    void inputChanged();
    void currentToolBoxChanged();

public slots:
    virtual void changeCurrentToolBox(int index);
    virtual void changeCurrentToolBox(const QString &identifier);
    void clear();
    void onInputSelected(medAbstractData *data);

private:
    medSelectorToolBoxPrivate *d;
};


