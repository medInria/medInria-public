#pragma once
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

#include <QWidget>

#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QMenu>
#include <QWidgetAction>

#include <medNotif.h>
#include <medWidgetsExport.h>

class medNotifStatusbarSubpart : public QWidget
{
    Q_OBJECT
public:
    medNotifStatusbarSubpart(QWidget * parent);
    ~medNotifStatusbarSubpart();

    void mousePressEvent(QMouseEvent *event) override;

signals:
    void clicked();

public:
    QLabel *m_titleWidget;
    QProgressBar *m_progressBar;
    QHBoxLayout *m_layout;
};

