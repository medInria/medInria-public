/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkDWIBasicThresholdingProcessPresenter.h>

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

#include <medViewContainerSplitter.h>
#include <medViewContainer.h>
#include <medDataManager.h>
#include <medAbstractImageData.h>

#include <medIntParameterPresenter.h>
#include <QLabel>

medItkDWIBasicThresholdingProcessPresenter::medItkDWIBasicThresholdingProcessPresenter(medItkDWIBasicThresholdingProcess *parent)
    : medAbstractDWIMaskingProcessPresenter(parent)
{
    m_process = parent;

    m_b0VolPresenter = new medIntParameterPresenter(m_process->b0VolumeNumberParameter());
    m_thresholdPresenter = new medIntParameterPresenter(m_process->thresholdValueParameter());
}

QWidget *medItkDWIBasicThresholdingProcessPresenter::buildToolBoxWidget()
{
    QWidget *tbWidget = new QWidget;
    QVBoxLayout *tbLayout = new QVBoxLayout;
    tbWidget->setLayout(tbLayout);

    QLabel *b0VolLabel = new QLabel(m_b0VolPresenter->parameter()->caption(), tbWidget);
    QHBoxLayout *b0VolLayout = new QHBoxLayout;
    b0VolLayout->addWidget(b0VolLabel);
    b0VolLayout->addWidget(m_b0VolPresenter->buildWidget());
    tbLayout->addLayout(b0VolLayout);

    QLabel *thresholdLabel = new QLabel(m_thresholdPresenter->parameter()->caption(), tbWidget);
    QHBoxLayout *thrLayout = new QHBoxLayout;
    thrLayout->addWidget(thresholdLabel);
    thrLayout->addWidget(m_thresholdPresenter->buildWidget());
    tbLayout->addLayout(thrLayout);

    if (this->useRunControls())
    {
        tbLayout->addWidget(this->buildRunButton());
        tbLayout->addWidget(this->buildCancelButton());
    }

    return tbWidget;
}
