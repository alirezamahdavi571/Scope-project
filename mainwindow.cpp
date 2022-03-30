#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scope.show();



    //points[1] = QPointF(0,5);
    //
    //ui->scopeWidget->setBoundRect(QRectF(QPointF(0,10)  , QPointF(10,-10)));
    //ui->scopeWidget->setPoints(points,0,1);
}
//----------------------------------------------------------------------------------------

MainWindow::~MainWindow()
{
    delete ui;
}




void MainWindow::on_pushButton_clicked()
{
     QPointF *points = new  QPointF[1000];
     scope.setBoundRect(QRectF(QPointF(0,1000)  , QPointF(1000,-1000)));

     for (int i = 0; i < 1000; i++) {
         //QPointF p((qreal) i, qSin(M_PI / 100 * i) * 100);
         //p.ry() += QRandomGenerator::global()->bounded(20);
         points[i].setX((qreal) i);
         points[i].setY(qSin(M_PI / 100 * i) * 100);

     }



     scope.setPoints(points,0,1000);

}
