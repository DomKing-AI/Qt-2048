#include "widget.h"
#include "ui_widget.h"
#include <fstream>
#include <QMessageBox>
#include<QThread>
using namespace std;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    for(int row=0; row<4; row++)
    {
        for(int col=0; col<4; col++)
        {
            labs[row][col] = new Lable(row,col,this);
            /* 设置动画目标 */
            animation[row][col].setTargetObject(labs[row][col]);
            /* 设置窗口几何属性(位置，大小)作为动画参考 */
            animation[row][col].setPropertyName("geometry");
            /* 设置循环次数：-1为无限次 */
            animation[row][col].setLoopCount(1);
            /* 设置动画持续时长 */
            animation[row][col].setDuration(100);
        }
    }


    load_best();
    load_labs();
}

Widget::~Widget()
{
    if(!is_exit())
        save_labs();

    save_best();
    delete ui;
}

void Widget::on_pushButton_new_game_clicked()
{
    score = 0;
    for(int row=0; row<4; row++)
    {
        for(int col=0; col<4; col++)
        {
            arr[row][col] = 0;
        }
    }

    is_rand = true;
    rand_num();
    rand_num();
    updata_labs();
}

void Widget::mouseDoubleClickEvent(QMouseEvent* event)
{
    int row = (event->y()-250) / 110;
    int col = event->x() / 110;

    int not_null_cnt = 0;
    for(int row=0; row<4; row++)
    {
        for(int col=0; col<4; col++)
        {
            if(arr[row][col])
                not_null_cnt++;
        }
    }


    if(not_null_cnt >2 )
    {
        arr[row][col] = 0;
        labs[row][col]->setValue(0);
    }

    qDebug("%d %d",row,col);
}

