/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "components/herocardwidget.h"
#include "components/userbadgewidget.h"

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QVBoxLayout *verticalLayout_Main;
    QHBoxLayout *horizontalLayout_TopBar;
    QLabel *labelTitle;
    QSpacerItem *horizontalSpacer;
    UserBadgeWidget *userBadge;
    QStackedWidget *stackedMain;
    QWidget *pageHome;
    QVBoxLayout *verticalLayout_Home;
    QSpacerItem *verticalSpacer_home_top;
    QGridLayout *gridLayout_HomeButtons;
    QPushButton *btnData;
    QPushButton *btnNews;
    QPushButton *btnGuides;
    QPushButton *btnInteract;
    QSpacerItem *verticalSpacer_home_bottom;
    QWidget *pageData;
    QVBoxLayout *verticalLayout_Data;
    QWidget *widgetDataTabsContainer;
    QHBoxLayout *horizontalLayout_DataTabsContainer;
    QSpacerItem *horizontalSpacer_left;
    QHBoxLayout *horizontalLayout_DataTabs;
    QPushButton *btnTabHeroes;
    QPushButton *btnTabItems;
    QPushButton *btnTabRunes;
    QSpacerItem *horizontalSpacer_right;
    QStackedWidget *stackedData;
    QWidget *pageHeroes;
    QVBoxLayout *verticalLayout_Heroes;
    QWidget *widgetHeroesHeader;
    QHBoxLayout *horizontalLayout_HeroesHeader;
    QLabel *labelHeroesTitle;
    QSpacerItem *horizontalSpacer3;
    QComboBox *comboFilterType;
    QComboBox *comboFilterValue;
    QScrollArea *scrollHeroes;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridHeroes;
    QWidget *pageItems;
    QVBoxLayout *verticalLayout_Items;
    QLabel *labelItems;
    QWidget *pageRunes;
    QVBoxLayout *verticalLayout_Runes;
    QLabel *labelRunes;
    QWidget *pageNews;
    QVBoxLayout *verticalLayout_News;
    QLabel *labelNews;
    QWidget *pageGuides;
    QVBoxLayout *verticalLayout_Guides;
    QLabel *labelGuides;
    QWidget *pageInteract;
    QVBoxLayout *verticalLayout_Interact;
    QLabel *labelInteract;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName("Widget");
        Widget->resize(1200, 720);
        Widget->setMinimumSize(QSize(1200, 720));
        Widget->setMaximumSize(QSize(1200, 720));
        Widget->setStyleSheet(QString::fromUtf8("QWidget#Widget { background: #0a0e13; }"));
        verticalLayout_Main = new QVBoxLayout(Widget);
        verticalLayout_Main->setSpacing(0);
        verticalLayout_Main->setContentsMargins(0, 0, 0, 0);
        verticalLayout_Main->setObjectName("verticalLayout_Main");
        horizontalLayout_TopBar = new QHBoxLayout();
        horizontalLayout_TopBar->setObjectName("horizontalLayout_TopBar");
        horizontalLayout_TopBar->setContentsMargins(16, 12, 16, 12);
        labelTitle = new QLabel(Widget);
        labelTitle->setObjectName("labelTitle");
        labelTitle->setStyleSheet(QString::fromUtf8("font-size: 20px; font-weight: bold; color: #00d4ff;"));

        horizontalLayout_TopBar->addWidget(labelTitle);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_TopBar->addItem(horizontalSpacer);

        userBadge = new UserBadgeWidget(Widget);
        userBadge->setObjectName("userBadge");

        horizontalLayout_TopBar->addWidget(userBadge);


        verticalLayout_Main->addLayout(horizontalLayout_TopBar);

        stackedMain = new QStackedWidget(Widget);
        stackedMain->setObjectName("stackedMain");
        pageHome = new QWidget();
        pageHome->setObjectName("pageHome");
        pageHome->setStyleSheet(QString::fromUtf8("QWidget { background: #0a0e13; }"));
        verticalLayout_Home = new QVBoxLayout(pageHome);
        verticalLayout_Home->setObjectName("verticalLayout_Home");
        verticalSpacer_home_top = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_Home->addItem(verticalSpacer_home_top);

        gridLayout_HomeButtons = new QGridLayout();
        gridLayout_HomeButtons->setObjectName("gridLayout_HomeButtons");
        gridLayout_HomeButtons->setHorizontalSpacing(32);
        gridLayout_HomeButtons->setVerticalSpacing(24);
        gridLayout_HomeButtons->setContentsMargins(160, 0, 160, 0);
        btnData = new QPushButton(pageHome);
        btnData->setObjectName("btnData");
        btnData->setMinimumSize(QSize(240, 140));
        btnData->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"              font-size: 22px; \n"
