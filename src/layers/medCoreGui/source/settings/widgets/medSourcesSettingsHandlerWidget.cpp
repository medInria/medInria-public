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
#include "medSourcesSettingsHandlerWidget.h"

#include <medSourcesSettings.h>
#include <medAbstractSource.h>

#include <QHeaderView>
#include <QLabel>
#include <QObject>
#include <QVBoxLayout>

 /**
  * @brief medSourcesSettingsHandlerWidget constructor.
  *
  * @param [in] parent is the parent medSourcesSettings widget.
  */
medSourcesSettingsHandlerWidget::medSourcesSettingsHandlerWidget(medSourcesSettings *pi_parent) : QWidget(pi_parent)
{
    //--- Fill Sources panel
    auto * sourceLayout = new QHBoxLayout();
    setLayout(sourceLayout);

    //--- Sources details and actions
    auto * sourceSettingsLayout = new QVBoxLayout();
    sourceLayout->addLayout(sourceSettingsLayout);

    auto * detailsLabel = new QLabel("\nSelect a source to use actions:\n");
    sourceSettingsLayout->addWidget(detailsLabel);

    m_setDefaultButton = new QPushButton(tr("Set as default"));
    m_setDefaultButton->setToolTip(tr("Set the selected source as default"));
    sourceSettingsLayout->addWidget(m_setDefaultButton);

    m_connectButton = new QPushButton(tr("Connect"));
    m_connectButton->setToolTip(tr("Switch ON or OFF the selected source"));
    sourceSettingsLayout->addWidget(m_connectButton);

    m_sourceInformation = new QTreeWidget();
    m_sourceInformation->setColumnCount(2);
    m_sourceInformation->setAlternatingRowColors(true);
    m_sourceInformation->setHeaderLabels(QStringList()<<"Information"<<"Result");
    m_sourceInformation->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_sourceInformation->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_sourceInformation->header()->setStretchLastSection(false);
    for (int i = 0; i < 3; ++i) // Init tree
    {
        m_sourceInformation->insertTopLevelItem(i, new QTreeWidgetItem(m_sourceInformation, {"",""}));
    }
    m_sourceInformation->setFixedHeight(98);
    sourceSettingsLayout->addWidget(m_sourceInformation);

    m_removeButton = new QPushButton(tr("Remove source"));
    m_removeButton->setToolTip(tr("Remove the selected source"));
    sourceSettingsLayout->addWidget(m_removeButton);

    sourceSettingsLayout->addStretch();

    //--- Now that Qt widgets are set: create connections
    connect(m_setDefaultButton, &QPushButton::clicked, pi_parent, &medSourcesSettings::setAsDefault); // Change default source
    connect(m_removeButton,     &QPushButton::clicked, pi_parent, &medSourcesSettings::removeSource); // Ask to remove source
    connect(m_connectButton, &QPushButton::clicked, pi_parent, &medSourcesSettings::updateSourceConnection); 

    connect(pi_parent, &medSourcesSettings::createdSource, [&]
    {
        m_connectButton->setDisabled(false);
    });
}

/**
 * @brief This slot handle click on the default button.
 * @details This slot must be connected to m_setDefaultButton.
 */
void medSourcesSettingsHandlerWidget::setAsDefault()
{
    m_removeButton->setDisabled(true);
    m_setDefaultButton->setDisabled(true);
}

/**
 * @brief This method update the connect button and its behavior.
 * @param [in] pi_pSource new source.
 * @param [in] p_default boolean indicate if the new source is the default working source.
 */
void medSourcesSettingsHandlerWidget::sourceChange(medAbstractSource * pi_pSource, bool p_default)
{
    disconnect(m_qtConnections[0]);

    if (pi_pSource != nullptr)
    {
        m_bLocal = pi_pSource->isLocal();
        m_bWritable = pi_pSource->isWritable();
        m_bCached = pi_pSource->isLocal();
        m_bOnline = pi_pSource->isOnline();
        m_bDefault = p_default;

        updateSourceInformation(pi_pSource);
        updateConnectButton();
        updateDefaultButton();
        m_removeButton->setDisabled(p_default);

        m_qtConnections[0] = connect(pi_pSource, &medAbstractSource::connectionStatus, this, &medSourcesSettingsHandlerWidget::sourceConnectStatusChange);
    }
    else
    {
        m_connectButton->setDisabled(true);
        m_setDefaultButton->setDisabled(true);
        m_removeButton->setDisabled(true);

        m_bLocal = false;
        m_bWritable = false;
        m_bCached = false;
        m_bOnline = false;
    }
}


/**
 * @brief This method must be called when source online status change to update GUI.
 * @param [in] p_status is a boolean to indicate if the current source in online (true) or not (false).
 * @details This function must be connected to signal from medAbstractSource::connectionStatus into function sourceChange.
 */
void medSourcesSettingsHandlerWidget::sourceConnectStatusChange(bool p_status)
{
    m_bOnline = p_status;
    updateConnectButton();
    updateDefaultButton();
}

/**
 * @brief This method update the default source button and its behavior.
 */
void medSourcesSettingsHandlerWidget::updateDefaultButton()
{
    bool enableDefault = !m_bDefault && m_bWritable && (m_bLocal || (!m_bLocal && m_bCached)) && m_bOnline;

    m_setDefaultButton->setEnabled(enableDefault);
}

/**
 * @brief This method update the connect button and its behavior.
 */
void medSourcesSettingsHandlerWidget::updateConnectButton()
{
    m_connectButton->setText(m_bOnline ? "Disconnect" : "Connect");
}

/**
 * @brief This method fills the information widget with info from this selected source
 * @param [in] pi_pSource is the source pointer to extract status to display it.
 */
void medSourcesSettingsHandlerWidget::updateSourceInformation(medAbstractSource * pi_pSource)
{
    m_sourceInformation->setVisible(false);
    QTreeWidgetItem *hasCache = m_sourceInformation->topLevelItem(2);;
    hasCache->setText(0, "Has cache: ");
    hasCache->setText(1, pi_pSource->isCached() ? "true" : "false");

    QTreeWidgetItem *isLocal = m_sourceInformation->topLevelItem(1);
    isLocal->setText(0, "Is local: ");
    isLocal->setText(1, pi_pSource->isLocal() ? "true" : "false");

    QTreeWidgetItem *isWritable = m_sourceInformation->topLevelItem(0);
    isWritable->setText(0, "Is writable: ");
    isWritable->setText(1, pi_pSource->isWritable() ? "true" : "false");
    m_sourceInformation->setVisible(true);
}
