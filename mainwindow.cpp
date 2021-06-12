#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QDebug>
#include <QKeyEvent>
#define BLOCK01_X_POS 300
#define BLOCK01_Y_POS 40
#define BLOCK02_X_POS 2300
#define BLOCK02_Y_POS 110
#define BLOCK03_X_POS 900
#define BLOCK03_Y_POS 190
#define BLOCK04_X_POS 2800
#define BLOCK04_Y_POS 260
#define BLOCK05_X_POS 1700
#define BLOCK05_Y_POS 310
#define BLOCK06_X_POS 1500
#define BLOCK06_Y_POS 340

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    game_status = 0;

    //隱藏障礙物(初始畫面)：移出畫布就好
    ui->block01->setGeometry(QRect(-100, 0, 64, 64));
    ui->block02->setGeometry(QRect(-100, 0, 64, 64));
    ui->block03->setGeometry(QRect(-100, 0, 64, 64));
    ui->block04->setGeometry(QRect(-100, 0, 64, 64));
    ui->block05->setGeometry(QRect(-100, 0, 64, 64));
    ui->block06->setGeometry(QRect(-100, 0, 64, 64));


    //移動物體(讓背景往後移動)
    bgm_pos = 0;
    object_timer = new QTimer(this); //建立計時器
    connect(object_timer, SIGNAL(timeout()), this, SLOT(update_object()));  //訊號曹
    object_timer->start(10);    //0.01s

    //移動時間
    time = 30;
    clock_timer = new QTimer(this); //建立計時器
    connect(clock_timer, SIGNAL(timeout()), this, SLOT(update_time()));  //訊號曹
    //clock_timer->start(1000);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_object(){
    //qDebug() << "timeUp!";

    bgm_pos -= 1;   //背景每次往左移動

    ui->background->setGeometry(QRect(bgm_pos, 0, 4800, 512));

    //讓背景無限捲動
    if(bgm_pos == -32){
        bgm_pos = 0;
    }

    if(game_status ==1){
        //playing
        car_distance += 1;  //增加移動距離
        ui->label->setText("行駛距離: " + QString::number(car_distance, 'f', 0) + "公尺");   //顯示移動距離
        move_car();         //呼叫 move_car()移動車子位置
        move_blocks();      //呼叫 moce_blocks()移動障礙物
        detect_bolcks();    //呼叫 detect_bolcks() 偵測障礙物
    }
}

void MainWindow::update_time(){
    time -= 1;
    ui->lcd_clock->display(time);

    if(time == 0){
        game_status = 3;
        game_stop();
    }
}

void MainWindow::game_stop(){
    clock_timer->stop();
    object_timer->stop();

    //停止狀態判斷34
    if(game_status == 3){
        //時間到了
        ui->label_title->setPixmap(QPixmap(":/src/game_timeout.png"));
    }else if(game_status == 4){
        ui->label_title->setPixmap(QPixmap(":/src/game_falied.png"));
    }

    //重新顯示被隱藏起來的東西
    ui->label_title->setVisible(true);
    ui->button_start_game->setVisible(true);

    ui->actionstart_game->setText("開始遊戲");  //設定選單[開始遊戲]按鈕文字
}

void MainWindow::on_button_start_game_clicked()
{
    game_start();
}

void MainWindow::game_start(){
    game_status = 1;                            //設為playing狀態
    ui->label_title->setVisible(false);         //隱藏遊戲標題
    ui->button_start_game->setVisible(false);   //隱藏按鈕

    clock_timer->start(1000);                   //每1s更新一次
    time = 30;                                  //重設遊戲時間
    ui->lcd_clock->display(time);               //顯示遊戲時間

    car_pos = 220;                              //重設車子垂直位置(設於中央)
    car_distance = 0;                           //重設移動距離
    car_direction = 0;                          //初始方向

    if(!object_timer->isActive()){
        //若物體移動計時器未被啟用, 則將它啟用
        object_timer->start(10);
    }
    ui->actionstart_game->setText("暫停遊戲");  //設定選單[暫停遊戲]按鈕文字
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    switch (event->key()) {
        case Qt::Key_Up:
            car_direction = -1;
            qDebug() << "up";
            break;
        case Qt::Key_Down:
            car_direction = 1;
            qDebug() << "down";
            break;
    }
}

