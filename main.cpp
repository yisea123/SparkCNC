#include <QApplication>
#include <QtGui>
#include <QtGlobal>
#include <QFont>
#include <QTranslator>
#include "maininterface.h"
#include "sparkinfo.h"
#include "setting.h"
#include "qdebug.h"

/*定义全局变量SparkInfo，保存运行参数*/
SparkInfo *spark_info = new SparkInfo();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    /*从命令行中设置系统语言*/
    QTranslator translator;
    bool b = false;
    b = translator.load(":/"+QString(argv[1])+".qm");
    if(b)
        a.installTranslator(&translator);

    MainInterface m;
/*
    a.setStyleSheet(
        "QPushButton{border:3px solid rgb(0,0,0); background:rgb(255,255,255);border-radius: 10px;}"
        "QPushButton:hover{border-color:rgb(96,96,96); background:rgb(128,128,128);}"
        "QPushButton:focus{border-color:rgb(96,96,96); background:rgb(128,128,128);}"
        "QPushButton:checked{border-color:rgb(255,255,255);background:rgb(128,128,128);}"
        "QHeaderView::section{background-color:rgb(128,128,128);color:white;border:1px solid #6c6c6c;}"
        "QTableView::item:selected{background-color:rgb(255,206,0);}"
        "QTableView::item:focus{background-color:rgb(255,206,0);}"
                    );
*/
    a.setStyleSheet(
        "QPushButton{border-image:url(:/nomal.png); color: white;}"
        "QPushButton:checked{border-image:url(:/press.png); color: green;}"
        "QPushButton:hover{border-image:url(:/hover.png); color: white;}"
        "QPushButton:focus{border-image:url(:/hover.png); color: white;}"
        "QHeaderView::section{background-color:rgb(128,128,128);color:white;border:1px solid #6c6c6c;}"
        "QTableView::item:selected{background-color:rgb(255,206,0);}"
        "QTableView::item:focus{background-color:rgb(255,206,0);}"
                    );
    QFont font = a.font();

#ifdef ARM
    QRect screen_size = QApplication::desktop()->screenGeometry();
    m.setGeometry(0, 0, screen_size.width(), screen_size.height());
    font.setFamily("SparkFont");
#endif

    a.setFont(font);
    m.show();

    return a.exec();
}
