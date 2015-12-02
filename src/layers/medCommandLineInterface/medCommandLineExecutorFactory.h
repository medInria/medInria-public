#pragma once

#include <QtCore>
#include "medCommandLineInterfaceExport.h"

#include "medCommandLineExecutor.h"


class medCommandLineExecutorFactory : public QObject
{
public:
     medCommandLineExecutorFactory(void);
    ~medCommandLineExecutorFactory(void);

     static medCommandLineExecutorFactory* instance();

public:
    void record(const QString& key, medCommandLineExecutor* executor);

public:
    medCommandLineExecutor *get(const QString& key) const;

public:
    QStringList keys(void) const;

private:
    QHash<QString, medCommandLineExecutor*> executors;
    static medCommandLineExecutorFactory* s_instance;
};
