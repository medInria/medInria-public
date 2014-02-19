/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QFrame>

#include <medGuiExport.h>

class QUuid;
class medAbstractView;
class medAbstractData;
class medDataIndex;
class medToolBox;

class medViewContainerPrivate;
class MEDGUI_EXPORT medViewContainer: public QFrame
{
    Q_OBJECT
    Q_PROPERTY(bool selected READ isSelected)

public:
    medViewContainer(QWidget* parent = 0);
    ~medViewContainer();


    bool isSelected() const;
    bool isMaximised() const;
    bool isClosable() const;
    medAbstractView* view() const;
    QUuid& uuid() const;
    medToolBox* toolBox() const;

public slots:
    void setSelected(bool selected);
    void setUnSelected(bool unSelected);
    void setMaximised(bool maximised);
    void setView(medAbstractView* view);
    void addData(medAbstractData* data);
    void setClosable(bool cloasable);

signals:
    void maximised(QUuid& uuid, bool maximised);
    void selected(QUuid& uuid);
    void vSplitRequest();
    void hSplitRequest();
    void splitRequest(medDataIndex, Qt::AlignmentFlag);
    void viewChanged();


protected:
    void focusInEvent(QFocusEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

    void recomputeStyleSheet();

protected slots:
    void removeView();

private slots:
    void removeInterneView();
    void selfDestroy();
    void createDragLabels();
    void destroyDragLabels();


private:
    medViewContainerPrivate *d;

};
