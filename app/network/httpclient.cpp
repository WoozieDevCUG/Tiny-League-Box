#include "httpclient.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QPixmap>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QFile>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>

HttpClient::HttpClient(QObject *parent)
    : QObject(parent)
{
}

void HttpClient::fetchRatings(int heroId, const QString &topic)
{
    QUrl url = m_baseUrl.resolved(QUrl(QString("/api/ratings/%1").arg(heroId)));
    QUrlQuery q;
    if (!topic.isEmpty()) q.addQueryItem("topic", topic);
    url.setQuery(q);
    QNetworkRequest req(url);
    auto *reply = m_nam.get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply, heroId]() {
        QByteArray data = reply->readAll();
        const auto error = reply->error();
        const QString err = reply->errorString();
        reply->deleteLater();
        if (error != QNetworkReply::NoError) {
            emit ratingsFailed(QString::fromUtf8(data.isEmpty() ? err.toUtf8() : data));
            return;
        }
        QList<UserRating> items;
        auto doc = QJsonDocument::fromJson(data);
        auto arr = doc.array();
        items.reserve(arr.size());
        for (const auto &v : arr) {
            auto o = v.toObject();
            UserRating ur;
            ur.id = o.value("id").toInt();
            ur.userId = o.value("user_id").toInt();
            ur.heroId = o.value("hero_id").toInt(heroId);
            ur.stars = o.value("stars").toInt();
            ur.comment = o.value("comment").toString();
            ur.createdAt = o.value("created_at").toString();
            ur.username = o.value("username").toString();
            ur.avatarUrl = o.value("avatar_url").toString();
            items.append(ur);
        }
        emit ratingsFetched(heroId, items);
    });
}
void HttpClient::setBaseUrl(const QUrl &url)
{
    m_baseUrl = url;
}

void HttpClient::setAuthToken(const QString &token)
{
    m_token = token;
}

void HttpClient::login(const QString &username, const QString &password)
{
    QUrl url = m_baseUrl.resolved(QUrl("/api/auth/login"));
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject body{{"username", username}, {"password", password}};
    auto *reply = m_nam.post(req, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        QByteArray data = reply->readAll();
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit loginFailed(QString::fromUtf8(data.isEmpty() ? reply->errorString().toUtf8() : data));
            return;
        }
        auto doc = QJsonDocument::fromJson(data);
        auto obj = doc.object();
        const QString token = obj.value("token").toString();
        const QString username = obj.value("username").toString();
        const QString avatarUrl = obj.value("avatar_url").toString();
        if (token.isEmpty()) {
            emit loginFailed(tr("Token 缺失"));
            return;
        }
        emit loginSucceeded(token, username, avatarUrl);
    });
}

void HttpClient::registerUser(const QString &username, const QString &password)
{
    QUrl url = m_baseUrl.resolved(QUrl("/api/auth/register"));
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject body{{"username", username}, {"password", password}};
    auto *reply = m_nam.post(req, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        QByteArray data = reply->readAll();
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit registerFailed(QString::fromUtf8(data.isEmpty() ? reply->errorString().toUtf8() : data));
            return;
        }
        emit registerSucceeded();
    });
}

void HttpClient::fetchHeroes(const QString &filterType, const QString &filterValue)
{
    QUrl url = m_baseUrl.resolved(QUrl("/api/heroes"));
    QUrlQuery q;
    if (!filterType.isEmpty() && !filterValue.isEmpty()) {
        q.addQueryItem("filter_type", filterType);
        q.addQueryItem("filter_value", filterValue);
    }
    url.setQuery(q);

    QNetworkRequest req(url);
    if (!m_token.isEmpty()) {
        req.setRawHeader("Authorization", QByteArray("Bearer ") + m_token.toUtf8());
    }

    auto *reply = m_nam.get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        QByteArray data = reply->readAll();
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit requestFailed(QString::fromUtf8(data.isEmpty() ? reply->errorString().toUtf8() : data));
            return;
        }
        QList<HeroBrief> list;
        auto doc = QJsonDocument::fromJson(data);
        auto arr = doc.array();
        for (const auto &v : arr) {
            auto o = v.toObject();
            HeroBrief h;
            h.id = o.value("id").toInt();
            h.title = o.value("title").toString();
            h.name = o.value("name").toString();
            h.roleRoute = o.value("role_route").toString();
            h.roleClass = o.value("role_class").toString();
            h.squareImageUrl = o.value("square_image_url").toString();
            list.append(h);
        }
        emit heroesFetched(list);
    });
}

