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

class medAbstractView;
class medAbstractData;

class medViewContainer2Private;
class MEDGUI_EXPORT medViewContainer2: public QFrame
{
    Q_OBJECT
public:
    medViewContainer2(QWidget* parent = 0);
    ~medViewContainer2();

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

protected:
    void focusInEvent(QFocusEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

    void recomputeStyleSheet();

private slots:
    void removeInterneView();
    void selfDestruct();


private:
    medViewContainer2Private *d;

};
