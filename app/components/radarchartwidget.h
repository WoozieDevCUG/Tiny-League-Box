#ifndef RADARCHARTWIDGET_H
#define RADARCHARTWIDGET_H

#include <QWidget>
#include <array>

class RadarChartWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RadarChartWidget(QWidget *parent = nullptr);

    // 值范围 0~5（允许为空表示无数据）
    void setValues(std::array<int,5> values); // 顺序：伤害、韧性、控制、机动、功能
    void setLabels(const QStringList &labels); // 5 个标签

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    std::array<int,5> m_values; // 0~5
    QStringList m_labels;       // 中文标签
};

#endif // RADARCHARTWIDGET_H


