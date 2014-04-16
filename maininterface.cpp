#include "maininterface.h"
#include "ui_maininterface.h"
#include "sparkinfo.h"
#include "qdebug.h"

/*根据Qt编译器的版本判断目标机的类型*/

#if(QT_VERSION == 0x040704)
    #define X86
#else
    #define ARM
#endif

/*蜂鸣器文件描述符*/
static int beep_fb;

MainInterface::MainInterface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainInterface)
{
    ui->setupUi(this);

    key_pressed = false;

#ifdef ARM
    initHardware();
#endif

    XPos_Update();
    YPos_Update();
    ZPos_Update();

    connect(spark_info ,SIGNAL(longChange()) ,this ,SLOT(XPos_Update()));
    connect(spark_info ,SIGNAL(longChange()) ,this ,SLOT(YPos_Update()));
    connect(spark_info ,SIGNAL(longChange()) ,this ,SLOT(ZPos_Update()));

    model = new QStandardItemModel;
    /*数据表索引发生改变*/
    connect(spark_info ,SIGNAL(tableChange()) ,this ,SLOT(tableDataUpdate()));
    /*数据表的加工段和当前加工行发生改变*/
    connect(spark_info ,SIGNAL(tableRowChange()),this ,SLOT(tableRollUpdate()));

    /*初始化功能栏*/
    initFuncBar();

    /*初始化为显示态*/
    tableStateUpdate(TABLE_SHOW);

    spark = new SparkThread();

    mesg = new MesgBox(this);
    mesg ->setHidden(false);

    ui->verticalLayout->addWidget(mesg);

    command = new QCommand(this);
    command ->setHidden(false);
    /*命令行输入结束时的信号连接*/
    connect(command ,SIGNAL(finish()) ,this ,SLOT(commandFinish()));

    ui->horizontalLayout_3->addWidget(command);

    keyboard = new KeyBoard(this);
    keyboard ->setHidden(true);

}

void MainInterface::initHardware()
{
    QFile f(BEEP_FILE);
    if (f.exists())
    {
            beep_fb = open(BEEP_FILE, O_RDWR);
            if (beep_fb < 0)
            {
                    perror("open device leds fail");
            }
    }

    FM25V02_Init();
    printf("FM25V02_Init()!\n");

    EightBytes  rd_x;
    EightBytes  rd_y;
    EightBytes  rd_z;
    memset(rd_x.bytes , 0 ,sizeof rd_x);
    memset(rd_y.bytes , 0 ,sizeof rd_y);
    memset(rd_z.bytes , 0 ,sizeof rd_z);

    FM25V02_READ(X_AXIS_ADDR , rd_x.bytes ,sizeof rd_x);
    FM25V02_READ(Y_AXIS_ADDR , rd_y.bytes ,sizeof rd_y);
    FM25V02_READ(Z_AXIS_ADDR , rd_z.bytes ,sizeof rd_z);

    printf("FM25V02_READ()! X,Y,Z\n");

    spark_info->setLong(L_X_CURRENT , rd_x.longs);

    spark_info->setLong(L_Y_CURRENT , rd_y.longs);

    spark_info->setLong(L_Z_CURRENT , rd_z.longs);
}

void MainInterface::initFuncBar()
{
    toplevel *top ;
    seconlevel_f3 *sec_f3;
    seconlevel_f4 *sec_f4;
    seconlevel_f5 *sec_f5;
    seconlevel_f6 *sec_f6;

    top = new toplevel(this);
    top->setHidden(true);

    sec_f3 = new seconlevel_f3(this);
    sec_f3 ->setHidden(true);

    sec_f4 = new seconlevel_f4(this);
    sec_f4 ->setHidden(true);

    sec_f5 = new seconlevel_f5(this);
    sec_f5 ->setHidden(true);

    sec_f6 = new seconlevel_f6(this);
    sec_f6 ->setHidden(true);

    barmap.clear();
    barmap.insert(23 ,sec_f3);
    barmap.insert(24 ,sec_f4);
    barmap.insert(25 ,sec_f5);
    barmap.insert(26 ,sec_f6);
    barmap.insert(28 ,top);

    barui = barmap[28];

    barui->setHidden(false);
    ui->horizontalLayout->addWidget(barui);
    connect(this ,SIGNAL(barF0()) ,barui ,SLOT(F0()));
    emit barF0();
}

