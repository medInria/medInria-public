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
#include <QRunnable>
#include <QPushButton>

class medAbstractParameter;
class medViewContainerSplitter;

class medAbstractProcessPrivate;

class medAbstractProcess: public QObject, public QRunnable
{
    Q_OBJECT
public:
    medAbstractProcess(QObject *parent = NULL);
    virtual ~medAbstractProcess();

    void addParameter(medAbstractParameter *parameter);
    medAbstractParameter* parameter(QString name) const;
    QList<medAbstractParameter*> parameters() const;

    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual QString version() const = 0;

    virtual QWidget* toolbox() const = 0;
    virtual medViewContainerSplitter* viewContainerSplitter() const = 0;

    QPushButton* runButton() const;

    void addTags(QStringList tags);
    QStringList tags() const;

protected slots:
    void runInThread();

public:
    virtual void cancel() = 0;

signals:
    void success();
    void failure();
    void progressed(float progression);

private:
    medAbstractProcessPrivate* d;
};
