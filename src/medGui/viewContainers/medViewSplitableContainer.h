/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QWidget>

#include <medGuiExport.h>

class medAbstractView;
class medAbstractData;
class medDataIndex;

class medViewContainer2Private;
class MEDGUI_EXPORT medViewSplitableContainer: public QWidget
{
    Q_OBJECT
public:
    medViewSplitableContainer(QWidget* parent = 0);
    ~medViewSplitableContainer();

    bool isSelected() const;
    bool isMaximised() const;
    medAbstractView* view() const;
    void addRmSplittButton();

public slots:
    void setSelected(bool selected);
    void setUnSelected(bool unSelected);
    void setMaximised(bool maximised);
    void setView(medAbstractView* view);
    void addData(medAbstractData* data);

signals:
    void maximised(bool maximised);
    void selected();
    void vSplitRequest();
    void hSplitRequest();
    void splitRequest(medDataIndex, Qt::AlignmentFlag);

protected:
    void focusInEvent(QFocusEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

    void recomputeStyleSheet();

private slots:
    void removeInterneView();
    void selfDestroy();
    void createDragLabels();
    void destroyDragLabels();


private:
    medViewContainer2Private *d;

};
