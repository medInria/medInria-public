/* medRunnableProcess.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:40:14 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:40:15 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDRUNNABLEPROCESS_H
#define MEDRUNNABLEPROCESS_H

#include "medJobItem.h"

#include "medCoreExport.h"

class medRunnableProcessPrivate;
class dtkAbstractProcess;

class MEDCORE_EXPORT medRunnableProcess : public medJobItem
{
    Q_OBJECT

public:
     medRunnableProcess(void);
    ~medRunnableProcess(void);

    void run(void);

    void setProcess (dtkAbstractProcess *proc);

public slots:
    void onSuccess    (void);
    void onFailure    (void);
    void onProgressed (int);

    void onCancel (QObject*);
    
private:
    medRunnableProcessPrivate *d;
};

#endif // MEDRUNNABLEPROCESS_H
