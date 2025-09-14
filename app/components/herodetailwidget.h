#ifndef HERODETAILWIDGET_H
#define HERODETAILWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QPixmap>
#include <QStringList>
#include <QMap>
#include <QJsonObject>

class QSlider;

class QLabel;
class QPushButton;
class QScrollArea;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QProgressBar;
class QFrame;
class RadarChartWidget;
class TypeIndicatorWidget;

class HeroDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HeroDetailWidget(QWidget *parent = nullptr);

    void setHeroData(const QJsonObject &heroData);
    void setHeroImage(const QPixmap &pixmap);
    void setSkillIcon(const QString &skillType, const QPixmap &pixmap);
    void setSkinImage(const QPixmap &pixmap, const QString &skinName);

signals:
    void backRequested();
    void skinChanged(int index, const QString &imageUrl, const QString &skinName);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onBackClicked();
    void onPreviousSkinClicked();
    void onNextSkinClicked();
    void onSkillHovered(const QString &skillType);
    void onSkillLeft();

private:
    void setupUI();
    void setupSkillsSection();
    void setupRadarSection();
    void createSkillIcon(const QString &skillType, const QString &skillName, const QString &description);
    void updateSkillTooltip(const QString &skillType, const QString &skillName, const QString &description);
    void hideSkillTooltip();
    void setDifficultySegments(int level);
    
    // 帮助函数
    QString formatRichText(const QString &text);

    // 界面组件
    QPushButton *m_backButton;
    QLabel *m_heroImageLabel;
    QLabel *m_skinNameLabel;
    QPushButton *m_prevSkinButton;
    QPushButton *m_nextSkinButton;
    QLabel *m_heroDescriptionLabel;
    
    // 技能区
    QWidget *m_skillsWidget;
    QHBoxLayout *m_skillsLayout;
    QLabel *m_skillTooltip;
    QMap<QString, QPushButton*> m_skillButtons;
    // 雷达图与右侧信息面板
    RadarChartWidget *m_radarWidget;
    QWidget *m_radarRightPanel; // 容纳类型与难度条
    // 类型条（0-100，越大越偏魔法）
    TypeIndicatorWidget *m_typeBar;
    // 难度三格
    QWidget *m_difficultyWidget;
    QVector<QFrame*> m_difficultySegments;
    
    // 数据
    QJsonObject m_heroData;
    QPixmap m_heroPixmap;
    int m_currentSkinIndex;
    QList<QPair<QString, QString>> m_skins;
};

#endif // HERODETAILWIDGET_H
