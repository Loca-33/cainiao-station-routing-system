#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QPen>
#include<vector>
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("菜鸟驿站送货小车路径规划系统");
    updateMapDisplay();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loadMapButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择地图文件", "", "文本文件 (*.txt)");
    if (fileName.isEmpty()) return;

    if (FileIO::loadMapData(fileName.toStdString(), map)) {
        mapLoaded = true;
        QMessageBox::information(this, "成功", "地图加载成功!");
        updateMapDisplay();
    } else {
        QMessageBox::warning(this, "错误", "地图加载失败!");
    }
}

void MainWindow::on_task3Button_clicked()
{
    if (!mapLoaded) {
        QMessageBox::warning(this, "错误", "请先加载地图数据!");
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(this, "选择任务三输入文件", "", "文本文件 (*.txt)");
    if (fileName.isEmpty()) return;

    vector<Vehicle> vehicles;
    vector<Package> packages;
    if (!FileIO::loadTask3Data(fileName.toStdString(), vehicles, packages)) {
        QMessageBox::warning(this, "错误", "加载任务三数据失败!");
        return;
    }

    vector<vector<int>> routes;
    vector<int> latePackages;
    int totalCost = Algorithm::deliveryPlan(map, vehicles, packages, routes, latePackages);

    QString result = QString("总成本: %1\n逾期包裹: ").arg(totalCost);
    for (int id : latePackages) {
        result += QString::number(id) + " ";
    }

    result += "\n\n配送路线:\n";
    for (const auto &route : routes) {
        for (int loc : route) {
            result += QString::number(loc) + " -> ";
        }
        result += "\n";
    }

    showResult("任务三结果", result);
}

void MainWindow::on_task4Button_clicked()
{
    if (!mapLoaded) {
        QMessageBox::warning(this, "错误", "请先加载地图数据!");
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(this, "选择任务四输入文件", "", "文本文件 (*.txt)");
    if (fileName.isEmpty()) return;

    vector<Vehicle> vehicles;
    vector<string> returnLocations;
    if (!FileIO::loadTask4Data(fileName.toStdString(), vehicles, returnLocations)) {
        QMessageBox::warning(this, "错误", "加载任务四数据失败!");
        return;
    }

    if (vehicles.empty()) {
        QMessageBox::warning(this, "错误", "没有可用小车!");
        return;
    }

    vector<int> route;
    int totalDistance = Algorithm::returnPlan(map, vehicles[0], returnLocations, route);

    QString result = QString("总距离: %1\n路径: ").arg(totalDistance);
    for (size_t i = 0; i < route.size(); ++i) {
        if (i != 0) result += " -> ";
        result += QString::fromStdString(map.getLocationName(route[i]));
    }

    showResult("任务四结果", result);
}

void MainWindow::on_task5Button_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择任务五输入文件", "", "文本文件 (*.txt)");
    if (fileName.isEmpty()) return;

    Map task5Map;
    vector<pair<int, int>> queries;
    if (!FileIO::loadTask5Data(fileName.toStdString(), task5Map, queries)) {
        QMessageBox::warning(this, "错误", "加载任务五数据失败!");
        return;
    }

    Algorithm::prepareMinMaxEdge(task5Map);
    QString result = "任务五结果:\n";
    for (const auto& [x, y] : queries) {
        result += QString("从 %1 到 %2 的最少充电距离: %3\n").arg(x).arg(y).arg(task5Map.getMinMaxEdge(x, y));
    }

    showResult("任务五结果", result);
}

void MainWindow::on_extTask1Button_clicked()
{
    if (!mapLoaded) {
        QMessageBox::warning(this, "错误", "请先加载地图数据!");
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(this, "选择扩展任务一输入文件", "", "文本文件 (*.txt)");
    if (fileName.isEmpty()) return;

    vector<Vehicle> vehicles;
    vector<Package> packages;
    if (!FileIO::loadExtensionTask1Data(fileName.toStdString(), vehicles, packages)) {
        QMessageBox::warning(this, "错误", "加载扩展任务一数据失败!");
        return;
    }

    if (vehicles.size() < 2) {
        QMessageBox::warning(this, "错误", "需要至少两辆小车!");
        return;
    }

    vector<vector<vector<int>>> routes;
    vector<int> latePackages;
    int totalCost = Algorithm::dualVehicleDelivery(map, vehicles, packages, routes, latePackages);

    QString result = QString("总成本: %1\n逾期包裹: ").arg(totalCost);
    for (int id : latePackages) {
        result += QString::number(id) + " ";
    }

    result += "\n\n两车配送路线:\n";
    for (size_t i = 0; i < routes.size(); ++i) {
        result += QString("小车 %1:\n").arg(i + 1);
        for (const auto &route : routes[i]) {
            for (int loc : route) {
                result += QString::number(loc) + " -> ";
            }
            result += "\n";
        }
    }

    showResult("扩展任务一结果", result);
}

void MainWindow::on_extTask2Button_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择扩展任务二输入文件", "", "文本文件 (*.txt)");
    if (fileName.isEmpty()) return;

    int n, K;
    long long X;
    vector<long long> arrivalTimes;
    if (!FileIO::loadExtensionTask2Data(fileName.toStdString(), n, K, X, arrivalTimes)) {
        QMessageBox::warning(this, "错误", "加载扩展任务二数据失败!");
        return;
    }

    long long totalDissatisfaction = Algorithm::scheduleDeliveries(n, K, X, arrivalTimes);
    showResult("扩展任务二结果", QString("总不满意度: %1").arg(totalDissatisfaction));
}

void MainWindow::showResult(const QString &title, const QString &content)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(title);
    msgBox.setText(content);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void MainWindow::updateMapDisplay()
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    ui->mapGraphicsView->setScene(scene);

    if (!mapLoaded) {
        scene->addText("请先加载地图数据");
        return;
    }

    // 简单的圆形布局
    const int centerX = 300;
    const int centerY = 200;
    const int radius = 150;
    const int nodeRadius = 20;

    // 绘制地点节点
    QMap<int, QPointF> nodePositions;
    for (int i = 1; i <= map.getNumLocations(); ++i) {
        double angle = 2 * M_PI * i / map.getNumLocations();
        int x = centerX + radius * cos(angle);
        int y = centerY + radius * sin(angle);
        nodePositions[i] = QPointF(x, y);

        QGraphicsEllipseItem *node = scene->addEllipse(x - nodeRadius, y - nodeRadius,
                                                       nodeRadius * 2, nodeRadius * 2);
        node->setBrush(Qt::lightGray);

        QGraphicsTextItem *text = scene->addText(QString::number(i));
        text->setPos(x - nodeRadius/2, y - nodeRadius/2);
    }

    // 绘制道路
    for (int u = 1; u <= map.getNumLocations(); ++u) {
        const auto &adj = map.getAdjacent(u);
        for (const auto &[v, w] : adj) {
            if (u < v) { // 避免重复绘制双向边
                QPointF p1 = nodePositions[u];
                QPointF p2 = nodePositions[v];
                QGraphicsLineItem *line = scene->addLine(p1.x(), p1.y(), p2.x(), p2.y());
                line->setPen(QPen(Qt::blue, 2));

                // 显示权重
                QPointF mid = (p1 + p2) / 2;
                QGraphicsTextItem *weightText = scene->addText(QString::number(w));
                weightText->setPos(mid);
                weightText->setDefaultTextColor(Qt::darkGreen);
            }
        }
    }

    // 标记驿站(假设1号地点是驿站)
    if (map.getNumLocations() >= 1) {
        QPointF stationPos = nodePositions[1];
        QGraphicsEllipseItem *station = scene->addEllipse(
            stationPos.x() - nodeRadius, stationPos.y() - nodeRadius,
            nodeRadius * 2, nodeRadius * 2);
        station->setBrush(Qt::red);
    }
}
