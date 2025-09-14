#include "widget.h"
#include "ui_widget.h"

#include <QMessageBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QLineEdit>
#include <QUrl>
#include <QSettings>
#include <QJsonArray>
#include <QJsonObject>
#include <QSharedPointer>
#include <QPointer>
#include <QTimer>
#include <QProgressBar>
#include <algorithm>

#include "components/userbadgewidget.h"
#include "components/authdialog.h"
#include "components/herocardwidget.h"
#include "components/herodetailwidget.h"
#include "components/avatarwidget.h"
#include "network/httpclient.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , m_authDialog(new AuthDialog(this))
    , m_http(new HttpClient(this))
    , m_avatarWidget(new AvatarWidget(this))
    , m_heroDetailWidget(new HeroDetailWidget(this))
{
    ui->setupUi(this);

    // 顶部返回按钮（非首页显示）
    m_btnBack = new QPushButton(QStringLiteral("\u2190 返回主页"), this);
    m_btnBack->setFixedHeight(32);
    m_btnBack->setStyleSheet(
        "QPushButton {"
        "  background: #1e2328;"
        "  border: 2px solid #463714;"
        "  border-radius: 4px;"
        "  color: #cdbe91;"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "  padding: 0 12px;"
        "}"
        "QPushButton:hover {"
        "  background: #463714;"
        "  border-color: #c8aa6e;"
        "}"
    );
    // 单独放在标题下方一行，保持标题位置不变
    auto backRow = new QWidget(this);
    auto backRowLayout = new QHBoxLayout(backRow);
    backRowLayout->setContentsMargins(16, 6, 16, 0); // 向下挪一点并与左右保持一致
    backRowLayout->setSpacing(0);
    backRowLayout->addWidget(m_btnBack, 0, Qt::AlignLeft);
    backRowLayout->addStretch();
    ui->verticalLayout_Main->insertWidget(1, backRow);
    m_btnBack->setVisible(false);
    connect(m_btnBack, &QPushButton::clicked, this, [this]() {
        ui->stackedMain->setCurrentWidget(ui->pageHome);
    });
    connect(ui->stackedMain, &QStackedWidget::currentChanged, this, [this](int){
        m_btnBack->setVisible(ui->stackedMain->currentWidget() != ui->pageHome);
    });

    // 默认后端地址
    m_http->setBaseUrl(QUrl("http://127.0.0.1:8000"));

    // 设置头像组件的HTTP客户端
    m_avatarWidget->setHttpClient(m_http);
    m_avatarWidget->hide(); // 初始隐藏

    // 添加英雄详细页面到stackedData
    ui->stackedData->addWidget(m_heroDetailWidget);

    setupConnections();

    // 启动时尝试读取持久化登录
    loadAuthFromSettings();

    // 首页按钮导航
    connect(ui->btnData, &QPushButton::clicked, this, [this]() {
        ui->stackedMain->setCurrentWidget(ui->pageData);
    });
    connect(ui->btnNews, &QPushButton::clicked, this, [this]() {
        ui->stackedMain->setCurrentWidget(ui->pageNews);
    });
    connect(ui->btnGuides, &QPushButton::clicked, this, [this]() {
        ui->stackedMain->setCurrentWidget(ui->pageGuides);
    });
    connect(ui->btnInteract, &QPushButton::clicked, this, [this]() {
        // 每次进入都根据“上单/top”筛选动态构建
        buildInteractPage();
        ui->stackedMain->setCurrentWidget(ui->pageInteract);
    });

    // 资料页 tabs
    connect(ui->btnTabHeroes, &QPushButton::clicked, this, [this]() {
        ui->btnTabHeroes->setChecked(true);
        ui->btnTabItems->setChecked(false);
        ui->btnTabRunes->setChecked(false);
        ui->stackedData->setCurrentWidget(ui->pageHeroes);
    });
    connect(ui->btnTabItems, &QPushButton::clicked, this, [this]() {
        ui->btnTabHeroes->setChecked(false);
        ui->btnTabItems->setChecked(true);
        ui->btnTabRunes->setChecked(false);
        ui->stackedData->setCurrentWidget(ui->pageItems);
    });
    connect(ui->btnTabRunes, &QPushButton::clicked, this, [this]() {
        ui->btnTabHeroes->setChecked(false);
        ui->btnTabItems->setChecked(false);
        ui->btnTabRunes->setChecked(true);
        ui->stackedData->setCurrentWidget(ui->pageRunes);
    });

    // 筛选框联动
    connect(ui->comboFilterType, qOverload<int>(&QComboBox::currentIndexChanged), this, &Widget::onFilterTypeChanged);
    connect(ui->comboFilterValue, qOverload<int>(&QComboBox::currentIndexChanged), this, &Widget::onFilterValueChanged);

    // 初始化筛选 - 这会自动触发fetchHeroes
    onFilterTypeChanged(ui->comboFilterType->currentIndex());
}

Widget::~Widget()
{
    delete ui;
}

