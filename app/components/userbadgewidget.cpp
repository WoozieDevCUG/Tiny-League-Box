#include "userbadgewidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include <QPainter>
#include <QPainterPath>

UserBadgeWidget::UserBadgeWidget(QWidget *parent)
    : QWidget(parent), m_avatarLabel(new QLabel(this)), m_nameLabel(new QLabel(this)), m_loggedIn(false)
{
    setFixedHeight(44);
    setStyleSheet("UserBadgeWidget { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #1e2328, stop:1 #2d3748); border: 1px solid #4a5568; border-radius: 12px; padding: 4px 8px; }");

    m_avatarLabel->setFixedSize(36, 36);
    m_avatarLabel->setStyleSheet("QLabel { border-radius: 18px; background: #4a5568; border: 2px solid #00d4ff; }");
    m_avatarLabel->setScaledContents(true);
    m_avatarLabel->setAlignment(Qt::AlignCenter);

    m_nameLabel->setText(tr("未登录"));
    m_nameLabel->setStyleSheet("QLabel { font-size: 14px; font-weight: bold; color: #ccd6f6; background: transparent; padding-left: 8px; }");

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 4, 8, 4);
    layout->setSpacing(6);
    layout->addWidget(m_avatarLabel);
    layout->addWidget(m_nameLabel);

    updateUi();
}

void UserBadgeWidget::setUsername(const QString &username)
{
    m_username = username;
    m_loggedIn = !username.isEmpty();
    updateUi();
}

void UserBadgeWidget::setAvatar(const QPixmap &pixmap)
{
    if (pixmap.isNull()) {
        m_avatarLabel->setPixmap(QPixmap());
        return;
    }

    const int size = 36;
    QPixmap circle(size, size);
    circle.fill(Qt::transparent);

    QPainter painter(&circle);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QPainterPath path;
    path.addEllipse(0, 0, size, size);
    painter.setClipPath(path);

    QPixmap scaled = pixmap.scaled(size, size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    const int x = (size - scaled.width()) / 2;
    const int y = (size - scaled.height()) / 2;
    painter.drawPixmap(x, y, scaled);

    
    painter.setClipping(false);
    QPen pen(QColor("#00d4ff"));
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawEllipse(1, 1, size - 2, size - 2);

    m_avatarLabel->setPixmap(circle);
}

bool UserBadgeWidget::isLoggedIn() const
{
    return m_loggedIn;
}

void UserBadgeWidget::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    QMenu menu(this);
    if (!m_loggedIn) {
        QAction *actLogin = menu.addAction(tr("登录"));
        QAction *actRegister = menu.addAction(tr("注册"));
        QAction *chosen = menu.exec(mapToGlobal(event->pos()));
        if (chosen == actLogin) emit requestLogin();
        else if (chosen == actRegister) emit requestRegister();
    } else {
        QAction *actAvatar = menu.addAction(tr("更换头像"));
        menu.addSeparator();
        QAction *actLogout = menu.addAction(tr("退出登录"));
        QAction *chosen = menu.exec(mapToGlobal(event->pos()));
        if (chosen == actAvatar) emit requestAvatarUpload();
        else if (chosen == actLogout) emit requestLogout();
    }
}

void UserBadgeWidget::updateUi()
{
    if (m_loggedIn) {
        m_nameLabel->setText(m_username);
    } else {
        m_nameLabel->setText(tr("未登录"));
    }
} 