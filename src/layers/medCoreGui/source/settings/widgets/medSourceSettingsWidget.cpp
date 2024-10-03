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
#include "medSourceSettingsWidget.h"

#include <QGroupBox>
#include <QPainter>
#include <QVBoxLayout>
#include <QKeyEvent>

#include <QGuiApplication>
#include <QScreen>

#include <medAbstractParameterPresenter.h>

medSourceSettingsWidget::medSourceSettingsWidget(medAbstractSource * pSource, QListWidgetItem *widgetItem, QWidget * parent) : QFrame(parent)
{
    m_pSource = pSource;
    m_pItem = widgetItem;

    auto * widgetLayout = new QVBoxLayout;
    setLayout(widgetLayout);
    widgetLayout->setAlignment(Qt::AlignTop);

    //--- Title area
    m_titleLayout = new QHBoxLayout;
    widgetLayout->addLayout(m_titleLayout);
    m_titleLayout->setAlignment(Qt::AlignTop);

    // Add on/off icons
    m_onOffIcon = new QImage;
    m_imageLabel = new QLabel(""); // QImage is not a widget, we need to add it in a QLabel
    m_titleLayout->addWidget(m_imageLabel);

    // Add a title 
    m_titleLabel    = new QLabel(pSource->getInstanceName());
    m_titleLineEdit = new QLineEdit();
    m_titleLabel->setStyleSheet("font-weight: bold");
    m_titleStack.addWidget(m_titleLabel);
    m_titleStack.addWidget(m_titleLineEdit);
    m_titleLabel->installEventFilter(this);
    m_titleLineEdit->installEventFilter(this);
    m_titleLayout->addWidget(&m_titleStack);
    m_titleLayout->addStretch();

    // Default message
    m_defaultLabel = new QLabel("");
    m_defaultLabel->setStyleSheet("font: italic");
    m_titleLayout->addWidget(m_defaultLabel);

    // should be in medInria.qss, in order to allow other themes to change colors or styles.
    QString buttonStyle = "QPushButton {"
                          " background-color: none; }"
                          "QPushButton:hover {"
                          " background-color: grey; }";

    // Minimize button
    m_minimizeSourceButton = new QPushButton("");
    QIcon minimizeIcon;
    minimizeIcon.addPixmap(QIcon::fromTheme("arrow-bot").pixmap(24,24), QIcon::Normal);
    minimizeIcon.addPixmap(QIcon(":/arrow-bot-disabled.svg").pixmap(24,24), QIcon::Disabled);
    m_minimizeSourceButton->setIcon(minimizeIcon);
    m_minimizeSourceButton->setFixedWidth(25);
    m_minimizeSourceButton->setToolTip(tr("Show or hide the body of this source item"));
    m_minimizeSourceButton->setStyleSheet(buttonStyle);
    m_titleLayout->addWidget(m_minimizeSourceButton);

    //--- Fill parameters in body
    auto * parametersLayout = new QVBoxLayout;
    m_parametersWidget = new QWidget();
    m_parametersWidget->setLayout(parametersLayout);
    widgetLayout->addWidget(m_parametersWidget);

    auto params = pSource->getAllParameters();
    for (auto * param : params)
    {
        auto * pHLayout = new QHBoxLayout;
        auto * pParamPresenter = medAbstractParameterPresenter::buildFromParameter(param);
        auto * pWidget = pParamPresenter->buildWidget();
        if (dynamic_cast<QPushButton*>(pWidget) == nullptr && dynamic_cast<QGroupBox*>(pWidget) == nullptr)
        {
            auto * pLabel = pParamPresenter->buildLabel();
            pHLayout->addWidget(pLabel);
        }
        pHLayout->addWidget(pWidget);
        parametersLayout->addLayout(pHLayout);
    }

    //-- Init parameters
    switchConnectionIcon(m_pSource->isOnline());
    setToDefault(false);
    m_sourceSelected = false;

    //--- Now that Qt widgets are set: create connections
    connect(m_minimizeSourceButton, &QPushButton::clicked, [=](bool checked)
    {
        switchMinimization();
    });
}

