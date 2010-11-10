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
class medToolBoxSource;
class medToolBoxPacsHost;
class medToolBoxPacsNodes;
class medToolBoxPacsSearch;

class medDatabasePreview;
class medDatabaseModel;
class medDatabaseView;

class dtkFinder;
class dtkFinderPathBar;
class dtkFinderSideView;

class medPacsWidget;

class QStatusBar;

class medBrowserAreaPrivate
{
public:
    medToolBoxContainer *toolbox_container;

    medBrowserToolBoxJobs *toolbox_jobs;
    medToolBoxSource *toolbox_source;
    medToolBoxPacsHost *toolbox_pacs_host;
    medToolBoxPacsNodes *toolbox_pacs_nodes;
    medToolBoxPacsSearch *toolbox_pacs_search;

    medDatabasePreview *preview;
    medDatabaseModel *model;
    medDatabaseView *view;

    dtkFinder *finder;
    dtkFinderPathBar *path;
    dtkFinderSideView *side;

    medPacsWidget *pacs;

    QStackedWidget *stack;
    QStatusBar *status;
};

#endif
