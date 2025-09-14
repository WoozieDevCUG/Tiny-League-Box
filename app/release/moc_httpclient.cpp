/****************************************************************************
** Meta object code from reading C++ file 'httpclient.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../network/httpclient.h"
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'httpclient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN10HttpClientE_t {};
} // unnamed namespace

template <> constexpr inline auto HttpClient::qt_create_metaobjectdata<qt_meta_tag_ZN10HttpClientE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "HttpClient",
        "loginSucceeded",
        "",
        "token",
        "username",
        "avatarUrl",
        "loginFailed",
        "message",
        "registerSucceeded",
        "registerFailed",
        "heroesFetched",
        "QList<HeroBrief>",
        "heroes",
        "heroDetailFetched",
        "heroId",
        "HeroFullInfo",
        "hero",
        "heroDetailFailed",
        "imageDownloaded",
        "pixmap",
        "imageDownloadedByUrl",
        "url",
        "requestFailed",
        "avatarUploadSucceeded",
        "avatarUploadFailed",
        "userProfileFetched",
        "level",
        "userProfileFailed"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'loginSucceeded'
        QtMocHelpers::SignalData<void(const QString &, const QString &, const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 4 }, { QMetaType::QString, 5 },
        }}),
        // Signal 'loginFailed'
        QtMocHelpers::SignalData<void(const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Signal 'registerSucceeded'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'registerFailed'
        QtMocHelpers::SignalData<void(const QString &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Signal 'heroesFetched'
        QtMocHelpers::SignalData<void(const QList<HeroBrief> &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 11, 12 },
        }}),
        // Signal 'heroDetailFetched'
        QtMocHelpers::SignalData<void(int, const HeroFullInfo &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 14 }, { 0x80000000 | 15, 16 },
        }}),
        // Signal 'heroDetailFailed'
        QtMocHelpers::SignalData<void(const QString &)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Signal 'imageDownloaded'
        QtMocHelpers::SignalData<void(int, const QPixmap &)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 14 }, { QMetaType::QPixmap, 19 },
        }}),
        // Signal 'imageDownloadedByUrl'
        QtMocHelpers::SignalData<void(const QString &, const QPixmap &)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 21 }, { QMetaType::QPixmap, 19 },
        }}),
        // Signal 'requestFailed'
        QtMocHelpers::SignalData<void(const QString &)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Signal 'avatarUploadSucceeded'
        QtMocHelpers::SignalData<void(const QString &)>(23, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Signal 'avatarUploadFailed'
        QtMocHelpers::SignalData<void(const QString &)>(24, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Signal 'userProfileFetched'
        QtMocHelpers::SignalData<void(const QString &, int, const QString &)>(25, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 4 }, { QMetaType::Int, 26 }, { QMetaType::QString, 5 },
        }}),
        // Signal 'userProfileFailed'
        QtMocHelpers::SignalData<void(const QString &)>(27, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<HttpClient, qt_meta_tag_ZN10HttpClientE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject HttpClient::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10HttpClientE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10HttpClientE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10HttpClientE_t>.metaTypes,
    nullptr
} };

void HttpClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<HttpClient *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->loginSucceeded((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 1: _t->loginFailed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->registerSucceeded(); break;
        case 3: _t->registerFailed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->heroesFetched((*reinterpret_cast< std::add_pointer_t<QList<HeroBrief>>>(_a[1]))); break;
        case 5: _t->heroDetailFetched((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<HeroFullInfo>>(_a[2]))); break;
        case 6: _t->heroDetailFailed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->imageDownloaded((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QPixmap>>(_a[2]))); break;
        case 8: _t->imageDownloadedByUrl((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QPixmap>>(_a[2]))); break;
        case 9: _t->requestFailed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 10: _t->avatarUploadSucceeded((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->avatarUploadFailed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 12: _t->userProfileFetched((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 13: _t->userProfileFailed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (HttpClient::*)(const QString & , const QString & , const QString & )>(_a, &HttpClient::loginSucceeded, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (HttpClient::*)(const QString & )>(_a, &HttpClient::loginFailed, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (HttpClient::*)()>(_a, &HttpClient::registerSucceeded, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (HttpClient::*)(const QString & )>(_a, &HttpClient::registerFailed, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (HttpClient::*)(const QList<HeroBrief> & )>(_a, &HttpClient::heroesFetched, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (HttpClient::*)(int , const HeroFullInfo & )>(_a, &HttpClient::heroDetailFetched, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (HttpClient::*)(const QString & )>(_a, &HttpClient::heroDetailFailed, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (HttpClient::*)(int , const QPixmap & )>(_a, &HttpClient::imageDownloaded, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (HttpClient::*)(const QString & , const QPixmap & )>(_a, &HttpClient::imageDownloadedByUrl, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (HttpClient::*)(const QString & )>(_a, &HttpClient::requestFailed, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (HttpClient::*)(const QString & )>(_a, &HttpClient::avatarUploadSucceeded, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (HttpClient::*)(const QString & )>(_a, &HttpClient::avatarUploadFailed, 11))
            return;
        if (QtMocHelpers::indexOfMethod<void (HttpClient::*)(const QString & , int , const QString & )>(_a, &HttpClient::userProfileFetched, 12))
            return;
        if (QtMocHelpers::indexOfMethod<void (HttpClient::*)(const QString & )>(_a, &HttpClient::userProfileFailed, 13))
            return;
    }
}

const QMetaObject *HttpClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HttpClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10HttpClientE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int HttpClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void HttpClient::loginSucceeded(const QString & _t1, const QString & _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2, _t3);
}

// SIGNAL 1
void HttpClient::loginFailed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void HttpClient::registerSucceeded()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void HttpClient::registerFailed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void HttpClient::heroesFetched(const QList<HeroBrief> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void HttpClient::heroDetailFetched(int _t1, const HeroFullInfo & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2);
}

// SIGNAL 6
void HttpClient::heroDetailFailed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void HttpClient::imageDownloaded(int _t1, const QPixmap & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1, _t2);
}

// SIGNAL 8
void HttpClient::imageDownloadedByUrl(const QString & _t1, const QPixmap & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1, _t2);
}

// SIGNAL 9
void HttpClient::requestFailed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1);
}

// SIGNAL 10
void HttpClient::avatarUploadSucceeded(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 10, nullptr, _t1);
}

// SIGNAL 11
void HttpClient::avatarUploadFailed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 11, nullptr, _t1);
}

// SIGNAL 12
void HttpClient::userProfileFetched(const QString & _t1, int _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 12, nullptr, _t1, _t2, _t3);
}

// SIGNAL 13
void HttpClient::userProfileFailed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 13, nullptr, _t1);
}
QT_WARNING_POP
