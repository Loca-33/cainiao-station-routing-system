#include "MapWidget.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <cmath>

MapWidget::MapWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(600, 400);
}

void MapWidget::setMap(const Map &map)
{
    m_map = map;
    m_hasMap = true;
    update();
}

void MapWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (!m_hasMap) {
        painter.drawText(rect(), Qt::AlignCenter, "请先加载地图数据");
        return;
    }

    // 设置绘图区域
    QRect drawingRect = rect().adjusted(20, 20, -20, -20);
    const int nodeRadius = 15;

    // 简单的圆形布局
    const int centerX = drawingRect.center().x();
    const int centerY = drawingRect.center().y();
    const int radius = qMin(drawingRect.width(), drawingRect.height()) / 2 - nodeRadius;

    // 计算节点位置
    QMap<int, QPoint> nodePositions;
    for (int i = 1; i <= m_map.getNumLocations(); ++i) {
        double angle = 2 * M_PI * i / m_map.getNumLocations();
        int x = centerX + radius * cos(angle);
        int y = centerY + radius * sin(angle);
        nodePositions[i] = QPoint(x, y);
    }

    // 绘制道路
    painter.setPen(QPen(Qt::blue, 2));
    for (int u = 1; u <= m_map.getNumLocations(); ++u) {
        const auto &adj = m_map.getAdjacent(u);
        for (const auto &[v, w] : adj) {
            if (u < v) { // 避免重复绘制双向边
                QPoint p1 = nodePositions[u];
                QPoint p2 = nodePositions[v];
                painter.drawLine(p1, p2);

                // 显示权重
                QPoint mid = (p1 + p2) / 2;
                painter.drawText(mid, QString::number(w));
            }
        }
    }

    // 绘制节点
    painter.setPen(QPen(Qt::black, 1));
    for (int i = 1; i <= m_map.getNumLocations(); ++i) {
        QPoint center = nodePositions[i];
        QRect nodeRect(center.x() - nodeRadius, center.y() - nodeRadius,
                       nodeRadius * 2, nodeRadius * 2);

        // 驿站用红色标记
        if (i == 1) {
            painter.setBrush(Qt::red);
        } else {
            painter.setBrush(Qt::lightGray);
        }

        painter.drawEllipse(nodeRect);
        painter.drawText(nodeRect, Qt::AlignCenter, QString::number(i));
    }
}
