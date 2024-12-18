#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVirtualRepresentation.h>

#include <QAction>
#include <QWidget>
#include <QObject>
#include <QLabel>
#include <QMenu>
#include <QToolButton>
#include <QTreeView>
#include <QVBoxLayout>



class medVirtualRepresentationWidget : public QWidget
{
    Q_OBJECT

public:
    medVirtualRepresentationWidget();
    ~medVirtualRepresentationWidget();

    void create(medVirtualRepresentation *virtualRepresentation);

    QTreeView *getVirtualTree() { return virtualTree;};
public slots:
    void expandTree();
   

private:
    QVBoxLayout m_layout;
    QHBoxLayout *titleLayout;
    QTreeView   *virtualTree;
};
