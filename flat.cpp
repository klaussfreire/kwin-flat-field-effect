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

#include "flat.h"

// KConfigSkeleton
#include "flatconfig.h"

#include <QAction>
#include <QFile>
#include <QApplication>
#include <QDesktopWidget>
#include <kwinglutils.h>
#include <kwinglplatform.h>
#include <KGlobalAccel>
#include <KLocalizedString>
#include <QStandardPaths>
#include <iostream>

#include <QMatrix4x4>

namespace KWin
{

  FlatCalibrationEffect::FlatCalibrationEffect()
    :   m_inited(false),
        m_valid(true),
        m_allWindows(false),
        m_shader(nullptr),
        m_flattex(nullptr)
  {
    initConfig<FlatConfig>();

    QAction* a = new QAction(this);
    a->setObjectName(QStringLiteral("FlatCalibration"));
    a->setText(i18n("Toggle Flat Calibration Effect"));
    KGlobalAccel::self()->setDefaultShortcut(a, QList<QKeySequence>() << Qt::CTRL + Qt::META + Qt::Key_F);
    KGlobalAccel::self()->setShortcut(a, QList<QKeySequence>() << Qt::CTRL + Qt::META + Qt::Key_F);
    effects->registerGlobalShortcut(Qt::CTRL + Qt::META + Qt::Key_F, a);
    connect(a, &QAction::triggered, this, &FlatCalibrationEffect::toggleFlatCalibration);

    reconfigure(ReconfigureAll);
  }

  FlatCalibrationEffect::~FlatCalibrationEffect()
  {
    delete m_shader;
    delete m_flattex;
  }

  bool FlatCalibrationEffect::supported()
  {
#if QT_VERSION < QT_VERSION_CHECK(5, 13, 0)
    return effects->compositingType() == OpenGL2Compositing;
#else
    std::cout <<  (effects->compositingType() == OpenGLCompositing) << std::endl;
    return effects->compositingType() == OpenGLCompositing;
#endif
  }

  void FlatCalibrationEffect::reconfigure(ReconfigureFlags)
  {
    FlatConfig::self()->read();
    m_gain = FlatConfig::gain();
    m_strength = FlatConfig::strength();
    m_offset = FlatConfig::flatOffset();
    m_flatpath = FlatConfig::flatPath();
  }

  bool FlatCalibrationEffect::loadData()
  {
    m_inited = true;

    QString shadersDir(QStringLiteral("kwin/shaders/1.10/"));
#ifdef KWIN_HAVE_OPENGLES
    const qint64 coreVersionNumber = kVersionNumber(3, 0);
#else
    const qint64 version = KWin::kVersionNumber(1, 40);
#endif
    if (KWin::GLPlatform::instance()->glslVersion() >= version)
      shadersDir = QStringLiteral("kwin/shaders/1.40/");

    const QString fragmentshader = QStandardPaths::locate(QStandardPaths::GenericDataLocation, shadersDir + QStringLiteral("flat.frag"));

    QFile file(fragmentshader);
    if (file.open(QFile::ReadOnly))
    {
      QByteArray frag = file.readAll();
      m_shader = KWin::ShaderManager::instance()->generateCustomShader(KWin::ShaderTrait::MapTexture, QByteArray(), frag);
      file.close();

      if (!m_shader->isValid()) {
        //qCCritical(KWINEFFECTS) << "The shader failed to load!";
        return false;
      }

      m_gain_loc = m_shader->uniformLocation("gain");
      m_offset_loc = m_shader->uniformLocation("offset");
      m_strength_loc = m_shader->uniformLocation("strength");
      m_inv_screenres_loc = m_shader->uniformLocation("inv_screenres");
      m_correction_loc = m_shader->uniformLocation("correction");
      m_flattex = new GLTexture(m_flatpath);

      return true;
    }
    else {
      deleteLater();
      return false;
    }
  }

  QString FlatCalibrationEffect::getWindowApplicationName(EffectWindow * w) {
    auto windowClass = w->windowClass();
    return windowClass.split(" ")[1].toLower();
  }

  void FlatCalibrationEffect::drawWindow(EffectWindow* w, int mask, const QRegion &region, WindowPaintData& data)
  {
    // Load if we haven't already
    if (m_valid && !m_inited)
      m_valid = loadData();

    bool useShader = m_valid && m_allWindows;
    auto shader = m_shader;

    if (useShader) {
      ShaderManager *shaderManager = ShaderManager::instance();
      shaderManager->pushShader(shader);

      data.shader = shader;

      updateShader(w->screen());
    }

    effects->drawWindow(w, mask, region, data);

    if (useShader) {
      ShaderManager::instance()->popShader();
    }
  }

  void FlatCalibrationEffect::paintEffectFrame(KWin::EffectFrame* frame, const QRegion &region, double opacity, double frameOpacity)
  {
    if (m_valid && m_allWindows) {
      frame->setShader(m_shader);
      ShaderBinder binder(m_shader);

      updateShader(nullptr);

      effects->paintEffectFrame(frame, region, opacity, frameOpacity);
    } else {
      effects->paintEffectFrame(frame, region, opacity, frameOpacity);
    }
  }

  void FlatCalibrationEffect::toggleFlatCalibration()
  {
    m_valid = loadData(); //hotswap
    m_allWindows = !m_allWindows;
    effects->addRepaintFull();
  }

  bool FlatCalibrationEffect::isActive() const
  {
    return m_valid && m_allWindows;
  }

  bool FlatCalibrationEffect::provides(Feature f)
  {
    return f == ScreenInversion;
  }

  void FlatCalibrationEffect::updateShader(EffectScreen *screen)
  {
    m_shader->setUniform(m_gain_loc, (float)gain());
    m_shader->setUniform(m_offset_loc, (float)offset());
    m_shader->setUniform(m_strength_loc, 1.0f / (float)strength());
    if (screen != nullptr) {
	QRect rec = screen->geometry();
        m_shader->setUniform(m_inv_screenres_loc, QVector2D(
            (float)(1.0 / rec.width()), (float)(1.0 / rec.height())));
    }
    m_shader->setUniform(m_correction_loc, 1);

    glActiveTexture(GL_TEXTURE0 + 1);
    m_flattex->bind();
    glActiveTexture(GL_TEXTURE0);
  }

} // namespace
