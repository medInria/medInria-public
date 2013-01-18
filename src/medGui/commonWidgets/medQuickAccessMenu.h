#ifndef MEDQUICKACCESSMENU_H
#define MEDQUICKACCESSMENU_H

#include "medGuiExport.h"
#include <medHomepageButton.h>

#include <QtGui>

class MEDGUI_EXPORT medQuickAccessMenu : public QFrame
{
  Q_OBJECT
public:
    medQuickAccessMenu ( bool vertical, QWidget* parent = 0, Qt::WindowFlags f = 0 );
    
    void reset(bool optimizeLayout);
    
protected:
  /**
   *  emit the hideMenu() signal when the widget lost the focus
   */
  void focusOutEvent(QFocusEvent * event);
    
    void mouseMoveEvent (QMouseEvent * event);
    
    void keyPressEvent ( QKeyEvent * event );
    
    void updateCurrentlySelectedRight();
    void updateCurrentlySelectedLeft();
    
    void switchToCurrentlySelected ();

    void createHorizontalQuickAccessMenu();
    void createVerticalQuickAccessMenu();
    
    void onMouseMovedInWidget(unsigned int identifier);
    
signals:
    void hideMenu(void);
    void switchToHomepageArea(void);
    void switchToBrowserArea(void);
    void showWorkspace(QString);

private:
    int currentSelected;
    
    QList <medHomepagePushButton *> buttonsList;
};

#endif // MEDQUICKACCESSMENU_H
