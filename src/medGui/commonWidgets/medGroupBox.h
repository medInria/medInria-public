#ifndef MEDGROUPBOX_H
#define MEDGROUPBOX_H

#include "medGuiExport.h"

#include <QGroupBox>
#include <QChildEvent>
class medGroupBoxPrivate;

/**
 * @brief Extends a QGroupBox to make it collapsible.
 *
 */
class MEDGUI_EXPORT medGroupBox : public QGroupBox
{
    Q_OBJECT

    Q_PROPERTY(bool collapsible READ isCollapsible WRITE setCollapsible)
public:
    /**
     * @brief
     *
     * @param parent
     */
    explicit medGroupBox(QWidget *parent = 0);
    ~medGroupBox();
    bool isCollapsible();
    void setCollapsible(bool collapsible);
    bool isCollapsed();
    bool isExpanded();


signals:

public slots:
    void setCollapsed(bool collapse);
    void setExpanded(bool expand);

protected:
        virtual void childEvent(QChildEvent* event);

private:
    medGroupBoxPrivate * d;
};

#endif // MEDGROUPBOX_H
