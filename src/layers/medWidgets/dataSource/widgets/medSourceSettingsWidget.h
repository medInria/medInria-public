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

#include <QFrame>
#include <QImage>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QTreeWidget>
#include <QWidget>

#include <medAbstractSource.h>
#include <medSourcesLoader.h>
#include <medWidgetsExport.h>

class MEDWIDGETS_EXPORT medSourceSettingsWidget : public QFrame
{
    Q_OBJECT
public:

    enum type {ON, OFF};

    medSourceSettingsWidget(medSourcesLoader * pSourceLoader, 
                            medAbstractSource *pSource, 
                            QTreeWidget * sourceInformation, 
                            QWidget *parent = nullptr);

    QString getInstanceName();

    void setIconToConnection();
    void switchConnection();
    void switchDefault();
    void setToDefault(bool askedDefault);
    void deleteThisSource(medSourcesLoader * pSourceLoader, medAbstractSource * pSource);

public slots:
    void setIcon(type askedType);

protected:
    void paintEvent(QPaintEvent *event);
    void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void defaultChosen(QString); // Signal to indicate this widget is the default one
    void deletedWidget();

private:
    medAbstractSource * _pSource;
    medSourcesLoader * _pSourceLoader;
    QTreeWidget * _sourceInformation;

    QLabel * titleLabel;
    QLabel * defaultLabel;
    bool isDefault;
    QImage * onOffIcon;
    QLabel * imageLabel;
    type currentIcon;
    QPushButton * connectButton;
    QPushButton* removeSourceButton;
};
