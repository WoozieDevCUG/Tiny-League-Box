#include "herodetailwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QProgressBar>
#include <QFrame>
#include <QJsonObject>
#include <QJsonArray>
#include <QPainter>
#include <QMouseEvent>
#include <QTimer>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QtMath>
#include <QRegularExpression>
#include <QSlider>
#include <QIcon>
#include <QSize>
#include "radarchartwidget.h"
#include "typeindicatorwidget.h"
#include "parallelogrambar.h"

 

HeroDetailWidget::HeroDetailWidget(QWidget *parent)
    : QWidget(parent)
    , m_backButton(nullptr)
    , m_heroImageLabel(nullptr)
    , m_prevSkinButton(nullptr)
    , m_nextSkinButton(nullptr)
    , m_heroDescriptionLabel(nullptr)
    , m_skillsWidget(nullptr)
    , m_skillsLayout(nullptr)
    , m_skillTooltip(nullptr)
    , m_currentSkinIndex(0)
{
    setupUI();
}

void HeroDetailWidget::setupUI()
{
    // 与英雄界面一致的深色背景
    setStyleSheet("HeroDetailWidget { background: #0a0e13; }");
    
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    // 返回按钮
    m_backButton = new QPushButton("← 返回英雄列表", this);
    m_backButton->setFixedHeight(36);
    m_backButton->setStyleSheet(
        "QPushButton {"
        "  background: #1e2328;"
        "  border: 2px solid #463714;"
        "  border-radius: 4px;"
        "  color: #cdbe91;"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "  padding: 0 20px;"
        "}"
        "QPushButton:hover {"
        "  background: #463714;"
        "  border-color: #c8aa6e;"
        "}"
        "QPushButton:pressed {"
        "  background: #5bc0de;"
        "}"
    );
    connect(m_backButton, &QPushButton::clicked, this, &HeroDetailWidget::onBackClicked);
    mainLayout->addWidget(m_backButton, 0, Qt::AlignLeft);

    auto *contentWidget = new QWidget();
    
    contentWidget->setStyleSheet("QWidget { background: #0a0e13; }");
    auto *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(16);

    
    auto *heroTopWidget = new QWidget();
    heroTopWidget->setStyleSheet("QWidget { background: transparent; }");
    auto *heroTopLayout = new QHBoxLayout(heroTopWidget);
    heroTopLayout->setSpacing(16);

    
    auto *imageSection = new QWidget();
    imageSection->setStyleSheet("QWidget { background: transparent; }");
    imageSection->setFixedWidth(460);
    auto *imageLayout = new QVBoxLayout(imageSection);
    
    
    auto *skinNavWidget = new QWidget();
    auto *skinNavLayout = new QHBoxLayout(skinNavWidget);
    skinNavLayout->setSpacing(8);
    
    m_prevSkinButton = new QPushButton("◀", this);
    m_nextSkinButton = new QPushButton("▶", this);
    
    QString skinButtonStyle = 
        "QPushButton {"
        "  background: rgba(30, 35, 40, 180);"
        "  border: 2px solid #463714;"
        "  border-radius: 20px;"
        "  color: #cdbe91;"
        "  font-size: 16px;"
        "  font-weight: bold;"
        "  min-width: 32px;"
        "  min-height: 32px;"
        "}"
        "QPushButton:hover {"
        "  background: rgba(70, 55, 20, 200);"
        "  border-color: #c8aa6e;"
        "}"
        "QPushButton:pressed {"
        "  background: #5bc0de;"
        "}";
    
    m_prevSkinButton->setStyleSheet(skinButtonStyle);
    m_nextSkinButton->setStyleSheet(skinButtonStyle);
    
    connect(m_prevSkinButton, &QPushButton::clicked, this, &HeroDetailWidget::onPreviousSkinClicked);
    connect(m_nextSkinButton, &QPushButton::clicked, this, &HeroDetailWidget::onNextSkinClicked);
    
    
    m_heroImageLabel = new QLabel();
    m_heroImageLabel->setFixedSize(308, 560);
    m_heroImageLabel->setScaledContents(true);
    m_heroImageLabel->setAlignment(Qt::AlignCenter);
    m_heroImageLabel->setStyleSheet(
        "QLabel {"
        "  border: 3px solid #463714;"
        "  border-radius: 8px;"
        "  background: #1e2328;"
        "}"
    );
    
    {
        const QSize phSize = m_heroImageLabel->size();
        QPixmap ph(phSize);
        ph.fill(QColor("#1e2328"));
        QPainter pp(&ph);
        pp.setRenderHint(QPainter::Antialiasing, true);
        QRect r(8, 8, phSize.width() - 16, phSize.height() - 16);
        pp.setPen(QPen(QColor("#463714"), 2));
        pp.drawRoundedRect(r, 8, 8);
        QFont f = pp.font(); f.setPointSize(14); f.setBold(true); pp.setFont(f);
        pp.setPen(QColor("#c8aa6e"));
        pp.drawText(QRect(0, 0, phSize.width(), phSize.height()), Qt::AlignCenter, QStringLiteral("图片占位"));
        pp.end();
        m_heroImageLabel->setPixmap(ph);
    }
    
    skinNavLayout->addWidget(m_prevSkinButton);
    skinNavLayout->addWidget(m_heroImageLabel);
    skinNavLayout->addWidget(m_nextSkinButton);
    
    imageLayout->addWidget(skinNavWidget);
    // 皮肤名称
    m_skinNameLabel = new QLabel("皮肤名称");
    m_skinNameLabel->setAlignment(Qt::AlignCenter);
    m_skinNameLabel->setStyleSheet("QLabel { color: #cdbe91; font-size: 13px; margin-top: 6px; }");
    imageLayout->addWidget(m_skinNameLabel);
    
    
    auto *infoSection = new QWidget();
    infoSection->setStyleSheet("QWidget { background: transparent; }");
    auto *infoLayout = new QVBoxLayout(infoSection);
    infoLayout->setSpacing(8);

    m_heroDescriptionLabel = new QLabel();
    m_heroDescriptionLabel->setWordWrap(true);
    m_heroDescriptionLabel->setStyleSheet(
        "QLabel {"
        "  font-size: 13px;"
        "  color: #a09b8c;"
        "  line-height: 1.6;"
        "  background: transparent;"
        "  border: none;"
        "}"
    );
    
    
    auto *storyPanel = new QWidget();
    storyPanel->setObjectName("storyPanel");
    storyPanel->setStyleSheet(
        "#storyPanel { background: #1e2328; border: 2px solid #463714; border-radius: 8px; }"
    );
    auto *storyLayout = new QVBoxLayout(storyPanel);
    storyLayout->setContentsMargins(12, 10, 12, 10);
    storyLayout->setSpacing(8);
    auto *storyTitle = new QLabel(QStringLiteral("英雄故事"));
    storyTitle->setStyleSheet("QLabel { font-size: 16px; font-weight: bold; color: #c8aa6e; }");
    storyLayout->addWidget(storyTitle);
    storyLayout->addWidget(m_heroDescriptionLabel);
    infoLayout->addWidget(storyPanel);
    
    
    setupSkillsSection();
    infoLayout->addWidget(m_skillsWidget);

    
    auto *radarRow = new QWidget();
    auto *radarRowLayout = new QHBoxLayout(radarRow);
    radarRowLayout->setSpacing(24);
    radarRowLayout->setContentsMargins(0, 0, 0, 0);

    m_radarWidget = new RadarChartWidget();
    radarRowLayout->addWidget(m_radarWidget, 0, Qt::AlignLeft | Qt::AlignVCenter);

    
    m_radarRightPanel = new QWidget();
    auto *rightPanelLayout = new QVBoxLayout(m_radarRightPanel);
    rightPanelLayout->setContentsMargins(0, 12, 0, 0); // 整体向下偏移
    rightPanelLayout->setSpacing(10);

    
    {
        auto *typeBox = new QWidget();
        auto *typeLayout = new QVBoxLayout(typeBox);
        typeLayout->setContentsMargins(0, 0, 0, 0);
        typeLayout->setSpacing(6);
        m_typeBar = new TypeIndicatorWidget();
        typeLayout->addWidget(m_typeBar);
        rightPanelLayout->addWidget(typeBox);
    }

    
    {
        auto *diffBox = new QWidget();
        auto *diffLayoutCol = new QVBoxLayout(diffBox);
        diffLayoutCol->setContentsMargins(0, 0, 0, 0);
        diffLayoutCol->setSpacing(6);
        auto *lbl = new QLabel(QStringLiteral("难度"));
        lbl->setStyleSheet("QLabel { color: #c8aa6e; font-weight: bold; }");
        m_difficultyWidget = new QWidget();
        auto *diffLayout = new QHBoxLayout(m_difficultyWidget);
        diffLayout->setContentsMargins(0, 0, 0, 0);
        diffLayout->setSpacing(-4); // 使用负间距以消除斜边缝隙
        m_difficultySegments.clear();
        for (int i = 0; i < 3; ++i) {
            auto *seg = new ParallelogramBar();
            seg->setFixedSize(72, 10);
            m_difficultySegments.append(seg);
            diffLayout->addWidget(seg);
        }
        diffLayoutCol->addWidget(lbl);
        diffLayoutCol->addWidget(m_difficultyWidget);
        rightPanelLayout->addWidget(diffBox);
    }

    radarRowLayout->addWidget(m_radarRightPanel, 0, Qt::AlignVCenter);
    infoLayout->addWidget(radarRow);
    infoLayout->addStretch();
    
    heroTopLayout->addWidget(imageSection);
    heroTopLayout->addWidget(infoSection, 1);
    contentLayout->addWidget(heroTopWidget);
    
    

    

    
    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet(
        "QScrollArea { background: #0a0e13; border: none; }"
        "QWidget#qt_scrollarea_viewport { background: #0a0e13; }"
    );
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidget(contentWidget);

    mainLayout->addWidget(scrollArea, 1);
}