void MainInterface::keyPressEvent( QKeyEvent *k )
{
    int current;

    /*去除按键重复事件*/
    if(!k->isAutoRepeat()){
        k->accept();
        QPushButton *Fn ;

#ifdef ARM
        if (beep_fb > 0)
                ioctl(beep_fb, 1, 20);
#endif

        switch(k->key())
        {
        case Qt::Key_F1:
            Fn = barui->findChild<QPushButton *> ("pushButton_F1");
            if(Fn != NULL)
                Fn->click();
            break;
        case Qt::Key_F2:
            Fn = barui->findChild<QPushButton *> ("pushButton_F2");
            if(Fn != NULL)
                Fn->click();
            break;
        case Qt::Key_F3:
            Fn = barui->findChild<QPushButton *> ("pushButton_F3");
            if(Fn != NULL)
                Fn->click();
            break;
        case Qt::Key_F4:
            Fn = barui->findChild<QPushButton *> ("pushButton_F4");
            if(Fn != NULL)
                Fn->click();
            break;
        case Qt::Key_F5:
            Fn = barui->findChild<QPushButton *> ("pushButton_F5");
            if(Fn != NULL)
                Fn->click();
            break;
        case Qt::Key_F6:
            Fn = barui->findChild<QPushButton *> ("pushButton_F6");
            if(Fn != NULL)
                Fn->click();
            break;
        case Qt::Key_F7:
            /*Call the soft keyboard*/
/*
            if(keyboard->isHidden())
                keyboard->setHidden(false);
            else
                keyboard->setHidden(true);
*/

            Fn = barui->findChild<QPushButton *> ("pushButton_F7");
            if(Fn != NULL)
                Fn->click();
            break;
        case Qt::Key_F8:
            Fn = barui->findChild<QPushButton *> ("pushButton_F8");
            if(Fn != NULL)
                Fn->click();
            break;
        case Qt::Key_X:
            command ->setFocus();
            command ->setStatus(0x10);
            command ->setType(false ,true ,0x04 ,true ,0x03);
            command ->setTarget(L_SLOT ,L_X_CURRENT);

            break;
        case Qt::Key_Y:
            command ->setFocus();
            command ->setStatus(0x10);
            command ->setType(false ,true ,0x04 ,true ,0x03);
            command ->setTarget(L_SLOT ,L_Y_CURRENT);

            break;
        case Qt::Key_Z:
            command ->setFocus();
            command ->setStatus(0x10);
            command ->setType(false ,true ,0x04 ,true ,0x03);
            command ->setTarget(L_SLOT ,L_Z_CURRENT);

            break;
        /*放电开关，监听‘D’键*/
        case Qt::Key_D:
            spark_info->reverseBool(4);
            if(spark_info->b_array[4]&&!spark->isRunning())
                spark->start();
            else if(!spark_info->b_array[4]&&spark->isRunning())
                spark->terminate();
            break;
        /*这里的Enter键用来响应放电数据表的段选和删除行*/
        case Qt::Key_Enter:
            if(table_state == TABLE_EDIT)
                qDebug()<<"1";
            else if(table_state == TABLE_SELECT)
                qDebug()<<"2";
            else if(table_state == TABLE_DELETE){
                current = ui->tableView->currentIndex().row();
                model->removeRow(current);
                submitTable();
            }
            else
                qDebug()<<"0";
            break;
        default :
            break;
        }
    }
    else
        k->ignore();
}

void MainInterface::keyReleaseEvent(QKeyEvent *k)
{
    k->accept();

    if (beep_fb > 0)
            ioctl(beep_fb, 0, 1);

    switch(k->key())
    {
    case Qt::Key_F1:
        break;
    case Qt::Key_F2:
//        ui->pushButton_F2->setEnabled(true);
        break;
    case Qt::Key_F3:
//        ui->pushButton_F3->setEnabled(true);
        break;
    case Qt::Key_F4:
//        ui->pushButton_F4->setEnabled(true);
        break;
    case Qt::Key_F5:
//        ui->pushButton_F5->setEnabled(true);
        break;
    case Qt::Key_F6:
//        ui->pushButton_F6->setEnabled(true);
        break;
    case Qt::Key_F7:
//        ui->pushButton_F7->setEnabled(true);
        break;
    case Qt::Key_F8:
        break;
    default :
        break;
    }
}

