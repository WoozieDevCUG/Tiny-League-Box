#ifndef PARALLELOGRAMBAR_H
#define PARALLELOGRAMBAR_H

#include <QFrame>
#include <QColor>

class ParallelogramBar : public QFrame
{
    Q_OBJECT
public:
    explicit ParallelogramBar(QWidget *parent = nullptr);

    void setActive(bool active);
    bool isActive() const { return m_active; }

    void setColors(const QColor &activeFill,
                   const QColor &inactiveFill,
                   const QColor &borderColor);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    bool m_active;
    QColor m_activeFill;
    QColor m_inactiveFill;
    QColor m_borderColor;
    int m_borderWidth;
    int m_slant; // 顶边的水平偏移量，用于形成平行四边形
};

#endif // PARALLELOGRAMBAR_H


