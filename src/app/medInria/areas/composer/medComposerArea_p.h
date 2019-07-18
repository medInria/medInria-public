/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore>
#include <QtWidgets>

class dtkComposerWidget;
class dtkComposerControls;
class dtkComposerNodeFactoryView;
class dtkComposerGraphView;
class dtkComposerSceneModel;
class dtkComposerSceneNodeEditor;
class dtkComposerSceneView;
class dtkComposerStackView;

class dtkDistributor;

class dtkRecentFilesMenu;

class dtkSplitter;

class dtkWidgetsLogView;

class dtkComposerViewManager;
class dtkPlotViewSettings;

class medComposerArea;

class medComposerAreaPrivate : public QObject
{
    Q_OBJECT

public:
    bool maySave(void);

public:
    void setCurrentFile(const QString& file);

public slots:
    void setModified(bool modified);

public:
    dtkComposerWidget *composer;
    dtkComposerControls *controls;
    dtkComposerNodeFactoryView *nodes;
    dtkComposerGraphView *graph;
    dtkComposerSceneModel *model;
    dtkComposerSceneNodeEditor *editor;
    dtkComposerSceneView *scene;
    dtkComposerStackView *stack;

public:
    dtkDistributor *distributor;

public:
    dtkComposerViewManager *view_manager;
    dtkPlotViewSettings *plot_view_settings;

public:
    dtkWidgetsLogView *log_view;

public:
    dtkSplitter *inner;

public:
    QMenu *composition_menu;
    QAction *composition_open_action;
    QAction *composition_save_action;
    QAction *composition_saveas_action;
    QAction *composition_insert_action;

    QMenu *edit_menu;
    QAction *undo_action;
    QAction *redo_action;

    dtkRecentFilesMenu *recent_compositions_menu;

public:
    QPushButton *compo_button;
    QPushButton *distr_button;
    QPushButton *debug_button;
    QPushButton *view_button;

public:
    bool closing;

public:
    QString current_composition;

public:
    int wl;
    int wr;
    int wg;

public:
    medComposerArea *q;
};