void HttpClient::fetchHeroDetail(int heroId)
{
    QUrl url = m_baseUrl.resolved(QUrl(QString("/api/heroes/%1").arg(heroId)));
    QNetworkRequest req(url);
    if (!m_token.isEmpty()) {
        req.setRawHeader("Authorization", QByteArray("Bearer ") + m_token.toUtf8());
    }

    auto *reply = m_nam.get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply, heroId]() {
        QByteArray data = reply->readAll();
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit heroDetailFailed(QString::fromUtf8(data.isEmpty() ? reply->errorString().toUtf8() : data));
            return;
        }
        
        auto doc = QJsonDocument::fromJson(data);
        auto obj = doc.object();
        
        HeroFullInfo heroFull;
        heroFull.id = obj.value("id").toInt();
        heroFull.title = obj.value("title").toString();
        heroFull.name = obj.value("name").toString();
        heroFull.roleRoute = obj.value("role_route").toString();
        heroFull.roleClass = obj.value("role_class").toString();
        heroFull.squareImageUrl = obj.value("square_image_url").toString();
        
        // 解析detail
        auto detailObj = obj.value("detail").toObject();
        if (!detailObj.isEmpty()) {
            heroFull.detail.heroId = detailObj.value("hero_id").toInt();
            heroFull.detail.coverImageUrl = detailObj.value("cover_image_url").toString();
            heroFull.detail.story = detailObj.value("story").toString();
            heroFull.detail.type = detailObj.value("type").toInt();
            heroFull.detail.difficulty = detailObj.value("difficulty").toInt();
            heroFull.detail.radarDamage = detailObj.value("radar_damage").toInt();
            heroFull.detail.radarUtility = detailObj.value("radar_utility").toInt();
            heroFull.detail.radarMobility = detailObj.value("radar_mobility").toInt();
            heroFull.detail.radarControl = detailObj.value("radar_control").toInt();
            heroFull.detail.radarToughness = detailObj.value("radar_toughness").toInt();

            heroFull.detail.passiveName = detailObj.value("passive_name").toString();
            heroFull.detail.passiveResult = detailObj.value("passive_result").toString();
            heroFull.detail.qName = detailObj.value("q_name").toString();
            heroFull.detail.qResult = detailObj.value("q_result").toString();
            heroFull.detail.wName = detailObj.value("w_name").toString();
            heroFull.detail.wResult = detailObj.value("w_result").toString();
            heroFull.detail.eName = detailObj.value("e_name").toString();
            heroFull.detail.eResult = detailObj.value("e_result").toString();
            heroFull.detail.rName = detailObj.value("r_name").toString();
            heroFull.detail.rResult = detailObj.value("r_result").toString();

            heroFull.detail.passiveImage = detailObj.value("passive_image").toString();
            heroFull.detail.qImage = detailObj.value("q_image").toString();
            heroFull.detail.wImage = detailObj.value("w_image").toString();
            heroFull.detail.eImage = detailObj.value("e_image").toString();
            heroFull.detail.rImage = detailObj.value("r_image").toString();
        }
        
        // 解析skins
        auto skinsArray = obj.value("skins").toArray();
        for (const auto &sv : skinsArray) {
            auto so = sv.toObject();
            HeroSkinInfo skin;
            skin.heroId = so.value("hero_id").toInt();
            skin.skinIndex = so.value("skin_index").toInt();
            skin.skinName = so.value("skin_name").toString();
            skin.skinImage = so.value("skin_image").toString();
            heroFull.skins.append(skin);
        }

        // 解析skills
        auto skillsArray = obj.value("skills").toArray();
        for (const auto &skillValue : skillsArray) {
            auto skillObj = skillValue.toObject();
            HeroSkillInfo skill;
            skill.id = skillObj.value("id").toInt();
            skill.key = skillObj.value("key").toString();
            skill.name = skillObj.value("name").toString();
            skill.description = skillObj.value("description").toString();
            skill.iconUrl = skillObj.value("icon_url").toString();
            heroFull.skills.append(skill);
        }
        
        // 解析stats
        auto statsArray = obj.value("stats").toArray();
        for (const auto &statValue : statsArray) {
            auto statObj = statValue.toObject();
            HeroStatInfo stat;
            stat.level = statObj.value("level").toInt();
            stat.attack = statObj.value("attack").toInt();
            stat.attackSpeed = statObj.value("attack_speed").toDouble();
            stat.attackRange = statObj.value("attack_range").toInt();
            stat.moveSpeed = statObj.value("move_speed").toInt();
            stat.health = statObj.value("health").toInt();
            stat.mana = statObj.value("mana").toInt();
            stat.armor = statObj.value("armor").toInt();
            stat.mr = statObj.value("mr").toInt();
            heroFull.stats.append(stat);
        }
        
        emit heroDetailFetched(heroId, heroFull);
    });
}

