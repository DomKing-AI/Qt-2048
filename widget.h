#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QKeyEvent>
#include <QMouseEvent>
#include<QPropertyAnimation>

class Lable : public QLabel
{
public:
    Lable(int row,int col,QWidget *parent):QLabel(parent)
    {
        move(10+col*110,260+row*110);
        resize(100,100);

        setAlignment(Qt::AlignCenter);
        setFont(QFont("Arial",25));
        setValue(0);
    }
    void setValue(int value)
    {
        QString style = "border-radius:3px;";
        switch(value)
        {
        case 0:
            style += "background-color: rgb(238, 228, 218);";
            break;
        case 2:
        case 4:
        case 8:
            style += "color: rgb(119, 110, 109);";
            style += "background-color: rgb(238, 228, 110);";
            break;
        case 16:
        case 32:
        case 64:
            style += "color: rgb(249, 246, 242);";
            style += "background-color: rgb(245, 149, 99);";
            break;
        case 128:
        case 256:
        case 512:
            style += "color: rgb(249, 246, 242);";
            style += "background-color: rgb(243, 207, 114);";
            break;
        case 1024:
        case 2048:
        case 4096:
        case 8192:
            style += "color: rgb(249, 246, 242);";
            style += "background-color: rgb(243, 200, 80);";
            break;
        default:
            style += "color: rgb(255, 255, 255);";
            style += "background-color: rgb(0, 0, 0);";
            break;
        }

        this->setStyleSheet(style);
        if(value)
            setText(QString::number(value));
        else
            setText("");
    }
};

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void keyReleaseEvent(QKeyEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void updata_labs(void);
    void rand_num(void);
    void up_move(void);
    void down_move(void);
    void left_move(void);
    void right_move(void);
    bool is_exit(void);

    void load_best(void);
    void save_best(void);

    void load_labs(void);
    void save_labs(void);


private slots:
    void on_pushButton_new_game_clicked();

    void on_pushButton_clicked();

private:
    Ui::Widget *ui;
    Lable* labs[4][4];
    int arr[4][4];
    int score;
    int best;
    bool is_rand;
    QPropertyAnimation animation[4][4];
};

#endif // WIDGET_H
