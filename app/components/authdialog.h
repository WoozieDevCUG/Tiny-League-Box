#ifndef AUTHDIALOG_H
#define AUTHDIALOG_H

#include <QDialog>

class QLineEdit;
class QPushButton;
class QLabel;

class AuthDialog : public QDialog
{
    Q_OBJECT
public:
    enum Mode { Login, Register };

    explicit AuthDialog(QWidget *parent = nullptr);

    void setMode(Mode mode);

signals:
    void submitLogin(const QString &username, const QString &password);
    void submitRegister(const QString &username, const QString &password);

private slots:
    void onSubmit();

private:
    void updateUi();

private:
    Mode m_mode;
    QLineEdit *m_editUsername;
    QLineEdit *m_editPassword;
    QPushButton *m_btnSubmit;
    QPushButton *m_btnSwitch;
    QLabel *m_labelTitle;
};

#endif // AUTHDIALOG_H 