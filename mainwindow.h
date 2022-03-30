#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <BaseScope.h>
#include <QPointF>
#include <QtCore/QtMath>
#include <QtCore/QRandomGenerator>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QPointF points[2];
    BaseScope scope;
};
#endif // MAINWINDOW_H