void HeroDetailWidget::setupSkillsSection()
{
    m_skillsWidget = new QWidget();
    auto *layout = new QVBoxLayout(m_skillsWidget);
    layout->setSpacing(12);
    
    // 标题
    auto *titleLabel = new QLabel("英雄技能");
    titleLabel->setStyleSheet(
        "QLabel {"
        "  font-size: 18px;"
        "  font-weight: bold;"
        "  color: #c8aa6e;"
        "  padding-bottom: 6px;"
        "  border-bottom: 2px solid #463714;"
        "}"
    );
    layout->addWidget(titleLabel);
    
    // 技能容器
    auto *skillsContainer = new QWidget();
    m_skillsLayout = new QHBoxLayout(skillsContainer);
    m_skillsLayout->setSpacing(12);
    m_skillsLayout->setAlignment(Qt::AlignCenter);
    
    layout->addWidget(skillsContainer);
    
    
    m_skillTooltip = new QLabel(this);
    m_skillTooltip->setWordWrap(true);
    m_skillTooltip->setMaximumWidth(320);
    m_skillTooltip->setTextFormat(Qt::RichText);
    m_skillTooltip->setStyleSheet(
        "QLabel {"
        "  background: rgba(30, 35, 40, 240);"
        "  border: 2px solid #c8aa6e;"
        "  border-radius: 8px;"
        "  color: #ffffff;"
        "  font-size: 13px;"
        "  padding: 12px;"
        "  line-height: 1.4;"
        "}"
    );
    m_skillTooltip->hide();
    
    
    auto *shadowEffect = new QGraphicsDropShadowEffect();
    shadowEffect->setBlurRadius(12);
    shadowEffect->setColor(QColor(0, 0, 0, 160));
    shadowEffect->setOffset(0, 4);
    m_skillTooltip->setGraphicsEffect(shadowEffect);
}