/**
 * @brief Set the icon of the source status
 * 
 * @param connection boolean 
 */
void medSourceSettingsWidget::switchConnectionIcon(bool connection)
{
    QIcon newIcon;
    if (connection)
    {
        newIcon =  QIcon(":/icons/connect_on.svg");
    }
    else
    {
        newIcon = QIcon(":/icons/connect_off.svg");
    }

    QImage newImage = newIcon.pixmap(QSize(15,15)).toImage();
    m_onOffIcon->swap(newImage);
    m_imageLabel->setPixmap(QPixmap::fromImage(*m_onOffIcon));
}

/**
 * @brief Resize the widget and switch the icon after minimization or un-minimization, and show or hide the parameter widget
 */
void medSourceSettingsWidget::switchMinimization()
{
    QIcon newIcon;
    if (m_parametersWidget->isHidden())
    {
        m_parametersWidget->show();
        newIcon = QIcon::fromTheme("arrow-bot");
    }
    else
    {
        m_parametersWidget->hide();
        newIcon =  QIcon::fromTheme("arrow-right");
    }
    m_minimizeSourceButton->setIcon(newIcon);
    emit(minimizationAsked(m_parametersWidget->isHidden()));
    resize(size().width(), sizeHint().height());  // Adjust the new height of the QFrame
}

void medSourceSettingsWidget::titleChanged()
{
    m_titleLabel->setText(m_pSource->getInstanceName());
}

/**
 * @brief Change the visualisation of the source item if selected or not
 * 
 * @param selected 
 */
void medSourceSettingsWidget::setSelectedVisualisation(bool selected)
{
    m_sourceSelected = selected;
    repaint();
}

/**
 * @brief Get unique source name associated with this widget
 * 
 * @return QString the source name
 */
QString medSourceSettingsWidget::getInstanceName()
{
    return m_pSource->getInstanceName();
}

/**
 * @brief When this source item is chosen to be default or not, add or remove the "default" label,
 * and enable or not the delete button (a default source item cannot be deleted)
 * 
 * @param askedDefault 
 */
void medSourceSettingsWidget::setToDefault(bool askedDefault)
{
    m_defaultLabel->setText(askedDefault?"default":"");
}

/**
 * @brief This method saves the initial size of the widget at the creation, with all parameters shown
 * 
 * @param initialSize 
 */
void medSourceSettingsWidget::saveInitialSize(QSize initialSize)
{
    m_sourceWidgetSize = initialSize;
}

/**
 * @brief This method returns the initial size of the widget at the creation, with all parameters shown
 * 
 * @return QSize 
 */
QSize medSourceSettingsWidget::getInitialSize()
{
    return m_sourceWidgetSize;
}




bool medSourceSettingsWidget::eventFilter(QObject * watched, QEvent * event)
{
    if (watched == m_titleLineEdit) 
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if ((event->type() == QEvent::FocusOut) || 
            (event->type() == QEvent::KeyPress && (
                keyEvent->key() == Qt::Key_Return ||
                keyEvent->key() == Qt::Key_Escape ||
                keyEvent->key() == Qt::Key_Enter)))
        {
                m_titleStack.setCurrentIndex(0);
                if (m_titleLabel->text() != m_titleLineEdit->text())
                {
                    emit sourceRename(m_pSource->getInstanceId(), m_titleLineEdit->text());
                }
        }
    }
    else if (watched == m_titleLabel)
    {
        if (event->type() == QEvent::MouseButtonDblClick)
        {
            m_titleStack.setCurrentIndex(1);
            m_titleLineEdit->setText(m_titleLabel->text());
            m_titleLineEdit->setFocus();
        }
    }

    return QWidget::eventFilter(watched, event);
}

void medSourceSettingsWidget::currentItemChanged(QListWidgetItem * current, QListWidgetItem * previous)
{
    if(current)
    {
        auto *pListWidget = current->listWidget();
        if (pListWidget->itemWidget(current) == this)
        {
            m_sourceSelected = true;
            repaint();
        }
        else if (pListWidget->itemWidget(previous) == this)
        {
            m_sourceSelected = false;
            repaint();
        }
    }
}
