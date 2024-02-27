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
#include <QLineEdit>
#include <QStackedWidget>
#include <QPushButton>
#include <QListWidgetItem>

#include <medAbstractSource.h>
#include <medSourcesLoader.h>
#include <medCoreGuiExport.h>

class QHBoxLayout;
class MEDCOREGUI_EXPORT medSourceSettingsWidget : public QFrame
{
    Q_OBJECT
    
public:

    medSourceSettingsWidget(medAbstractSource *pSource, QListWidgetItem *widgetItem, QWidget *parent = nullptr);
    ~medSourceSettingsWidget() = default;

    QString getInstanceName();

    void switchConnectionIcon(bool connection);
    void switchMinimization();
    void setToDefault(bool askedDefault);
    void setSelectedVisualisation(bool selected);
    void saveInitialSize(QSize initialSize);
    QSize getInitialSize();


    /**
     * @brief Get the abstract source associated with this widget
     *
     * @return medAbstractSource*
     */
    inline medAbstractSource * getSource() { return m_pSource; }

    bool eventFilter(QObject *watched, QEvent *event) override;

public slots:
    void currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void titleChanged();

protected:
    void paintEvent(QPaintEvent *event);

signals:
    void minimizationAsked(bool);               // Signal to indicate a [un]minimization is done
    void sourceRename(QString, QString);

private:
    int                 m_maxHeight;
    medAbstractSource * m_pSource;
    QListWidgetItem   * m_pItem;
    bool                m_sourceSelected;

    QHBoxLayout    * m_titleLayout;
    QLabel         * m_titleLabel;
    QLineEdit      * m_titleLineEdit;
    QStackedWidget   m_titleStack;

    QLabel      * m_defaultLabel;
    QImage      * m_onOffIcon;
    QLabel      * m_imageLabel;
    QPushButton * m_connectButton;
    QPushButton * m_removeSourceButton;
    QPushButton * m_minimizeSourceButton;
    QWidget     * m_parametersWidget;
    QSize         m_sourceWidgetSize;
};