void MainInterface::commandFinish()
{
    /*恢复输入之前的屏幕焦点*/
    tableStateUpdate(table_state);
}

void MainInterface::commandSwitch(char status ,char type ,char slot,char index)
{
    command ->setFocus();

    command ->setStatus(status);
    command ->setType(type);
    command ->setTarget(slot ,index);
}

void MainInterface::tableStateUpdate(char c)
{
    tableDataUpdate();

    int width = 54;
    ui->tableView->setColumnWidth(0,width+width);
    ui->tableView->setColumnWidth(1,width);
    ui->tableView->setColumnWidth(2,width);
    ui->tableView->setColumnWidth(3,width);
    ui->tableView->setColumnWidth(4,width);
    ui->tableView->setColumnWidth(5,width);
    ui->tableView->setColumnWidth(6,width);
    ui->tableView->setColumnWidth(7,width);
    ui->tableView->setColumnWidth(8,width);
    ui->tableView->setColumnWidth(9,width);
    ui->tableView->setColumnWidth(10,width);

    for(int i=0 ;i < TABLE_ROWS;i++)
        ui->tableView->setRowHeight(i ,20);

    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setFixedHeight(17);
    //ui->tableView->setEnabled(false);

    ui->tableView->verticalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->tableView->verticalHeader()->setFixedWidth(20);

    table_state = c;
    QModelIndex start;

    if(table_state == TABLE_SHOW){
        disconnect(model,0,this ,0);

        ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);
        ui->tableView->setFocusPolicy(Qt::NoFocus);
        ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
        setFocus();
        tableRollUpdate();
    }
    else if(table_state == TABLE_EDIT){
        ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectItems);
        ui->tableView->setFocusPolicy(Qt::StrongFocus);
        ui->tableView->setEditTriggers(QTableView::CurrentChanged);
        start = ui->tableView->model()->index(0 ,0);
        ui->tableView->setCurrentIndex(start);
        ui->tableView->setFocus();

        connect(model ,SIGNAL(dataChanged(QModelIndex,QModelIndex)) ,this ,SLOT(tableItemChange(QModelIndex,QModelIndex)));
    }
    else if(table_state == TABLE_SELECT){
        disconnect(model,0,this ,0);

        ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableView->setFocusPolicy(Qt::StrongFocus);
        ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
        ui->tableView->selectRow(0);
        ui->tableView->setFocus();
    }
    else if(table_state == TABLE_DELETE){
        disconnect(model,0,this ,0);

        ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableView->setFocusPolicy(Qt::StrongFocus);
        ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
        ui->tableView->selectRow(0);
        ui->tableView->setFocus();
    }
}

