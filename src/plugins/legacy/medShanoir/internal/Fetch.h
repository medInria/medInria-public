#pragma once

#include <QRunnable>
#include <QObject>
#include <QJsonObject>

class Fetch : public QObject, public QRunnable
{
	Q_OBJECT

protected:
	int m_id;

public:
    Fetch(int id) { m_id = id;}
    ~Fetch() = default;

    virtual void run() =0;

signals:
	void fetchEnded(int /*id*/, QJsonObject /*received_data*/);    
};
