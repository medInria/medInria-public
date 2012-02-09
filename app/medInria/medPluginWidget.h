#ifndef MEDPLUGINWIDGET_H
#define MEDPLUGINWIDGET_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
class medPluginWidgetPrivate;

class medPluginWidget : public QWidget
{
    Q_OBJECT
public:
    explicit medPluginWidget(QWidget *parent = 0);
    ~medPluginWidget();

signals:

public slots:
    void onPluginTreeItemActivated(QTreeWidgetItem* item ,int column);
    void onErrorTableItemActivated(QTableWidgetItem* item);
protected:
    void reset();

private:
    medPluginWidgetPrivate * d;
};

#endif // MEDPLUGINWIDGET_H
