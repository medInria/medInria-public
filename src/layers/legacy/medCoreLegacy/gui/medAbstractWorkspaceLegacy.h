#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtCore>
#include <QtGui>

#include <medCoreLegacyExport.h>
#include <medProgressionStack.h>

class QUuid;
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

class medAbstractParameterGroupL;
class medViewParameterGroupL;
class medLayerParameterGroupL;

class medAbstractWorkspaceLegacyPrivate;
class MEDCORELEGACY_EXPORT medAbstractWorkspaceLegacy : public QObject
{
    Q_OBJECT

public:

    medAbstractWorkspaceLegacy(QWidget *parent = nullptr);
    virtual ~medAbstractWorkspaceLegacy();

    virtual QString identifier() const = 0;
    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual QString category() const =0;

    QList <medToolBox*> toolBoxes() const;
    void setDatabaseVisibility(bool);
    bool isDatabaseVisible() const;
    void setToolBoxesVisibility(bool);
    bool areToolBoxesVisible() const;
    virtual void setupTabbedViewContainer();
    medTabbedViewContainers * tabbedViewContainers() const;

    medToolBox* getMouseInteractionToolBox() const;
    medToolBox* getNavigatorToolBox() const;
    medToolBox* getLayersToolBox() const;
    medToolBox* getProgressionStackToolBox() const;

    void setUserLayerPoolable(bool poolable);
    void setUserViewPoolable(bool poolable);
    void setUserLayerClosable(bool Closable);
    bool isUserLayerPoolable() const;
    bool isUserViewPoolable() const;
    bool isUserLayerClosable() const;
    virtual void setInitialGroups();

    QList<int> getSelectedLayerIndices();

    medProgressionStack *getProgressionStack();

    void addToolBox(medToolBox *toolbox);
    void insertToolBox(int index, medToolBox* toolbox);
    void removeToolBox(medToolBox *toolbox);

public slots:
    virtual void addNewTab();
    void updateNavigatorsToolBox();
    void updateMouseInteractionToolBox();
    void updateLayersToolBox();
    void updateInteractorsToolBox();

    virtual void open(const medDataIndex& index);

    void resetCameraOnSelectedLayer(QListWidgetItem *item);

protected slots:
    void handleLayerSelectionChange();
    void changeCurrentLayer(int row);
    void removeLayer();

    void addViewGroup(QString);
    void addLayerGroup(QString);
    void addViewGroup(medViewParameterGroupL*);
    void addLayerGroup(medLayerParameterGroupL*);
    void setViewGroups(QList<medViewParameterGroupL*>);
    void setLayerGroups(QList<medLayerParameterGroupL*>);

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

signals:
    void layerSelectionChanged(QList<int> selectedLayersIndices);
    void toolBoxInserted(int index, medToolBox* toolbox);
    void toolBoxRemoved(medToolBox* toolbox);

private:
    QWidget* buildViewLinkMenu();
    QWidget* buildLayerLinkMenu(QList<QListWidgetItem*>);

    medAbstractWorkspaceLegacyPrivate *d;
};

#define MED_WORKSPACE_INTERFACE(_name,_desc,_cate) \
public:\
    static QString staticIdentifier() {return QString(staticMetaObject.className());}\
    static QString staticName() {return QString::fromUtf8(_name);}\
    static QString staticDescription() {return QString::fromUtf8(_desc);}\
    static QString staticCategory() {return QString::fromUtf8(_cate);}\
    virtual QString identifier() const {return staticIdentifier();}\
    virtual QString name() const {return staticName();}\
    virtual QString description() const {return staticDescription();}\
    virtual QString category() const {return staticCategory();}
