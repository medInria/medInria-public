/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QFrame>

#include <medCoreExport.h>


struct QUuid;
class medAbstractView;
class medAbstractData;
class medDataIndex;
class medToolBox;
class medViewContainerSplitter;
class medAbstractParameter;

class medViewContainerPrivate;
class MEDCORE_EXPORT medViewContainer: public QFrame
{
    Q_OBJECT

public:
    enum ClosingMode
    {
        CLOSE_CONTAINER, /** The close button close / delete the container itelf as well as the view if any. */
        CLOSE_VIEW_ONLY, /** The close button close / delete the view if any. */
        CLOSE_BUTTON_HIDDEN /** No close button */
    };

private:
    enum DropArea
    {
        AREA_OUT = 0,
        AREA_TOP,
        AREA_BOTTOM,
        AREA_LEFT,
        AREA_RIGHT,
        AREA_CENTER
    };

public:
    medViewContainer(medViewContainerSplitter* parent = 0);
    ~medViewContainer();

    medAbstractView* view() const;
    QUuid uuid() const;
    void setContainerParent(medViewContainerSplitter* splitter);

    bool isSelected() const;
    bool isMaximized() const;
    bool isUserSplittable() const;
    ClosingMode closingMode() const;
    bool isMultiLayered() const;
    bool isUserOpenable() const;

    medViewContainer* splitVertically();
    medViewContainer* splitHorizontally();
    medViewContainer* split(Qt::AlignmentFlag alignement = Qt::AlignRight);

    void setDefaultWidget(QWidget *defaultWidget);
    QWidget* defaultWidget() const;

    void addColorIndicator(QColor color, QString description="");
    void removeColorIndicator(QColor color);

public slots:
    void setView(medAbstractView* view);
    void removeView();
    void addData(medAbstractData* data);
    void addData(medDataIndex index);

    void setSelected(bool selected);
    void setUnSelected(bool unSelected);
    void toggleMaximized();
    void setUserSplittable(bool splittable);
    void setClosingMode(enum ClosingMode mode);
    void setMultiLayered(bool multiLayer);
    void setUserOpenable(bool openable);

    void highlight(QString color = "#FFAA88");
    void unHighlight();

    void splitContainer(unsigned int numY, unsigned int numX);

signals:
    void maximized(QUuid uuid, bool maximized);
    void maximized(bool maximized);
    void containerSelected(QUuid uuid);
    void containerUnSelected(QUuid uuid);
    void currentLayerChanged();
    void vSplitRequest();
    void hSplitRequest();
    void splitRequest(medDataIndex, Qt::AlignmentFlag);
    void viewChanged();
    void viewContentChanged();
    void viewRemoved();


protected:
    void focusInEvent(QFocusEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void closeEvent(QCloseEvent * event);

    void recomputeStyleSheet();

protected slots:
    void openFromSystem();
    void updateToolBar();
    void dataReady(medDataIndex index, QUuid uuid);

private slots:
    void removeInternView();
    DropArea computeDropArea(int x, int y);
    void popupMenu();

private:
    medViewContainerPrivate *d;

};
