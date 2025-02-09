// SPDX-FileCopyrightText: 2022 Mikhail Zolotukhin <mail@gikari.com>
// SPDX-License-Identifier: MIT

#include "client.hpp"
#include "toplevel.hpp"

namespace PlasmaApi
{

Client::Client(QObject *kwinImpl)
    : TopLevel(kwinImpl){};

Client::Client(const Client &rhs)
    : TopLevel(rhs){};
}
