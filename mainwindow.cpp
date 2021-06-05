#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QDebug>
#include <QKeyEvent>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    game_status = 0;

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
    if(bgm_pos ==32){
        bgm_pos = 0;
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
}


void MainWindow::KeyPressEvent(QKeyEvent *event){
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
