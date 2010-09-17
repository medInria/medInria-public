// QT includes
#include <QApplication>
#include <QCleanlooksStyle>
#include "SimpleView.h"

extern int qInitResources_icons();

int main( int argc, char** argv )
{

  // QT Stuff
  QApplication app( argc, argv );

  QApplication::setStyle(new QCleanlooksStyle);
  
  qInitResources_icons();
  
  SimpleView mySimpleView;
  mySimpleView.show();


  return app.exec();

}
