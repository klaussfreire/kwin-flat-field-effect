#include "flat.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 13, 0)
KWIN_EFFECT_FACTORY_ENABLED(FlatCalibrationEffectFactory,
                            KWin::FlatCalibrationEffect,
                            "flatCalibration.json",
                            return false;)
#else
KWIN_EFFECT_FACTORY_ENABLED(KWin::FlatCalibrationEffect,
                            "flatCalibration.json",
                            return false;)
#endif

#include "plugin.moc"
