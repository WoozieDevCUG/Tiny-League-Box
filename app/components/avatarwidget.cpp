#include "avatarwidget.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QStandardPaths>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFont>

#include "../network/httpclient.h"

AvatarWidget::AvatarWidget(QWidget *parent)
    : QDialog(parent)
    , m_avatarLabel(nullptr)
    , m_uploadButton(nullptr)
    , m_httpClient(nullptr)
{
    setupUi();
}

void AvatarWidget::setupUi()
{
    
    setFixedSize(320, 400);
    
    
    m_avatarLabel = new QLabel;
    m_avatarLabel->setFixedSize(120, 120);
    m_avatarLabel->setStyleSheet(
        "QLabel {"
        "    border: 3px solid #4CAF50;"
        "    border-radius: 60px;"
        "    background-color: #f9f9f9;"
        "}"
    );
    m_avatarLabel->setAlignment(Qt::AlignCenter);
    m_avatarLabel->setScaledContents(true);
    
    
    QPixmap defaultAvatar(120, 120);
    defaultAvatar.fill(Qt::lightGray);
    QPainter painter(&defaultAvatar);
    painter.setPen(Qt::darkGray);
    painter.setFont(QFont("Arial", 36));
    painter.drawText(defaultAvatar.rect(), Qt::AlignCenter, "👤");
    updateAvatarDisplay(defaultAvatar);
    
    
    QPushButton *selectButton = new QPushButton(tr("选择图片"));
    selectButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #2196F3;"
        "    color: white;"
        "    border: none;"
        "    padding: 10px 20px;"
        "    border-radius: 5px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #1976D2;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #1565C0;"
        "}"
    );
    
    
    m_uploadButton = new QPushButton(tr("上传头像"));
    m_uploadButton->setEnabled(false);
    m_uploadButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #4CAF50;"
        "    color: white;"
        "    border: none;"
        "    padding: 10px 20px;"
        "    border-radius: 5px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #3d8b40;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #cccccc;"
        "    color: #666666;"
        "}"
    );
    
    
    QPushButton *cancelButton = new QPushButton(tr("取消"));
    cancelButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #f44336;"
        "    color: white;"
        "    border: none;"
        "    padding: 10px 20px;"
        "    border-radius: 5px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #d32f2f;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #c62828;"
        "}"
    );
    
    
    QLabel *hintLabel = new QLabel(tr("支持 PNG、JPG、JPEG、GIF、BMP 格式\n文件大小不超过 5MB"));
    hintLabel->setAlignment(Qt::AlignCenter);
    hintLabel->setStyleSheet(
        "QLabel {"
        "    color: #666666;"
        "    font-size: 12px;"
        "    padding: 10px;"
        "}"
    );
    
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addStretch();
    mainLayout->addWidget(m_avatarLabel, 0, Qt::AlignCenter);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(hintLabel, 0, Qt::AlignCenter);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(selectButton, 0, Qt::AlignCenter);
    mainLayout->addSpacing(10);
    
    
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(m_uploadButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();
    mainLayout->setContentsMargins(30, 30, 30, 30);
    
    
    connect(selectButton, &QPushButton::clicked, this, &AvatarWidget::onSelectClicked);
    connect(m_uploadButton, &QPushButton::clicked, this, &AvatarWidget::onUploadClicked);
    connect(cancelButton, &QPushButton::clicked, this, &AvatarWidget::close);
}

void AvatarWidget::setHttpClient(HttpClient *client)
{
    if (m_httpClient) {
        disconnect(m_httpClient, nullptr, this, nullptr);
    }
    
    m_httpClient = client;
    
    if (m_httpClient) {
        connect(m_httpClient, &HttpClient::avatarUploadSucceeded, 
                this, &AvatarWidget::onAvatarUploadSucceeded);
        connect(m_httpClient, &HttpClient::avatarUploadFailed, 
                this, &AvatarWidget::onAvatarUploadFailed);
    }
}

void AvatarWidget::setAvatarUrl(const QString &url)
{
    m_currentAvatarUrl = url;
    if (!url.isEmpty()) {
        loadAvatar(url);
    }
}

void AvatarWidget::setUsername(const QString &username)
{
    m_username = username;
}

void AvatarWidget::onSelectClicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("选择头像图片"),
        QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
        tr("图片文件 (*.png *.jpg *.jpeg *.gif *.bmp)")
    );
    
    if (!filePath.isEmpty()) {
        m_selectedFilePath = filePath;
        
        
        QPixmap pixmap(filePath);
        if (!pixmap.isNull()) {
            updateAvatarDisplay(pixmap);
            m_uploadButton->setEnabled(true);
        } else {
            QMessageBox::warning(this, tr("错误"), tr("无法读取选择的图片文件"));
        }
    }
}

void AvatarWidget::onUploadClicked()
{
    if (!m_httpClient) {
        QMessageBox::warning(this, tr("错误"), tr("网络客户端未设置"));
        return;
    }
    
    if (m_selectedFilePath.isEmpty()) {
        QMessageBox::warning(this, tr("错误"), tr("请先选择一个图片文件"));
        return;
    }
    
    m_uploadButton->setEnabled(false);
    m_uploadButton->setText(tr("上传中..."));
    m_httpClient->uploadAvatar(m_selectedFilePath);
}

void AvatarWidget::onAvatarUploadSucceeded(const QString &avatarUrl)
{
    m_uploadButton->setEnabled(false);
    m_uploadButton->setText(tr("上传头像"));
    
    m_currentAvatarUrl = avatarUrl;
    
    emit avatarChanged(avatarUrl);
    QMessageBox::information(this, tr("成功"), tr("头像上传成功！"));
    
    // 清空选择的文件路径，准备下次选择
    m_selectedFilePath.clear();
    
    // 关闭对话框
    accept();
}

void AvatarWidget::onAvatarUploadFailed(const QString &message)
{
    // 恢复按钮状态，允许重新尝试上传
    m_uploadButton->setEnabled(!m_selectedFilePath.isEmpty());
    m_uploadButton->setText(tr("上传头像"));
    
    QMessageBox::warning(this, tr("上传失败"), message);
}

void AvatarWidget::loadAvatar(const QString &url)
{
    if (url.isEmpty() || !m_httpClient) {
        return;
    }
    
    
    QString fullUrl = url;
    if (url.startsWith("/")) {
        
        QUrl baseUrl = QUrl("http://127.0.0.1:8000");
        fullUrl = baseUrl.resolved(QUrl(url)).toString();
    }
    
    
    QNetworkAccessManager *nam = new QNetworkAccessManager(this);
    QNetworkRequest networkRequest{QUrl(fullUrl)};
    QNetworkReply *reply = nam->get(networkRequest);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        reply->manager()->deleteLater();
        
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QPixmap pixmap;
            if (pixmap.loadFromData(data)) {
                updateAvatarDisplay(pixmap);
            }
        }
    });
}

void AvatarWidget::updateAvatarDisplay(const QPixmap &pixmap)
{
    if (pixmap.isNull()) {
        return;
    }
    
    
    QPixmap roundedPixmap(120, 120);
    roundedPixmap.fill(Qt::transparent);
    
    QPainter painter(&roundedPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    
    
    QPainterPath path;
    path.addEllipse(0, 0, 120, 120);
    painter.setClipPath(path);
    
    
    QPixmap scaledPixmap = pixmap.scaled(120, 120, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    int x = (120 - scaledPixmap.width()) / 2;
    int y = (120 - scaledPixmap.height()) / 2;
    painter.drawPixmap(x, y, scaledPixmap);
    
    m_avatarLabel->setPixmap(roundedPixmap);
} 