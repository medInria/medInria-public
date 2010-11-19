
#include <iostream>
#include "dcmtkNode.h"

class dcmtkFindScu;
class dcmtkMoveScu;


class SpeedTest
{

public:

    SpeedTest(dcmtkNode hostNode, dcmtkNode peerNode);
    ~SpeedTest();

    // return milliseconds
    int moveStudyLevel(std::string patientName);

    // return milliseconds
    int moveSeriesLevel(std::string patientName);

    // return milliseconds
    int moveImageLevel(std::string patientName,bool useBatch = false);

private:

    void move(int group, int elem, const char* query);

    dcmtkFindScu* m_findScu;
    dcmtkMoveScu* m_moveScu;

    dcmtkNode peerNode;
    dcmtkNode hostNode;

};