void HttpClient::downloadImage(const QString &imageUrl, int heroId)
{
    if (imageUrl.isEmpty()) {
        return;
    }
    
    QUrl url;
    if (imageUrl.startsWith("http")) {
        url = QUrl(imageUrl);
    } else {
        // 相对路径，拼接基础URL
        url = m_baseUrl.resolved(QUrl(imageUrl));
    }
    
    QNetworkRequest req(url);
    auto *reply = m_nam.get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply, heroId]() {
        QByteArray data = reply->readAll();
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            return; // 图片下载失败时静默处理
        }
        
        QPixmap pixmap;
        if (pixmap.loadFromData(data)) {
            emit imageDownloaded(heroId, pixmap);
        }
    });
}

void HttpClient::downloadImageByUrl(const QString &imageUrl)
{
    if (imageUrl.isEmpty()) {
        return;
    }
    
    QUrl url;
    if (imageUrl.startsWith("http")) {
        url = QUrl(imageUrl);
    } else {
        // 相对路径，拼接基础URL
        url = m_baseUrl.resolved(QUrl(imageUrl));
    }
    
    QNetworkRequest req(url);
    auto *reply = m_nam.get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply, imageUrl]() {
        QByteArray data = reply->readAll();
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            return; // 图片下载失败时静默处理
        }
        
        QPixmap pixmap;
        if (pixmap.loadFromData(data)) {
            emit imageDownloadedByUrl(imageUrl, pixmap);
        }
    });
}

void HttpClient::uploadAvatar(const QString &filePath)
{
    if (m_token.isEmpty()) {
        emit avatarUploadFailed(tr("用户未登录"));
        return;
    }
    
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        emit avatarUploadFailed(tr("文件不存在"));
        return;
    }
    
    QMimeDatabase mimeDb;
    QMimeType mimeType = mimeDb.mimeTypeForFile(fileInfo);
    
    // 创建multipart数据
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    
    // 使用堆对象，确保在请求期间不被销毁
    QFile *file = new QFile(filePath);
    if (!file->open(QIODevice::ReadOnly)) {
        file->deleteLater();
        emit avatarUploadFailed(tr("无法读取文件"));
        multiPart->deleteLater();
        return;
    }
    file->setParent(multiPart);
    
    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, mimeType.name());
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QString("form-data; name=\"file\"; filename=\"%1\"").arg(fileInfo.fileName()));
    filePart.setBodyDevice(file);
    
    multiPart->append(filePart);
    
    QUrl url = m_baseUrl.resolved(QUrl("/api/auth/avatar"));
    QNetworkRequest req(url);
    req.setRawHeader("Authorization", QByteArray("Bearer ") + m_token.toUtf8());
    
    QNetworkReply *reply = m_nam.post(req, multiPart);
    multiPart->setParent(reply); // reply销毁时自动销毁
    
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        const QByteArray data = reply->readAll();
        const auto error = reply->error();
        const QString errorStr = reply->errorString();
        reply->deleteLater();
        
        if (error != QNetworkReply::NoError) {
            emit avatarUploadFailed(QString::fromUtf8(data.isEmpty() ? errorStr.toUtf8() : data));
            return;
        }
        
        const QJsonDocument doc = QJsonDocument::fromJson(data);
        const QJsonObject obj = doc.object();
        const QString avatarUrl = obj.value("avatar_url").toString();
        if (avatarUrl.isEmpty()) {
            emit avatarUploadFailed(tr("服务器响应异常"));
            return;
        }
        
        emit avatarUploadSucceeded(avatarUrl);
    });
}

