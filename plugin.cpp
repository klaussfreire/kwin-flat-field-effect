#include "flat.h"

KWIN_EFFECT_FACTORY_ENABLED(FlatCalibrationEffectFactory,
                            KWin::FlatCalibrationEffect,
                            "flatCalibration.json",
                            return false;)

#include "plugin.moc"