"              font-weight: bold; \n"
"              color: #ffffff; \n"
"              background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #667eea, stop:1 #764ba2); \n"
"              border: 2px solid #667eea; \n"
"              border-radius: 12px;\n"
"            }\n"
"            QPushButton:hover {\n"
"              background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #764ba2, stop:1 #667eea); \n"
"              border-color: #00d4ff;\n"
"              transform: translateY(-2px);\n"
"            }\n"
"            QPushButton:pressed {\n"
"              background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #5a67d8, stop:1 #553c9a);\n"
"            }"));

        gridLayout_HomeButtons->addWidget(btnData, 0, 0, 1, 1);

        btnNews = new QPushButton(pageHome);
        btnNews->setObjectName("btnNews");
        btnNews->setMinimumSize(QSize(240, 140));
        btnNews->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"              font-size: 22px; \n"
"              font-weight: bold; \n"
"              color: #ffffff; \n"
"              background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f093fb, stop:1 #f5576c); \n"
"              border: 2px solid #f093fb; \n"
"              border-radius: 12px;\n"
"            }\n"
"            QPushButton:hover {\n"
"              background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f5576c, stop:1 #f093fb); \n"
"              border-color: #00d4ff;\n"
"            }\n"
"            QPushButton:pressed {\n"
"              background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #e53e3e, stop:1 #d53f8c);\n"
"            }"));

        gridLayout_HomeButtons->addWidget(btnNews, 0, 1, 1, 1);

        btnGuides = new QPushButton(pageHome);
        btnGuides->setObjectName("btnGuides");
        btnGuides->setMinimumSize(QSize(240, 140));
        btnGuides->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"              font-size: 22px; \n"
"              font-weight: bold; \n"
"              color: #ffffff; \n"
"              background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #4facfe, stop:1 #00f2fe); \n"
"              border: 2px solid #4facfe; \n"
"              border-radius: 12px;\n"
"            }\n"
"            QPushButton:hover {\n"
"              background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #00f2fe, stop:1 #4facfe); \n"
"              border-color: #00d4ff;\n"
"            }\n"
"            QPushButton:pressed {\n"
"              background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #0ea5e9, stop:1 #06b6d4);\n"
"            }"));

        gridLayout_HomeButtons->addWidget(btnGuides, 1, 0, 1, 1);

        btnInteract = new QPushButton(pageHome);
        btnInteract->setObjectName("btnInteract");
        btnInteract->setMinimumSize(QSize(240, 140));
        btnInteract->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"              font-size: 22px; \n"
