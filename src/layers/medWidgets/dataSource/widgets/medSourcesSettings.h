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
#include <QComboBox>
#include <QHeaderView>
#include <QPushButton>
#include <QTextEdit>
#include <QTreeWidget>
#include <QFileDialog>

#include <medSourcesLoader.h>
#include <medWidgetsExport.h>

class medSourceSettingsWidget;
class QListWidget;
class QListWidgetItem;

class MEDWIDGETS_EXPORT medSourcesSettings : public QWidget
{
    Q_OBJECT

public:
    explicit medSourcesSettings(medSourcesLoader * pSourceLoader, QWidget *parent = nullptr);

  public slots:  
    void setAsDefault();
    void connectSource();
    void removeSource();
    void updateSourceConnection();
    void sourceMoved(const QModelIndex & parent, int start, int end, const QModelIndex & destination, int row);
    void selectedSourceChange(int pi_index);
    void updateConnectButton(medAbstractSource * pSource);
    void connectButtonText();
    void sourceAdded(medAbstractSource *pi_pSource);
    void sourceRemoved(medAbstractSource *pi_pSource);
    void addNewSourceItem(medAbstractSource * pSource);
    void switchMinimization(medSourceSettingsWidget* sourceWidget, bool isMinimized);

protected:
    void updateSourceInformation(medAbstractSource * pi_pSource);
    void createNewSource();
    void updateSelectedSourceDescription(int currentIndex);


private:
    //Data
    medSourcesLoader                           * m_sourceLoader;
    QMap<medAbstractSource*, QListWidgetItem*>   m_sourceToItem;

    //Widgets
    QComboBox   * m_sourceTypeCombobox;
    QTextEdit   * m_sourceDescriptionWidget;
                
    QListWidget * m_sourceListWidget;
                
    QPushButton * m_setDefaultButton;
    QPushButton * m_connectButton;
    QTreeWidget * m_sourceInformation; 
    QPushButton * m_removeButton;
                
    QLineEdit   * m_confPathLineEdit;
    QPushButton * m_confOpenButton;
    QFileDialog * m_confDialogFile;
};
