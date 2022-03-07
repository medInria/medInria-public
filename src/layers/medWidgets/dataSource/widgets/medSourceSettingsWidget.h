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

#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QTreeWidget>

#include <medAbstractSource.h>
#include <medSourcesLoader.h>
#include <medWidgetsExport.h>

class MEDWIDGETS_EXPORT medSourceSettingsWidget : public QFrame
{
    Q_OBJECT
    
public:

    medSourceSettingsWidget(medSourcesLoader * pSourceLoader, 
                            medAbstractSource *pSource, 
                            QTreeWidget * sourceInformation, 
                            QWidget *parent = nullptr);

    QString getInstanceName();
    medAbstractSource * getInstanceSource();

    void switchConnectionIcon(bool connection);
    void switchMinimization();
    void setToDefault(bool askedDefault);
    void deleteThisSource(medSourcesLoader * pSourceLoader, medAbstractSource * pSource);
    void updateSourceInformation();
    void setSelectedVisualisation(bool selected);
    void saveInitialSize(QSize initialSize);
    QSize getInitialSize();

protected:
    void paintEvent(QPaintEvent *event);
    void mouseReleaseEvent(QMouseEvent *event) override;

signals:
    void sourceItemChosen(medAbstractSource *); // Signal to indicate this widget is clicked
    void deletedWidget(QString);                // Signal to indicate the widget needs to be destroy
    void minimizationAsked(bool);               // Signal to indicate a [un]minization is done

private:
    medAbstractSource * _pSource;
    medSourcesLoader * _pSourceLoader;
    QTreeWidget * _sourceInformation;

    bool sourceSelected;
    QLabel * titleLabel;
    QLabel * defaultLabel;
    QImage * onOffIcon;
    QLabel * imageLabel;
    QPushButton * connectButton;
    QPushButton* removeSourceButton;
    QPushButton* minimizeSourceButton;
    QWidget* parametersWidget;
    QSize sourceWidgetSize;
};
