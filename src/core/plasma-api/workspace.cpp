// SPDX-FileCopyrightText: 2022 Mikhail Zolotukhin <mail@gikari.com>
// SPDX-License-Identifier: MIT

#include "workspace.hpp"

#include <QQmlContext>

#include "logger.hpp"
#include "plasma-api/api.hpp"
#include "plasma-api/client.hpp"

namespace PlasmaApi
{

Workspace::Workspace(QQmlEngine *engine)
    : QObject()
    , m_engine(engine)
    , m_kwinImpl(m_engine->rootContext()->contextProperty(QStringLiteral("workspace")).value<QObject *>())
{
    wrapSignals();
}

Workspace::Workspace(const Workspace &rhs)
    : QObject()
    , m_engine(rhs.m_engine)
    , m_kwinImpl(rhs.m_kwinImpl)
{
    wrapSignals();
};

void Workspace::wrapSignals()
{
    auto wrapSimpleSignal = [this](const char *signalSignature) {
        auto signalsSignature = QMetaObject::normalizedSignature(signalSignature);
        connect(m_kwinImpl, signalsSignature, this, signalsSignature);
    };

    auto wrapComplexSignal = [this](const char *implSignalSignature, const char *thisSignalSignature) {
        auto implNormSignature = QMetaObject::normalizedSignature(implSignalSignature);
        auto thisNormSignature = QMetaObject::normalizedSignature(thisSignalSignature);
        connect(m_kwinImpl, implNormSignature, this, thisNormSignature);
    };

    wrapSimpleSignal(SIGNAL(numberScreensChanged(int count)));
    wrapSimpleSignal(SIGNAL(screenResized(int screen)));
    wrapSimpleSignal(SIGNAL(currentActivityChanged(const QString &)));

    wrapComplexSignal(SIGNAL(currentDesktopChanged(int, KWin::AbstractClient *)), SLOT(currentDesktopChangedTransformer(int, KWin::AbstractClient *)));
    wrapComplexSignal(SIGNAL(clientAdded(KWin::AbstractClient *)), SLOT(clientAddedTransformer(KWin::AbstractClient *)));
    wrapComplexSignal(SIGNAL(clientRemoved(KWin::AbstractClient *)), SLOT(clientRemovedTransformer(KWin::AbstractClient *)));
    wrapComplexSignal(SIGNAL(clientMinimized(KWin::AbstractClient *)), SLOT(clientMinimizedTransformer(KWin::AbstractClient *)));
    wrapComplexSignal(SIGNAL(clientUnminimized(KWin::AbstractClient *)), SLOT(clientUnminimizedTransformer(KWin::AbstractClient *)));
    wrapComplexSignal(SIGNAL(clientMaximizeSet(KWin::AbstractClient *, bool h, bool v)),
                      SLOT(clientMaximizeSetTransformer(KWin::AbstractClient *, bool h, bool v)));
};

QRect Workspace::clientArea(ClientAreaOption option, int screen, int desktop)
{
    BI_METHOD_IMPL_WRAP(QRect, "clientArea(ClientAreaOption, int, int)", Q_ARG(ClientAreaOption, option), Q_ARG(int, screen), Q_ARG(int, desktop));
};

void Workspace::currentDesktopChangedTransformer(int desktop, KWin::AbstractClient *kwinClient)
{
    // Since we don't know the KWin internal implementation we have to use reinterpret_cast
    auto clientWrapper = Client(reinterpret_cast<QObject *>(kwinClient));
    Q_EMIT currentDesktopChanged(desktop, clientWrapper);
};

void Workspace::clientAddedTransformer(KWin::AbstractClient *kwinClient)
{
    auto clientWrapper = Client(reinterpret_cast<QObject *>(kwinClient));
    Q_EMIT clientAdded(clientWrapper);
}

void Workspace::clientRemovedTransformer(KWin::AbstractClient *kwinClient)
{
    auto clientWrapper = Client(reinterpret_cast<QObject *>(kwinClient));
    Q_EMIT clientRemoved(clientWrapper);
}

void Workspace::clientMinimizedTransformer(KWin::AbstractClient *kwinClient)
{
    auto clientWrapper = Client(reinterpret_cast<QObject *>(kwinClient));
    Q_EMIT clientMinimized(clientWrapper);
}

void Workspace::clientUnminimizedTransformer(KWin::AbstractClient *kwinClient)
{
    auto clientWrapper = Client(reinterpret_cast<QObject *>(kwinClient));
    Q_EMIT clientUnminimized(clientWrapper);
}

void Workspace::clientMaximizeSetTransformer(KWin::AbstractClient *kwinClient, bool h, bool v)
{
    auto clientWrapper = Client(reinterpret_cast<QObject *>(kwinClient));
    Q_EMIT clientMaximizeSet(clientWrapper, h, v);
}

}
