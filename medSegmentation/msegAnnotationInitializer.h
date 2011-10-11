/*
 * msegAnnotationInializer.h
 *
 *  Created on: 31 mai 2011 13:38:56
 *      Author: jstark
 */

#ifndef MSEGANNOTATIONINITIALIZER_H_
#define MSEGANNOTATIONINITIALIZER_H_

namespace mseg {

class AnnotationInitializer {
public:
    AnnotationInitializer();
    virtual ~AnnotationInitializer();
    static bool initialize();
protected:
    static bool initializeAnnotationData();
    static bool initializeAnnotations();
};

}

#endif /* MSEGANNOTATIONINITIALIZER_H_ */