void HeroDetailWidget::setupRadarSection(){}

// 删除类型/难度、雷达和数值相关辅助方法

void HeroDetailWidget::createSkillIcon(const QString &skillType, const QString &skillName, const QString &description)
{
    auto *skillButton = new QPushButton();
    skillButton->setFixedSize(64, 64);
    // 不显示按键字符，仅显示图标
    skillButton->setText("");
    skillButton->setStyleSheet(
        "QPushButton {"
        "  background: #1e2328;"
        "  border: 2px solid #463714;"
        "  border-radius: 8px;"
        "  color: #c8aa6e;"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background: #463714;"
        "  border-color: #c8aa6e;"
        "  transform: scale(1.05);"
        "}"
    );
    
    // 存储技能数据
    skillButton->setProperty("skillType", skillType);
    skillButton->setProperty("skillName", skillName);
    skillButton->setProperty("skillDescription", description);
    
    // 连接悬停事件
    skillButton->installEventFilter(this);
    
    m_skillButtons[skillType] = skillButton;
    m_skillsLayout->addWidget(skillButton);
}

void HeroDetailWidget::setHeroData(const QJsonObject &heroData)
{
    m_heroData = heroData;
    
    // 设置描述
    if (heroData.contains("detail")) {
        QJsonObject detail = heroData["detail"].toObject();
        QString description = detail["description"].toString();
        m_heroDescriptionLabel->setText(description.isEmpty() ? "暂无英雄描述" : description);
    } else {
        m_heroDescriptionLabel->setText("暂无英雄描述");
    }
    
    // 皮肤集合
    m_skins.clear();
    if (heroData.contains("skins")) {
        QJsonArray skins = heroData["skins"].toArray();
        for (const auto &v : skins) {
            QJsonObject o = v.toObject();
            QString img = o.value("skin_image").toString();
            QString name = o.value("skin_name").toString();
            if (!img.isEmpty()) m_skins.append(qMakePair(img, name));
        }
        m_currentSkinIndex = 0;
        m_skinNameLabel->setText(heroData.value("title").toString());
    }

    // 清空现有技能
    for (auto *button : m_skillButtons.values()) {
        button->deleteLater();
    }
    m_skillButtons.clear();
    
    if (heroData.contains("detail")) {
        QJsonObject detail = heroData["detail"].toObject();
        struct SkillField { QString type; QString nameKey; QString resultKey; QString imageKey; };
        QList<SkillField> fields = {
            {"P", "passive_name", "passive_result", "passive_image"},
            {"Q", "q_name", "q_result", "q_image"},
            {"W", "w_name", "w_result", "w_image"},
            {"E", "e_name", "e_result", "e_image"},
            {"R", "r_name", "r_result", "r_image"}
        };

        for (const auto &f : fields) {
            QString fallbackName = (f.type == "P") ? QStringLiteral("被动") : f.type;
            QString name = detail.value(f.nameKey).toString();
            if (name.trimmed().isEmpty()) name = fallbackName;
            QString desc = detail.value(f.resultKey).toString();
            if (desc.trimmed().isEmpty()) desc = QStringLiteral("暂无技能描述");
            createSkillIcon(f.type, name, desc);
            if (m_skillButtons.contains(f.type)) {
                m_skillButtons[f.type]->setProperty("skillImageUrl", detail.value(f.imageKey).toString());
            }
        }
    } else if (heroData.contains("skills")) {
        const QJsonArray skills = heroData.value("skills").toArray();
        for (const auto &v : skills) {
            const QJsonObject o = v.toObject();
            const QString type = o.value("key").toString().toUpper();
            QString name = o.value("name").toString();
            if (name.trimmed().isEmpty()) name = (type == "P") ? QStringLiteral("被动") : type;
            QString desc = o.value("description").toString();
            if (desc.trimmed().isEmpty()) desc = QStringLiteral("暂无技能描述");
            createSkillIcon(type, name, desc);
            if (m_skillButtons.contains(type)) {
                m_skillButtons[type]->setProperty("skillImageUrl", o.value("icon_url").toString());
            }
        }
    }
    
    // 雷达值
    if (heroData.contains("detail")) {
        QJsonObject d = heroData.value("detail").toObject();
        std::array<int,5> values{
            d.value("radar_damage").toInt(),
            d.value("radar_toughness").toInt(),
            d.value("radar_control").toInt(),
            d.value("radar_mobility").toInt(),
            d.value("radar_utility").toInt()
        };
        if (m_radarWidget) m_radarWidget->setValues(values);

        // 类型：0-100，数值越大越偏魔法
        int typeVal = 0;
        if (d.contains("type")) typeVal = d.value("type").toInt();
        else if (d.contains("damage_type")) typeVal = d.value("damage_type").toInt();
        if (typeVal < 0) {
            typeVal = 0;
        }
        if (typeVal > 100) {
            typeVal = 100;
        }
        if (m_typeBar) m_typeBar->setValue(typeVal);

        // 难度：1-3
        int diff = d.value("difficulty").toInt();
        if (diff < 1) {
            diff = 1;
        }
        if (diff > 3) {
            diff = 3;
        }
        setDifficultySegments(diff);
    }
}

