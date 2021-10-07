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

#include <medNotificationWidget.h>

namespace med
{

NotificationWidget::NotificationWidget(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint)
{
    widgetParent = parent;

    // Attributes
    // Qt::FramelessWindowHint above removes title and borders of the widget
    setAttribute(Qt::WA_TranslucentBackground); // Transparent background for round corners    
    setFixedWidth(350);
    move((parent->width()/2.0 - width()/2.0), 0); // Position the notification on middle top

    QHBoxLayout *layout = new QHBoxLayout();
    setLayout(layout);

    // Icons
    notificationIcon = new QImage;
    imageLabel = new QLabel(""); // QImage is not a widget, we need to add it in a QLabel
    setIcon(MESSAGE);    
    layout->addWidget(imageLabel, 0, Qt::AlignLeft);

    // Notification text area
    notificationText = new QLabel("");
    notificationText->setWordWrap(true);
    notificationText->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    notificationText->setContentsMargins(15,0,0,0);
    layout->addWidget(notificationText, 0, Qt::AlignLeft);

    // Close button 
    notificationCloseButton = new QPushButton;
    notificationCloseButton->setIcon(QIcon(":/icons/cross_gray"));
    notificationCloseButton->setIconSize(QSize(15,15));
    notificationCloseButton->setToolTip("Close");
    notificationCloseButton->setContentsMargins(0,0,0,0);
    layout->addWidget(notificationCloseButton, 0, Qt::AlignRight);
    connect(notificationCloseButton, &QPushButton::clicked, this, &QWidget::close);

    // Set auto-close timer
    setTimer(3000);
}

void NotificationWidget::setText(QString text)
{
    notificationText->setText(text);
}

void NotificationWidget::setIcon(type askedType)
{
    QIcon newIcon;
    switch (askedType)
    {
        case ERROR:
        {
            newIcon =  QIcon(":/icons/notification_error.svg");
            break;
        }
        case SUCCESS:
        {
            newIcon = QIcon(":/icons/notification_success.svg");
            break;
        }
        case MESSAGE:
        default:
        {
            newIcon = QIcon(":/icons/notification_message.svg");
            break;
        }
    }
    QImage newImage = newIcon.pixmap(QSize(20,20)).toImage();
    notificationIcon->swap(newImage);
    imageLabel->setPixmap(QPixmap::fromImage(*notificationIcon));
}

void NotificationWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // smooth borders
    painter.setBrush(QBrush("#D9D9D9"));           // visible color of background
    painter.setPen(Qt::transparent);               // thin border color

    notificationText->setStyleSheet("color:#616161");                   // text color
    notificationCloseButton->setStyleSheet("background-color:#D9D9D9"); // background color of button
    
    // Change border radius
    QRect rect = this->rect();
    rect.setWidth(rect.width()-1);
    rect.setHeight(rect.height()-1);
    painter.drawRoundedRect(rect, 8, 8);

    // Move on middle top if the application has been resized
    move((widgetParent->width()/2.0 - width()/2.0), 0);

    QWidget::paintEvent(event);
}

void NotificationWidget::setTimer(int timeInMS)
{
    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &QWidget::close);
    timer->start(3000); // in ms
}

} // end namespace
