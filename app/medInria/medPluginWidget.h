#pragma once

#include <QTabWidget>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
class medPluginWidgetPrivate;

class medPluginWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit medPluginWidget(QWidget *parent = 0);
    ~medPluginWidget();

signals:

public slots:
    void onPluginTreeItemActivated(QTreeWidgetItem* item ,int column);
    void onErrorTreeItemActivated(QTreeWidgetItem* item,int column);
protected:
    void reset();

private:
    medPluginWidgetPrivate * d;
};


