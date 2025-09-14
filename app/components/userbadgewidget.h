#ifndef USERBADGEWIDGET_H
#define USERBADGEWIDGET_H

#include <QWidget>
#include <QPixmap>

class QPushButton;
class QLabel;

class UserBadgeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UserBadgeWidget(QWidget *parent = nullptr);

    void setUsername(const QString &username);
    void setAvatar(const QPixmap &pixmap);
    bool isLoggedIn() const;

signals:
    void requestLogin();
    void requestRegister();
    void requestLogout();
    void requestAvatarUpload();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    void updateUi();

private:
    QLabel *m_avatarLabel;
    QLabel *m_nameLabel;
    QString m_username;
    bool m_loggedIn;
};

#endif // USERBADGEWIDGET_H 