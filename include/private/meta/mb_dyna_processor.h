/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins-mb-dyna-processor
 * Created on: 25 нояб. 2020 г.
 *
 * lsp-plugins-mb-dyna-processor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins-mb-dyna-processor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins-mb-dyna-processor. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef PRIVATE_META_MB_DYNA_PROCESSOR_H_
#define PRIVATE_META_MB_DYNA_PROCESSOR_H_

#include <lsp-plug.in/dsp-units/misc/windows.h>
#include <lsp-plug.in/plug-fw/meta/types.h>
#include <lsp-plug.in/plug-fw/const.h>

namespace lsp
{
    //-------------------------------------------------------------------------
    // Plugin metadata
    namespace meta
    {
        typedef struct mb_dyna_processor
        {
            static constexpr float  THRESHOLD_MIN           = GAIN_AMP_M_72_DB;
            static constexpr float  THRESHOLD_MAX           = GAIN_AMP_P_24_DB;
            static constexpr float  THRESHOLD_DFL           = GAIN_AMP_M_12_DB;
            static constexpr float  THRESHOLD_STEP          = 0.05f;

            static constexpr float  ATTACK_LVL_MIN          = GAIN_AMP_M_72_DB;
            static constexpr float  ATTACK_LVL_MAX          = GAIN_AMP_P_24_DB;
            static constexpr float  ATTACK_LVL_DFL          = GAIN_AMP_M_12_DB;
            static constexpr float  ATTACK_LVL_STEP         = 0.05f;

            static constexpr float  RELEASE_LVL_MIN         = GAIN_AMP_M_72_DB;
            static constexpr float  RELEASE_LVL_MAX         = GAIN_AMP_P_24_DB;
            static constexpr float  RELEASE_LVL_DFL         = GAIN_AMP_M_12_DB;
            static constexpr float  RELEASE_LVL_STEP        = 0.05f;

            static constexpr float  ATTACK_TIME_MIN         = 0.0f;
            static constexpr float  ATTACK_TIME_MAX         = 5000.0f;
            static constexpr float  ATTACK_TIME_DFL         = 20.0f;
            static constexpr float  ATTACK_TIME_STEP        = 0.0025f;

            static constexpr float  RELEASE_TIME_MIN        = 0.0f;
            static constexpr float  RELEASE_TIME_MAX        = 5000.0f;
            static constexpr float  RELEASE_TIME_DFL        = 100.0f;
            static constexpr float  RELEASE_TIME_STEP       = 0.0025f;

            static constexpr float  KNEE_MIN                = GAIN_AMP_M_24_DB;
            static constexpr float  KNEE_MAX                = GAIN_AMP_0_DB;
            static constexpr float  KNEE_DFL                = GAIN_AMP_M_6_DB;
            static constexpr float  KNEE_STEP               = 0.01f;

            static constexpr float  MAKEUP_MIN              = GAIN_AMP_M_72_DB;
            static constexpr float  MAKEUP_MAX              = GAIN_AMP_P_24_DB;
            static constexpr float  MAKEUP_DFL              = GAIN_AMP_0_DB;
            static constexpr float  MAKEUP_STEP             = 0.05f;

            static constexpr float  RATIO_MIN               = 0.01f;
            static constexpr float  RATIO_MAX               = 100.0f;
            static constexpr float  RATIO_DFL               = 1.0f;
            static constexpr float  RATIO_STEP              = 0.0025f;

            static constexpr float  LOOKAHEAD_MIN           = 0.0f;
            static constexpr float  LOOKAHEAD_MAX           = 20.0f;
            static constexpr float  LOOKAHEAD_DFL           = 0.0f;
            static constexpr float  LOOKAHEAD_STEP          = 0.01f;

            static constexpr float  REACTIVITY_MIN          = 0.000;    // Minimum reactivity [ms]
            static constexpr float  REACTIVITY_MAX          = 250;      // Maximum reactivity [ms]
            static constexpr float  REACTIVITY_DFL          = 10;       // Default reactivity [ms]
            static constexpr float  REACTIVITY_STEP         = 0.01;     // Reactivity step

            static constexpr size_t SC_BAND_DFL             = 0;
            static constexpr size_t SC_MODE_DFL             = 1;
            static constexpr size_t SC_SOURCE_DFL           = 0;
            static constexpr size_t SC_TYPE_DFL             = 0;


            static constexpr size_t CURVE_MESH_SIZE         = 256;
            static constexpr float  CURVE_DB_MIN            = -72;
            static constexpr float  CURVE_DB_MAX            = +24;

            static constexpr size_t TIME_MESH_SIZE          = 400;
            static constexpr float  TIME_HISTORY_MAX        = 5.0f;

            static constexpr float  FREQ_MIN                = 10.0f;
            static constexpr float  FREQ_MAX                = 20000.0f;
            static constexpr float  FREQ_DFL                = 1000.0f;
            static constexpr float  FREQ_STEP               = 0.002f;

            static constexpr float  ZOOM_MIN                = GAIN_AMP_M_18_DB;
            static constexpr float  ZOOM_MAX                = GAIN_AMP_0_DB;
            static constexpr float  ZOOM_DFL                = GAIN_AMP_0_DB;
            static constexpr float  ZOOM_STEP               = 0.0125f;

            static constexpr float  FREQ_BOOST_MIN          = 10.0/9.0f;
            static constexpr float  FREQ_BOOST_MAX          = 20000.0f;

            static constexpr float  OUT_FREQ_MIN            = 0.0f;
            static constexpr float  OUT_FREQ_MAX            = MAX_SAMPLE_RATE;
            static constexpr float  OUT_FREQ_DFL            = 1000.0f;
            static constexpr float  OUT_FREQ_STEP           = 0.002f;


            static constexpr float  IN_GAIN_DFL             = 1.0f;
            static constexpr float  OUT_GAIN_DFL            = 1.0f;

            static constexpr float  REACT_TIME_MIN          = 0.000;
            static constexpr float  REACT_TIME_MAX          = 1.000;
            static constexpr float  REACT_TIME_DFL          = 0.200;
            static constexpr float  REACT_TIME_STEP         = 0.001;
            static constexpr size_t FFT_MESH_POINTS         = 640;

            static constexpr size_t FFT_RANK                = 13;
            static constexpr size_t FFT_ITEMS               = 1 << FFT_RANK;
            static constexpr size_t MESH_POINTS             = 640;
            static constexpr size_t FILTER_MESH_POINTS      = FFT_MESH_POINTS + 2;
            static constexpr size_t FFT_WINDOW              = dspu::windows::HANN;

            static constexpr size_t BANDS_MAX               = 8;
            static constexpr size_t BANDS_DFL               = 4;

            static constexpr size_t REFRESH_RATE            = 20;

            static constexpr size_t DOTS                    = 4;
            static constexpr size_t RANGES                  = DOTS + 1;

            enum boost_t
            {
                FB_OFF,
                FB_BT_3DB,
                FB_MT_3DB,
                FB_BT_6DB,
                FB_MT_6DB,

                FB_DEFAULT              = FB_BT_3DB
            };
        } mb_dyna_processor;

        // Plugin type metadata
        extern const meta::plugin_t mb_dyna_processor_mono;
        extern const meta::plugin_t mb_dyna_processor_stereo;
        extern const meta::plugin_t mb_dyna_processor_lr;
        extern const meta::plugin_t mb_dyna_processor_ms;
        extern const meta::plugin_t sc_mb_dyna_processor_mono;
        extern const meta::plugin_t sc_mb_dyna_processor_stereo;
        extern const meta::plugin_t sc_mb_dyna_processor_lr;
        extern const meta::plugin_t sc_mb_dyna_processor_ms;
    }
}

#endif /* PRIVATE_META_MB_DYNA_PROCESSOR_H_ */
