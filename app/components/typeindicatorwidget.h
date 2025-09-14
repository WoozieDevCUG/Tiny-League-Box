#ifndef TYPEINDICATORWIDGET_H
#define TYPEINDICATORWIDGET_H

#include <QWidget>
#include <QString>
class QPixmap;
class QString;

class TypeIndicatorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TypeIndicatorWidget(QWidget *parent = nullptr);
    ~TypeIndicatorWidget() override;

    void setValue(int value); // 0-100 数值
    int value() const { return m_value; }

    
    void setIcons(const QString &leftIconRelativePath, const QString &rightIconRelativePath);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_value; // 取值 0..100

    
    QPixmap *m_leftIconPixmap;
    QPixmap *m_rightIconPixmap;

    
    QString m_leftIconPath;
    QString m_rightIconPath;

    void loadIconsIfNeeded();
};

#endif // TYPEINDICATORWIDGET_H


