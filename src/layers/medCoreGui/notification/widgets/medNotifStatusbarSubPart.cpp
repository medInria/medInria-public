/*
 * medInria
 * Copyright (c) INRIA 2013. All rights reserved.
 * 
 * medInria is under BSD-2-Clause license. See LICENSE.txt for details in the root of the sources or:
 * https://github.com/medInria/medInria-public/blob/master/LICENSE.txt
 * 
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "medNotifStatusbarSubPart.h"


medNotifStatusbarSubpart::medNotifStatusbarSubpart(QWidget * parent) : QWidget(parent), m_layout(new QHBoxLayout(this)), m_progressBar(new QProgressBar(this)), m_titleWidget(new QLabel("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", this))
{
    setLayout(m_layout);
    m_layout->setMargin(0);
    m_layout->setSpacing(5);
    m_titleWidget->setMinimumSize(100, 22);
    m_progressBar->setMinimumSize(50, 22);
    m_layout->addWidget(m_titleWidget);
    m_layout->addWidget(m_progressBar);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    setFixedSize(150, 22);
    setContentsMargins(0, 0, 0, 0);
}

medNotifStatusbarSubpart::~medNotifStatusbarSubpart()
{

}

void medNotifStatusbarSubpart::mousePressEvent(QMouseEvent * event)
{
    if (m_titleWidget->underMouse() || m_progressBar->underMouse())
    {
        emit clicked();
    }
}
