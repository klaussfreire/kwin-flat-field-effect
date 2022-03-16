/********************************************************************
 KWin - the KDE window manager
 This file is part of the KDE project.

Copyright (C) 2007 Rivo Laks <rivolaks@hot.ee>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/

#ifndef KWIN_FLAT_CONFIG_H
#define KWIN_FLAT_CONFIG_H

#include <kcmodule.h>
#include "ui_flat_config.h"

class KShortcutsEditor;

namespace KWin
{

class FlatEffectConfigForm : public QWidget, public Ui::FlatEffectConfigForm
    {
        Q_OBJECT
        public:
            explicit FlatEffectConfigForm(QWidget* parent);
};
class FlatEffectConfig : public KCModule
{
    Q_OBJECT
public:
    explicit FlatEffectConfig(QWidget* parent = nullptr, const QVariantList& args = QVariantList());
    ~FlatEffectConfig() override;

public Q_SLOTS:
    void save() override;
    void defaults() override;

private:
    FlatEffectConfigForm* m_ui;
};

} // namespace

#endif