"              font-weight: bold; \n"
"              color: #ffffff; \n"
"              background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #fa709a, stop:1 #fee140); \n"
"              border: 2px solid #fa709a; \n"
"              border-radius: 12px;\n"
"            }\n"
"            QPushButton:hover {\n"
"              background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #fee140, stop:1 #fa709a); \n"
"              border-color: #00d4ff;\n"
"            }\n"
"            QPushButton:pressed {\n"
"              background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f59e0b, stop:1 #ef4444);\n"
"            }"));

        gridLayout_HomeButtons->addWidget(btnInteract, 1, 1, 1, 1);


        verticalLayout_Home->addLayout(gridLayout_HomeButtons);

        verticalSpacer_home_bottom = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_Home->addItem(verticalSpacer_home_bottom);

        stackedMain->addWidget(pageHome);
        pageData = new QWidget();
        pageData->setObjectName("pageData");
        verticalLayout_Data = new QVBoxLayout(pageData);
        verticalLayout_Data->setObjectName("verticalLayout_Data");
        verticalLayout_Data->setContentsMargins(0, 0, 0, 0);
        widgetDataTabsContainer = new QWidget(pageData);
        widgetDataTabsContainer->setObjectName("widgetDataTabsContainer");
        widgetDataTabsContainer->setMinimumHeight(80);
        widgetDataTabsContainer->setMaximumHeight(80);
        widgetDataTabsContainer->setStyleSheet(QString::fromUtf8("QWidget#widgetDataTabsContainer { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0f1419, stop:1 #1e2328); border-bottom: 2px solid #00d4ff; }"));
        horizontalLayout_DataTabsContainer = new QHBoxLayout(widgetDataTabsContainer);
        horizontalLayout_DataTabsContainer->setObjectName("horizontalLayout_DataTabsContainer");
        horizontalLayout_DataTabsContainer->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_left = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_DataTabsContainer->addItem(horizontalSpacer_left);

        horizontalLayout_DataTabs = new QHBoxLayout();
        horizontalLayout_DataTabs->setSpacing(2);
        horizontalLayout_DataTabs->setObjectName("horizontalLayout_DataTabs");
        btnTabHeroes = new QPushButton(widgetDataTabsContainer);
        btnTabHeroes->setObjectName("btnTabHeroes");
        btnTabHeroes->setMinimumSize(QSize(120, 50));
        btnTabHeroes->setStyleSheet(QString::fromUtf8("QPushButton { \n"
"                 font-size: 16px; \n"
"                 font-weight: bold; \n"
"                 color: #8892b0; \n"
"                 background: transparent; \n"
"                 border: none; \n"
"                 border-bottom: 3px solid transparent; \n"
"                 padding: 12px 24px; \n"
"               }\n"
"               QPushButton:checked { \n"
"                 color: #00d4ff; \n"
"                 border-bottom: 3px solid #00d4ff; \n"
"                 background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgba(0,212,255,0.1), stop:1 transparent); \n"
"               }\n"
"               QPushButton:hover:!checked { \n"
"                 color: #ccd6f6; \n"
"                 background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgba(255,255,255,0.05), stop:1 transparent); \n"
"               }"));
        btnTabHeroes->setCheckable(true);
        btnTabHeroes->setChecked(true);

        horizontalLayout_DataTabs->addWidget(btnTabHeroes);

        btnTabItems = new QPushButton(widgetDataTabsContainer);
        btnTabItems->setObjectName("btnTabItems");
        btnTabItems->setMinimumSize(QSize(120, 50));
        btnTabItems->setStyleSheet(QString::fromUtf8("QPushButton { \n"
"                 font-size: 16px; \n"
"                 font-weight: bold; \n"
"                 color: #8892b0; \n"
"                 background: transparent; \n"
"                 border: none; \n"
"                 border-bottom: 3px solid transparent; \n"
"                 padding: 12px 24px; \n"
"               }\n"
"               QPushButton:checked { \n"
"                 color: #00d4ff; \n"
"                 border-bottom: 3px solid #00d4ff; \n"
"                 background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgba(0,212,255,0.1), stop:1 transparent); \n"
"               }\n"
"               QPushButton:hover:!checked { \n"
"                 color: #ccd6f6; \n"
"                 background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgba(255,255,255,0.05), stop:1 transparent); \n"
"               }"));
        btnTabItems->setCheckable(true);

        horizontalLayout_DataTabs->addWidget(btnTabItems);

        btnTabRunes = new QPushButton(widgetDataTabsContainer);
        btnTabRunes->setObjectName("btnTabRunes");
        btnTabRunes->setMinimumSize(QSize(120, 50));
        btnTabRunes->setStyleSheet(QString::fromUtf8("QPushButton { \n"
"                 font-size: 16px; \n"
"                 font-weight: bold; \n"
"                 color: #8892b0; \n"
"                 background: transparent; \n"
"                 border: none; \n"
"                 border-bottom: 3px solid transparent; \n"
"                 padding: 12px 24px; \n"
"               }\n"
"               QPushButton:checked { \n"
"                 color: #00d4ff; \n"
"                 border-bottom: 3px solid #00d4ff; \n"
"                 background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgba(0,212,255,0.1), stop:1 transparent); \n"
"               }\n"
"               QPushButton:hover:!checked { \n"
"                 color: #ccd6f6; \n"
"                 background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgba(255,255,255,0.05), stop:1 transparent); \n"
"               }"));
        btnTabRunes->setCheckable(true);

        horizontalLayout_DataTabs->addWidget(btnTabRunes);


        horizontalLayout_DataTabsContainer->addLayout(horizontalLayout_DataTabs);

        horizontalSpacer_right = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_DataTabsContainer->addItem(horizontalSpacer_right);


        verticalLayout_Data->addWidget(widgetDataTabsContainer);

        stackedData = new QStackedWidget(pageData);
        stackedData->setObjectName("stackedData");
        stackedData->setStyleSheet(QString::fromUtf8("QStackedWidget { background: #0a0e13; }"));
        pageHeroes = new QWidget();
        pageHeroes->setObjectName("pageHeroes");
        verticalLayout_Heroes = new QVBoxLayout(pageHeroes);
        verticalLayout_Heroes->setObjectName("verticalLayout_Heroes");
        verticalLayout_Heroes->setContentsMargins(0, 0, 0, 0);
        widgetHeroesHeader = new QWidget(pageHeroes);
        widgetHeroesHeader->setObjectName("widgetHeroesHeader");
        widgetHeroesHeader->setMinimumHeight(60);
        widgetHeroesHeader->setMaximumHeight(60);
        widgetHeroesHeader->setStyleSheet(QString::fromUtf8("QWidget#widgetHeroesHeader { background: #1e2328; border-bottom: 1px solid #3c4043; }"));
        horizontalLayout_HeroesHeader = new QHBoxLayout(widgetHeroesHeader);
        horizontalLayout_HeroesHeader->setObjectName("horizontalLayout_HeroesHeader");
        horizontalLayout_HeroesHeader->setContentsMargins(24, 12, 24, 12);
        labelHeroesTitle = new QLabel(widgetHeroesHeader);
        labelHeroesTitle->setObjectName("labelHeroesTitle");
        labelHeroesTitle->setStyleSheet(QString::fromUtf8("font-size: 18px; font-weight: bold; color: #ccd6f6;"));

        horizontalLayout_HeroesHeader->addWidget(labelHeroesTitle);

        horizontalSpacer3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_HeroesHeader->addItem(horizontalSpacer3);

        comboFilterType = new QComboBox(widgetHeroesHeader);
        comboFilterType->addItem(QString());
        comboFilterType->addItem(QString());
        comboFilterType->addItem(QString());
        comboFilterType->setObjectName("comboFilterType");
        comboFilterType->setMinimumSize(QSize(140, 36));
        comboFilterType->setStyleSheet(QString::fromUtf8("QComboBox {\n"
"                   font-size: 14px;\n"
"                   color: #ccd6f6;\n"
"                   background: #2d3748;\n"
"                   border: 2px solid #4a5568;\n"
"                   border-radius: 6px;\n"
"                   padding: 6px 12px;\n"
"                   min-width: 120px;\n"
"                 }\n"
"                 QComboBox:hover {\n"
"                   border-color: #00d4ff;\n"
"                   background: #374151;\n"
"                 }\n"
"                 QComboBox:focus {\n"
"                   border-color: #00d4ff;\n"
"                   background: #374151;\n"
"                 }\n"
"                 QComboBox::drop-down {\n"
"                   border: none;\n"
"                   width: 20px;\n"
"                 }\n"
"                 QComboBox::down-arrow {\n"
"                   image: none;\n"
"                   border-left: 5px solid transparent;\n"
"                   border-right: 5px solid transparent;\n"
"                   border-top: 5px solid #0"
                        "0d4ff;\n"
"                   margin-right: 5px;\n"
"                 }\n"
"                 QComboBox QAbstractItemView {\n"
"                   background: #2d3748;\n"
"                   border: 2px solid #00d4ff;\n"
"                   border-radius: 6px;\n"
"                   color: #ccd6f6;\n"
"                   selection-background-color: #00d4ff;\n"
"                   selection-color: #0a0e13;\n"
"                   outline: none;\n"
"                 }"));

        horizontalLayout_HeroesHeader->addWidget(comboFilterType);

        comboFilterValue = new QComboBox(widgetHeroesHeader);
        comboFilterValue->addItem(QString());
        comboFilterValue->addItem(QString());
        comboFilterValue->addItem(QString());
        comboFilterValue->addItem(QString());
        comboFilterValue->addItem(QString());
        comboFilterValue->setObjectName("comboFilterValue");
        comboFilterValue->setMinimumSize(QSize(140, 36));
        comboFilterValue->setEnabled(false);
        comboFilterValue->setStyleSheet(QString::fromUtf8("QComboBox {\n"
"                   font-size: 14px;\n"
"                   color: #ccd6f6;\n"
"                   background: #2d3748;\n"
"                   border: 2px solid #4a5568;\n"
"                   border-radius: 6px;\n"
"                   padding: 6px 12px;\n"
"                   min-width: 120px;\n"
"                 }\n"
"                 QComboBox:hover {\n"
"                   border-color: #00d4ff;\n"
"                   background: #374151;\n"
"                 }\n"
"                 QComboBox:focus {\n"
"                   border-color: #00d4ff;\n"
"                   background: #374151;\n"
"                 }\n"
"                 QComboBox:disabled {\n"
"                   background: #1a1d23;\n"
"                   color: #4a5568;\n"
"                   border-color: #2d3748;\n"
"                 }\n"
"                 QComboBox::drop-down {\n"
"                   border: none;\n"
"                   width: 20px;\n"
"                 }\n"
"                 QComboBox::down-arrow {\n"
"    "
                        "               image: none;\n"
"                   border-left: 5px solid transparent;\n"
"                   border-right: 5px solid transparent;\n"
"                   border-top: 5px solid #00d4ff;\n"
"                   margin-right: 5px;\n"
"                 }\n"
"                 QComboBox::down-arrow:disabled {\n"
"                   border-top-color: #4a5568;\n"
"                 }\n"
"                 QComboBox QAbstractItemView {\n"
"                   background: #2d3748;\n"
"                   border: 2px solid #00d4ff;\n"
"                   border-radius: 6px;\n"
"                   color: #ccd6f6;\n"
"                   selection-background-color: #00d4ff;\n"
"                   selection-color: #0a0e13;\n"
"                   outline: none;\n"
"                 }"));

        horizontalLayout_HeroesHeader->addWidget(comboFilterValue);


        verticalLayout_Heroes->addWidget(widgetHeroesHeader);

        scrollHeroes = new QScrollArea(pageHeroes);
        scrollHeroes->setObjectName("scrollHeroes");
        scrollHeroes->setWidgetResizable(true);
        scrollHeroes->setStyleSheet(QString::fromUtf8("QScrollArea {\n"
"                background: #0a0e13;\n"
"                border: none;\n"
"              }\n"
"              QScrollBar:vertical {\n"
"                background: #1e2328;\n"
"                width: 12px;\n"
"                border-radius: 6px;\n"
"              }\n"
"              QScrollBar::handle:vertical {\n"
"                background: #4a5568;\n"
"                border-radius: 6px;\n"
"                min-height: 20px;\n"
"              }\n"
"              QScrollBar::handle:vertical:hover {\n"
"                background: #00d4ff;\n"
"              }\n"
"              QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {\n"
"                height: 0px;\n"
"              }"));
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setStyleSheet(QString::fromUtf8("QWidget { background: #0a0e13; }"));
        gridHeroes = new QGridLayout(scrollAreaWidgetContents);
        gridHeroes->setObjectName("gridHeroes");
        gridHeroes->setHorizontalSpacing(20);
        gridHeroes->setVerticalSpacing(20);
        gridHeroes->setContentsMargins(24, 24, 24, 24);
        scrollHeroes->setWidget(scrollAreaWidgetContents);

        verticalLayout_Heroes->addWidget(scrollHeroes);

        stackedData->addWidget(pageHeroes);
        pageItems = new QWidget();
        pageItems->setObjectName("pageItems");
        pageItems->setStyleSheet(QString::fromUtf8("QWidget { background: #0a0e13; }"));
        verticalLayout_Items = new QVBoxLayout(pageItems);
        verticalLayout_Items->setObjectName("verticalLayout_Items");
        labelItems = new QLabel(pageItems);
        labelItems->setObjectName("labelItems");
        labelItems->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        labelItems->setStyleSheet(QString::fromUtf8("font-size: 18px; color: #8892b0;"));

        verticalLayout_Items->addWidget(labelItems);

        stackedData->addWidget(pageItems);
        pageRunes = new QWidget();
        pageRunes->setObjectName("pageRunes");
        pageRunes->setStyleSheet(QString::fromUtf8("QWidget { background: #0a0e13; }"));
        verticalLayout_Runes = new QVBoxLayout(pageRunes);
        verticalLayout_Runes->setObjectName("verticalLayout_Runes");
        labelRunes = new QLabel(pageRunes);
        labelRunes->setObjectName("labelRunes");
        labelRunes->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        labelRunes->setStyleSheet(QString::fromUtf8("font-size: 18px; color: #8892b0;"));

        verticalLayout_Runes->addWidget(labelRunes);

        stackedData->addWidget(pageRunes);

        verticalLayout_Data->addWidget(stackedData);

        stackedMain->addWidget(pageData);
        pageNews = new QWidget();
        pageNews->setObjectName("pageNews");
        pageNews->setStyleSheet(QString::fromUtf8("QWidget { background: #0a0e13; }"));
        verticalLayout_News = new QVBoxLayout(pageNews);
        verticalLayout_News->setObjectName("verticalLayout_News");
        labelNews = new QLabel(pageNews);
        labelNews->setObjectName("labelNews");
        labelNews->setAlignment(Qt::AlignCenter);
        labelNews->setStyleSheet(QString::fromUtf8("font-size: 18px; color: #8892b0;"));

        verticalLayout_News->addWidget(labelNews);

        stackedMain->addWidget(pageNews);
        pageGuides = new QWidget();
        pageGuides->setObjectName("pageGuides");
        pageGuides->setStyleSheet(QString::fromUtf8("QWidget { background: #0a0e13; }"));
        verticalLayout_Guides = new QVBoxLayout(pageGuides);
        verticalLayout_Guides->setObjectName("verticalLayout_Guides");
        labelGuides = new QLabel(pageGuides);
        labelGuides->setObjectName("labelGuides");
        labelGuides->setAlignment(Qt::AlignCenter);
        labelGuides->setStyleSheet(QString::fromUtf8("font-size: 18px; color: #8892b0;"));

        verticalLayout_Guides->addWidget(labelGuides);

        stackedMain->addWidget(pageGuides);
        pageInteract = new QWidget();
        pageInteract->setObjectName("pageInteract");
        pageInteract->setStyleSheet(QString::fromUtf8("QWidget { background: #0a0e13; }"));
        verticalLayout_Interact = new QVBoxLayout(pageInteract);
        verticalLayout_Interact->setObjectName("verticalLayout_Interact");
        labelInteract = new QLabel(pageInteract);
        labelInteract->setObjectName("labelInteract");
        labelInteract->setAlignment(Qt::AlignCenter);
        labelInteract->setStyleSheet(QString::fromUtf8("font-size: 18px; color: #8892b0;"));

        verticalLayout_Interact->addWidget(labelInteract);

        stackedMain->addWidget(pageInteract);

        verticalLayout_Main->addWidget(stackedMain);


        retranslateUi(Widget);

        stackedMain->setCurrentIndex(0);
        stackedData->setCurrentIndex(0);
        comboFilterType->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "\347\224\265\347\253\236\346\225\260\346\215\256\345\212\251\346\211\213", nullptr));
        labelTitle->setText(QCoreApplication::translate("Widget", "\347\224\265\347\253\236\346\225\260\346\215\256\345\212\251\346\211\213", nullptr));
        btnData->setText(QCoreApplication::translate("Widget", "\350\265\204\346\226\231", nullptr));
        btnNews->setText(QCoreApplication::translate("Widget", "\350\265\204\350\256\257", nullptr));
        btnGuides->setText(QCoreApplication::translate("Widget", "\346\224\273\347\225\245", nullptr));
        btnInteract->setText(QCoreApplication::translate("Widget", "\344\272\222\345\212\250", nullptr));
        btnTabHeroes->setText(QCoreApplication::translate("Widget", "\350\213\261\351\233\204", nullptr));
        btnTabItems->setText(QCoreApplication::translate("Widget", "\350\243\205\345\244\207", nullptr));
        btnTabRunes->setText(QCoreApplication::translate("Widget", "\347\254\246\346\226\207", nullptr));
        labelHeroesTitle->setText(QCoreApplication::translate("Widget", "\350\213\261\351\233\204\346\225\260\346\215\256\345\272\223", nullptr));
        comboFilterType->setItemText(0, QCoreApplication::translate("Widget", "\346\230\276\347\244\272\346\211\200\346\234\211", nullptr));
        comboFilterType->setItemText(1, QCoreApplication::translate("Widget", "\350\267\257\347\272\277\345\256\232\344\275\215", nullptr));
        comboFilterType->setItemText(2, QCoreApplication::translate("Widget", "\345\212\237\350\203\275\345\256\232\344\275\215", nullptr));

        comboFilterValue->setItemText(0, QCoreApplication::translate("Widget", "\344\270\212\345\215\225", nullptr));
        comboFilterValue->setItemText(1, QCoreApplication::translate("Widget", "\344\270\255\345\215\225", nullptr));
        comboFilterValue->setItemText(2, QCoreApplication::translate("Widget", "\344\270\213\350\267\257", nullptr));
        comboFilterValue->setItemText(3, QCoreApplication::translate("Widget", "\346\211\223\351\207\216", nullptr));
        comboFilterValue->setItemText(4, QCoreApplication::translate("Widget", "\350\276\205\345\212\251", nullptr));

        labelItems->setText(QCoreApplication::translate("Widget", "\350\243\205\345\244\207\345\206\205\345\256\271\345\215\263\345\260\206\344\270\212\347\272\277", nullptr));
        labelRunes->setText(QCoreApplication::translate("Widget", "\347\254\246\346\226\207\345\206\205\345\256\271\345\215\263\345\260\206\344\270\212\347\272\277", nullptr));
        labelNews->setText(QCoreApplication::translate("Widget", "\350\265\204\350\256\257\351\241\265\351\235\242\357\274\210\345\215\240\344\275\215\357\274\211", nullptr));
        labelGuides->setText(QCoreApplication::translate("Widget", "\346\224\273\347\225\245\351\241\265\351\235\242\357\274\210\345\215\240\344\275\215\357\274\211", nullptr));
        labelInteract->setText(QCoreApplication::translate("Widget", "\344\272\222\345\212\250\351\241\265\351\235\242\357\274\210\345\215\240\344\275\215\357\274\211", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