void HeroDetailWidget::setHeroImage(const QPixmap &pixmap)
{
    m_heroPixmap = pixmap;
    m_heroImageLabel->setPixmap(pixmap);
}

void HeroDetailWidget::setSkinImage(const QPixmap &pixmap, const QString &skinName)
{
    m_heroPixmap = pixmap;
    m_heroImageLabel->setPixmap(pixmap);
    {
        if (!skinName.isEmpty()) {
            m_skinNameLabel->setText(skinName);
        } else {
            const QString title = m_heroData.value("title").toString();
            if (!title.isEmpty()) m_skinNameLabel->setText(title);
        }
    }
}

// 删除等级与数值更新

void HeroDetailWidget::setSkillIcon(const QString &skillType, const QPixmap &pixmap)
{
    if (!m_skillButtons.contains(skillType)) return;
    QPushButton *btn = m_skillButtons.value(skillType);
    btn->setIcon(QIcon(pixmap));
    btn->setIconSize(QSize(60, 60));
}

// 删除标签与数值名称映射

QString HeroDetailWidget::formatRichText(const QString &text)
{
    // 简单的富文本格式化
    QString formatted = text;
    
    // 规范化换行
    formatted.replace("\r\n", "\n");
    formatted.replace('\r', '\n');
    formatted.replace("\n", "<br>");

    // 替换颜色标签
    QRegularExpression colorRegex("<color=#([0-9a-fA-F]{6})>(.*?)</color>");
    formatted.replace(colorRegex, "<span style=\"color: #\\1;\">\\2</span>");
    
    // 替换加粗标签
    formatted.replace("<b>", "<strong>");
    formatted.replace("</b>", "</strong>");
    
    return formatted;
}

