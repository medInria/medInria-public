/* medBrowserArea_p.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Jun 28 15:03:27 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Oct  5 18:16:09 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 14
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
class medToolBoxJobs;
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
    medToolBoxJobs *toolbox_jobs;
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
