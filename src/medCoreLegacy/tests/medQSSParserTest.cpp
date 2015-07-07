/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medQSSParserTest.h>
#include <medStyleSheetParser.h>

void medQSSParserTest::testParse_data()
{
    QTest::addColumn<QString>("qss");
    QTest::addColumn<QString>("result");

    QTest::newRow("test 1") << "$var = 12;" << "";
    QTest::newRow("test 2") << "$var = 12; machinbidule;" << " machinbidule;";
    QTest::newRow("test 3") << "$var = 12; truc; $var ${var}" << " truc; 12 12";
    QTest::newRow("test 4") << "$var = 12; truc; $var = $var; property: ${var}px;" << " truc; property: 12px;";
    QTest::newRow("test 5") << "$var = 12; truc; $var = $var2; property: $var;" << " truc; property: ;";
    QTest::newRow("nested vars") << "$var = bob; $bobo = truc; $var2 = ${${var}o}; property: $var2;" << " property: truc;";

    QTest::newRow("multiline") << "$selector = medToolBoxBody QTabBar::tab:selected;\n"
                                  "$sub_control = ::tab:hover;\n"
                                  "$border_Color=     #2c2c2e  \t;\n"
                                  "$selector, medToolBoxBody QTabBar$sub_control {\n"
                                  "   border: 1px solid $border_Color;\n"
                                  "   border-top: none;\n"
                                  "   border-left: none;\n"
                                  "   $gradient=qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 $border_Color, stop:1 #2c2c2e)\n"
                                  "   border-radius: 0;\n"
                                  "   $Gradient=qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #656565, stop:1 $border_Color);\n"
                                  "   background: $Gradient;\n"
                                  "   height: 32px;\n"
                                  "   min-width: 40px;\n"
                                  "}\n"
                               << "medToolBoxBody QTabBar::tab:selected, medToolBoxBody QTabBar::tab:hover {\n"
                                  "   border: 1px solid #2c2c2e;\n"
                                  "   border-top: none;\n"
                                  "   border-left: none;\n"
                                  "   =qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2c2c2e, stop:1 #2c2c2e)\n"
                                  "   border-radius: 0;\n"
                                  "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #656565, stop:1 #2c2c2e);\n"
                                  "   height: 32px;\n"
                                  "   min-width: 40px;\n"
                                  "}";

    QTest::newRow("comment /* */")     << "$var = /*hello*/12; truc: $var;" << " truc: 12;";
    QTest::newRow("comment //")        << "$var = 12; //$var = 13;\nproperty: $var;" << "property: 12;";
    QTest::newRow("comment tricky")    << "$var = /*hel/* $var lo*/12; truc: $var;" << " truc: 12;";
    QTest::newRow("comment tricky 2")  << "$var = /*hel/* //$var lo*/12; truc: $var;// // $var" << " truc: 12;";
    QTest::newRow("comment multiline") << "$var = /*hello*/12;\n"
                                          " truc: $var;\n"
                                          " $var = 1/**/5; /* \n"
                                          " $var = 17;/*\n"
                                          " */ truc: $var;\n"
                                       << " truc: 12;\n"
                                          " truc: 15;";
}


void medQSSParserTest::testParse()
{
    QFETCH(QString, qss);
    QFETCH(QString, result);

    medStyleSheetParser parser(qss);

    QCOMPARE(parser.result(), result);
}

QTEST_MAIN(medQSSParserTest)