void HeroDetailWidget::onBackClicked()
{
    emit backRequested();
}

void HeroDetailWidget::onPreviousSkinClicked()
{
    if (m_skins.isEmpty()) return;
    if (m_currentSkinIndex > 0) m_currentSkinIndex--; else m_currentSkinIndex = m_skins.size() - 1;
    const auto &pair = m_skins[m_currentSkinIndex];
    emit skinChanged(m_currentSkinIndex, pair.first, pair.second);
    m_skinNameLabel->setText(m_heroData.value("title").toString());
}

void HeroDetailWidget::onNextSkinClicked()
{
    if (m_skins.isEmpty()) return;
    m_currentSkinIndex = (m_currentSkinIndex + 1) % m_skins.size();
    const auto &pair = m_skins[m_currentSkinIndex];
    emit skinChanged(m_currentSkinIndex, pair.first, pair.second);
    m_skinNameLabel->setText(m_heroData.value("title").toString());
}

void HeroDetailWidget::onSkillHovered(const QString &skillType)
{
    if (m_skillButtons.contains(skillType)) {
        QPushButton *button = m_skillButtons[skillType];
        QString skillName = button->property("skillName").toString();
        QString skillDescription = button->property("skillDescription").toString();
        
        updateSkillTooltip(skillType, skillName, skillDescription);
    }
}

void HeroDetailWidget::onSkillLeft()
{
    hideSkillTooltip();
}

void HeroDetailWidget::updateSkillTooltip(const QString &skillType, const QString &skillName, const QString &description)
{
    QString formattedText = QString("<b>%1 - %2</b><br><br>%3")
                           .arg(skillType)
                           .arg(skillName)
                           .arg(formatRichText(description));
    
    m_skillTooltip->setText(formattedText);
    // 先根据内容计算尺寸，避免出现只有边框没有文本的情况
    m_skillTooltip->adjustSize();
    
    
    QPoint globalPos = QCursor::pos();
    QPoint localPos = mapFromGlobal(globalPos);
    
    
    int x = localPos.x() + 15;
    int y = localPos.y() - m_skillTooltip->height() / 2;
    
    if (x + m_skillTooltip->width() > width()) {
        x = localPos.x() - m_skillTooltip->width() - 15;
    }
    if (y < 0) {
        y = 0;
    }
    if (y + m_skillTooltip->height() > height()) {
        y = height() - m_skillTooltip->height();
    }
    
    m_skillTooltip->move(x, y);
    m_skillTooltip->show();
    m_skillTooltip->raise();
}

void HeroDetailWidget::hideSkillTooltip()
{
    m_skillTooltip->hide();
}

bool HeroDetailWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (m_skillButtons.values().contains(qobject_cast<QPushButton*>(obj))) {
        QPushButton *button = qobject_cast<QPushButton*>(obj);
        if (event->type() == QEvent::Enter) {
            QString skillType = button->property("skillType").toString();
            onSkillHovered(skillType);
        } else if (event->type() == QEvent::Leave) {
            onSkillLeft();
        }
    }
    
    return QWidget::eventFilter(obj, event);
}

void HeroDetailWidget::setDifficultySegments(int level)
{
    // 1..3
    for (int i = 0; i < m_difficultySegments.size(); ++i) {
        const bool active = (i < level);
        if (auto *bar = qobject_cast<ParallelogramBar*>(m_difficultySegments[i])) {
            bar->setActive(active);
        } else {
            if (active) {
                m_difficultySegments[i]->setStyleSheet(
                    "QFrame { background: #2aa198; border: 2px solid #463714; border-radius: 5px; }"
                );
            } else {
                m_difficultySegments[i]->setStyleSheet(
                    "QFrame { background: #1e2328; border: 2px solid #463714; border-radius: 5px; }"
                );
            }
        }
    }
}