void MainWindow::move_car(){
    int car_new_pos = car_pos + car_direction;
    if((car_new_pos) >=30 && (car_new_pos) <= 410){ //設定車子邊界
        car_pos = car_new_pos;

        ui->car->setGeometry(QRect(10, car_pos, 237, 71));  //設定畫面中車子位置
    }else{
        //超過邊緣

        game_status = 4;
        game_stop();
    }
}

void MainWindow::move_blocks(){
    ui->block01->setGeometry(QRect(BLOCK01_X_POS - car_distance, BLOCK01_Y_POS, 64, 64));
    ui->block02->setGeometry(QRect(BLOCK02_X_POS - car_distance, BLOCK02_Y_POS, 64, 64));
    ui->block03->setGeometry(QRect(BLOCK03_X_POS - car_distance, BLOCK03_Y_POS, 64, 64));
    ui->block04->setGeometry(QRect(BLOCK04_X_POS - car_distance, BLOCK04_Y_POS, 64, 64));
    ui->block05->setGeometry(QRect(BLOCK05_X_POS - car_distance, BLOCK05_Y_POS, 64, 64));
    ui->block06->setGeometry(QRect(BLOCK06_X_POS - car_distance, BLOCK06_Y_POS, 64, 64));
}

void MainWindow::detect_bolcks(){
    int tmp_padding_x, tmp_padding_y;   //障礙物與車子距離

    tmp_padding_x = BLOCK01_X_POS - car_distance;
    tmp_padding_y = BLOCK01_Y_POS - ui->car->y();
    if(is_colliion(tmp_padding_x, tmp_padding_y)){
        return ;
    }

    tmp_padding_x = BLOCK02_X_POS - car_distance;
    tmp_padding_y = BLOCK02_Y_POS - ui->car->y();
    if(is_colliion(tmp_padding_x, tmp_padding_y)){
        return ;
    }

    tmp_padding_x = BLOCK03_X_POS - car_distance;
    tmp_padding_y = BLOCK03_Y_POS - ui->car->y();
    if(is_colliion(tmp_padding_x, tmp_padding_y)){
        return ;
    }

    tmp_padding_x = BLOCK03_X_POS - car_distance;
    tmp_padding_y = BLOCK03_Y_POS - ui->car->y();
    if(is_colliion(tmp_padding_x, tmp_padding_y)){
        return ;
    }

    tmp_padding_x = BLOCK04_X_POS - car_distance;
    tmp_padding_y = BLOCK04_Y_POS - ui->car->y();
    if(is_colliion(tmp_padding_x, tmp_padding_y)){
        return ;
    }

    tmp_padding_x = BLOCK05_X_POS - car_distance;
    tmp_padding_y = BLOCK05_Y_POS - ui->car->y();
    if(is_colliion(tmp_padding_x, tmp_padding_y)){
        return ;
    }

    tmp_padding_x = BLOCK06_X_POS - car_distance;
    tmp_padding_y = BLOCK06_Y_POS - ui->car->y();
    if(is_colliion(tmp_padding_x, tmp_padding_y)){
        return ;
    }
}

bool MainWindow::is_colliion(int x, int y){
    if((x < 247) && (x > -50)){
        if((y < 55) && (y > -55)){
            game_status = 4;
            game_stop();
            return true;
        }
    }
    return false;
}

void MainWindow::on_actionstart_game_triggered()
{
    switch (game_status){
        case 0: //init
        case 3: //time out
        case 4: //die
            game_start();
            break;
        case 1: //playing
        case 2: //pause
            game_pause();
            break;
    }
}


void MainWindow::on_actionclose_game_triggered()
{
    close();
}


void MainWindow::game_pause(){
    if(game_status == 1){
        //playing變為pause
        game_status = 2; //設為pause 狀態
        clock_timer->stop(); //停止遊戲計時器
        object_timer->stop();//停止物體移動計時器

        ui->label_title->setPixmap(QPixmap(":/src/game_pause.png"));    //遊戲暫停圖片
        ui->label_title->setVisible(true);  //顯示遊戲標題
        ui->actionstart_game->setText("開始遊戲");  //設定選單[開始遊戲]按鈕文字
    }else if(game_status == 2){
        //pause變為playing
        game_status = 1;    //設為playing 狀態
        clock_timer->start(1000);//啟動遊戲計時器
        object_timer->start(10);//啟動物體移動計時器

        ui->label_title->setVisible(false);
        ui->actionstart_game->setText("暫停遊戲");  //設定選單[暫停遊戲]按鈕文字
    }
}
