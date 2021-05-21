/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTimeLineParameterPresenter.h>
#include <medTimeLineParameter.h>

#include <medBoolParameterPresenter.h>
#include <medIntParameterPresenter.h>
#include <medDoubleParameterPresenter.h>
#include <medTriggerParameterPresenter.h>
#include <medStringListParameterPresenter.h>

#include <QWidget>
#include <QLayout>

#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QComboBox>

class medTimeLineParameterPresenterPrivate
{
public:
    medTimeLineParameter* parameter;

};

medTimeLineParameterPresenter::medTimeLineParameterPresenter(medTimeLineParameter *parent)
{
    d->parameter = parent;
}

medTimeLineParameterPresenter::~medTimeLineParameterPresenter()
{
}

QWidget * medTimeLineParameterPresenter::buildWidget()
{
    QWidget *pWidgetRes = new QWidget();

    QVBoxLayout *widgetLayout = new QVBoxLayout(pWidgetRes);
    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    QHBoxLayout *indicatorLayout = new QHBoxLayout;

    medBoolParameterPresenter       playPresenter(d->parameter->playParameter());
    medBoolParameterPresenter       loopPresenter(d->parameter->loopParameter());
    medTriggerParameterPresenter    nextPresenter(d->parameter->nextFrameParameter());
    medTriggerParameterPresenter    prevPresenter(d->parameter->previousFrameParameter());
    medDoubleParameterPresenter     timePresenter(d->parameter->currentTimeParameter());
    medIntParameterPresenter        speedPresenter(d->parameter->speedFactorParameter());
    medBoolParameterPresenter       displayTimePresenter(d->parameter->displayTimeParameter());
    medDoubleParameterPresenter     timeShiftPresenter(d->parameter->timeShiftParameter());
    medStringListParameterPresenter extensionShiftPresenter(d->parameter->extensionShiftParameter());

    QPushButton    *playButton   = playPresenter.buildPushButton();
    QPushButton    *prevButton   = prevPresenter.buildPushButton();
    QPushButton    *nextButton   = nextPresenter.buildPushButton();
    QWidget        *speedLabel   = speedPresenter.buildLabel();
    QSlider        *timeSlider   = timePresenter.buildSlider();
    QSpinBox       *speedSpinBox = speedPresenter.buildSpinBox();
    QCheckBox      *loopCheckBox = loopPresenter.buildCheckBox();
    QCheckBox      *dispCheckBox = displayTimePresenter.buildCheckBox();
    QWidget        *shiftLabel   = timeShiftPresenter.buildLabel();
    QDoubleSpinBox *shiftSpinBox = timeShiftPresenter.buildSpinBox();
    QComboBox      *timeExt      = extensionShiftPresenter.buildComboBox();


    playButton->setIcon(QIcon(":/icons/play.png"));
    prevButton->setIcon(QIcon(":/icons/backward.png"));
    nextButton->setIcon(QIcon(":/icons/forward.png"));
    speedSpinBox->setSingleStep(10);


    auto handlePlayButtonIcon = [=](bool isPlaying)
    {
        if (isPlaying)
        {
            playButton->setIcon(QPixmap(":/icons/pause.png"));
        }
        else
        {
            playButton->setIcon(QPixmap(":/icons/play.png"));
        }
    };
    connect(d->parameter, &medTimeLineParameter::playing, handlePlayButtonIcon);
    connect(playButton, SIGNAL(clicked()), d->parameter, SLOT(unlockTimeLine()));

    timeSlider->setOrientation(Qt::Horizontal);

    buttonsLayout->addWidget(playButton  );
    buttonsLayout->addWidget(prevButton  );
    buttonsLayout->addWidget(nextButton  );
    buttonsLayout->addWidget(speedLabel  );
    buttonsLayout->addWidget(speedSpinBox);
    buttonsLayout->addWidget(loopCheckBox);

    QLabel *frameLabel = new QLabel(QString::number(d->parameter->frame() + 1));
    connect(d->parameter, &medTimeLineParameter::timeChanged, [=]() {frameLabel->setText(QString::number(d->parameter->frame() + 1) ); });

    QLabel *numberOfFramesLabel = new QLabel("/ " + QString::number(d->parameter->numberOfFrame()) + " frames");
    connect(d->parameter, &medTimeLineParameter::timeChanged, [=]() {frameLabel->setText("/ " + QString::number(d->parameter->numberOfFrame()) + " frames"); });

    indicatorLayout->addWidget(timePresenter.buildLabel(), 0, Qt::AlignLeft);
    indicatorLayout->addWidget(new QLabel("/ " + QString::number(d->parameter->duration(), 'g', 2) + " s"), 0, Qt::AlignLeft);
    indicatorLayout->addStretch(1);
    indicatorLayout->addWidget(frameLabel, 0, Qt::AlignRight);
    indicatorLayout->addWidget(numberOfFramesLabel, 0, Qt::AlignRight);

    // Time Shift
    QHBoxLayout *shiftLayout = new QHBoxLayout;
    shiftLayout->setAlignment(Qt::AlignLeft);
    shiftLayout->addWidget(dispCheckBox);
    shiftLayout->addWidget(shiftLabel);
    shiftLayout->addWidget(shiftSpinBox);
    shiftLayout->addWidget(timeExt);

    widgetLayout->addLayout(buttonsLayout);
    widgetLayout->addWidget(timePresenter.buildSlider());
    widgetLayout->addLayout(indicatorLayout);
    widgetLayout->addLayout(shiftLayout);

    return pWidgetRes;
}

medAbstractParameter * medTimeLineParameterPresenter::parameter() const
{
    return d->parameter;
}