void Widget::keyReleaseEvent(QKeyEvent* event)
{
    is_rand = false;
    switch (event->key())
    {
    case Qt::Key_Up:
        up_move();
        break;
    case Qt::Key_Down:
        down_move();
        break;
    case Qt::Key_Left:
        left_move();
        break;
    case Qt::Key_Right:
        right_move();
        break;
    default:
        break;
    }

    rand_num();
    updata_labs();
    if(is_exit())
    {
        QMessageBox msgBox(this);
        msgBox.setText("游戏无法继续！");
        msgBox.setInformativeText("是否要重开一局?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);

        if(QMessageBox::Yes == msgBox.exec())
            on_pushButton_new_game_clicked();
        else
            close();
    }
    //QThread::sleep(0.8);
}

void Widget::updata_labs(void)
{
    for(int row=0; row<4; row++)
    {
        for(int col=0; col<4; col++)
        {
            labs[row][col]->setValue(arr[row][col]);
        }
    }

    ui->label_score->setText(QString::number(score));
    if(score > best)
    {
        best = score;
        ui->label_best->setText(QString::number(best));
    }
}

void Widget::rand_num(void)
{
    while(is_rand)
    {
        // 随机产生一个行下标
        int r = rand() % 4;
        // 随机产生一个列下标
        int c = rand() % 4;

        if(0 == arr[r][c])
        {

            //int x=labs[r][c]->x(),y=labs[r][c]->y();
            int x = 10+c*110, y = 260+r*110;
            //int w=labs[r][c]->width(),h=labs[r][c]->height();

            /* 设置动画开始坐标和大小(QRect) */
            animation[r][c].setStartValue(QRect(x+25, y+25, 50, 50));
               /* 设置动画结束坐标和大小(QRect) */
            animation[r][c].setEndValue(QRect(x,y,100,100));
            /* 开始动画 */
            animation[r][c].start();
            int odds = rand() % 100;
            arr[r][c] = odds < 80 ? 2 : 4;
            break;
        }
    }
}

void Widget::up_move(void)
{
    // 按列移动处理数字
    for(int c=0; c<4; c++)
    {
        // 合并的边
        int merge_end = -1;
        // 从第二行开始把数字向上移动
        for(int r=1; r<4; r++)
        {
            // 数字零不需要移
            if(0 == arr[r][c])
                continue;

            // 寻找arr[r][c]数字向上移动最终位
            int end = -1;
            for(int m=r-1; m>merge_end && (!arr[m][c]||arr[r][c]==arr[m][c]); m--)
                end = m;

            // 判断是否找到最终的移动位置
            if(-1 != end)
            {
                // 判断是否合并数字,计算得分
                if(arr[end][c])
                {
                    score += arr[r][c]*2;
                    merge_end = end;
                }

                // 移动或合并数
                arr[end][c] += arr[r][c];
                // 原位置清
                arr[r][c] = 0;
                // 随机标志设置为真
                is_rand = true;
            }
        }
    }
}

void Widget::down_move(void)
{
    for(int c=0; c<4; c++)
    {
        int merge_end = 4;
        for(int r=2; r>=0; r--)
        {
            if(0 == arr[r][c])
                continue;

            int end = -1;
            for(int m=r+1; m<merge_end && (!arr[m][c]||arr[r][c]==arr[m][c]); m++)
                end = m;

            if(-1 != end)
            {
                if(arr[end][c])
                {
                    score += arr[r][c]*2;
                    merge_end = end;
                }

                arr[end][c] += arr[r][c];
                arr[r][c] = 0;
                is_rand = true;
            }
        }
    }
}

void Widget::right_move(void)
{
    for(int r=0; r<4; r++)
    {
        int merge_end = 4;
        for(int c=2; c>=0; c--)
        {
            if(0 == arr[r][c])
                continue;

            int end = -1;
            for(int m=c+1; m<merge_end&&(!arr[r][m]||arr[r][c]==arr[r][m]); m++)
                end = m;

            if(-1 != end)
            {
                if(arr[r][end])
                {
                    score += arr[r][c]*2;
                    merge_end = end;
                }

                arr[r][end] += arr[r][c];
                arr[r][c] = 0;
                is_rand = true;
            }
        }
    }
}

void Widget::left_move(void)
{
    for(int r=0; r<4; r++)
    {
        int merge_end = -1;
        for(int c=1; c<4; c++)
        {
            if(0 == arr[r][c])
                continue;

            int end = -1;
            for(int m=c-1; m>merge_end && (!arr[r][m]||arr[r][m]==arr[r][c]); m--)
                end = m;

            if(-1 != end)
            {
                if(arr[r][end])
                {
                    score += arr[r][c]*2;
                    merge_end = end;
                }

                arr[r][end] += arr[r][c];
                arr[r][c] = 0;
                is_rand = true;
            }
        }
    }
}

bool Widget::is_exit(void)
{
    for(int r=0; r<4; r++)
    {
        for(int c=0; c<4; c++)
        {
            if(0 == arr[r][c])
                return false;
            if(c<3 && arr[r][c] == arr[r][c+1])
                return false;
            if(r<3 && arr[r][c] == arr[r+1][c])
                return false;
        }
    }

    return true;
}

void Widget::load_best(void)
{
    ifstream ifs("best.dat");
    if(ifs)
        ifs.read((char*)&best,sizeof(best));
    else
        best = 0;
    ui->label_best->setText(QString::number(best));
}

void Widget::save_best(void)
{
    ofstream ofs("best.dat");
    if(!ofs)
    {
        QMessageBox msgBox(this);
        msgBox.setText("保存数据失败，请检查当前用户权限！");
        msgBox.setStandardButtons(QMessageBox::Ok );
        msgBox.exec();
        return;
    }

    ofs.write((char*)&best,sizeof(best));
}

void Widget::load_labs(void)
{
    ifstream ifs("2048.dat");
    if(!ifs)
    {
        on_pushButton_new_game_clicked();
        return;
    }

    ifs.read((char*)&arr,sizeof(arr));
    ifs.read((char*)&score,sizeof(score));
    ifs.close();

    updata_labs();

    remove("2048.dat");
}

void Widget::save_labs(void)
{
    ofstream ofs("2048.dat");
    if(!ofs)
    {
        QMessageBox msgBox(this);
        msgBox.setText("保存数据失败，请检查当前用户权限！");
        msgBox.setStandardButtons(QMessageBox::Ok );
        msgBox.exec();
        return;
    }

    ofs.write((char*)arr,sizeof(arr));
    ofs.write((char*)&score,sizeof(score));
}

void Widget::on_pushButton_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("游戏操作手册：\n\n·使用方向键控制方块的移动方向：上、下、左、右。\n\n·当两个相邻且相同的数字方块相遇时，它们会合并成一个更大的数字方块。\n\n·当双击鼠标左/右键，可删除方块。\n\n·当游戏面板中没有空白方块，且无法通过移动方块来合并数字方块时，游戏结束。");
    msgBox.exec();
}
