#ifndef MAININTERFACE_H
#define MAININTERFACE_H

#include "mesgbox.h"
#include "toplevel.h"
#include "seconlevel_f3.h"
#include "seconlevel_f4.h"
#include "seconlevel_f5.h"
#include "seconlevel_f6.h"
#include "keyboard.h"
#include "qcommand.h"
#include "sparkthread.h"
#include <QWidget>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QModelIndex>
#include <QKeyEvent>
#include <QMap>
#include <QPushButton>
#include <QFile>
#include <QAbstractItemView>
#include <QItemSelectionModel>

#include "fcntl.h"
#include "sys/ioctl.h"
#include "fm25v02.h"
#include "setting.h"

#define TABLE_ROWS 10
#define TABLE_COLUMN 11

#define TABLE_SHOW 0x00
#define TABLE_EDIT 0x01
#define TABLE_SELECT 0x02
#define TABLE_DELETE 0x03

namespace Ui {
    class MainInterface;
}

class MainInterface : public QWidget
{
    Q_OBJECT

public:
    explicit MainInterface(QWidget *parent = 0);
    void initFuncBar();
    void initHardware();
    void submitTable();
    QStandardItemModel *model;
    bool is_table_edit;
    /*标志数据表格的状态，0x00显示态，0x01数据编辑态，0x02段选态,0x03行删除态*/
    char table_state;
    KeyBoard *keyboard;
    MesgBox *mesg;
    QCommand *command;
    /*放电加工线程*/
    SparkThread *spark;
    ~MainInterface();

private:
    /*标志键盘是否已经被按下*/
    bool key_pressed;
    Ui::MainInterface *ui;
    void keyPressEvent( QKeyEvent *k );
    void keyReleaseEvent( QKeyEvent *k );
    QWidget *barui;
    QMap<int , QWidget *> barmap;

signals:
    void barF0();

public slots:
    void commandFinish();
    void commandSwitch(char ,char ,char ,char);
    void tableStateUpdate(char);
    void tableDataUpdate();
    void tableRollUpdate();
    void tableItemChange(QModelIndex tl ,QModelIndex br);
    void funcbarUpdate(int);
    void XPos_Update();
    void YPos_Update();
    void ZPos_Update();
};

#endif // MAININTERFACE_H