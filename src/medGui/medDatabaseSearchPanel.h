#ifndef medDatabaseSearchPanel_h__
#define medDatabaseSearchPanel_h__

#include <medGui/medToolBox.h>
#include "medGuiExport.h"


class medDatabaseSearchPanelPrivate;

class MEDGUI_EXPORT medDatabaseSearchPanel : public medToolBox
{
    Q_OBJECT
public:
    medDatabaseSearchPanel(QWidget *parent = 0);
    ~medDatabaseSearchPanel();

    void setColumnNames(const QStringList &columns);

signals:
    void filter(const QString &text, int column);

protected slots:
    void onSetFilter(const QString &text);

private:
    medDatabaseSearchPanelPrivate* d;
};

#endif // medDatabaseSearchPanel_h__
