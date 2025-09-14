#ifndef AVATARWIDGET_H
#define AVATARWIDGET_H

#include <QDialog>

class QLabel;
class QPushButton;
class HttpClient;
class QPixmap;

class AvatarWidget : public QDialog
{
    Q_OBJECT
public:
    explicit AvatarWidget(QWidget *parent = nullptr);

    void setHttpClient(HttpClient *client);
    void setAvatarUrl(const QString &url);
    void setUsername(const QString &username);

signals:
    void avatarChanged(const QString &avatarUrl);

private slots:
    void onSelectClicked();
    void onUploadClicked();
    void onAvatarUploadSucceeded(const QString &avatarUrl);
    void onAvatarUploadFailed(const QString &message);

private:
    void setupUi();
    void loadAvatar(const QString &url);
    void updateAvatarDisplay(const QPixmap &pixmap);

private:
    QLabel *m_avatarLabel;
    QPushButton *m_uploadButton;
    HttpClient *m_httpClient;
    QString m_username;
    QString m_currentAvatarUrl;
    QString m_selectedFilePath;
};

#endif // AVATARWIDGET_H 