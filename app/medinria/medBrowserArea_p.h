/* medBrowserArea_p.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Jun 28 15:03:27 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Mon Jun 28 15:05:06 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 2
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

class medDatabasePreview;
class medDatabaseModel;
class medDatabaseView;

class dtkFinder;
class dtkFinderPathBar;
class dtkFinderSideView;

class QStatusBar;

class medBrowserAreaPrivate
{
public:
    medToolBoxContainer *toolbox_container;
    medToolBoxJobs *toolbox_jobs;
    medToolBoxSource *toolbox_source;

    medDatabasePreview *preview;
    medDatabaseModel *model;
    medDatabaseView *view;

    dtkFinder *finder;
    dtkFinderPathBar *path;
    dtkFinderSideView *side;

    QStatusBar *status;
};

#endif
