#include "herocardwidget.h"

#include <QVBoxLayout>
#include <QStackedLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>

HeroCardWidget::HeroCardWidget(QWidget *parent)
    : QWidget(parent), m_heroId(-1), m_idLabel(new QLabel(this)), m_imageLabel(new QLabel(this)), m_nameLabel(new QLabel(this)), m_tagsLabel(new QLabel(this))
{
    setFixedSize(180, 240);
    // 黑色背景，圆角边框
    setStyleSheet("HeroCardWidget { background: #1a1a1a; border: 1px solid #333; border-radius: 8px; }");

    // 图片标签 - 圆角
    m_imageLabel->setFixedSize(160, 160);
    m_imageLabel->setScaledContents(false);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setStyleSheet("QLabel { border-radius: 6px; background: #2a2a2a; border: 1px solid #444; }");

    // 右上角ID徽标（隐藏）
    m_idLabel->hide();

    // 英雄名称 - 白色文字
    m_nameLabel->setWordWrap(true);
    m_nameLabel->setAlignment(Qt::AlignCenter);
    m_nameLabel->setStyleSheet("QLabel { font-size: 13px; font-weight: bold; color: #ffffff; background: transparent; padding: 4px; }");

    // 标签 - 较小的灰色文字
    m_tagsLabel->setAlignment(Qt::AlignCenter);
    m_tagsLabel->setStyleSheet("QLabel { font-size: 11px; color: #cccccc; background: transparent; padding: 2px; }");

    // 布局：图片容器 + 右上角徽标
    auto *imageContainer = new QWidget(this);
    imageContainer->setFixedSize(160, 160);
    auto *imageLayout = new QVBoxLayout(imageContainer);
    imageLayout->setContentsMargins(0,0,0,0);
    imageLayout->setSpacing(0);
    imageLayout->addWidget(m_imageLabel);

    auto *badgeRow = new QHBoxLayout();
    badgeRow->setContentsMargins(0,0,0,0);
    badgeRow->setSpacing(0);
    badgeRow->addStretch();
    badgeRow->addWidget(m_idLabel, 0, Qt::AlignTop);
    auto *stack = new QVBoxLayout(imageContainer);
    stack->setContentsMargins(0,0,0,0);
    stack->setSpacing(0);
    // 使用绝对布局效果：将徽标放到容器上层右上角
    imageContainer->setLayout(nullptr);
    m_imageLabel->setParent(imageContainer);
    m_imageLabel->move(0,0);
    m_idLabel->setParent(imageContainer);
    m_idLabel->move(imageContainer->width() - m_idLabel->width() - 6, 6);
    imageContainer->installEventFilter(this);

    auto *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(10, 10, 10, 10);
    vbox->setSpacing(4);
    vbox->addWidget(imageContainer, 0, Qt::AlignCenter);
    vbox->addWidget(m_nameLabel);
    vbox->addWidget(m_tagsLabel);

    // 初始化标签映射
    setupTagMapping();
}

void HeroCardWidget::setupTagMapping()
{
    // 职业映射
    m_tagMapping["tank"] = "坦克";
    m_tagMapping["fighter"] = "战士";
    m_tagMapping["assassin"] = "刺客";
    m_tagMapping["marksman"] = "射手";
    m_tagMapping["mage"] = "法师";
    
    // 位置映射
    m_tagMapping["top"] = "上单";
    m_tagMapping["mid"] = "中单";
    m_tagMapping["bottom"] = "下路";
    m_tagMapping["jungle"] = "打野";
    m_tagMapping["support"] = "辅助";
}

QString HeroCardWidget::mapTagToChinese(const QString &tag)
{
    return m_tagMapping.value(tag.toLower(), tag);
}

void HeroCardWidget::setHeroId(int heroId)
{
    m_heroId = heroId;
}

void HeroCardWidget::setSquareImage(const QPixmap &pixmap)
{
    if (pixmap.isNull()) {
        m_imageLabel->clear();
        return;
    }
    // 按比例裁剪
    QSize target = m_imageLabel->size();
    QPixmap scaled = pixmap.scaled(target, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    if (scaled.size() != target) {
        QRect centerCrop((scaled.width() - target.width())/2,
                         (scaled.height() - target.height())/2,
                         target.width(), target.height());
        scaled = scaled.copy(centerCrop);
    }
    m_imageLabel->setPixmap(scaled);
}

void HeroCardWidget::setDisplayName(const QString &title, const QString &name)
{
    m_nameLabel->setText(QString("%1 - %2").arg(title, name));
}

void HeroCardWidget::setTags(const QStringList &tags)
{
    QStringList chineseTags;
    for (const QString &tag : tags) {
        chineseTags << mapTagToChinese(tag);
    }
    m_tagsLabel->setText(chineseTags.join("、"));
}

void HeroCardWidget::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    emit clicked(m_heroId);
} 