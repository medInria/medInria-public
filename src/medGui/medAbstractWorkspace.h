/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore>

#include <medGuiExport.h>

struct QUuid;
class QListWidgetItem;

class medToolBox;

class medTabbedViewContainers;
class medDataIndex;
class medViewContainer;

/**
 * @brief A Workspace holds medToolBoxes, medViewContainers and their relations.
 *
 *The main role of a workspace is to provide a coherent set of toolboxes and containers that interact with each other.
 *
 * A workspace is usually instantiated by a factory.
 * It owns several medViewContainers in a medTabbedViewContainers.
 * It also owns toolboxes, but does not place them, the medWorkspaceArea does it when
 * medWorkspaceArea::setupWorkspace is called.
 *
*/
class medAbstractWorkspacePrivate;
class MEDGUI_EXPORT medAbstractWorkspace : public QObject
{
    Q_OBJECT

public:

    medAbstractWorkspace(QWidget *parent=0);
    ~medAbstractWorkspace();

    virtual QString identifier() const = 0;
    virtual QString description() const = 0;

    QList <medToolBox*> toolBoxes() const;
    medToolBox* selectionToolBox() const;
    void setDatabaseVisibility(bool);
    bool isDatabaseVisible() const;
    void setToolBoxesVisibility(bool);
    bool areToolBoxesVisible() const;
    virtual void setupViewContainerStack()=0;
    medTabbedViewContainers * stackedViewContainers() const;

    void setName(QString &name);
    QString& name() const;

public slots:
    virtual void clear();
    virtual void addNewTab();
    void updateNavigatorsToolBox();
    void updateLayersToolBox();
    void updateInteractorsToolBox();
    void clearWorkspaceToolBoxes();
    void addToolBox(medToolBox *toolbox);
    void removeToolBox(medToolBox *toolbox);

    virtual void open(const medDataIndex& index);

protected slots:
    void changeCurrentLayer(int row);
    void setLayerVisibility(bool visibility);
    void removeLayer();
    void updateParameterPool(QString);


private slots:
    void buildTemporaryPool();

private:
        QIcon createIcon(const QString &colorName) const;

private:
    medAbstractWorkspacePrivate *d;
};