void MainInterface::tableDataUpdate()
{
    model->clear();

    /*设置表头*/
    model->setColumnCount(TABLE_COLUMN);
    model->setHeaderData(0,Qt::Horizontal,tr("设定深度"));
    model->setHeaderData(1,Qt::Horizontal,tr("电流"));
    model->setHeaderData(2,Qt::Horizontal,tr("脉宽"));
    model->setHeaderData(3,Qt::Horizontal,tr("休止"));
    model->setHeaderData(4,Qt::Horizontal,tr("间隙"));
    model->setHeaderData(5,Qt::Horizontal,tr("伺服"));
    model->setHeaderData(6,Qt::Horizontal,tr("排渣"));
    model->setHeaderData(7,Qt::Horizontal,tr("工时"));
    model->setHeaderData(8,Qt::Horizontal,tr("面积"));
    model->setHeaderData(9,Qt::Horizontal,tr("极性"));
    model->setHeaderData(10,Qt::Horizontal,tr("高压"));

    for(int i=0 ;i < TABLE_ROWS;i++)
    {
        if(spark_info->table.Index[i] > 0){
            QStandardItem* item1 = new QStandardItem();
            item1->setTextAlignment(Qt::AlignCenter);
            item1->setData(spark_info->table.Shendu[i] ,Qt::EditRole);
            QStandardItem* item2 = new QStandardItem(QString::number(spark_info->table.Dianliu[i]));
            item2->setTextAlignment(Qt::AlignCenter);
            QStandardItem* item3 = new QStandardItem(QString::number(spark_info->table.Maikuan[i]));
            item3->setTextAlignment(Qt::AlignCenter);
            QStandardItem* item4 = new QStandardItem(QString::number(spark_info->table.Xiuzhi[i]));
            item4->setTextAlignment(Qt::AlignCenter);
            QStandardItem* item5 = new QStandardItem(QString::number(spark_info->table.Jianxi[i]));
            item5->setTextAlignment(Qt::AlignCenter);
            QStandardItem* item6 = new QStandardItem(QString::number(spark_info->table.Sudu[i]));
            item6->setTextAlignment(Qt::AlignCenter);
            QStandardItem* item7 = new QStandardItem(QString::number(spark_info->table.Shenggao[i]));
            item7->setTextAlignment(Qt::AlignCenter);
            QStandardItem* item8 = new QStandardItem(QString::number(spark_info->table.Gongshi[i]));
            item8->setTextAlignment(Qt::AlignCenter);
            QStandardItem* item9 = new QStandardItem(QString::number(spark_info->table.Mianji[i]));
            item9->setTextAlignment(Qt::AlignCenter);
            QStandardItem* item10 = new QStandardItem(QString::number(spark_info->table.Jixin[i]));
            item10->setTextAlignment(Qt::AlignCenter);
            QStandardItem* item11 = new QStandardItem(QString::number(spark_info->table.Gaoya[i]));
            item11->setTextAlignment(Qt::AlignCenter);
            QList<QStandardItem*> item;
            item <<item1<<item2<<item3<<item4<<item5<<item6<<item7<<item8<<item9<<item10<<item11;
            model->appendRow(item);
        }
        else{
            continue;
        }
    }

    model->sort(0 ,Qt::AscendingOrder);

    ui->tableView->setModel(model);
}

void MainInterface::tableRollUpdate()
{
    unsigned int row = 0;
    unsigned int column = 0;
    for(row = 0;row < abs(model->rowCount());row++){
        QBrush brush;
        if(row == spark_info->uint_array[UINT_CURRENT_ROM]){
            brush = QBrush(QColor(0xFF ,0xCC ,0x00));
        }
        else if(row < spark_info->uint_array[UINT_START_ROW]){
            brush = QBrush(QColor(0xFF ,0xFF ,0xFF));
        }
        else if(row < spark_info->uint_array[UINT_CURRENT_ROM]){
            brush = QBrush(QColor(0x00 ,0xFF ,0x00));
        }
        else if(row < spark_info->uint_array[UINT_END_ROW]){
            brush = QBrush(QColor(0xFF ,0x00 ,0x00));
        }
        else{
            brush = QBrush(QColor(0xFF ,0xFF ,0xFF));
        }
        for(column = 0;column < 11;column ++){
            QStandardItem* item = model->item(row ,column);
            item->setBackground(brush);
        }
    }
}


/*数据发生改变时重新排序*/
void MainInterface::tableItemChange(QModelIndex tl ,QModelIndex br)
{
    if(tl.column() == 0){
        model->sort(tl.column() ,Qt::AscendingOrder);
        submitTable();
    }
    else
        br.column();
}

/*检查并提交数据表数据*/
void MainInterface::submitTable()
{
    int i = 0, j = 0;
    bool *ok =false;

    spark_info->tableClear();

    for(i = 0;i < model->rowCount();i++){
        j = 0;
        spark_info->table.Shendu[i] = model->item(i,j++)->text().toInt(ok ,10);
        spark_info->table.Dianliu[i] = model->item(i,j++)->text().toUInt(ok ,10);
        spark_info->table.Maikuan[i] = model->item(i,j++)->text().toUInt(ok ,10);
        spark_info->table.Xiuzhi[i] = model->item(i,j++)->text().toUInt(ok ,10);
        spark_info->table.Jianxi[i] = model->item(i,j++)->text().toUInt(ok ,10);
        spark_info->table.Sudu[i] = model->item(i,j++)->text().toUInt(ok ,10);
        spark_info->table.Shenggao[i] = model->item(i,j++)->text().toUInt(ok ,10);
        spark_info->table.Gongshi[i] = model->item(i,j++)->text().toUInt(ok ,10);
        spark_info->table.Mianji[i] = model->item(i,j++)->text().toUInt(ok ,10);
        spark_info->table.Jixin[i] = model->item(i,j++)->text().toUInt(ok ,10);
        spark_info->table.Gaoya[i] = model->item(i,j++)->text().toUInt(ok ,10);
        spark_info->table.Index[i] = i;
    }
}

