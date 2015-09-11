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

#include <medCoreExport.h>

class medAbstractParameter;
class medViewContainerSplitter;

struct MEDCORE_EXPORT medProcessDetails
{
    QString name;
    QString version;
    QStringList dependences;
};

class medAbstractProcessPrivate;
class MEDCORE_EXPORT medAbstractProcess: public QObject, public QRunnable
{
    Q_OBJECT
public:
    medAbstractProcess(QObject *parent = NULL);
    virtual ~medAbstractProcess();

    void addParameter(medAbstractParameter *parameter);
    medAbstractParameter* parameter(QString name) const;
    QList<medAbstractParameter*> parameters() const;

    virtual medProcessDetails details() const = 0;

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
    void running(bool isRunning);
    void success();
    void failure();
    void progressed(float progression);

private:
    void _emitNotRunning();

private:
    medAbstractProcessPrivate* d;
};
