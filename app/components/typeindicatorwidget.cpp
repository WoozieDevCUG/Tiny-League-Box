#include "typeindicatorwidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QtMath>
#include <QPixmap>
#include <QCoreApplication>
#include <QDir>

TypeIndicatorWidget::TypeIndicatorWidget(QWidget *parent)
    : QWidget(parent), m_value(50)
{
    setFixedHeight(34);
    m_leftIconPixmap = nullptr;
    m_rightIconPixmap = nullptr;
    
    m_leftIconPath = QStringLiteral("backend/static/type/2e6eed17-7041-42db-b9ee-ab385d10d98e.png");
    m_rightIconPath = QStringLiteral("backend/static/type/12f8354c-2e6c-45a8-b7de-3d54087e1383.png");
}

void TypeIndicatorWidget::setValue(int value)
{
    if (value < 0)
        value = 0;
    if (value > 100)
        value = 100;
    if (m_value == value) return;
    m_value = value;
    update();
}

QSize TypeIndicatorWidget::sizeHint() const
{
    return QSize(220, 16);
}

void TypeIndicatorWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    loadIconsIfNeeded();

    const int iconSize = 30; // 图标边长（像素）
    const int iconMargin = 8; // 图标与进度条之间的间距
    const int leftX = 0;
    const int rightX = width() - iconSize;
    const int iconY = (height() - iconSize) / 2;

    
    if (m_rightIconPixmap && !m_rightIconPixmap->isNull()) {
        p.drawPixmap(leftX, iconY, iconSize, iconSize, *m_rightIconPixmap);
    }
    if (m_leftIconPixmap && !m_leftIconPixmap->isNull()) {
        p.drawPixmap(rightX, iconY, iconSize, iconSize, *m_leftIconPixmap);
    }

    const qreal barLeftOriginal = leftX + iconSize + iconMargin;
    const qreal barRightOriginal = rightX - iconMargin;
    
    const qreal widthReduced = qMax<qreal>(0.0, (barRightOriginal - barLeftOriginal));
    const qreal barLeft = barLeftOriginal;
    const qreal barHeight = 6.0;
    const QRectF barRect(barLeft, height() / 2.0 - barHeight / 2.0, widthReduced, barHeight);
    
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#8a8f98"));
    p.drawRoundedRect(barRect, 3, 3);

    
    const qreal x = barRect.left() + (barRect.width()) * (m_value / 100.0);
    const QRectF progressRect(barRect.left(), barRect.top(), qMax<qreal>(0.0, x - barRect.left()), barRect.height());
    QColor progressColor("#63d2e6");
    progressColor.setAlpha(180);
    p.setBrush(progressColor);
    p.drawRoundedRect(progressRect, 3, 3);

    
    const qreal radius = 7.0;
    QColor glow = QColor("#63d2e6");
    glow.setAlpha(90);
    p.setBrush(glow);
    p.drawEllipse(QPointF(x, barRect.center().y()), radius + 3.0, radius + 3.0);
    p.setBrush(QColor("#63d2e6"));
    p.drawEllipse(QPointF(x, barRect.center().y()), radius, radius);
}

void TypeIndicatorWidget::setIcons(const QString &leftIconRelativePath, const QString &rightIconRelativePath)
{
    m_leftIconPath = leftIconRelativePath;
    m_rightIconPath = rightIconRelativePath;
    
    delete m_leftIconPixmap; m_leftIconPixmap = nullptr;
    delete m_rightIconPixmap; m_rightIconPixmap = nullptr;
    update();
}

void TypeIndicatorWidget::loadIconsIfNeeded()
{
    auto ensurePixmap = [](QPixmap *&ptr, const QString &relPath) {
        if (ptr) return;
        QString base = QCoreApplication::applicationDirPath();
        QStringList prefixes;
        prefixes << QString() << QStringLiteral("../") << QStringLiteral("../../")
                 << QStringLiteral("../../../") << QStringLiteral("../../../../");
        QPixmap *pm = nullptr;
        for (const QString &prefix : prefixes) {
            const QString abs = QDir(base).filePath(prefix + relPath);
            pm = new QPixmap(abs);
            if (!pm->isNull()) break;
            delete pm;
            pm = nullptr;
        }
        if (!pm) {
            
            pm = new QPixmap(relPath);
        }
        ptr = pm;
    };

    ensurePixmap(m_leftIconPixmap, m_leftIconPath);
    ensurePixmap(m_rightIconPixmap, m_rightIconPath);
}

TypeIndicatorWidget::~TypeIndicatorWidget()
{
    delete m_leftIconPixmap;
    delete m_rightIconPixmap;
}


