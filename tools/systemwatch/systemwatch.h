/*
 * systemwatch.h - Detect changes in the system state.
 * Copyright (C) 2005  Remko Troncon
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifndef SYSTEMWATCH_H
#define SYSTEMWATCH_H

#include <QObject>

class SystemWatch : public QObject
{
    Q_OBJECT

public:
    static SystemWatch* instance();
    virtual void proceedWithSleep();

signals:
    void sleep();
    void idleSleep();
    void wakeup();

protected:
    SystemWatch();

private:
    static SystemWatch* instance_;
};

#endif
