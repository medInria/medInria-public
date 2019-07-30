#ifndef MEDBOUTIQUESDROPWIDGET_H
#define MEDBOUTIQUESDROPWIDGET_H

#include <QWidget>

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