void Widget::setupConnections()
{
    // 用户徽章
    auto userBadge = findChild<UserBadgeWidget*>("userBadge");
    if (userBadge) {
        connect(userBadge, &UserBadgeWidget::requestLogin, this, &Widget::onLoginRequested);
        connect(userBadge, &UserBadgeWidget::requestRegister, this, &Widget::onRegisterRequested);
        connect(userBadge, &UserBadgeWidget::requestLogout, this, &Widget::onLogoutRequested);
        connect(userBadge, &UserBadgeWidget::requestAvatarUpload, this, &Widget::onAvatarUploadRequested);
    }

    // 认证对话框
    connect(m_authDialog, &AuthDialog::submitLogin, this, &Widget::onLoginSubmit);
    connect(m_authDialog, &AuthDialog::submitRegister, this, &Widget::onRegisterSubmit);

    // 英雄详细页面
    connect(m_heroDetailWidget, &HeroDetailWidget::backRequested, this, &Widget::onHeroDetailBackRequested);

    // 网络回调
    connect(m_http, &HttpClient::loginSucceeded, this, &Widget::onLoginSucceeded);
    connect(m_http, &HttpClient::loginFailed, this, &Widget::onLoginFailed);
    connect(m_http, &HttpClient::registerSucceeded, this, &Widget::onRegisterSucceeded);
    connect(m_http, &HttpClient::registerFailed, this, &Widget::onRegisterFailed);
    connect(m_http, &HttpClient::ratingsFetched, this, &Widget::onRatingsFetched);
    connect(m_http, &HttpClient::heroesFetched, this, [this](const QList<HeroBrief> &heroes){
        // 仅在资料->英雄列表页可见时更新网格，避免其他页面的fetchHeroes触发不必要刷新
        if (ui->stackedData->currentWidget() != ui->pageHeroes) {
            // 但仍需更新卡片数据缓存
            m_heroCards.clear();
            return;
        }
        // 清空网格
        auto grid = findChild<QGridLayout*>("gridHeroes");
        if (!grid) return;
        QLayoutItem *child;
        while ((child = grid->takeAt(0)) != nullptr) {
            if (child->widget()) child->widget()->deleteLater();
            delete child;
        }
        m_heroCards.clear();

        // 固定为5列布局
        int colCount = 5;
        int row = 0, col = 0;
        for (const auto &h : heroes) {
            auto *card = new HeroCardWidget(this);
            card->setHeroId(h.id);
            card->setProperty("heroId", h.id);  // 设置属性用于后续查找
            card->setDisplayName(h.title, h.name);
            card->setTags(QStringList() << h.roleClass << h.roleRoute);
            
            // 下载图片（优先使用后端提供的URL；若URL缺失或看起来不是以/static开头，则尝试后备路径）
            if (!h.squareImageUrl.isEmpty()) {
                m_http->downloadImage(h.squareImageUrl, h.id);
                if (!h.squareImageUrl.startsWith("/static")) {
                    m_http->downloadImage(QString("/static/hero_squares/%1.png").arg(h.id), h.id);
                }
            } else {
                m_http->downloadImage(QString("/static/hero_squares/%1.png").arg(h.id), h.id);
            }
            
            grid->addWidget(card, row, col, Qt::AlignHCenter);
            m_heroCards.append(card);
            connect(card, &HeroCardWidget::clicked, this, [this](int heroId){
                showHeroDetail(heroId);
            });
            col++;
            if (col >= colCount) { col = 0; row++; }
        }
    });
    
    // 图片下载完成回调
    connect(m_http, &HttpClient::imageDownloaded, this, [this](int heroId, const QPixmap &pixmap){
        // 英雄卡片
        for (auto *card : m_heroCards) {
            if (card && card->property("heroId").toInt() == heroId) {
                card->setSquareImage(pixmap);
                break;
            }
        }
        // 互动页条目头像
        for (auto *item : m_interactItems) {
            if (item && item->heroId == heroId && item->imageLabel) {
                item->imageLabel->setPixmap(pixmap.scaled(48,48, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
            }
        }
    });
    
    connect(m_http, &HttpClient::requestFailed, this, [this](const QString &msg){
        QMessageBox::warning(this, tr("错误"), msg);
    });

    // 评分相关信号
    connect(m_http, &HttpClient::ratingSubmitted, this, &Widget::onRatingSubmitted);
    connect(m_http, &HttpClient::ratingSubmitFailed, this, [this](const QString &msg){
        // 处理登录失效
        if (msg.contains("Invalid token", Qt::CaseInsensitive)) {
            clearAuthInSettings();
            m_token.clear();
            m_currentUsername.clear();
            m_currentAvatarUrl.clear();
            m_http->setAuthToken(QString());
            QMessageBox::warning(this, tr("登录已过期"), tr("请重新登录"));
            m_authDialog->setMode(AuthDialog::Login);
            m_authDialog->show();
            return;
        }
        
        // 检查是否是内容审核失败
        if (msg.contains("不符合社区规范") || msg.contains("community standards")) {
            QMessageBox::warning(this, tr("评论审核"), msg);
        } else {
        QMessageBox::warning(this, tr("提交失败"), msg);
        }
    });
    connect(m_http, &HttpClient::ratingSummaryFetched, this, &Widget::onRatingSummaryFetched);
}

void Widget::onLoginRequested()
{
    m_authDialog->setMode(AuthDialog::Login);
    m_authDialog->show();
}

void Widget::onRegisterRequested()
{
    m_authDialog->setMode(AuthDialog::Register);
    m_authDialog->show();
}

void Widget::onLogoutRequested()
{
    m_token.clear();
    m_currentUsername.clear();
    m_currentAvatarUrl.clear();
    m_http->setAuthToken("");
    clearAuthInSettings();
    if (auto badge = findChild<UserBadgeWidget*>("userBadge")) {
        badge->setUsername("");
    }
    // 隐藏头像上传组件
    m_avatarWidget->hide();
}

void Widget::onLoginSubmit(const QString &username, const QString &password)
{
    m_http->login(username, password);
}

void Widget::onRegisterSubmit(const QString &username, const QString &password)
{
    m_http->registerUser(username, password);
}

void Widget::onLoginSucceeded(const QString &token, const QString &username, const QString &avatarUrl)
{
    m_token = token;
    m_currentUsername = username;
    m_currentAvatarUrl = avatarUrl;
    m_http->setAuthToken(token);
    saveAuthToSettings(token, username, avatarUrl);
    
    if (auto badge = findChild<UserBadgeWidget*>("userBadge")) {
        badge->setUsername(username);
        if (!avatarUrl.isEmpty()) {
            // 先连接，后下载，避免竞态
            connect(m_http, &HttpClient::imageDownloaded, this, [this, badge](int heroId, const QPixmap &pixmap){
                if (heroId == -1) {
                    badge->setAvatar(pixmap);
                }
            }, Qt::SingleShotConnection);
            m_http->downloadImage(avatarUrl, -1);
        }
    }
    
    // 更新头像组件信息
    m_avatarWidget->setUsername(username);
    m_avatarWidget->setAvatarUrl(avatarUrl);
    
    // 登录后主动拉取一次资料，确保头像等字段最新
    m_http->fetchUserProfile();
    connect(m_http, &HttpClient::userProfileFetched, this, [this](const QString &uname, int level, const QString &avatar){
        Q_UNUSED(uname);
        Q_UNUSED(level);
        if (!avatar.isEmpty()) {
            m_currentAvatarUrl = avatar;
            if (auto badge = findChild<UserBadgeWidget*>("userBadge")) {
                connect(m_http, &HttpClient::imageDownloaded, this, [this, badge](int heroId, const QPixmap &pixmap){
                    if (heroId == -1) badge->setAvatar(pixmap);
                }, Qt::SingleShotConnection);
                m_http->downloadImage(avatar, -1);
            }
        }
    }, Qt::SingleShotConnection);
    
    m_authDialog->hide();
}

void Widget::onLoginFailed(const QString &message)
{
    QMessageBox::warning(this, tr("登录失败"), message);
}

void Widget::onRegisterSucceeded()
{
    QMessageBox::information(this, tr("注册成功"), tr("请使用新账号登录"));
    m_authDialog->setMode(AuthDialog::Login);
}

void Widget::onRegisterFailed(const QString &message)
{
    QMessageBox::warning(this, tr("注册失败"), message);
}

void Widget::onHomeButtons()
{
}

void Widget::onDataTabs()
{
}

void Widget::onFilterTypeChanged(int index)
{
    Q_UNUSED(index);
    
    // 检查是否选择了"显示所有"
    if (ui->comboFilterType->currentIndex() == 0) {
        // 选择了"显示所有"，禁用第二个combobox
        ui->comboFilterValue->setEnabled(false);
        ui->comboFilterValue->clear();
        ui->comboFilterValue->addItem(tr("无筛选"));
        // 获取所有英雄，不进行筛选
        m_http->fetchHeroes("", "");
    } else if (ui->comboFilterType->currentIndex() == 1) {
        // 选择了"路线定位"
        ui->comboFilterValue->setEnabled(true);
        setFilterValuesForRoute();
        // 切换类型后立即刷新
        m_http->fetchHeroes(currentFilterTypeKey(), currentFilterValueKey());
    } else {
        // 选择了"功能定位"
        ui->comboFilterValue->setEnabled(true);
        setFilterValuesForClass();
        // 切换类型后立即刷新
        m_http->fetchHeroes(currentFilterTypeKey(), currentFilterValueKey());
    }
}

void Widget::onFilterValueChanged(int index)
{
    Q_UNUSED(index);
    // 只有在第二个combobox启用时才进行筛选
    if (ui->comboFilterValue->isEnabled()) {
        m_http->fetchHeroes(currentFilterTypeKey(), currentFilterValueKey());
    }
}

void Widget::refreshHeroesGrid()
{
    // 获取筛选参数
    QString filterType = currentFilterTypeKey();
    QString filterValue = currentFilterValueKey();
    
    // 请求英雄数据
    m_http->fetchHeroes(filterType, filterValue);
}

void Widget::showHeroDetail(int heroId)
{
    
    auto connection = connect(m_http, &HttpClient::heroDetailFetched, this, [this](int fetchedHeroId, const HeroFullInfo &heroInfo) {
        // 转换为QJsonObject格式
        QJsonObject heroData;
        heroData["id"] = heroInfo.id;
        heroData["name"] = heroInfo.name;
        heroData["title"] = heroInfo.title;
        heroData["role_route"] = heroInfo.roleRoute;
        heroData["role_class"] = heroInfo.roleClass;
        heroData["square_image_url"] = heroInfo.squareImageUrl;
        
        // 添加详细信息
        if (heroInfo.detail.heroId == heroInfo.id) {
            QJsonObject detail;
            detail["description"] = heroInfo.detail.story;
            detail["difficulty"] = heroInfo.detail.difficulty;
            detail["damage_type"] = heroInfo.detail.type;
            detail["radar_damage"] = heroInfo.detail.radarDamage;
            detail["radar_utility"] = heroInfo.detail.radarUtility;
            detail["radar_mobility"] = heroInfo.detail.radarMobility;
            detail["radar_control"] = heroInfo.detail.radarControl;
            detail["radar_toughness"] = heroInfo.detail.radarToughness;
            heroData["detail"] = detail;
        }
        
        // 添加皮肤信息
        if (!heroInfo.skins.isEmpty()) {
            QJsonArray skins;
            for (const auto &sk : heroInfo.skins) {
                QJsonObject o;
                o["hero_id"] = sk.heroId;
                o["skin_index"] = sk.skinIndex;
                o["skin_name"] = sk.skinName;
                o["skin_image"] = sk.skinImage;
                skins.append(o);
            }
            heroData["skins"] = skins;
        }
        
        // 英雄技能（来自 hero_details 的 5 个图与名称描述）优先
        if (heroInfo.detail.heroId == heroInfo.id) {
            QJsonArray skills;
            struct Item { QString key; QString name; QString desc; QString img; };
            QList<Item> items = {
                {"P", heroInfo.detail.passiveName, heroInfo.detail.passiveResult, heroInfo.detail.passiveImage},
                {"Q", heroInfo.detail.qName, heroInfo.detail.qResult, heroInfo.detail.qImage},
                {"W", heroInfo.detail.wName, heroInfo.detail.wResult, heroInfo.detail.wImage},
                {"E", heroInfo.detail.eName, heroInfo.detail.eResult, heroInfo.detail.eImage},
                {"R", heroInfo.detail.rName, heroInfo.detail.rResult, heroInfo.detail.rImage},
            };
            for (const auto &it : items) {
                QJsonObject o; o["key"] = it.key; o["name"] = it.name; o["description"] = it.desc; o["icon_url"] = it.img; skills.append(o);
            }
            heroData["skills"] = skills;
            
            QJsonObject detail = heroData.value("detail").toObject();
            detail["passive_name"] = heroInfo.detail.passiveName;
            detail["passive_result"] = heroInfo.detail.passiveResult;
            detail["q_name"] = heroInfo.detail.qName;
            detail["q_result"] = heroInfo.detail.qResult;
            detail["w_name"] = heroInfo.detail.wName;
            detail["w_result"] = heroInfo.detail.wResult;
            detail["e_name"] = heroInfo.detail.eName;
            detail["e_result"] = heroInfo.detail.eResult;
            detail["r_name"] = heroInfo.detail.rName;
            detail["r_result"] = heroInfo.detail.rResult;
            detail["passive_image"] = heroInfo.detail.passiveImage;
            detail["q_image"] = heroInfo.detail.qImage;
            detail["w_image"] = heroInfo.detail.wImage;
            detail["e_image"] = heroInfo.detail.eImage;
            detail["r_image"] = heroInfo.detail.rImage;
            heroData["detail"] = detail;
        } else {
            // 退化为 hero_skills 表
            QJsonArray skills;
            for (const auto &skill : heroInfo.skills) {
                QJsonObject skillObj;
                skillObj["key"] = skill.key;
                skillObj["name"] = skill.name;
                skillObj["description"] = skill.description;
                if (!skill.iconUrl.isEmpty()) skillObj["icon_url"] = skill.iconUrl;
                skills.append(skillObj);
            }
            heroData["skills"] = skills;
        }
        
        // 添加数值信息
        QJsonArray stats;
        for (const auto &stat : heroInfo.stats) {
            QJsonObject statObj;
            statObj["level"] = stat.level;
            statObj["attack"] = stat.attack;
            statObj["health"] = stat.health;
            statObj["armor"] = stat.armor;
            statObj["move_speed"] = stat.moveSpeed;
            statObj["attack_range"] = stat.attackRange;
            statObj["mr"] = stat.mr;
            stats.append(statObj);
        }
        heroData["stats"] = stats;
        
        m_heroDetailWidget->setHeroData(heroData);
        
        // 下载并展示皮肤图片（优先皮肤，否则用 square_image_url）
        QString firstSkinUrl;
        QString firstSkinName;
        if (!heroInfo.skins.isEmpty()) {
            firstSkinUrl = heroInfo.skins.first().skinImage;
            firstSkinName = heroInfo.skins.first().skinName;
        } else {
            firstSkinUrl = heroInfo.squareImageUrl;
            firstSkinName = QString();
        }
        if (!firstSkinUrl.isEmpty()) {
            m_http->downloadImageByUrl(firstSkinUrl);
            auto skinConn = QSharedPointer<QMetaObject::Connection>::create();
            *skinConn = connect(m_http, &HttpClient::imageDownloadedByUrl, this,
                                [this, firstSkinUrl, firstSkinName, skinConn](const QString &url, const QPixmap &pixmap){
                                    if (url == firstSkinUrl) {
                                        m_heroDetailWidget->setSkinImage(pixmap, firstSkinName);
                                        QObject::disconnect(*skinConn);
                                    }
                                });
        }

        connect(m_heroDetailWidget, &HeroDetailWidget::skinChanged, this, [this, fetchedHeroId](int index, const QString &imageUrl, const QString &skinName){
            Q_UNUSED(index);
            if (imageUrl.isEmpty()) return;
            m_http->downloadImageByUrl(imageUrl);
            auto skinSwitchConn = QSharedPointer<QMetaObject::Connection>::create();
            *skinSwitchConn = connect(m_http, &HttpClient::imageDownloadedByUrl, this,
                                      [this, imageUrl, skinName, skinSwitchConn](const QString &url, const QPixmap &pix){
                                          if (url == imageUrl) {
                                              m_heroDetailWidget->setSkinImage(pix, skinName);
                                              QObject::disconnect(*skinSwitchConn);
                                          }
                                      });
        });

        auto pending = QSharedPointer<QMap<QString, QString>>::create(); // URL 到技能键的映射
        if (heroInfo.detail.heroId == heroInfo.id) {
            QMap<QString, QString> k2url{
                {"P", heroInfo.detail.passiveImage},
                {"Q", heroInfo.detail.qImage},
                {"W", heroInfo.detail.wImage},
                {"E", heroInfo.detail.eImage},
                {"R", heroInfo.detail.rImage},
            };
            for (auto it = k2url.begin(); it != k2url.end(); ++it) {
                if (!it.value().isEmpty()) {
                    (*pending)[it.value()] = it.key();
                    m_http->downloadImageByUrl(it.value());
                }
            }
        } else {
            for (const auto &s : heroInfo.skills) {
                if (!s.iconUrl.isEmpty()) {
                    (*pending)[s.iconUrl] = s.key.toUpper();
                    m_http->downloadImageByUrl(s.iconUrl);
                }
            }
        }
        if (!pending->isEmpty()) {
            auto iconConn = QSharedPointer<QMetaObject::Connection>::create();
            *iconConn = connect(m_http, &HttpClient::imageDownloadedByUrl, this,
                                [this, pending, iconConn](const QString &url, const QPixmap &pixmap){
                                    auto it = pending->find(url);
                                    if (it != pending->end()) {
                                        m_heroDetailWidget->setSkillIcon(it.value(), pixmap);
                                        pending->erase(it);
                                        if (pending->isEmpty()) {
                                            QObject::disconnect(*iconConn);
                                        }
                                    }
                                });
        }
        
        ui->stackedData->setCurrentWidget(m_heroDetailWidget);
    }, Qt::SingleShotConnection);
    
    
    m_http->fetchHeroDetail(heroId);
}

void Widget::onHeroDetailBackRequested()
{
    // 返回英雄列表页面
    ui->stackedData->setCurrentWidget(ui->pageHeroes);
}

void Widget::buildInteractPage()
{
    ui->stackedMain->setCurrentWidget(ui->pageInteract);
    auto vbox = ui->verticalLayout_Interact;
    while (QLayoutItem *child = vbox->takeAt(0)) {
        if (child->widget()) child->widget()->deleteLater();
        delete child;
    }

    auto title = new QLabel(tr("英雄讨论区"), this);
    title->setStyleSheet("font-size: 20px; color: #ccd6f6; font-weight: bold; margin-bottom: 10px;");
    vbox->addWidget(title);

    auto listHost = new QWidget(this);
    auto listLayout = new QVBoxLayout(listHost);
    listLayout->setContentsMargins(8, 8, 8, 8);
    listLayout->setSpacing(16);

    struct ForumTopic {
        QString key;
        QString title;
        QString description;
        QString lastActivity;
        int replyCount;
        int viewCount;
        QString icon;
    };

    const QList<ForumTopic> topics = {
        { 
            QStringLiteral("hot_heroes"), 
            tr("🔥 热门英雄评分讨论"), 
            tr("分享你对各英雄的使用体验，为英雄打分评论，帮助其他玩家选择适合的英雄"),
            tr("2分钟前"),
            127,
            2341,
            "🎮"
        }
    };

    for (const auto &topic : topics) {
        auto card = new QWidget(listHost);
        card->setStyleSheet("QWidget{background:#1a1f24;border:none;border-radius:6px;}");
        card->setFixedHeight(80);
        card->setProperty("topicKey", topic.key); // 添加属性用于识别话题
        card->installEventFilter(this); // 安装事件过滤器
        
        auto cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(12, 10, 12, 10);
        cardLayout->setSpacing(6);

        auto topicTitle = new QLabel(topic.title, card);
        topicTitle->setStyleSheet("color:#ccd6f6;font-size:15px;font-weight:bold;");
        cardLayout->addWidget(topicTitle);

        auto description = new QLabel(topic.description, card);
        description->setStyleSheet("color:#94a3b8;font-size:12px;");
        description->setWordWrap(true);
        cardLayout->addWidget(description);

        listLayout->addWidget(card);
    }

    auto placeholderCard = new QWidget(listHost);
    placeholderCard->setStyleSheet("QWidget{background:#1a1f24;border:none;border-radius:6px;}");
    placeholderCard->setFixedHeight(60);
    
    auto placeholderLayout = new QVBoxLayout(placeholderCard);
    placeholderLayout->setContentsMargins(12, 15, 12, 15);
    
    auto placeholderText = new QLabel(tr("更多精彩话题即将上线"), placeholderCard);
    placeholderText->setStyleSheet("color:#6b7280;font-size:13px;");
    placeholderText->setAlignment(Qt::AlignCenter);
    placeholderLayout->addWidget(placeholderText);
    
    listLayout->addWidget(placeholderCard);

    listLayout->addStretch(); // 底部留白

    auto scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("QScrollArea{background:transparent;border:none;} QScrollBar:vertical{background:#1e2328;width:12px;border-radius:6px;} QScrollBar::handle:vertical{background:#4a5568;border-radius:6px;min-height:20px;} QScrollBar::handle:vertical:hover{background:#00d4ff;} QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical{height:0px;}");
    scroll->setWidget(listHost);
    vbox->addWidget(scroll, 1);
}


void Widget::buildInteractTopicDetail(const QString &topicKey)
{
    m_currentInteractTopicKey = topicKey;
    // 确保切换到互动页
    ui->stackedMain->setCurrentWidget(ui->pageInteract);
    auto vbox = ui->verticalLayout_Interact;
    while (QLayoutItem *child = vbox->takeAt(0)) {
        if (child->widget()) child->widget()->deleteLater();
        delete child;
    }

    // 标题
    auto title = new QLabel(
        topicKey == QStringLiteral("hot_heroes")
            ? tr("热门英雄评分")
            : (topicKey == QStringLiteral("top_tank_strength") ? tr("上路坦克强度评分") : topicKey),
        this);
    title->setStyleSheet("font-size: 18px; color: #ccd6f6; font-weight: bold;");
    vbox->addWidget(title);

    // 返回按钮
    {
        auto backToList = new QPushButton(tr("返回话题列表"), this);
        backToList->setStyleSheet("QPushButton{color:#c8aa6e;background:#1e2328;border:1px solid #463714;border-radius:4px;padding:4px 10px;} QPushButton:hover{background:#463714;}");
    connect(backToList, &QPushButton::clicked, this, [this]() { buildInteractPage(); });
        vbox->addWidget(backToList, 0, Qt::AlignLeft);
    }

    // 加载状态（使用QPointer避免页面切换后悬空）
    auto statusLabel = new QLabel(tr("正在加载..."), this);
    statusLabel->setStyleSheet("color:#94a3b8; padding:6px 2px;");
    vbox->addWidget(statusLabel);
    QPointer<QLabel> statusLabelPtr(statusLabel);

    // 动态构建：监听一次 heroes 列表，创建单列列表（每行：头像 + 名称 + 星级(0-5) + 评论 + 提交）
    const QString activeTopic = m_currentInteractTopicKey;
    auto conn = QSharedPointer<QMetaObject::Connection>::create();
    *conn = connect(m_http, &HttpClient::heroesFetched, this, [this, vbox, statusLabelPtr, conn, activeTopic](const QList<HeroBrief> &heroes){
        // 如果在请求返回前已切换话题，则丢弃此次结果
        if (m_currentInteractTopicKey != activeTopic) {
            QObject::disconnect(*conn);
            return;
        }
        // 清理旧条目
        for (auto *old : m_interactItems) delete old;
        m_interactItems.clear();

        QList<HeroBrief> list = heroes;
        std::sort(list.begin(), list.end(), [](const HeroBrief &a, const HeroBrief &b){ return a.id < b.id; });
        // 仅取前20个
        if (list.size() > 20) list = list.mid(0, 20);

        
        if (list.isEmpty()) {
            if (statusLabelPtr) statusLabelPtr->setText(tr("未获取到英雄数据"));
            QObject::disconnect(*conn);
            return;
        }

        // 容器：单列
        auto host = new QWidget(this);
        auto vbAll = new QVBoxLayout(host);
        vbAll->setContentsMargins(8, 8, 8, 8);
        vbAll->setSpacing(10);

        for (const auto &hb : list) {
            const int heroId = hb.id;

            auto row = new QWidget(host);
            auto h = new QHBoxLayout(row);
            h->setContentsMargins(6, 6, 6, 6);
            h->setSpacing(12);

            // 头像
            auto img = new QLabel(row);
            img->setFixedSize(48, 48);
            img->setStyleSheet("background:#1e2328;border:1px solid #3c4043;border-radius:6px;");
            img->setAlignment(Qt::AlignCenter);

            // 名称
            auto name = new QLabel(QString("#%1 %2").arg(heroId).arg(hb.name), row);
            name->setStyleSheet("color:#ccd6f6;font-size:14px;");

            // 星级按钮 0-5
            auto starBox = new QWidget(row);
            auto starLayout = new QHBoxLayout(starBox);
            starLayout->setContentsMargins(0,0,0,0);
            starLayout->setSpacing(4);

            InteractItem *it = new InteractItem();
            it->heroId = heroId;
            it->imageLabel = img;
            it->idLabel = name;
            it->selectedStars = 0;

            auto updateStars = [it]() {
                if (!it) return;
                for (int i = 0; i < it->starButtons.size(); ++i) {
                    auto *btn = it->starButtons.at(i);
                    const bool on = (i < it->selectedStars);
                    btn->setText(on ? QString::fromUtf8("★") : QString::fromUtf8("☆"));
                }
            };

            for (int s = 1; s <= 5; ++s) {
                auto btn = new QPushButton(QString::fromUtf8("☆"), starBox);
                btn->setFixedWidth(24);
                btn->setStyleSheet("QPushButton{color:#f6e05e;background:transparent;border:none;font-size:18px;} QPushButton:hover{color:#ffe08a;}");
                connect(btn, &QPushButton::clicked, this, [it, s, updateStars]() mutable {
                    it->selectedStars = s;
                    updateStars();
                });
                it->starButtons.append(btn);
                starLayout->addWidget(btn);
            }

            // 评论输入（仅字母数字中文空格，<=50）
            auto edit = new QLineEdit(row);
            edit->setPlaceholderText(tr("评论（50字以内）"));
            edit->setMaxLength(50);
            edit->setStyleSheet("QLineEdit{background:#11161b;color:#e2e8f0;border:none;border-radius:4px;padding:4px 8px;}");
            // 允许自由输入，提交时再校验

            it->commentEdit = edit;

            // 均分
            auto avg = new QLabel(tr("均分 -- 参与 --"), row);
            avg->setStyleSheet("color:#94a3b8;");
            it->avgLabel = avg;

            // 提交按钮
            auto submit = new QPushButton(tr("提交"), row);
            submit->setStyleSheet("QPushButton{color:#0a0e13;background:#00d4ff;border:none;border-radius:4px;padding:4px 10px;font-weight:bold;} QPushButton:hover{background:#67e8f9;}");
            connect(submit, &QPushButton::clicked, this, [this, heroId]() { submitHeroRating(heroId); });

            // 详情按钮：进入该英雄的评分详情页（展示全部评论和统计）
            auto detailBtn = new QPushButton(tr("详情"), row);
            detailBtn->setStyleSheet("QPushButton{color:#cbd5e1;background:#374151;border:none;border-radius:4px;padding:4px 10px;} QPushButton:hover{background:#4b5563;}");
            connect(detailBtn, &QPushButton::clicked, this, [this, heroId]() { buildHeroRatingDetail(heroId); });

            h->addWidget(img, 0);
            h->addWidget(name, 0);
            h->addWidget(starBox, 0);
            h->addWidget(edit, 1);
            h->addWidget(avg, 0);
            h->addWidget(submit, 0);
            h->addWidget(detailBtn, 0);

            vbAll->addWidget(row);

            // 简单评论区（显示最近提交的若干条：头像占位 + 用户ID + 评论）
            auto comments = new QWidget(host);
            auto commentsLayout = new QVBoxLayout(comments);
            commentsLayout->setContentsMargins(54, 0, 0, 8); // 与头像对齐缩进
            commentsLayout->setSpacing(4);
            comments->setStyleSheet("QWidget{background:transparent;}");
            it->commentsHost = comments;
            vbAll->addWidget(comments);

            // 下载头像并在匹配 heroId 时设置
            auto imgConn = QSharedPointer<QMetaObject::Connection>::create();
            QPointer<QLabel> imgPtr(img);
            *imgConn = connect(m_http, &HttpClient::imageDownloaded, this,
                               [imgPtr, heroId, imgConn](int fetchedId, const QPixmap &pixmap){
                                   if (fetchedId == heroId && imgPtr) {
                                       imgPtr->setPixmap(pixmap.scaled(48, 48, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
                                       QObject::disconnect(*imgConn);
                                   }
                               });

            if (!hb.squareImageUrl.isEmpty()) {
                m_http->downloadImage(hb.squareImageUrl, heroId);
                if (!hb.squareImageUrl.startsWith("/static")) {
                    m_http->downloadImage(QString("/static/hero_squares/%1.png").arg(heroId), heroId);
                }
            } else {
                m_http->downloadImage(QString("/static/hero_squares/%1.png").arg(heroId), heroId);
            }

            m_interactItems.append(it);
            // 拉取汇总与最新评论
            m_http->fetchRatingSummary(heroId, m_currentInteractTopicKey);
            m_http->fetchRatings(heroId, m_currentInteractTopicKey);
        }

        // 更新加载文案
        if (statusLabelPtr) {
            statusLabelPtr->setText(tr("加载完成：共%1个英雄").arg(list.size()));
        }
        // 使用可滚动容器承载列表，避免内容高度过大时不可见
        auto scroll = new QScrollArea(this);
        scroll->setWidgetResizable(true);
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setStyleSheet("QScrollArea{background:transparent;border:none;} QScrollBar:vertical{background:#1e2328;width:12px;border-radius:6px;} QScrollBar::handle:vertical{background:#4a5568;border-radius:6px;min-height:20px;} QScrollBar::handle:vertical:hover{background:#00d4ff;} QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical{height:0px;}");
        scroll->setWidget(host);
        vbox->addWidget(scroll, 1);
        QObject::disconnect(*conn);
    }, Qt::SingleShotConnection);

    
    auto errConn = QSharedPointer<QMetaObject::Connection>::create();
    *errConn = connect(m_http, &HttpClient::requestFailed, this, [statusLabelPtr, errConn](const QString &msg){
        if (statusLabelPtr) statusLabelPtr->setText(QObject::tr("加载失败：%1").arg(msg));
        QObject::disconnect(*errConn);
    }, Qt::SingleShotConnection);

    
    QTimer::singleShot(5000, this, [statusLabelPtr]() {
        if (statusLabelPtr && statusLabelPtr->text() == QObject::tr("正在加载...")) {
            statusLabelPtr->setText(QObject::tr("加载超时，请稍后重试"));
        }
    });

    
    m_http->fetchHeroes(QString(), QString());
}

void Widget::requestAllSummaries()
{
    for (auto *it : m_interactItems) {
        m_http->fetchRatingSummary(it->heroId, m_currentInteractTopicKey);
    }
}

void Widget::buildHeroRatingDetail(int heroId)
{
    // 进入详情前，清理互动列表状态，避免全局槽根据 heroId 命中已销毁的行
    for (auto *old : m_interactItems) delete old;
    m_interactItems.clear();
    // 切到互动页的容器并清空
    ui->stackedMain->setCurrentWidget(ui->pageInteract);
    auto vbox = ui->verticalLayout_Interact;
    while (QLayoutItem *child = vbox->takeAt(0)) {
        if (child->widget()) child->widget()->deleteLater();
        delete child;
    }

    // 标题 + 返回
    auto header = new QWidget(this);
    auto hh = new QHBoxLayout(header);
    hh->setContentsMargins(0,0,0,0);
    auto title = new QLabel(tr("评分详情"), header);
    title->setStyleSheet("font-size:18px;color:#ccd6f6;font-weight:bold;");
    auto backBtn = new QPushButton(tr("返回话题"), header);
    backBtn->setStyleSheet("QPushButton{color:#c8aa6e;background:#1e2328;border:1px solid #463714;border-radius:4px;padding:4px 10px;} QPushButton:hover{background:#463714;}");
    connect(backBtn, &QPushButton::clicked, this, [this]() { buildInteractTopicDetail(m_currentInteractTopicKey); });
    hh->addWidget(title);
    hh->addStretch();
    hh->addWidget(backBtn);
    vbox->addWidget(header);

    // 顶部统计区 + 底部评论区（1/2 高度给评论列表）
    auto splitHost = new QWidget(this);
    auto splitLayout = new QVBoxLayout(splitHost);
    splitLayout->setContentsMargins(0,0,0,0);
    splitLayout->setSpacing(8);
    vbox->addWidget(splitHost, 1);

    // 统计区
    auto statHost = new QWidget(splitHost);
    auto statLayout = new QVBoxLayout(statHost);
    statLayout->setContentsMargins(8, 8, 8, 8);
    statLayout->setSpacing(6);
    statHost->setStyleSheet("QWidget{background:#11161b;border-radius:6px;}");
    auto avgLabel = new QLabel(tr("均分 -- 参与 --"), statHost);
    avgLabel->setStyleSheet("color:#cbd5e1;font-size:14px;font-weight:bold;");
    statLayout->addWidget(avgLabel);

    // 5星到1星的分布条
    QList<QProgressBar*> bars;
    for (int s = 5; s >= 1; --s) {
        auto row = new QWidget(statHost);
        auto h = new QHBoxLayout(row);
        h->setContentsMargins(0,0,0,0);
        h->setSpacing(8);
        auto star = new QLabel(QString::number(s) + tr(" 星"), row);
        star->setStyleSheet("color:#94a3b8;min-width:32px;");
        auto bar = new QProgressBar(row);
        bar->setRange(0, 100);
        bar->setValue(0);
        bar->setTextVisible(false);
        bar->setStyleSheet("QProgressBar{background:#1f2937;border:none;border-radius:4px;height:10px;} QProgressBar::chunk{background:#60a5fa;border-radius:4px;}");
        h->addWidget(star, 0);
        h->addWidget(bar, 1);
        statLayout->addWidget(row);
        bars.append(bar);
    }
    splitLayout->addWidget(statHost, 1);

    // 评论列表容器
    auto commentsContainer = new QWidget(splitHost);
    auto commentsContainerLayout = new QVBoxLayout(commentsContainer);
    commentsContainerLayout->setContentsMargins(0,0,0,0);
    commentsContainerLayout->setSpacing(8);
    
    // 分页控件
    auto paginationHost = new QWidget(commentsContainer);
    auto paginationLayout = new QHBoxLayout(paginationHost);
    paginationLayout->setContentsMargins(8,4,8,4);
    paginationLayout->setSpacing(8);
    
    m_prevPageBtn = new QPushButton(tr("上一页"), paginationHost);
    m_prevPageBtn->setStyleSheet("QPushButton{color:#cbd5e1;background:#374151;border:none;border-radius:4px;padding:4px 12px;} QPushButton:hover{background:#4b5563;} QPushButton:disabled{color:#6b7280;background:#1f2937;}");
    connect(m_prevPageBtn, &QPushButton::clicked, this, &Widget::onPrevPageClicked);
    
    m_pageLabel = new QLabel(tr("第 1 页 / 共 1 页"), paginationHost);
    m_pageLabel->setStyleSheet("color:#9ca3af;font-size:12px;");
    m_pageLabel->setAlignment(Qt::AlignCenter);
    
    m_nextPageBtn = new QPushButton(tr("下一页"), paginationHost);
    m_nextPageBtn->setStyleSheet("QPushButton{color:#cbd5e1;background:#374151;border:none;border-radius:4px;padding:4px 12px;} QPushButton:hover{background:#4b5563;} QPushButton:disabled{color:#6b7280;background:#1f2937;}");
    connect(m_nextPageBtn, &QPushButton::clicked, this, &Widget::onNextPageClicked);
    
    paginationLayout->addStretch();
    paginationLayout->addWidget(m_prevPageBtn);
    paginationLayout->addWidget(m_pageLabel);
    paginationLayout->addWidget(m_nextPageBtn);
    paginationLayout->addStretch();
    
    commentsContainerLayout->addWidget(paginationHost);
    
    // 评论列表
    auto listScroll = new QScrollArea(commentsContainer);
    listScroll->setWidgetResizable(true);
    listScroll->setFrameShape(QFrame::NoFrame);
    listScroll->setStyleSheet("QScrollArea{background:transparent;border:none;}");
    auto listHost = new QWidget(listScroll);
    auto listLayout = new QVBoxLayout(listHost);
    listLayout->setContentsMargins(8,8,8,8);
    listLayout->setSpacing(8);
    listScroll->setWidget(listHost);
    commentsContainerLayout->addWidget(listScroll, 1);
    
    // 保存布局引用供后续使用
    m_commentsLayout = listLayout;
    
    splitLayout->addWidget(commentsContainer, 2); // 评论占更大高度
    
    // 初始化分页状态
    m_currentPage = 0;
    m_totalPages = 1;
    m_allRatings.clear();

    // 取数据：汇总 + 全部评论
    m_http->fetchRatingSummary(heroId, m_currentInteractTopicKey);
    m_http->fetchRatings(heroId, m_currentInteractTopicKey);

    // 更新均分：绑定到 HttpClient 的信号，并以 avgLabel 作为上下文，避免视图销毁后回调悬空。
    // 不使用 SingleShot，避免先收到其他 heroId 的数据导致过早断开；在正确 heroId 到来后显式断开。
    {
        auto avgConn = QSharedPointer<QMetaObject::Connection>::create();
        QPointer<QLabel> avgLabelPtr(avgLabel);
        *avgConn = connect(m_http, &HttpClient::ratingSummaryFetched, avgLabel,
                           [avgLabelPtr, heroId, avgConn](int hId, double avg, int count){
                               if (!avgLabelPtr) { QObject::disconnect(*avgConn); return; }
                               if (hId != heroId) return;
                               avgLabelPtr->setText(QObject::tr("均分 %1 参与 %2").arg(QString::number(avg, 'f', 1)).arg(count));
                               QObject::disconnect(*avgConn);
                           });
    }

    // 渲染全部评论并计算分布：以 listHost 作为上下文，避免视图销毁后回调悬空。
    // 不使用 SingleShot，避免先收到其他 heroId 的数据导致过早断开；在正确 heroId 到来后显式断开。
    {
        auto listConn = QSharedPointer<QMetaObject::Connection>::create();
        QPointer<QWidget> listHostPtr(listHost);
        *listConn = connect(m_http, &HttpClient::ratingsFetched, listHost, [this, heroId, listLayout, bars, listConn, listHostPtr](int hid, const QList<UserRating> &items){
        if (!listHostPtr) { QObject::disconnect(*listConn); return; }
        if (hid != heroId) return;
        // 清空
        QLayoutItem *child;
        while ((child = listLayout->takeAt(0)) != nullptr) {
            if (child->widget()) child->widget()->deleteLater();
            delete child;
        }
        // 统计分布
        int counts[6] = {0,0,0,0,0,0};
        for (const auto &r : items) {
            if (r.stars >= 0 && r.stars <= 5) counts[r.stars]++;
        }
        const int total = items.size();
        for (int s = 5, idx = 0; s >= 1; --s, ++idx) {
            int cnt = counts[s];
            int percent = (total > 0) ? int(double(cnt) / double(total) * 100.0 + 0.5) : 0;
            if (idx >= 0 && idx < bars.size()) bars[idx]->setValue(percent);
        }
        // 存储所有评论数据
        m_allRatings = items;
        
        // 计算总页数
        m_totalPages = (items.size() + COMMENTS_PER_PAGE - 1) / COMMENTS_PER_PAGE;
        if (m_totalPages == 0) m_totalPages = 1;
        
        // 重置到第一页
        m_currentPage = 0;
        
        // 显示当前页的评论
        updateCommentsDisplay();
        
        // 更新分页控件状态
        updatePaginationControls();
        QObject::disconnect(*listConn);
    });
    }
}

void Widget::submitHeroRating(int heroId)
{
    if (m_token.isEmpty()) {
        QMessageBox::warning(this, tr("错误"), tr("请先登录"));
        return;
    }
    for (auto *it : m_interactItems) {
        if (it->heroId == heroId) {
            const int stars = it->selectedStars;
            if (stars < 0 || stars > 5) {
                QMessageBox::warning(this, tr("错误"), tr("请选择0-5星"));
                return;
            }
            // 仅允许字母数字中文空格
            const QString text = it->commentEdit->text();
            QRegularExpression re("^[\\w\u4e00-\u9fa5 ]{0,50}$");
            if (!re.match(text).hasMatch()) {
                QMessageBox::warning(this, tr("错误"), tr("评论仅允许字母数字中文与空格，50字内"));
                return;
            }
            m_http->submitRating(heroId, stars, text, m_currentInteractTopicKey.isEmpty() ? QStringLiteral("hot_heroes") : m_currentInteractTopicKey);
            return;
        }
    }
}

void Widget::onRatingSubmitted(int heroId, int stars)
{
    Q_UNUSED(stars);
    
    m_http->fetchRatingSummary(heroId, m_currentInteractTopicKey);
    
    m_http->fetchRatings(heroId, m_currentInteractTopicKey);
    
    for (auto *it : m_interactItems) {
        if (it->heroId == heroId && it->commentEdit) {
            it->commentEdit->clear();
            break;
        }
    }
}

void Widget::onRatingSummaryFetched(int heroId, double avg, int count)
{
    for (auto *it : m_interactItems) {
        if (it->heroId == heroId && it->avgLabel) {
            it->avgLabel->setText(tr("均分 %1 参与 %2").arg(QString::number(avg, 'f', 1)).arg(count));
            break;
        }
    }
}

void Widget::onRatingsFetched(int heroId, const QList<UserRating> &items)
{
        for (auto *it : m_interactItems) {
            if (it->heroId != heroId || !it->commentsHost) continue;
        // 清空现有
            if (auto lay = it->commentsHost->layout()) {
                while (QLayoutItem *child = lay->takeAt(0)) {
                    if (child->widget()) child->widget()->deleteLater();
                    delete child;
                }
            }
            int shown = 0;
            for (const auto &r : items) {
            if (shown >= 1) break; // 仅显示最新1条
                auto row = new QWidget(it->commentsHost);
                auto h = new QHBoxLayout(row);
                h->setContentsMargins(0,0,0,0);
                h->setSpacing(6);
                auto avatar = new QLabel(row);
                avatar->setFixedSize(20,20);
                avatar->setStyleSheet("background:#24303a;border:none;border-radius:10px;");
            
            // 如果有头像URL，则下载并设置头像
                if (!r.avatarUrl.isEmpty()) {
                    auto avatarConn = QSharedPointer<QMetaObject::Connection>::create();
                    QPointer<QLabel> avatarPtr(avatar);
                    *avatarConn = connect(m_http, &HttpClient::imageDownloadedByUrl, this,
                                         [avatarPtr, r, avatarConn](const QString &url, const QPixmap &pixmap){
                                             if (url == r.avatarUrl && avatarPtr) {
                                                 avatarPtr->setPixmap(pixmap.scaled(20, 20, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
                                                 QObject::disconnect(*avatarConn);
                                             }
                                         });
                    m_http->downloadImageByUrl(r.avatarUrl);
                }
            
            // 显示用户名，如果没有则显示UID
                QString displayName = r.username.isEmpty() ? QString("UID %1").arg(r.userId) : r.username;
                auto username = new QLabel(displayName, row);
                username->setStyleSheet("color:#9aa4b2;font-size:12px;font-weight:bold;");
                auto text = new QLabel(r.comment, row);
                text->setStyleSheet("color:#cbd5e1;font-size:12px;");
                text->setWordWrap(true);
                h->addWidget(avatar, 0);
                h->addWidget(username, 0);
                h->addWidget(text, 1);
                it->commentsHost->layout()->addWidget(row);
                ++shown;
            }
        break;
    }
}
void Widget::setFilterValuesForRoute()
{
    ui->comboFilterValue->clear();
    ui->comboFilterValue->addItems(QStringList() << tr("上单") << tr("中单") << tr("下路") << tr("打野") << tr("辅助"));
    ui->comboFilterValue->setCurrentIndex(0);
}

void Widget::setFilterValuesForClass()
{
    ui->comboFilterValue->clear();
    ui->comboFilterValue->addItems(QStringList() << tr("坦克") << tr("战士") << tr("刺客") << tr("射手") << tr("法师"));
    ui->comboFilterValue->setCurrentIndex(0);
}

QString Widget::currentFilterTypeKey() const
{
    // 0: 显示所有, 1: 路线定位, 2: 功能定位
    if (ui->comboFilterType->currentIndex() == 0) {
        return QString(); // 返回空字符串表示不筛选
    } else if (ui->comboFilterType->currentIndex() == 1) {
        return QStringLiteral("route");
    } else {
        return QStringLiteral("class");
    }
}

void Widget::updateCommentsDisplay()
{
    if (!m_commentsLayout) return;
    
    // 清空现有评论
    QLayoutItem *child;
    while ((child = m_commentsLayout->takeAt(0)) != nullptr) {
                if (child->widget()) child->widget()->deleteLater();
                delete child;
            }
    
    // 计算当前页的评论范围
    int startIdx = m_currentPage * COMMENTS_PER_PAGE;
    int endIdx = qMin(startIdx + COMMENTS_PER_PAGE, m_allRatings.size());
    
    // 渲染当前页的评论
    for (int i = startIdx; i < endIdx; ++i) {
        const auto &r = m_allRatings[i];
        auto row = new QWidget(m_commentsLayout->parentWidget());
            auto h = new QHBoxLayout(row);
        h->setContentsMargins(0,0,0,0);
            h->setSpacing(6);
            auto avatar = new QLabel(row);
        avatar->setFixedSize(24,24);
        avatar->setStyleSheet("background:#24303a;border:none;border-radius:12px;");
        
            if (!r.avatarUrl.isEmpty()) {
                auto avatarConn = QSharedPointer<QMetaObject::Connection>::create();
                QPointer<QLabel> avatarPtr(avatar);
                *avatarConn = connect(m_http, &HttpClient::imageDownloadedByUrl, this,
                                     [avatarPtr, r, avatarConn](const QString &url, const QPixmap &pixmap){
                                         if (url == r.avatarUrl && avatarPtr) {
                        avatarPtr->setPixmap(pixmap.scaled(24, 24, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
                                             QObject::disconnect(*avatarConn);
                                         }
                                     });
                m_http->downloadImageByUrl(r.avatarUrl);
            }
        
            QString displayName = r.username.isEmpty() ? QString("UID %1").arg(r.userId) : r.username;
        auto meta = new QLabel(QString("%1  ·  %2★").arg(displayName).arg(r.stars), row);
        meta->setStyleSheet("color:#9aa4b2;font-size:12px;font-weight:bold;");
        auto text = new QLabel(r.comment, row);
        text->setStyleSheet("color:#cbd5e1;font-size:12px;");
            text->setWordWrap(true);
            h->addWidget(avatar, 0);
        h->addWidget(meta, 0);
        h->addWidget(text, 1);
        m_commentsLayout->addWidget(row);
    }
    
    m_commentsLayout->addStretch();
}

void Widget::onPrevPageClicked()
{
    if (m_currentPage > 0) {
        m_currentPage--;
        updateCommentsDisplay();
        updatePaginationControls();
    }
}

void Widget::onNextPageClicked()
{
    if (m_currentPage < m_totalPages - 1) {
        m_currentPage++;
        updateCommentsDisplay();
        updatePaginationControls();
    }
}

void Widget::updatePaginationControls()
{
    if (!m_pageLabel || !m_prevPageBtn || !m_nextPageBtn) return;
    
    // 更新页码显示
    m_pageLabel->setText(tr("第 %1 页 / 共 %2 页").arg(m_currentPage + 1).arg(m_totalPages));
    
    // 更新按钮状态
    m_prevPageBtn->setEnabled(m_currentPage > 0);
    m_nextPageBtn->setEnabled(m_currentPage < m_totalPages - 1);
}

QString Widget::currentFilterValueKey() const
{
    // 如果选择了"显示所有"，返回空字符串
    if (ui->comboFilterType->currentIndex() == 0) {
        return QString();
    }
    
    const QString v = ui->comboFilterValue->currentText();
    
    if (ui->comboFilterType->currentIndex() == 1) {
        
        if (v == tr("上单")) return "top";
        if (v == tr("中单")) return "mid";
        if (v == tr("下路")) return "bottom";
        if (v == tr("打野")) return "jungle";
        if (v == tr("辅助")) return "support";
    } else if (ui->comboFilterType->currentIndex() == 2) {
        
        if (v == tr("坦克")) return "tank";
        if (v == tr("战士")) return "fighter";
        if (v == tr("刺客")) return "assassin";
        if (v == tr("射手")) return "marksman";
        if (v == tr("法师")) return "mage";
    }
    return v;
}

void Widget::loadAuthFromSettings()
{
    QSettings s;
    const QString token = s.value("auth/token").toString();
    const QString username = s.value("auth/username").toString();
    const QString avatarUrl = s.value("auth/avatar_url").toString();
    if (!token.isEmpty() && !username.isEmpty()) {
        m_token = token;
        m_currentUsername = username;
        m_currentAvatarUrl = avatarUrl;
        m_http->setAuthToken(token);
        if (auto badge = findChild<UserBadgeWidget*>("userBadge")) {
            badge->setUsername(username);
            if (!avatarUrl.isEmpty()) {
                m_http->downloadImage(avatarUrl, -1);
                connect(m_http, &HttpClient::imageDownloaded, this, [this, badge](int heroId, const QPixmap &pixmap){
                    if (heroId == -1) badge->setAvatar(pixmap);
                }, Qt::SingleShotConnection);
            } else {
                
                m_http->fetchUserProfile();
                connect(m_http, &HttpClient::userProfileFetched, this, [this, badge](const QString &uname, int level, const QString &avatar){
                    Q_UNUSED(uname); Q_UNUSED(level);
                    if (!avatar.isEmpty()) {
                        m_currentAvatarUrl = avatar;
                        m_http->downloadImage(avatar, -1);
                        connect(m_http, &HttpClient::imageDownloaded, this, [this, badge](int heroId, const QPixmap &pixmap){
                            if (heroId == -1) badge->setAvatar(pixmap);
                        }, Qt::SingleShotConnection);
                    }
                }, Qt::SingleShotConnection);
            }
        }
    }
}

void Widget::saveAuthToSettings(const QString &token, const QString &username, const QString &avatarUrl)
{
    QSettings s;
    s.setValue("auth/token", token);
    s.setValue("auth/username", username);
    s.setValue("auth/avatar_url", avatarUrl);
}

void Widget::clearAuthInSettings()
{
    QSettings s;
    s.remove("auth/token");
    s.remove("auth/username");
}

void Widget::onAvatarUploadRequested()
{
    if (m_token.isEmpty()) {
        QMessageBox::warning(this, tr("错误"), tr("请先登录"));
        return;
    }
    
    // 连接头像变更信号（只连接一次）
    connect(m_avatarWidget, &AvatarWidget::avatarChanged, this, &Widget::onAvatarChanged, Qt::UniqueConnection);
    
    // 设置当前用户信息
    m_avatarWidget->setUsername(m_currentUsername);
    m_avatarWidget->setAvatarUrl(m_currentAvatarUrl);
    
    // 显示头像上传对话框
    m_avatarWidget->setWindowTitle(tr("更换头像"));
    m_avatarWidget->exec();  // 使用exec()显示为模态对话框
}

void Widget::onAvatarChanged(const QString &avatarUrl)
{
    m_currentAvatarUrl = avatarUrl;
    
    // 下载新的头像并更新到用户徽章
    if (auto badge = findChild<UserBadgeWidget*>("userBadge")) {
        // 通过HttpClient下载图片
        
        m_http->downloadImage(avatarUrl, -1);
        connect(m_http, &HttpClient::imageDownloaded, this, [this, badge](int heroId, const QPixmap &pixmap){
            if (heroId == -1) {
                badge->setAvatar(pixmap);
            }
        }, Qt::SingleShotConnection);
    }
    
    // 对话框会在上传成功后自动关闭
}

bool Widget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QWidget *widget = qobject_cast<QWidget*>(obj);
        if (widget && widget->property("topicKey").isValid()) {
            QString topicKey = widget->property("topicKey").toString();
            buildInteractTopicDetail(topicKey);
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}
