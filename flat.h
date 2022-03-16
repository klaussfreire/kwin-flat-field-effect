/********************************************************************
 KWin - the KDE window manager
 This file is part of the KDE project.

Copyright (C) 2007 Rivo Laks <rivolaks@hot.ee>
Copyright (C) 2008 Lucas Murray <lmurray@undefinedfire.com>

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

#ifndef KWIN_FLAT_H
#define KWIN_FLAT_H

#include <kwineffects.h>
#include <kwinglutils.h>

namespace KWin
{

class GLShader;

/**
 * Applies flat calibration to desktop
 */
class FlatCalibrationEffect
    : public Effect
{
    Q_OBJECT
    Q_PROPERTY(double offset READ offset);
    Q_PROPERTY(double gain READ gain);
    Q_PROPERTY(double strength READ strength);
    Q_PROPERTY(QString flatpath READ flatpath);
public:
    FlatCalibrationEffect();
    ~FlatCalibrationEffect() override;
    void reconfigure(ReconfigureFlags) override;

    void drawWindow(EffectWindow* w, int mask, const QRegion &region, WindowPaintData& data) override;
    void paintEffectFrame(KWin::EffectFrame* frame, const QRegion &region, double opacity, double frameOpacity) override;
    bool isActive() const override;
    bool provides(Feature) override;
    QString getWindowApplicationName(EffectWindow * w);

    int requestedEffectChainPosition() const override;

    static bool supported();

    double offset() const;
    double gain() const;
    double strength() const;
    const QString& flatpath() const;
public Q_SLOTS:
    void toggleFlatCalibration();

protected:
    bool loadData();
    void updateShader();

private:
    bool m_inited;
    bool m_valid;
    bool m_allWindows;
    double m_offset;
    double m_gain;
    double m_strength;
    QString m_flatpath;

    GLShader* m_shader;
    GLTexture* m_flattex;

    int m_gain_loc;
    int m_offset_loc;
    int m_strength_loc;
    int m_inv_screenres_loc;
    int m_correction_loc;
};

inline double FlatCalibrationEffect::offset() const
{
        return m_offset;
}

inline double FlatCalibrationEffect::gain() const
{
        return m_gain;
}

inline double FlatCalibrationEffect::strength() const
{
        return m_strength;
}

inline const QString& FlatCalibrationEffect::flatpath() const
{
        return m_flatpath;
}

inline int FlatCalibrationEffect::requestedEffectChainPosition() const
{
    return 99;
}

} // namespace

#endif
