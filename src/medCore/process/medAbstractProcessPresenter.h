/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QObject>

class medAbstractProcess;
class medAsbtractProcessPresenterPrivate;

class medViewContainerSplitter;

class medAsbtractProcessPresenter : public QObject
{

public:
    medAsbtractProcessPresenter(QObject *parent = 0);
    ~medAsbtractProcessPresenter();

    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual QString version() const = 0;

    virtual medAbstractProcess* process() const = 0;
    virtual QWidget* toolbox() const = 0;
    virtual medViewContainerSplitter* viewContainerSplitter() const = 0;

    void addTags(QStringList tags);
    QStringList tags() const;

private:
    medAsbtractProcessPresenterPrivate *d;
};
