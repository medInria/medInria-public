/* medComposerArea_p.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008-2011 - Julien Wintz, Inria.
 * Created: Wed Feb  1 12:37:28 2012 (+0100)
 * Version: $Id: 82f3beb9fd4e7a25cbb10a39def9d298b1e35c92 $
 * Last-Updated: Tue Jun 25 10:04:05 2013 (+0200)
 *           By: Selim Kraria
 *     Update #: 154
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

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

#if defined(Q_OS_MAC) && (MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_6)
    void enableFullScreenSupport(void);
    void showFullScreen(void);
#endif

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
    QAction *composition_quit_action;

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

