#include "parallelogrambar.h"

#include <QPainter>
#include <QPaintEvent>

ParallelogramBar::ParallelogramBar(QWidget *parent)
    : QFrame(parent)
    , m_active(false)
    , m_activeFill(QStringLiteral("#2aa198"))
    , m_inactiveFill(QStringLiteral("#0f1419"))
    , m_borderColor(QStringLiteral("#6a5324"))
    , m_borderWidth(2)
    , m_slant(4)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void ParallelogramBar::setActive(bool active)
{
    if (m_active == active) return;
    m_active = active;
    update();
}

void ParallelogramBar::setColors(const QColor &activeFill, const QColor &inactiveFill, const QColor &borderColor)
{
    m_activeFill = activeFill;
    m_inactiveFill = inactiveFill;
    m_borderColor = borderColor;
    update();
}

void ParallelogramBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    const QRect r = rect();
    const int s = qMin(m_slant, r.width() / 3);

    QPolygon poly;
    poly << QPoint(r.left() + s, r.top())
         << QPoint(r.right(), r.top())
         << QPoint(r.right() - s, r.bottom())
         << QPoint(r.left(), r.bottom());

    // 填充
    p.setPen(Qt::NoPen);
    p.setBrush(m_active ? m_activeFill : m_inactiveFill);
    p.drawPolygon(poly);

    // 边框
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(m_borderColor, m_borderWidth));
    p.drawPolygon(poly);
}


