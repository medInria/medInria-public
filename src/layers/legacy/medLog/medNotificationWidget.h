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
#include <medLogExport.h>

#include <QtWidgets>

namespace med
{

class MEDLOG_EXPORT NotificationWidget: public QWidget
{
    Q_OBJECT

public :

    enum type {MESSAGE, ERROR, SUCCESS};

    NotificationWidget(QWidget *parent);

    /**
     * @brief Set the notification text message
     * 
     * @param text
     */
    void setText(QString text);

    /**
     * @brief Set the icon of the type of notification
     * 
     * @param askedType
     */
    void setIcon(type askedType);

    /**
     * @brief efine the graphic behavior when the widget needs to be updated
     * 
     * @param event
     */
    void paintEvent(QPaintEvent *event);

private :
    /**
     * @brief Set the Timer object, after X ms the notification automatically closes itself
     * 
     * @param timeInMS 
     */
    void setTimer(int timeInMS);

    QWidget *widgetParent;
    QImage *notificationIcon;
    QLabel *imageLabel;
    QLabel *notificationText;
    QPushButton *notificationCloseButton;
};

} // end namespace
