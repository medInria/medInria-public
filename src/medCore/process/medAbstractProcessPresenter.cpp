/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractProcessPresenter.h>

#include <QStringList>
#include <QThreadPool>
#include <QPushButton>

#include <medAbstractProcess.h>

class medAsbtractProcessPresenterPrivate
{
public:
    QStringList tags;
};

medAsbtractProcessPresenter::medAsbtractProcessPresenter(QObject *parent): QObject(parent),
    d(new medAsbtractProcessPresenterPrivate)
{

}

medAsbtractProcessPresenter::~medAsbtractProcessPresenter()
{
    delete d;
}

void medAsbtractProcessPresenter::addTags(QStringList tags)
{
    d->tags << tags;
}

QStringList medAsbtractProcessPresenter::tags() const
{
    return d->tags;
}

QPushButton* medAsbtractProcessPresenter::runButton() const
{
    QPushButton *runButton = new QPushButton(tr("Run"));
    connect(runButton, &QPushButton::clicked,
            this, &medAsbtractProcessPresenter::runProcessInThread);

    return runButton;
}

void medAsbtractProcessPresenter::runProcessInThread()
{
    QThreadPool::globalInstance()->start(this->process());
}