void HttpClient::fetchUserProfile()
{
    if (m_token.isEmpty()) {
        emit userProfileFailed(tr("用户未登录"));
        return;
    }
    
    QUrl url = m_baseUrl.resolved(QUrl("/api/auth/profile"));
    QNetworkRequest req(url);
    req.setRawHeader("Authorization", QByteArray("Bearer ") + m_token.toUtf8());
    
    auto *reply = m_nam.get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        QByteArray data = reply->readAll();
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit userProfileFailed(QString::fromUtf8(data.isEmpty() ? reply->errorString().toUtf8() : data));
            return;
        }
        
        auto doc = QJsonDocument::fromJson(data);
        auto obj = doc.object();
        const QString username = obj.value("username").toString();
        const int level = obj.value("level").toInt();
        const QString avatarUrl = obj.value("avatar_url").toString();
        
        emit userProfileFetched(username, level, avatarUrl);
    });
} 

void HttpClient::submitRating(int heroId, int stars, const QString &comment, const QString &topic)
{
    if (m_token.isEmpty()) {
        emit ratingSubmitFailed(tr("用户未登录"));
        return;
    }
    QUrl url = m_baseUrl.resolved(QUrl("/api/ratings"));
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", QByteArray("Bearer ") + m_token.toUtf8());

    QJsonObject body{{"hero_id", heroId}, {"stars", stars}};
    if (!comment.isEmpty()) body.insert("comment", comment);
    if (!topic.isEmpty()) body.insert("topic", topic);

    auto *reply = m_nam.post(req, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply, heroId, stars]() {
        QByteArray data = reply->readAll();
        const auto error = reply->error();
        const QString err = reply->errorString();
        reply->deleteLater();
        if (error != QNetworkReply::NoError) {
            QString errorMessage;
            if (!data.isEmpty()) {
                // 尝试解析JSON错误响应
                QJsonParseError parseError;
                QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
                if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
                    QJsonObject obj = doc.object();
                    if (obj.contains("detail")) {
                        errorMessage = obj["detail"].toString();
                    } else {
                        errorMessage = QString::fromUtf8(data);
                    }
                } else {
                    errorMessage = QString::fromUtf8(data);
                }
            } else {
                errorMessage = err;
            }
            emit ratingSubmitFailed(errorMessage);
            return;
        }
        emit ratingSubmitted(heroId, stars);
    });
}

void HttpClient::fetchRatingSummary(int heroId, const QString &topic)
{
    QUrl url = m_baseUrl.resolved(QUrl(QString("/api/ratings/summary/%1").arg(heroId)));
    QUrlQuery q;
    if (!topic.isEmpty()) q.addQueryItem("topic", topic);
    url.setQuery(q);
    QNetworkRequest req(url);
    auto *reply = m_nam.get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply, heroId]() {
        QByteArray data = reply->readAll();
        const auto error = reply->error();
        const QString err = reply->errorString();
        reply->deleteLater();
        if (error != QNetworkReply::NoError) {
            emit ratingSummaryFailed(QString::fromUtf8(data.isEmpty() ? err.toUtf8() : data));
            return;
        }
        auto doc = QJsonDocument::fromJson(data);
        auto obj = doc.object();
        const double avg = obj.value("avg").toDouble();
        const int count = obj.value("count").toInt();
        emit ratingSummaryFetched(heroId, avg, count);
    });
}