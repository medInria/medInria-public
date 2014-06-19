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


class medLinkMenuPrivate;

/**
  * QListWidget doesn't seem to be able to resize itself to its content
  * medLayerListWidget should do so.
  */
class medListWidget : public QListWidget
{
public:
    medListWidget(QWidget *parent = 0):QListWidget(parent)
    {}

    virtual QSize sizeHint() const
    {
        int height = 0;
        for(int i=0; i< this->count(); i++)
        {
           QListWidgetItem* item = this->item(i);
           if(this->itemWidget(item))
             height += this->itemWidget(item)->height();
           //else height += 10;
        }
        //height += 10;

        if(this->count() == 0)
            return QListWidget::sizeHint();
        else return QSize(QListWidget::sizeHintForColumn(0), height);
        //else return QSize(QListWidget::sizeHint().width()-10, height);

    }
};


class medLinkMenu : public QPushButton
{
    Q_OBJECT

public:
    medLinkMenu(QWidget * parent = 0);
    void setAvailableParameters(QStringList parameters);
    bool eventFilter(QObject *object, QEvent *event);

public slots:
    void showPopup ();    

private slots:
    void createNewGroup();
    void test();
    void selectItem(QListWidgetItem *);
    void showSubMenu(QListWidgetItem *);
    void showSubMenu();
    void hideSubMenu();

protected:
    void moveEvent(QMoveEvent *);
    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *);

private:
    medLinkMenuPrivate *d;
};


class medGroupWidget : public QWidget
{
    Q_OBJECT

public:
    medGroupWidget(QString groupName, QWidget *parent = 0);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

signals:
    void enterEvent();
    void leaveEvent();

};


class medLeftArrow : public QWidget
{
    Q_OBJECT

public:
    medLeftArrow(QWidget * parent = 0): QWidget(parent){ this->setFixedSize(15,15);}

    void paintEvent(QPaintEvent*);
};

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
    virtual void setupViewContainerStack()=0;
    medTabbedViewContainers * stackedViewContainers() const;

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

private slots:
    void buildTemporaryPool();

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
