#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include "Map.h"
#include "Algorithm.h"
#include "FileIO.h"

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
    void on_loadMapButton_clicked();
    void on_task3Button_clicked();
    void on_task4Button_clicked();
    void on_task5Button_clicked();
    void on_extTask1Button_clicked();
    void on_extTask2Button_clicked();

private:
    Ui::MainWindow *ui;
    Map map;
    bool mapLoaded = false;

    void showResult(const QString &title, const QString &content);
    void updateMapDisplay();
};
#endif // MAINWINDOW_H
