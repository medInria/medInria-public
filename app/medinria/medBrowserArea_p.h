/* medBrowserArea_p.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Jun 28 15:03:27 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 11:14:51 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 19
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDBROWSERAREA_P_H
#define MEDBROWSERAREA_P_H

class medToolBoxContainer;
class medBrowserToolBoxJobs;
class medBrowserToolBoxSource;
class medBrowserToolBoxPacsHost;
class medBrowserToolBoxPacsNodes;
class medBrowserToolBoxPacsSearch;
class medPacsSelector;

class medDatabasePreview;
class medDatabaseModel;
class medDatabaseView;

class dtkFinder;
class dtkFinderPathBar;
class dtkFinderSideView;
class dtkFinderToolBar;

class medPacsWidget;

class QStatusBar;

class medBrowserAreaPrivate
{
public:
    medToolBoxContainer *toolbox_container;
    medBrowserToolBoxJobs *toolbox_jobs;
    medBrowserToolBoxSource *toolbox_source;
    medBrowserToolBoxPacsHost *toolbox_pacs_host;
    medBrowserToolBoxPacsNodes *toolbox_pacs_nodes;
    medBrowserToolBoxPacsSearch *toolbox_pacs_search;
    medPacsSelector *pacs_selector;

    medDatabasePreview *preview;
    medDatabaseModel *model;
    medDatabaseView *view;

    dtkFinder *finder;
    dtkFinderPathBar *path;
    dtkFinderSideView *side;
    dtkFinderToolBar *toolbar;

    medPacsWidget *pacs;

    QStackedWidget *stack;
    QStatusBar *status;
};

#endif
