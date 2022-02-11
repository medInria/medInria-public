
#include<medAbstractParameter.h   >
#include<medBoolParameter.h       >
#include<medVariantListParameter.h>
#include<medDataListParameter.h   >
#include<medDoubleParameter.h     >
#include<medIntParameter.h        >
#include<medStringListParameter.h >
#include<medStringParameter.h     >
#include<medTimeLineParameter.h   >
#include<medDataParameter.h       >
#include<medTriggerParameter.h    >
#include<medVector2DParameter.h   >
#include<medVector3DParameter.h   >

class MEDCORE_EXPORT medTestParameter
{


public:
    medTestParameter(QObject *parent = nullptr);
    ~medTestParameter();
    
    medBoolParameter a;
    medVariantListParameter b;
    medDataListParameter c;
    medDoubleParameter d;
    medIntParameter e;
    medStringListParameter f;
    medStringParameter g;
    medTimeLineParameter h;
    medTriggerParameter i;
    medVector2DParameter j;
    medVector3DParameter k;
    medDataParameter l;
};

medTestParameter::medTestParameter(QObject * parent): a(QString("a")), b(QString("b")),
                                                      c(QString("c")), d(QString("d")),
                                                      e(QString("e")), f(QString("f")),
                                                      g(QString("g")), h(QString("h")),
                                                      i(QString("i")), j(QString("j")),
                                                      k(QString("k")), l(QString("l"))
{

}

medTestParameter::~medTestParameter()
{
}
