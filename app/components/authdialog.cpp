#include "authdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

AuthDialog::AuthDialog(QWidget *parent)
    : QDialog(parent), m_mode(Login)
{
    setWindowTitle(tr("认证"));
    setModal(true);
    setFixedSize(360, 240);

    m_labelTitle = new QLabel(tr("登录"), this);
    m_labelTitle->setStyleSheet("font-size:18px;font-weight:600;padding:8px 0;");

    m_editUsername = new QLineEdit(this);
    m_editUsername->setPlaceholderText(tr("用户名"));

    m_editPassword = new QLineEdit(this);
    m_editPassword->setPlaceholderText(tr("密码"));
    m_editPassword->setEchoMode(QLineEdit::Password);

    m_btnSubmit = new QPushButton(tr("登录"), this);
    m_btnSubmit->setStyleSheet("QPushButton{background:#2d8cf0;color:#fff;border-radius:6px;padding:6px 12px;}QPushButton:hover{background:#3aa0ff;}");

    m_btnSwitch = new QPushButton(tr("去注册"), this);
    m_btnSwitch->setFlat(true);

    auto *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(16, 16, 16, 16);
    vbox->setSpacing(12);
    vbox->addWidget(m_labelTitle);
    vbox->addWidget(m_editUsername);
    vbox->addWidget(m_editPassword);

    auto *hbox = new QHBoxLayout();
    hbox->addStretch(1);
    hbox->addWidget(m_btnSwitch);
    hbox->addWidget(m_btnSubmit);
    vbox->addLayout(hbox);

    connect(m_btnSubmit, &QPushButton::clicked, this, &AuthDialog::onSubmit);
    connect(m_btnSwitch, &QPushButton::clicked, this, [this]() {
        setMode(m_mode == Login ? Register : Login);
    });

    updateUi();
}

void AuthDialog::setMode(Mode mode)
{
    m_mode = mode;
    updateUi();
}

void AuthDialog::updateUi()
{
    if (m_mode == Login) {
        m_labelTitle->setText(tr("登录"));
        m_btnSubmit->setText(tr("登录"));
        m_btnSwitch->setText(tr("去注册"));
    } else {
        m_labelTitle->setText(tr("注册"));
        m_btnSubmit->setText(tr("注册"));
        m_btnSwitch->setText(tr("去登录"));
    }
}

void AuthDialog::onSubmit()
{
    const QString u = m_editUsername->text().trimmed();
    const QString p = m_editPassword->text();
    if (m_mode == Login) emit submitLogin(u, p);
    else emit submitRegister(u, p);
} 