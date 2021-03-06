/*
 * This file is part of superterm.
 *
 * Copyright (c) 2014-2018 Stefan Scheler.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HIGHLIGHTING_H
#define HIGHLIGHTING_H

#include <QColor>
#include <QMetaType>
#include <QString>

class Highlighting
{
public:
    QString pattern;
    QColor  color;
};

Q_DECLARE_METATYPE(Highlighting)

QDataStream& operator<<(QDataStream& out, const Highlighting& v);
QDataStream& operator>>(QDataStream& in, Highlighting& v);

#endif // HIGHLIGHTING_H
