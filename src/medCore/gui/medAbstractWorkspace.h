/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore>
#include <QtGui>

#include <medCoreExport.h>

struct QUuid;
class QListWidgetItem;
class QAction;

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

class medAbstractParameterGroup;
class medViewParameterGroup;
class medLayerParameterGroup;

class medAbstractWorkspacePrivate;
class MEDCORE_EXPORT medAbstractWorkspace : public QObject
{
    Q_OBJECT

public:

    medAbstractWorkspace(QWidget *parent=0);
    ~medAbstractWorkspace();

    virtual QString identifier() const = 0;
    virtual QString name() const = 0;
    virtual QString description() const = 0;

    QList <medToolBox*> toolBoxes() const;
    medToolBox* selectionToolBox() const;
    void setDatabaseVisibility(bool);
    bool isDatabaseVisible() const;
    void setToolBoxesVisibility(bool);
    bool areToolBoxesVisible() const;
    virtual void setupTabbedViewContainer() = 0;
    medTabbedViewContainers * tabbedViewContainers() const;

    void setUserLayerPoolable(bool poolable);
    void setUserViewPoolable(bool poolable);
    void setUserLayerClosable(bool Closable);
    bool isUserLayerPoolable() const;
    bool isUserViewPoolable() const;
    bool isUserLayerClosable() const;

public slots:
    virtual void clear();
    virtual void addNewTab();
    void updateNavigatorsToolBox();
    void updateMouseInteractionToolBox();
    void updateLayersToolBox();
    void updateInteractorsToolBox();
    void clearWorkspaceToolBoxes();
    void addToolBox(medToolBox *toolbox);
    void removeToolBox(medToolBox *toolbox);

    virtual void open(const medDataIndex& index);

protected slots:
    void changeCurrentLayer(int row);
    void removeLayer();

    void addViewGroup(QString);
    void addLayerGroup(QString);
    void addViewGroup(medViewParameterGroup*);
    void addLayerGroup(medLayerParameterGroup*);
    void setViewGroups(QList<medViewParameterGroup*>);
    void setLayerGroups(QList<medLayerParameterGroup*>);

private slots:
    void buildTemporaryPool();

    void addViewstoGroup(QString);
    void removeViewsFromGroup(QString);

    void addLayerstoGroup(QString);
    void removeLayersFromGroup(QString);

    void removeViewGroup(QString);
    void removeLayerGroup(QString);

    void changeViewGroupColor(QString group, QColor color);
    void changeLayerGroupColor(QString group, QColor color);

private:
    QWidget* buildViewLinkMenu();
    QWidget* buildLayerLinkMenu(QList<QListWidgetItem*>);

private:
    medAbstractWorkspacePrivate *d;
};

#define MED_WORKSPACE_INTERFACE(_name,_desc) \
public:\
    static QString staticIdentifier() {return QString(staticMetaObject.className());}\
    static QString staticName() {return QString::fromUtf8(_name);}\
    static QString staticDescription() {return QString::fromUtf8(_desc);}\
    virtual QString identifier() const {return staticIdentifier();}\
    virtual QString name() const {return staticName();}\
    virtual QString description() const {return staticDescription();}
