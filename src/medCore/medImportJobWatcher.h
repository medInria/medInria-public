#ifndef MEDIMPORTJOBWATCHER_H
#define MEDIMPORTJOBWATCHER_H

#include <QObject>

class medDataIndex;

class medImportJobWatcher : public QObject
{
    Q_OBJECT
public:
    explicit medImportJobWatcher(QObject *parent = 0);

signals:
    void imported(const medDataIndex& index);
};

#endif // MEDIMPORTJOBWATCHER_H
