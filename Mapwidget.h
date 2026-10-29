#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include "Map.h"

class MapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MapWidget(QWidget *parent = nullptr);
    void setMap(const Map &map);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Map m_map;
    bool m_hasMap = false;
};

#endif // MAPWIDGET_H
