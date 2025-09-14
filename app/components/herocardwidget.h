#ifndef HEROCARDWIDGET_H
#define HEROCARDWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QStringList>
#include <QMap>

class QLabel;

class HeroCardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HeroCardWidget(QWidget *parent = nullptr);

    void setHeroId(int heroId);
    void setSquareImage(const QPixmap &pixmap);
    void setDisplayName(const QString &title, const QString &name);
    void setTags(const QStringList &tags);

signals:
    void clicked(int heroId);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QString mapTagToChinese(const QString &tag);
    void setupTagMapping();

    int m_heroId;
    // 图片容器上的元素
    QLabel *m_idLabel;
    QLabel *m_imageLabel;
    QLabel *m_nameLabel;
    QLabel *m_tagsLabel;
    QMap<QString, QString> m_tagMapping;
};

#endif // HEROCARDWIDGET_H 