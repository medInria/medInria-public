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

#include <medCoreGuiExport.h>

#include <QMap>              // for QMap
#include <QWidget>           // for QWidget

class QComboBox;
class QFileDialog;
class QLineEdit;
class QModelIndex;
class QPushButton;
class QTextEdit;
class medAbstractSource;
class medSourcesLoader;

class medSourcesSettingsHandlerWidget;
class QListWidget;
class QListWidgetItem;


/**
 * @class   medSourcesSettings
 * @author  Florent Leray
 * @author  Mathilde Merle
 * @date    27/04/2022
 *
 * @brief   medSourcesSettings is the main widget to handle sources.
 * This widget allows:
 *   - creation and deletion of source connections. 
 *   - modify parameters for existing connections.
 *   - change place of sources describe file:
 *     - You can select directory that contains file by the file explorer.
 *       - if DataSources.json exists it will takes into a count after software reboot.
 *       - if file does not exist, the file is created with actual configuration.
 *     - You can specified a configuration file with different name directly in line edit.
 *     - If path to directory is invalid after software reboot, it will assume to use current directory.
 */
class MEDCOREGUI_EXPORT medSourcesSettings : public QWidget
{
    Q_OBJECT

public:
     medSourcesSettings(medSourcesLoader * pi_pSourceLoader, QWidget *pi_parent = nullptr);
    ~medSourcesSettings() = default;

  public slots:
    void selectedSourceChange(int pi_index);
    void sourceMoved(const QModelIndex & parent, int start, int end, const QModelIndex & destination, int row);

    void createSource();
    void removeSource();
    bool setAsDefault();

    void sourceCreated(medAbstractSource *pi_pSource);
    void sourceRemoved(medAbstractSource *pi_pSource);
    void updateSourceConnection();

protected:
    void updateSelectedSourceDescription(int pi_index);
    QWidget * createConfPathWidget();
    
private:
    //Data
    medSourcesLoader                           * m_sourceLoader; /*!< pointer on source loader. */
    QMap<medAbstractSource*, QListWidgetItem*>   m_sourceToItem; /*!< Maps of items used by m_sourceListWidget stored by associated medAbstractSource pointer. */

    //Widgets
    QComboBox   * m_sourceTypeCombobox;      /*!< Combo-box that displays type of sources available for creation. */
    QTextEdit   * m_sourceDescriptionWidget; /*!< Text area that display a description for type selected in m_sourceTypeCombobox. */
                
    QListWidget * m_sourceListWidget; /*!< Widget list of sources instances. It's left panel. */
    medSourcesSettingsHandlerWidget * m_SettingsHandlerWidget; /*!< Sub-Widget to display and handle current selected source instance inside the m_sourceListWidget. It's right panel. */
                
    QLineEdit   * m_confPathLineEdit; /*!< Widget for directory path that contains sources description file */
    QPushButton * m_confOpenButton; /*!< Widget button to select the directory that contains sources description file */
};
