#ifndef MEDPACSMOVER_H
#define MEDPACSMOVER_H

#include "medPacsExport.h"

#include <QtCore>

class medPacsMoverPrivate;

#include "medMoveCommandItem.h"

class MEDPACS_EXPORT medPacsMover : public QObject, public QRunnable
{
    Q_OBJECT

public:
     medPacsMover(const QVector<medMoveCommandItem>& cmdList);
    ~medPacsMover(void);

    void run(void);

    void doQueuedMove();

signals:
    void import(QString);

    void progressed(int);
    void success (void);
    void failure (void);
    void showError(QObject*,const QString&,unsigned int timeout);
    void cancelled(void);

public slots:
    void onCancel();

private:
    medPacsMoverPrivate *d;
};

#endif 
