#ifndef MEDBOUTIQUESDROPWIDGET_H
#define MEDBOUTIQUESDROPWIDGET_H

#include <QWidget>
#include "medBoutiquesConfiguration.h"

// A simple widget which emits drop, dragMove and dragEnter signals when it receives dropEvent, dragMoveEvent and dragEnterEvent respetively
// Used to be able to react to drag events without writing a new dedicated widget overriding events which each time
class medBoutiquesDropWidget : public QWidget
{
    Q_OBJECT
public:
    explicit medBoutiquesDropWidget(QWidget *parent = nullptr);

signals:
    void dragEnter(QDragEnterEvent *event);
    void dragMove(QDragMoveEvent *event);
    void drop(QDropEvent *event);

public slots:
protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

#endif // MEDBOUTIQUESDROPWIDGET_H
