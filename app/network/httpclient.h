#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>

struct HeroBrief
{
    int id;
    QString title;
    QString name;
    QString roleRoute;   // 路线：top, mid, bottom, jungle, support
    QString roleClass;   // 职能：tank, fighter, assassin, marksman, mage
    QString squareImageUrl;
};

// 新增：英雄详情相关结构
struct HeroSkillInfo
{
    int id = 0;
    QString key;           // P/Q/W/E/R
    QString name;
    QString description;   // 富文本 HTML
    QString iconUrl;
};

struct HeroDetailInfo
{
    int heroId = 0;
    QString coverImageUrl; // 可为空
    QString story;         // 富文本 HTML
    int type = 0;          // 0-100，数值越大越偏魔法
    int difficulty = 0;    // 1-3
    int radarDamage = 0;
    int radarUtility = 0;
    int radarMobility = 0;
    int radarControl = 0;
    int radarToughness = 0;

    // 5 个技能的名字/描述与图片（来自 hero_details）
    QString passiveName;
    QString passiveResult;
    QString qName;
    QString qResult;
    QString wName;
    QString wResult;
    QString eName;
    QString eResult;
    QString rName;
    QString rResult;

    QString passiveImage;
    QString qImage;
    QString wImage;
    QString eImage;
    QString rImage;
};

struct HeroSkinInfo
{
    int heroId = 0;
    int skinIndex = 0;
    QString skinName;
    QString skinImage; // 图片 URL
};

struct HeroStatInfo
{
    int level = 1;
    int attack = 0;
    double attackSpeed = 0.0;
    int attackRange = 0;
    int moveSpeed = 0;
    int health = 0;
    int mana = 0;
    int armor = 0;
    int mr = 0;
};

struct HeroFullInfo
{
    int id = 0;
    QString title;
    QString name;
    QString roleRoute;
    QString roleClass;
    QString squareImageUrl;

    HeroDetailInfo detail;
    QList<HeroSkinInfo> skins;
    QList<HeroSkillInfo> skills;
    QList<HeroStatInfo> stats; // 等级 1..18
};

struct UserRating
{
    int id = 0;
    int userId = 0;
    int heroId = 0;
    int stars = 0;
    QString comment;
    QString createdAt;
    QString username;  // 用户名
    QString avatarUrl; // 用户头像URL
};

class HttpClient : public QObject
{
    Q_OBJECT
public:
    explicit HttpClient(QObject *parent = nullptr);

    void setBaseUrl(const QUrl &url);
    void setAuthToken(const QString &token);

    void login(const QString &username, const QString &password);
    void registerUser(const QString &username, const QString &password);
    void fetchHeroes(const QString &filterType = QString(), const QString &filterValue = QString());
    void fetchHeroDetail(int heroId);
    void downloadImage(const QString &imageUrl, int heroId);
    void downloadImageByUrl(const QString &imageUrl);
    void uploadAvatar(const QString &filePath);
    void fetchUserProfile();

    // 评分相关
    void submitRating(int heroId, int stars, const QString &comment, const QString &topic = QString("hot_heroes"));
    void fetchRatingSummary(int heroId, const QString &topic = QString("hot_heroes"));
    void fetchRatings(int heroId, const QString &topic = QString("hot_heroes"));

signals:
    void loginSucceeded(const QString &token, const QString &username, const QString &avatarUrl);
    void loginFailed(const QString &message);

    void registerSucceeded();
    void registerFailed(const QString &message);

    void heroesFetched(const QList<HeroBrief> &heroes);
    void heroDetailFetched(int heroId, const HeroFullInfo &hero);
    void heroDetailFailed(const QString &message);

    void imageDownloaded(int heroId, const QPixmap &pixmap);
    void imageDownloadedByUrl(const QString &url, const QPixmap &pixmap);
    void requestFailed(const QString &message);
    
    void avatarUploadSucceeded(const QString &avatarUrl);
    void avatarUploadFailed(const QString &message);
    
    void userProfileFetched(const QString &username, int level, const QString &avatarUrl);
    void userProfileFailed(const QString &message);

    // 评分信号
    void ratingSubmitted(int heroId, int stars);
    void ratingSubmitFailed(const QString &message);
    void ratingSummaryFetched(int heroId, double average, int count);
    void ratingSummaryFailed(const QString &message);
    void ratingsFetched(int heroId, const QList<UserRating> &items);
    void ratingsFailed(const QString &message);

private:
    QNetworkAccessManager m_nam;
    QUrl m_baseUrl;
    QString m_token;
};

#endif // HTTPCLIENT_H 