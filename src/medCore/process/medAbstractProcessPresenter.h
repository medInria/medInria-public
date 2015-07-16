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

class QPushButton;
class medAbstractProcess;
class medViewContainerSplitter;

class medAsbtractProcessPresenterPrivate;

class medAsbtractProcessPresenter : public QObject
{
    Q_OBJECT

public:
    medAsbtractProcessPresenter(QObject *parent = NULL);
    ~medAsbtractProcessPresenter();

    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual QString version() const = 0;

    virtual medAbstractProcess* process() const = 0;
    virtual QWidget* toolbox() const = 0;
    virtual medViewContainerSplitter* viewContainerSplitter() const = 0;

    QPushButton* runButton() const;

    void addTags(QStringList tags);
    QStringList tags() const;

protected slots:
    void runProcessInThread();

private:
    medAsbtractProcessPresenterPrivate *d;
};