void MainInterface::funcbarUpdate(int i)
{
    disconnect(barui ,SLOT(F0()));
    ui->horizontalLayout->removeWidget(barui);
    barui->setHidden(true);
    //delete barui;

    barui = barmap[i];
    barui ->setHidden(false);
    ui->horizontalLayout->addWidget(barui);
    connect(this ,SIGNAL(barF0()) ,barui ,SLOT(F0()));
    emit barF0();

}

void MainInterface::XPos_Update()
{
    QString s;

    if(spark_info->l_array[L_X_CURRENT] < 0)
        s.append('-');

    int m = 0;
    long tmp = 0;
    long g = 10000000;
    char ch = '0';
    bool dot = false;
    bool first = false;

    for(m = 0;m < 7;m++){
        if(m == 4){
            if(!first){
                s.append('0');
                s.append('.');
                first = true;
            }
            else
                s.append('.');
            dot = true;
        }
        tmp = spark_info->l_array[L_X_CURRENT] % g;
        g = g / 10;
        tmp = tmp / g;
        tmp = abs(tmp);
        ch = tmp & 0xFF;
        ch = ch + 48;

        if(ch != '0'){
            first =true;
        }

        if(first)
            s.append(ch);
    }

#ifdef ARM

        EightBytes wr_x;
        wr_x.longs = 0;
        wr_x.longs = spark_info->l_array[L_X_CURRENT];
        FM25V02_WRITE(X_AXIS_ADDR , wr_x.bytes, sizeof wr_x);

#endif

    ui->label_cux->setText(s);
}

void MainInterface::YPos_Update()
{
    QString s;

    if(spark_info->l_array[L_Y_CURRENT] < 0)
        s.append('-');

    int m = 0;
    long tmp = 0;
    long g = 10000000;
    char ch = '0';
    bool dot = false;
    bool first = false;

    for(m = 0;m < 7;m++){
        if(m == 4){
            if(!first){
                s.append('0');
                s.append('.');
                first = true;
            }
            else
                s.append('.');
            dot = true;
        }
        tmp = spark_info->l_array[L_Y_CURRENT] % g;
        g = g / 10;
        tmp = tmp / g;
        tmp = abs(tmp);
        ch = tmp & 0xFF;
        ch = ch + 48;

        if(ch != '0'){
            first =true;
        }

        if(first)
            s.append(ch);
    }

#ifdef ARM

        EightBytes wr_y;
        wr_y.longs = 0;
        wr_y.longs = spark_info->l_array[L_Y_CURRENT];
        FM25V02_WRITE(Y_AXIS_ADDR , wr_y.bytes, sizeof wr_y);

#endif

    ui->label_cuy->setText(s);
}

void MainInterface::ZPos_Update()
{
    QString s;

    if(spark_info->l_array[L_Z_CURRENT] < 0)
        s.append('-');

    int m = 0;
    long tmp = 0;
    long g = 10000000;
    char ch = '0';
    bool dot = false;
    bool first = false;

    for(m = 0;m < 7;m++){
        if(m == 4){
            if(!first){
                s.append('0');
                s.append('.');
                first = true;
            }
            else
                s.append('.');
            dot = true;
        }
        tmp = spark_info->l_array[L_Z_CURRENT] % g;
        g = g / 10;
        tmp = tmp / g;
        tmp = abs(tmp);
        ch = tmp & 0xFF;
        ch = ch + 48;

        if(ch != '0'){
            first =true;
        }

        if(first)
            s.append(ch);
    }

#ifdef ARM

        EightBytes wr_z;
        wr_z.longs = 0;
        wr_z.longs = spark_info->l_array[L_Z_CURRENT];
        FM25V02_WRITE(Z_AXIS_ADDR , wr_z.bytes, sizeof wr_z);

#endif

    ui->label_cuz->setText(s);
}

MainInterface::~MainInterface()
{
    delete ui;
}
