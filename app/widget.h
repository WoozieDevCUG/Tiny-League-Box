#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QList>
#include <QPointer>
#include <QVBoxLayout>
#include "network/httpclient.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class AuthDialog;
class HttpClient;
class HeroCardWidget;
class HeroDetailWidget;
class AvatarWidget;
class QPushButton;
class QLabel;
class QLineEdit;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void onLoginRequested();
    void onRegisterRequested();
    void onLogoutRequested();
    void onAvatarUploadRequested();
    void onAvatarChanged(const QString &avatarUrl);

    void onLoginSubmit(const QString &username, const QString &password);
    void onRegisterSubmit(const QString &username, const QString &password);

    void onLoginSucceeded(const QString &token, const QString &username, const QString &avatarUrl);
    void onLoginFailed(const QString &message);
    void onRegisterSucceeded();
    void onRegisterFailed(const QString &message);

    void onHomeButtons();
    void onDataTabs();

    void onFilterTypeChanged(int index);
    void onFilterValueChanged(int index);

    void refreshHeroesGrid();
    void showHeroDetail(int heroId);
    void onHeroDetailBackRequested();
    
    // 评分交互回调
    void onRatingSubmitted(int heroId, int stars);
    void onRatingSummaryFetched(int heroId, double avg, int count);
    void onRatingsFetched(int heroId, const QList<UserRating> &items);

private:
    void setupConnections();
    void setFilterValuesForRoute();
    void setFilterValuesForClass();
    QString currentFilterTypeKey() const; // 筛选类型：route | class
    QString currentFilterValueKey() const; // 规范化英文键

    void loadAuthFromSettings();
    void saveAuthToSettings(const QString &token, const QString &username, const QString &avatarUrl);
    void clearAuthInSettings();

    // 互动页（上路坦克强度评分）
    void buildInteractPage();
    void buildInteractTopicDetail(const QString &topicKey);
    void buildHeroRatingDetail(int heroId);
    void requestAllSummaries();
    void submitHeroRating(int heroId);
    
    // 分页相关方法
    void updateCommentsDisplay();
    void onPrevPageClicked();
    void onNextPageClicked();
    void updatePaginationControls();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::Widget *ui;
    AuthDialog *m_authDialog;
    HttpClient *m_http;
    AvatarWidget *m_avatarWidget;
    HeroDetailWidget *m_heroDetailWidget;
    QPushButton *m_btnBack;
    QString m_token;
    QString m_currentUsername;
    QString m_currentAvatarUrl;
    QList<HeroCardWidget*> m_heroCards;

    // 互动页数据结构
    struct InteractItem {
        int heroId = 0;
        QPointer<QLabel> imageLabel;
        QPointer<QLabel> idLabel;
        QList<QPushButton*> starButtons;
        QPointer<QLineEdit> commentEdit;
        QPointer<QLabel> avgLabel;
        QPointer<QWidget> commentsHost; // 简单评论列表容器
        int selectedStars = 0;
    };
    QList<InteractItem*> m_interactItems;
    QString m_currentInteractTopicKey;
    
    // 评分详情页分页相关
    QList<UserRating> m_allRatings;  // 存储所有评论
    int m_currentPage;               // 当前页码（从0开始）
    int m_totalPages;                // 总页数
    static const int COMMENTS_PER_PAGE = 30;  // 每页评论数
    QPointer<QLabel> m_pageLabel;    // 页码显示标签
    QPointer<QPushButton> m_prevPageBtn;  // 上一页按钮
    QPointer<QPushButton> m_nextPageBtn;  // 下一页按钮
    QPointer<QVBoxLayout> m_commentsLayout;  // 评论列表布局
};
#endif // WIDGET_H
