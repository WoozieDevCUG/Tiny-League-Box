#include "radarchartwidget.h"

#include <QPainter>
#include <QtMath>

RadarChartWidget::RadarChartWidget(QWidget *parent)
    : QWidget(parent)
    , m_values{0,0,0,0,0}
{
    // 更紧凑尺寸
    setMinimumSize(240, 240);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    if (m_labels.isEmpty()) {
        m_labels << QStringLiteral("伤害")
                 << QStringLiteral("韧性")
                 << QStringLiteral("控制")
                 << QStringLiteral("机动")
                 << QStringLiteral("功能");
    }
}

void RadarChartWidget::setValues(std::array<int,5> values)
{
    m_values = values;
    update();
}

void RadarChartWidget::setLabels(const QStringList &labels)
{
    if (labels.size() == 5) {
        m_labels = labels;
        update();
    }
}

void RadarChartWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QRectF rect = this->rect().adjusted(8, 8, -8, -8);
    // 将圆心左移，使整个雷达左对齐
    QPointF center(rect.left() + qMin(rect.width(), rect.height())/2.2, rect.center().y());
    qreal radius = qMin(rect.width(), rect.height()) / 2.2;

    // 基础参数
    const int N = 5;
    const int levels = 5; // 0~5 的 5 层网格
    const qreal startAngleRad = -M_PI / 2; // 从上方开始

    // 青蓝偏白配色
    const QColor gridDark(190, 240, 255);
    const QColor gridMid(150, 220, 240);
    const QColor ringFillA(120, 210, 240, 40);
    const QColor ringFillB(100, 190, 230, 28);
    const QColor polyEdge(60, 200, 255);
    const QColor polyFill(18, 139, 197, 150);
    const QColor labelColor(225, 240, 250);

    // 预计算外圈顶点
    QVector<QPointF> axisPoints;
    axisPoints.reserve(N);
    for (int i = 0; i < N; ++i) {
        qreal angle = startAngleRad + i * 2 * M_PI / N;
        axisPoints << QPointF(center.x() + radius * qCos(angle), center.y() + radius * qSin(angle));
    }

    // 环形交替填充（由内向外画，产生清晰的层次）
    for (int lv = 1; lv <= levels; ++lv) {
        qreal f = static_cast<qreal>(lv) / levels; // 半径比例
        qreal r = radius * f;
        p.setPen(Qt::NoPen);
        p.setBrush((lv % 2 == 0) ? ringFillA : ringFillB);
        p.drawEllipse(center, r, r);
    }

    // 圆形网格线（统一细线，不加粗）
    for (int lv = 1; lv <= levels; ++lv) {
        qreal f = static_cast<qreal>(lv) / levels;
        qreal r = radius * f;
        p.setPen(QPen(lv == levels ? gridDark : gridMid, 2));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(center, r, r);
    }

    // 径向分割线
    p.setPen(QPen(gridMid, 2));
    for (const auto &pt : axisPoints) {
        p.drawLine(center, pt);
    }

    // 标签（略小），根据文本真实尺寸定位，并避免越界裁切
    p.setPen(labelColor);
    QFont font = p.font();
    font.setPointSize(9);
    font.setBold(true);
    p.setFont(font);
    QFontMetrics fm(font);
    const int edgePadding = 2; // 防止贴边
    for (int i = 0; i < N && i < m_labels.size(); ++i) {
        const QPointF pt = axisPoints[i];
        QPointF dir = (pt - center);
        dir /= qSqrt(dir.x()*dir.x() + dir.y()*dir.y());
        QPointF labelCenter = pt + dir * 18; // 轴端再外移

        const QString text = m_labels[i];
        const int textW = fm.horizontalAdvance(text);
        const int textH = fm.height();

        qreal x = labelCenter.x() - textW / 2.0;
        qreal y = labelCenter.y() - textH / 2.0;

        // 钳制到部件区域内，避免显示不全
        if (x < edgePadding) x = edgePadding;
        if (y < edgePadding) y = edgePadding;
        if (x + textW > width() - edgePadding) x = width() - edgePadding - textW;
        if (y + textH > height() - edgePadding) y = height() - edgePadding - textH;

        QRectF textRect(x, y, textW, textH);
        p.drawText(textRect, Qt::AlignCenter, text);
    }

    // 值区域多边形（半透明填充）
    QVector<QPointF> valuePoints;
    for (int i = 0; i < N; ++i) {
        int v = qBound(0, m_values[i], 5);
        qreal factor = v / 5.0;
        qreal angle = startAngleRad + i * 2 * M_PI / N;
        QPointF pt(center.x() + radius * factor * qCos(angle), center.y() + radius * factor * qSin(angle));
        valuePoints << pt;
    }

    QPolygonF poly(valuePoints);
    p.setPen(QPen(polyEdge, 3));
    p.setBrush(polyFill);
    p.drawPolygon(poly);

    // 内圈描边
    p.setPen(QPen(polyEdge, 2));
    p.drawPolyline(poly);

    // 中心小圆点，增强层次
    p.setBrush(QColor(200, 170, 110, 180));
    p.setPen(Qt::NoPen);
    p.drawEllipse(center, 4, 4);
}


