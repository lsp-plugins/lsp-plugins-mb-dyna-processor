/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#include <lsp-plug.in/dsp-units/util/Sidechain.h>
#include <lsp-plug.in/plug-fw/meta/ports.h>
#include <lsp-plug.in/shared/meta/developers.h>
#include <private/meta/mb_dyna_processor.h>

#define LSP_PLUGINS_MB_DYNA_PROCESSOR_VERSION_MAJOR       1
#define LSP_PLUGINS_MB_DYNA_PROCESSOR_VERSION_MINOR       0
#define LSP_PLUGINS_MB_DYNA_PROCESSOR_VERSION_MICRO       22

#define LSP_PLUGINS_MB_DYNA_PROCESSOR_VERSION  \
    LSP_MODULE_VERSION( \
        LSP_PLUGINS_MB_DYNA_PROCESSOR_VERSION_MAJOR, \
        LSP_PLUGINS_MB_DYNA_PROCESSOR_VERSION_MINOR, \
        LSP_PLUGINS_MB_DYNA_PROCESSOR_VERSION_MICRO  \
    )

namespace lsp
{
    namespace meta
    {
        //-------------------------------------------------------------------------
        // Multiband dynamics processor
        static const int plugin_classes[]           = { C_DYNAMICS, -1 };
        static const int clap_features_mono[]       = { CF_AUDIO_EFFECT, CF_MONO, -1 };
        static const int clap_features_stereo[]     = { CF_AUDIO_EFFECT, CF_STEREO, -1 };

        static const port_item_t mb_dyna_sc_type[] =
        {
            { "Internal",       "sidechain.internal"        },
            { "Link",           "sidechain.link"            },
            { NULL, NULL }
        };

        static const port_item_t mb_dyna_sc_type_sc[] =
        {
            { "Internal",       "sidechain.internal"        },
            { "External",       "sidechain.external"        },
            { "Link",           "sidechain.link"            },
            { NULL, NULL }
        };

        static const port_item_t mb_dyna_sc_modes[] =
        {
            { "Peak",           "sidechain.peak"            },
            { "RMS",            "sidechain.rms"             },
            { "LPF",            "sidechain.lpf"             },
            { "SMA",            "sidechain.sma"             },
            { NULL, NULL }
        };

        static const port_item_t mb_dyna_sc_source[] =
        {
            { "Middle",         "sidechain.middle"          },
            { "Side",           "sidechain.side"            },
            { "Left",           "sidechain.left"            },
            { "Right",          "sidechain.right"           },
            { "Min",            "sidechain.min"             },
            { "Max",            "sidechain.max"             },
            { NULL, NULL }
        };

        static const port_item_t mb_dyna_sc_split_source[] =
        {
            { "Left/Right",     "sidechain.left_right"      },
            { "Right/Left",     "sidechain.right_left"      },
            { "Mid/Side",       "sidechain.mid_side"        },
            { "Side/Mid",       "sidechain.side_mid"        },
            { "Min",            "sidechain.min"             },
            { "Max",            "sidechain.max"             },
            { NULL, NULL }
        };

        static const port_item_t mb_dyna_sc_boost[] =
        {
            { "None",           "sidechain.boost.none" },
            { "Pink BT",        "sidechain.boost.pink_bt" },
            { "Pink MT",        "sidechain.boost.pink_mt" },
            { "Brown BT",       "sidechain.boost.brown_bt" },
            { "Brown MT",       "sidechain.boost.brown_mt" },
            { NULL, NULL }
        };

        static const port_item_t mb_global_dyna_modes[] =
        {
            { "Classic",        "mb_dyna_processor.classic"         },
            { "Modern",         "mb_dyna_processor.modern"          },
            { "Linear Phase",   "mb_dyna_processor.linear_phase"    },
            { NULL, NULL }
        };

        static const port_item_t mb_dyna_sc_bands[] =
        {
            { "Split",          "mb_dyna_processor.split" },
            { "Band 0",         "mb_dyna_processor.band0" },
            { "Band 1",         "mb_dyna_processor.band1" },
            { "Band 2",         "mb_dyna_processor.band2" },
            { "Band 3",         "mb_dyna_processor.band3" },
            { "Band 4",         "mb_dyna_processor.band4" },
            { "Band 5",         "mb_dyna_processor.band5" },
            { "Band 6",         "mb_dyna_processor.band6" },
            { "Band 7",         "mb_dyna_processor.band7" },
            { NULL, NULL }
        };

        static const port_item_t mb_dyna_sc_lr_bands[] =
        {
            { "Split Left",     "mb_dyna_processor.split_left"      },
            { "Split Right",    "mb_dyna_processor.split_right"     },
            { "Band 0 Left",    "mb_dyna_processor.band0_left"      },
            { "Band 0 Right",   "mb_dyna_processor.band0_right"     },
            { "Band 1 Left",    "mb_dyna_processor.band1_left"      },
            { "Band 1 Right",   "mb_dyna_processor.band1_right"     },
            { "Band 2 Left",    "mb_dyna_processor.band2_left"      },
            { "Band 2 Right",   "mb_dyna_processor.band2_right"     },
            { "Band 3 Left",    "mb_dyna_processor.band3_left"      },
            { "Band 3 Right",   "mb_dyna_processor.band3_right"     },
            { "Band 4 Left",    "mb_dyna_processor.band4_left"      },
            { "Band 4 Right",   "mb_dyna_processor.band4_right"     },
            { "Band 5 Left",    "mb_dyna_processor.band5_left"      },
            { "Band 5 Right",   "mb_dyna_processor.band5_right"     },
            { "Band 6 Left",    "mb_dyna_processor.band6_left"      },
            { "Band 6 Right",   "mb_dyna_processor.band6_right"     },
            { "Band 7 Left",    "mb_dyna_processor.band7_left"      },
            { "Band 7 Right",   "mb_dyna_processor.band7_right"     },
            { NULL, NULL }
        };

        static const port_item_t mb_dyna_sc_ms_bands[] =
        {
            { "Split Mid",      "mb_dyna_processor.split_middle"    },
            { "Split Side",     "mb_dyna_processor.split_side"      },
            { "Band 0 Mid",     "mb_dyna_processor.band0_middle"    },
            { "Band 0 Side",    "mb_dyna_processor.band0_side"      },
            { "Band 1 Mid",     "mb_dyna_processor.band1_middle"    },
            { "Band 1 Side",    "mb_dyna_processor.band1_side"      },
            { "Band 2 Mid",     "mb_dyna_processor.band2_middle"    },
            { "Band 2 Side",    "mb_dyna_processor.band2_side"      },
            { "Band 3 Mid",     "mb_dyna_processor.band3_middle"    },
            { "Band 3 Side",    "mb_dyna_processor.band3_side"      },
            { "Band 4 Mid",     "mb_dyna_processor.band4_middle"    },
            { "Band 4 Side",    "mb_dyna_processor.band4_side"      },
            { "Band 5 Mid",     "mb_dyna_processor.band5_middle"    },
            { "Band 5 Side",    "mb_dyna_processor.band5_side"      },
            { "Band 6 Mid",     "mb_dyna_processor.band6_middle"    },
            { "Band 6 Side",    "mb_dyna_processor.band6_side"      },
            { "Band 7 Mid",     "mb_dyna_processor.band7_middle"    },
            { "Band 7 Side",    "mb_dyna_processor.band7_side"      },
            { NULL, NULL }
        };

        #define MB_DYNA_SHM_LINK_MONO \
                OPT_RETURN_MONO("link", "shml", "Side-chain shared memory link")

        #define MB_DYNA_SHM_LINK_STEREO \
                OPT_RETURN_STEREO("link", "shml_", "Side-chain shared memory link")

        #define MB_COMMON(bands) \
                BYPASS, \
                COMBO("mode", "Dynamics Processor mode", "Mode", 1, mb_global_dyna_modes), \
                AMP_GAIN("g_in", "Input gain", mb_dyna_processor::IN_GAIN_DFL, 10.0f), \
                AMP_GAIN("g_out", "Output gain", mb_dyna_processor::OUT_GAIN_DFL, 10.0f), \
                AMP_GAIN("g_dry", "Dry gain", 0.0f, 10.0f), \
                AMP_GAIN("g_wet", "Wet gain", 1.0f, 10.0f), \
                PERCENTS("drywet", "Dry/Wet balance", 100.0f, 0.1f), \
                LOG_CONTROL("react", "FFT reactivity", "Reactivity", U_MSEC, mb_dyna_processor::REACT_TIME), \
                AMP_GAIN("shift", "Shift gain", 1.0f, 100.0f), \
                LOG_CONTROL("zoom", "Graph zoom", "Zoom", U_GAIN_AMP, mb_dyna_processor::ZOOM), \
                COMBO("envb", "Envelope boost", "Env boost", mb_dyna_processor::FB_DEFAULT, mb_dyna_sc_boost), \
                COMBO("bsel", "Band selection", "Band selector", mb_dyna_processor::SC_BAND_DFL, bands)

        #define MB_SPLIT(id, label, alias, enable, freq) \
                SWITCH("cbe" id, "Dynamics Processor band enable" label, "Split on" alias, enable), \
                LOG_CONTROL_DFL("sf" id, "Split frequency" label, "Split" alias, U_HZ, mb_dyna_processor::FREQ, freq)

        #define MB_DYNA_POINT(idx, on, id, label, alias, level) \
                SWITCH("pe" #idx id, "Point enable " #idx label, "Point on" #idx alias, on), \
                LOG_CONTROL_DFL("tl" #idx id, "Threshold " #idx label, "Thresh " #idx alias, U_GAIN_AMP, mb_dyna_processor::THRESHOLD, level), \
                LOG_CONTROL_DFL("gl" #idx id, "Gain " #idx label, "Gain " #idx alias, U_GAIN_AMP, mb_dyna_processor::THRESHOLD, level), \
                LOG_CONTROL("kn" #idx id, "Knee " #idx label, "Knee " #idx alias, U_GAIN_AMP, mb_dyna_processor::KNEE), \
                SWITCH("ae" #idx id, "Attack enable " #idx label, "Att on" #idx alias, 0.0f), \
                LOG_CONTROL_DFL("al" #idx id, "Attack level " #idx label, "Att lvl " #idx alias, U_GAIN_AMP, mb_dyna_processor::ATTACK_LVL, level), \
                LOG_CONTROL("at" #idx id, "Attack time " #idx label, "Att time " #idx alias, U_MSEC, mb_dyna_processor::ATTACK_TIME), \
                SWITCH("re" #idx id, "Release enable " #idx label, "Rel on" #idx alias, 0.0f), \
                LOG_CONTROL_DFL("rl" #idx id, "Release level " #idx label, "Rel lvl " #idx alias, U_GAIN_AMP, mb_dyna_processor::RELEASE_LVL, level), \
                LOG_CONTROL("rt" #idx id, "Release time " #idx label, "Rel time " #idx alias, U_MSEC, mb_dyna_processor::RELEASE_TIME)

        #define MB_BAND_COMMON(id, label, alias, x, total, fe, fs) \
                COMBO("scm" id, "Sidechain mode" label, "SC mode" alias, mb_dyna_processor::SC_MODE_DFL, mb_dyna_sc_modes), \
                CONTROL("sla" id, "Sidechain lookahead" label, "SC look" alias, U_MSEC, mb_dyna_processor::LOOKAHEAD), \
                LOG_CONTROL("scr" id, "Sidechain reactivity" label, "SC react" alias, U_MSEC, mb_dyna_processor::REACTIVITY), \
                AMP_GAIN100("scp" id, "Sidechain preamp" label, GAIN_AMP_0_DB), \
                SWITCH("sclc" id, "Sidechain custom lo-cut" label, "SC LCF on" alias, 0), \
                SWITCH("schc" id, "Sidechain custom hi-cut" label, "SC HCF on" alias, 0), \
                LOG_CONTROL_DFL("sclf" id, "Sidechain lo-cut frequency" label, "SC LCF" alias, U_HZ, mb_dyna_processor::FREQ, fe), \
                LOG_CONTROL_DFL("schf" id, "Sidechain hi-cut frequency" label, "SC HCF" alias, U_HZ, mb_dyna_processor::FREQ, fs), \
                MESH("bfc" id, "Side-chain band frequency chart" label, 2, mb_dyna_processor::MESH_POINTS + 4), \
                \
                SWITCH("pe" id, "Processor enable" label, "On " alias, 1.0f), \
                SWITCH("bs" id, "Solo band" label, "Solo " alias, 0.0f), \
                SWITCH("bm" id, "Mute band" label, "Mute " alias, 0.0f), \
                LOG_CONTROL("atd" id, "Attack time default" label, "Att time" alias, U_MSEC, mb_dyna_processor::ATTACK_TIME), \
                LOG_CONTROL("rtd" id, "Release time default" label, "Rel time" alias, U_MSEC, mb_dyna_processor::RELEASE_TIME), \
                MB_DYNA_POINT(0, 1.0f, id, label, alias, GAIN_AMP_M_12_DB), \
                MB_DYNA_POINT(1, 0.0f, id, label, alias, GAIN_AMP_M_24_DB), \
                MB_DYNA_POINT(2, 0.0f, id, label, alias, GAIN_AMP_M_36_DB), \
                MB_DYNA_POINT(3, 0.0f, id, label, alias, GAIN_AMP_M_48_DB), \
                CONTROL("ht" id, "Hold time" label, "Hold time" alias, U_MSEC, mb_dyna_processor::HOLD_TIME), \
                LOG_CONTROL("llr" id, "Low-level ratio" label, "Low ratio" alias, U_NONE, mb_dyna_processor::RATIO), \
                LOG_CONTROL("hlr" id, "High-level ratio" label, "High ratio" alias, U_NONE, mb_dyna_processor::RATIO), \
                LOG_CONTROL("mk" id, "Makeup gain" label, "Makeup" alias, U_GAIN_AMP, mb_dyna_processor::MAKEUP), \
                SWITCH("cmv" id, "Curve modelling visibility" label, "Show curve " alias, 1.0f), \
                HUE_CTL("hue" id, "Hue " label, float(x) / float(total)), \
                \
                METER("fre" id, "Frequency range end" label, U_HZ,  mb_dyna_processor::OUT_FREQ), \
                MESH("cmg" id, "Curve modelling graph" label, 2, mb_dyna_processor::CURVE_MESH_SIZE), \
                MESH("ccg" id, "Compression curve graph" label, 2, mb_dyna_processor::CURVE_MESH_SIZE)

        #define MB_BAND_METERS(id, label) \
                METER_OUT_GAIN("elm" id, "Envelope level meter" label, GAIN_AMP_P_36_DB), \
                METER_OUT_GAIN("clm" id, "Curve level meter" label, GAIN_AMP_P_36_DB), \
                METER_OUT_GAIN("rlm" id, "Reduction level meter" label, GAIN_AMP_P_72_DB)

        #define MB_MONO_BAND(id, label, alias, x, total, fe, fs) \
                COMBO("sce" id, "External sidechain source" label, "Ext SC src" alias, 0.0f, mb_dyna_sc_type), \
                MB_BAND_COMMON(id, label, alias, x, total, fe, fs)

        #define MB_STEREO_BAND(id, label, alias, x, total, fe, fs) \
                COMBO("sce" id, "External sidechain source" label, "Ext SC src" alias, 0.0f, mb_dyna_sc_type), \
                COMBO("scs" id, "Sidechain source" label, "SC source" alias, 0, mb_dyna_sc_source), \
                COMBO("sscs" id, "Split sidechain source" label, "SC split" alias, 0, mb_dyna_sc_split_source), \
                MB_BAND_COMMON(id, label, alias, x, total, fe, fs)

        #define MB_SPLIT_BAND(id, label, alias, x, total, fe, fs) \
                COMBO("sce" id, "External sidechain source" label, "Ext SC src" alias, 0.0f, mb_dyna_sc_type), \
                COMBO("scs" id, "Sidechain source" label, "SC source" alias, dspu::SCS_MIDDLE, mb_dyna_sc_source), \
                MB_BAND_COMMON(id, label, alias, x, total, fe, fs)

        #define MB_SC_MONO_BAND(id, label, alias, x, total, fe, fs) \
                COMBO("sce" id, "External sidechain source" label, "Ext SC src" alias, 0.0f, mb_dyna_sc_type_sc), \
                MB_BAND_COMMON(id, label, alias, x, total, fe, fs)

        #define MB_SC_STEREO_BAND(id, label, alias, x, total, fe, fs) \
                COMBO("sce" id, "External sidechain source" label, "Ext SC src" alias, 0.0f, mb_dyna_sc_type_sc), \
                COMBO("scs" id, "Sidechain source" label, "SC source" alias, 0, mb_dyna_sc_source), \
                COMBO("sscs" id, "Split sidechain source" label, "SC split" alias, 0, mb_dyna_sc_split_source), \
                MB_BAND_COMMON(id, label, alias, x, total, fe, fs)

        #define MB_SC_SPLIT_BAND(id, label, alias, x, total, fe, fs) \
                COMBO("sce" id, "External sidechain source" label, "Ext SC src" alias, 0.0f, mb_dyna_sc_type_sc), \
                COMBO("scs" id, "Sidechain source" label, "SC source" alias, dspu::SCS_MIDDLE, mb_dyna_sc_source), \
                MB_BAND_COMMON(id, label, alias, x, total, fe, fs)

        #define MB_STEREO_CHANNEL \
                SWITCH("flt", "Band filter curves", "Show filters", 1.0f), \
                MESH("ag_l", "Pprocessor amplitude graph Left", 2, mb_dyna_processor::FFT_MESH_POINTS), \
                MESH("ag_r", "Processor amplitude graph Right", 2, mb_dyna_processor::FFT_MESH_POINTS), \
                SWITCH("ssplit", "Stereo split", "Stereo split", 0.0f)

        #define MB_CHANNEL(id, label, alias) \
                SWITCH("flt" id, "Band filter curves" label, "Show flt" alias, 1.0f), \
                MESH("ag" id, "Processor amplitude graph " label, 2, mb_dyna_processor::FFT_MESH_POINTS)

        #define MB_FFT_METERS(id, label, alias) \
                SWITCH("ife" id, "Input FFT graph enable" label, "FFT In" alias, 1.0f), \
                SWITCH("ofe" id, "Output FFT graph enable" label, "FFT Out" alias, 1.0f), \
                MESH("ifg" id, "Input FFT graph" label, 2, mb_dyna_processor::FFT_MESH_POINTS + 2), \
                MESH("ofg" id, "Output FFT graph" label, 2, mb_dyna_processor::FFT_MESH_POINTS)

        #define MB_CHANNEL_METERS(id, label) \
                METER_GAIN("ilm" id, "Input level meter" label, GAIN_AMP_P_24_DB), \
                METER_GAIN("olm" id, "Output level meter" label, GAIN_AMP_P_24_DB)


    /*
     List of frequencies:
     40
     100,3960576873
     251,984209979
     632,4555320337
     1587,4010519682
     3984,2201896585
     10000
     */

        static const port_t mb_dyna_processor_mono_ports[] =
        {
            PORTS_MONO_PLUGIN,
            MB_DYNA_SHM_LINK_MONO,
            MB_COMMON(mb_dyna_sc_bands),
            MB_CHANNEL("", "", ""),
            MB_FFT_METERS("", "", ""),
            MB_CHANNEL_METERS("", ""),

            MB_SPLIT("_1", " 1", " 1", 0.0f, 40.0f),
            MB_SPLIT("_2", " 2", " 2", 1.0f, 100.0f),
            MB_SPLIT("_3", " 3", " 3", 0.0f, 252.0f),
            MB_SPLIT("_4", " 4", " 4", 1.0f, 632.0f),
            MB_SPLIT("_5", " 5", " 5", 0.0f, 1587.0f),
            MB_SPLIT("_6", " 6", " 6", 1.0f, 3984.0f),
            MB_SPLIT("_7", " 7", " 7", 0.0f, 10000.0f),

            MB_MONO_BAND("_0", " 0", " 0", 0, 8, 10.0f, 40.0f),
            MB_MONO_BAND("_1", " 1", " 1", 1, 8, 40.0f, 100.0f),
            MB_MONO_BAND("_2", " 2", " 2", 2, 8, 100.0f, 252.0f),
            MB_MONO_BAND("_3", " 3", " 3", 3, 8, 252.0f, 632.0f),
            MB_MONO_BAND("_4", " 4", " 4", 4, 8, 632.0f, 1587.0f),
            MB_MONO_BAND("_5", " 5", " 5", 5, 8, 1587.0f, 3984.0f),
            MB_MONO_BAND("_6", " 6", " 6", 6, 8, 3984.0f, 10000.0f),
            MB_MONO_BAND("_7", " 7", " 7", 7, 8, 10000.0f, 20000.0f),

            MB_BAND_METERS("_0", " 0"),
            MB_BAND_METERS("_1", " 1"),
            MB_BAND_METERS("_2", " 2"),
            MB_BAND_METERS("_3", " 3"),
            MB_BAND_METERS("_4", " 4"),
            MB_BAND_METERS("_5", " 5"),
            MB_BAND_METERS("_6", " 6"),
            MB_BAND_METERS("_7", " 7"),

            PORTS_END
        };

        static const port_t mb_dyna_processor_stereo_ports[] =
        {
            PORTS_STEREO_PLUGIN,
            MB_DYNA_SHM_LINK_STEREO,
            MB_COMMON(mb_dyna_sc_bands),
            MB_STEREO_CHANNEL,
            MB_FFT_METERS("_l", " Left", " L"),
            MB_CHANNEL_METERS("_l", " Left"),
            MB_FFT_METERS("_r", " Right", " R"),
            MB_CHANNEL_METERS("_r", " Right"),

            MB_SPLIT("_1", " 1", " 1", 0.0f, 40.0f),
            MB_SPLIT("_2", " 2", " 2", 1.0f, 100.0f),
            MB_SPLIT("_3", " 3", " 3", 0.0f, 252.0f),
            MB_SPLIT("_4", " 4", " 4", 1.0f, 632.0f),
            MB_SPLIT("_5", " 5", " 5", 0.0f, 1587.0f),
            MB_SPLIT("_6", " 6", " 6", 1.0f, 3984.0f),
            MB_SPLIT("_7", " 7", " 7", 0.0f, 10000.0f),

            MB_STEREO_BAND("_0", " 0", " 0", 0, 8, 10.0f, 40.0f),
            MB_STEREO_BAND("_1", " 1", " 1", 1, 8, 40.0f, 100.0f),
            MB_STEREO_BAND("_2", " 2", " 2", 2, 8, 100.0f, 252.0f),
            MB_STEREO_BAND("_3", " 3", " 3", 3, 8, 252.0f, 632.0f),
            MB_STEREO_BAND("_4", " 4", " 4", 4, 8, 632.0f, 1587.0f),
            MB_STEREO_BAND("_5", " 5", " 5", 5, 8, 1587.0f, 3984.0f),
            MB_STEREO_BAND("_6", " 6", " 6", 6, 8, 3984.0f, 10000.0f),
            MB_STEREO_BAND("_7", " 7", " 7", 7, 8, 10000.0f, 20000.0f),

            MB_BAND_METERS("_0l", " 0 Left"),
            MB_BAND_METERS("_1l", " 1 Left"),
            MB_BAND_METERS("_2l", " 2 Left"),
            MB_BAND_METERS("_3l", " 3 Left"),
            MB_BAND_METERS("_4l", " 4 Left"),
            MB_BAND_METERS("_5l", " 5 Left"),
            MB_BAND_METERS("_6l", " 6 Left"),
            MB_BAND_METERS("_7l", " 7 Left"),

            MB_BAND_METERS("_0r", " 0 Right"),
            MB_BAND_METERS("_1r", " 1 Right"),
            MB_BAND_METERS("_2r", " 2 Right"),
            MB_BAND_METERS("_3r", " 3 Right"),
            MB_BAND_METERS("_4r", " 4 Right"),
            MB_BAND_METERS("_5r", " 5 Right"),
            MB_BAND_METERS("_6r", " 6 Right"),
            MB_BAND_METERS("_7r", " 7 Right"),

            PORTS_END
        };

        static const port_t mb_dyna_processor_lr_ports[] =
        {
            PORTS_STEREO_PLUGIN,
            MB_DYNA_SHM_LINK_STEREO,
            MB_COMMON(mb_dyna_sc_lr_bands),
            MB_CHANNEL("_l", " Left", " L"),
            MB_CHANNEL("_r", " Right", " R"),
            MB_FFT_METERS("_l", " Left", " L"),
            MB_CHANNEL_METERS("_l", " Left"),
            MB_FFT_METERS("_r", " Right", " R"),
            MB_CHANNEL_METERS("_r", " Right"),

            MB_SPLIT("_1l", " 1 Left", " 1 L", 0.0f, 40.0f),
            MB_SPLIT("_2l", " 2 Left", " 2 L", 1.0f, 100.0f),
            MB_SPLIT("_3l", " 3 Left", " 3 L", 0.0f, 252.0f),
            MB_SPLIT("_4l", " 4 Left", " 4 L", 1.0f, 632.0f),
            MB_SPLIT("_5l", " 5 Left", " 5 L", 0.0f, 1587.0f),
            MB_SPLIT("_6l", " 6 Left", " 6 L", 1.0f, 3984.0f),
            MB_SPLIT("_7l", " 7 Left", " 7 L", 0.0f, 10000.0f),

            MB_SPLIT("_1r", " 1 Right", " 1 R", 0.0f, 40.0f),
            MB_SPLIT("_2r", " 2 Right", " 2 R", 1.0f, 100.0f),
            MB_SPLIT("_3r", " 3 Right", " 3 R", 0.0f, 252.0f),
            MB_SPLIT("_4r", " 4 Right", " 4 R", 1.0f, 632.0f),
            MB_SPLIT("_5r", " 5 Right", " 5 R", 0.0f, 1587.0f),
            MB_SPLIT("_6r", " 6 Right", " 6 R", 1.0f, 3984.0f),
            MB_SPLIT("_7r", " 7 Right", " 7 R", 0.0f, 10000.0f),

            MB_SPLIT_BAND("_0l", " 0 Left", " 0 L", 0, 8, 10.0f, 40.0f),
            MB_SPLIT_BAND("_1l", " 1 Left", " 1 L", 1, 8, 40.0f, 100.0f),
            MB_SPLIT_BAND("_2l", " 2 Left", " 2 L", 2, 8, 100.0f, 252.0f),
            MB_SPLIT_BAND("_3l", " 3 Left", " 3 L", 3, 8, 252.0f, 632.0f),
            MB_SPLIT_BAND("_4l", " 4 Left", " 4 L", 4, 8, 632.0f, 1587.0f),
            MB_SPLIT_BAND("_5l", " 5 Left", " 5 L", 5, 8, 1587.0f, 3984.0f),
            MB_SPLIT_BAND("_6l", " 6 Left", " 6 L", 6, 8, 3984.0f, 10000.0f),
            MB_SPLIT_BAND("_7l", " 7 Left", " 7 L", 7, 8, 10000.0f, 20000.0f),

            MB_SPLIT_BAND("_0r", " 0 Right", " 0 R", 0, 8, 10.0f, 40.0f),
            MB_SPLIT_BAND("_1r", " 1 Right", " 1 R", 1, 8, 40.0f, 100.0f),
            MB_SPLIT_BAND("_2r", " 2 Right", " 2 R", 2, 8, 100.0f, 252.0f),
            MB_SPLIT_BAND("_3r", " 3 Right", " 3 R", 3, 8, 252.0f, 632.0f),
            MB_SPLIT_BAND("_4r", " 4 Right", " 4 R", 4, 8, 632.0f, 1587.0f),
            MB_SPLIT_BAND("_5r", " 5 Right", " 5 R", 5, 8, 1587.0f, 3984.0f),
            MB_SPLIT_BAND("_6r", " 6 Right", " 6 R", 6, 8, 3984.0f, 10000.0f),
            MB_SPLIT_BAND("_7r", " 7 Right", " 7 R", 7, 8, 10000.0f, 20000.0f),

            MB_BAND_METERS("_0l", " 0 Left"),
            MB_BAND_METERS("_1l", " 1 Left"),
            MB_BAND_METERS("_2l", " 2 Left"),
            MB_BAND_METERS("_3l", " 3 Left"),
            MB_BAND_METERS("_4l", " 4 Left"),
            MB_BAND_METERS("_5l", " 5 Left"),
            MB_BAND_METERS("_6l", " 6 Left"),
            MB_BAND_METERS("_7l", " 7 Left"),

            MB_BAND_METERS("_0r", " 0 Right"),
            MB_BAND_METERS("_1r", " 1 Right"),
            MB_BAND_METERS("_2r", " 2 Right"),
            MB_BAND_METERS("_3r", " 3 Right"),
            MB_BAND_METERS("_4r", " 4 Right"),
            MB_BAND_METERS("_5r", " 5 Right"),
            MB_BAND_METERS("_6r", " 6 Right"),
            MB_BAND_METERS("_7r", " 7 Right"),

            PORTS_END
        };

        static const port_t mb_dyna_processor_ms_ports[] =
        {
            PORTS_STEREO_PLUGIN,
            MB_DYNA_SHM_LINK_STEREO,
            MB_COMMON(mb_dyna_sc_ms_bands),
            MB_CHANNEL("_m", " Mid", " M"),
            MB_CHANNEL("_s", " Side", " S"),
            MB_FFT_METERS("_m", " Mid", " M"),
            MB_CHANNEL_METERS("_l", " Left"),
            MB_FFT_METERS("_s", " Side", " S"),
            MB_CHANNEL_METERS("_r", " Right"),

            MB_SPLIT("_1m", " 1 Mid", " 1 M", 0.0f, 40.0f),
            MB_SPLIT("_2m", " 2 Mid", " 2 M", 1.0f, 100.0f),
            MB_SPLIT("_3m", " 3 Mid", " 3 M", 0.0f, 252.0f),
            MB_SPLIT("_4m", " 4 Mid", " 4 M", 1.0f, 632.0f),
            MB_SPLIT("_5m", " 5 Mid", " 5 M", 0.0f, 1587.0f),
            MB_SPLIT("_6m", " 6 Mid", " 6 M", 1.0f, 3984.0f),
            MB_SPLIT("_7m", " 7 Mid", " 7 M", 0.0f, 10000.0f),

            MB_SPLIT("_1s", " 1 Side", " 1 S", 0.0f, 40.0f),
            MB_SPLIT("_2s", " 2 Side", " 2 S", 1.0f, 100.0f),
            MB_SPLIT("_3s", " 3 Side", " 3 S", 0.0f, 252.0f),
            MB_SPLIT("_4s", " 4 Side", " 4 S", 1.0f, 632.0f),
            MB_SPLIT("_5s", " 5 Side", " 5 S", 0.0f, 1587.0f),
            MB_SPLIT("_6s", " 6 Side", " 6 S", 1.0f, 3984.0f),
            MB_SPLIT("_7s", " 7 Side", " 7 S", 0.0f, 10000.0f),

            MB_SPLIT_BAND("_0m", " 0 Mid", " 0 M", 0, 8, 10.0f, 40.0f),
            MB_SPLIT_BAND("_1m", " 1 Mid", " 1 M", 1, 8, 40.0f, 100.0f),
            MB_SPLIT_BAND("_2m", " 2 Mid", " 2 M", 2, 8, 100.0f, 252.0f),
            MB_SPLIT_BAND("_3m", " 3 Mid", " 3 M", 3, 8, 252.0f, 632.0f),
            MB_SPLIT_BAND("_4m", " 4 Mid", " 4 M", 4, 8, 632.0f, 1587.0f),
            MB_SPLIT_BAND("_5m", " 5 Mid", " 5 M", 5, 8, 1587.0f, 3984.0f),
            MB_SPLIT_BAND("_6m", " 6 Mid", " 6 M", 6, 8, 3984.0f, 10000.0f),
            MB_SPLIT_BAND("_7m", " 7 Mid", " 7 M", 7, 8, 10000.0f, 20000.0f),

            MB_SPLIT_BAND("_0s", " 0 Side", " 0 S", 0, 8, 10.0f, 40.0f),
            MB_SPLIT_BAND("_1s", " 1 Side", " 1 S", 1, 8, 40.0f, 100.0f),
            MB_SPLIT_BAND("_2s", " 2 Side", " 2 S", 2, 8, 100.0f, 252.0f),
            MB_SPLIT_BAND("_3s", " 3 Side", " 3 S", 3, 8, 252.0f, 632.0f),
            MB_SPLIT_BAND("_4s", " 4 Side", " 4 S", 4, 8, 632.0f, 1587.0f),
            MB_SPLIT_BAND("_5s", " 5 Side", " 5 S", 5, 8, 1587.0f, 3984.0f),
            MB_SPLIT_BAND("_6s", " 6 Side", " 6 S", 6, 8, 3984.0f, 10000.0f),
            MB_SPLIT_BAND("_7s", " 7 Side", " 7 S", 7, 8, 10000.0f, 20000.0f),

            MB_BAND_METERS("_0m", " 0 Mid"),
            MB_BAND_METERS("_1m", " 1 Mid"),
            MB_BAND_METERS("_2m", " 2 Mid"),
            MB_BAND_METERS("_3m", " 3 Mid"),
            MB_BAND_METERS("_4m", " 4 Mid"),
            MB_BAND_METERS("_5m", " 5 Mid"),
            MB_BAND_METERS("_6m", " 6 Mid"),
            MB_BAND_METERS("_7m", " 7 Mid"),

            MB_BAND_METERS("_0s", " 0 Side"),
            MB_BAND_METERS("_1s", " 1 Side"),
            MB_BAND_METERS("_2s", " 2 Side"),
            MB_BAND_METERS("_3s", " 3 Side"),
            MB_BAND_METERS("_4s", " 4 Side"),
            MB_BAND_METERS("_5s", " 5 Side"),
            MB_BAND_METERS("_6s", " 6 Side"),
            MB_BAND_METERS("_7s", " 7 Side"),

            PORTS_END
        };

        static const port_t sc_mb_dyna_processor_mono_ports[] =
        {
            PORTS_MONO_PLUGIN,
            PORTS_MONO_SIDECHAIN,
            MB_DYNA_SHM_LINK_MONO,
            MB_COMMON(mb_dyna_sc_bands),
            MB_CHANNEL("", "", ""),
            MB_FFT_METERS("", "", ""),
            MB_CHANNEL_METERS("", ""),

            MB_SPLIT("_1", " 1", " 1", 0.0f, 40.0f),
            MB_SPLIT("_2", " 2", " 2", 1.0f, 100.0f),
            MB_SPLIT("_3", " 3", " 3", 0.0f, 252.0f),
            MB_SPLIT("_4", " 4", " 4", 1.0f, 632.0f),
            MB_SPLIT("_5", " 5", " 5", 0.0f, 1587.0f),
            MB_SPLIT("_6", " 6", " 6", 1.0f, 3984.0f),
            MB_SPLIT("_7", " 7", " 7", 0.0f, 10000.0f),

            MB_SC_MONO_BAND("_0", " 0", " 0", 0, 8, 10.0f, 40.0f),
            MB_SC_MONO_BAND("_1", " 1", " 1", 1, 8, 40.0f, 100.0f),
            MB_SC_MONO_BAND("_2", " 2", " 2", 2, 8, 100.0f, 252.0f),
            MB_SC_MONO_BAND("_3", " 3", " 3", 3, 8, 252.0f, 632.0f),
            MB_SC_MONO_BAND("_4", " 4", " 4", 4, 8, 632.0f, 1587.0f),
            MB_SC_MONO_BAND("_5", " 5", " 5", 5, 8, 1587.0f, 3984.0f),
            MB_SC_MONO_BAND("_6", " 6", " 6", 6, 8, 3984.0f, 10000.0f),
            MB_SC_MONO_BAND("_7", " 7", " 7", 7, 8, 10000.0f, 20000.0f),

            MB_BAND_METERS("_0", " 0"),
            MB_BAND_METERS("_1", " 1"),
            MB_BAND_METERS("_2", " 2"),
            MB_BAND_METERS("_3", " 3"),
            MB_BAND_METERS("_4", " 4"),
            MB_BAND_METERS("_5", " 5"),
            MB_BAND_METERS("_6", " 6"),
            MB_BAND_METERS("_7", " 7"),

            PORTS_END
        };

        static const port_t sc_mb_dyna_processor_stereo_ports[] =
        {
            PORTS_STEREO_PLUGIN,
            PORTS_STEREO_SIDECHAIN,
            MB_DYNA_SHM_LINK_STEREO,
            MB_COMMON(mb_dyna_sc_bands),
            MB_STEREO_CHANNEL,
            MB_FFT_METERS("_l", " Left", " L"),
            MB_CHANNEL_METERS("_l", " Left"),
            MB_FFT_METERS("_r", " Right", " R"),
            MB_CHANNEL_METERS("_r", " Right"),

            MB_SPLIT("_1", " 1", " 1", 0.0f, 40.0f),
            MB_SPLIT("_2", " 2", " 2", 1.0f, 100.0f),
            MB_SPLIT("_3", " 3", " 3", 0.0f, 252.0f),
            MB_SPLIT("_4", " 4", " 4", 1.0f, 632.0f),
            MB_SPLIT("_5", " 5", " 5", 0.0f, 1587.0f),
            MB_SPLIT("_6", " 6", " 6", 1.0f, 3984.0f),
            MB_SPLIT("_7", " 7", " 7", 0.0f, 10000.0f),

            MB_SC_STEREO_BAND("_0", " 0", " 0", 0, 8, 10.0f, 40.0f),
            MB_SC_STEREO_BAND("_1", " 1", " 1", 1, 8, 40.0f, 100.0f),
            MB_SC_STEREO_BAND("_2", " 2", " 2", 2, 8, 100.0f, 252.0f),
            MB_SC_STEREO_BAND("_3", " 3", " 3", 3, 8, 252.0f, 632.0f),
            MB_SC_STEREO_BAND("_4", " 4", " 4", 4, 8, 632.0f, 1587.0f),
            MB_SC_STEREO_BAND("_5", " 5", " 5", 5, 8, 1587.0f, 3984.0f),
            MB_SC_STEREO_BAND("_6", " 6", " 6", 6, 8, 3984.0f, 10000.0f),
            MB_SC_STEREO_BAND("_7", " 7", " 7", 7, 8, 10000.0f, 20000.0f),

            MB_BAND_METERS("_0l", " 0 Left"),
            MB_BAND_METERS("_1l", " 1 Left"),
            MB_BAND_METERS("_2l", " 2 Left"),
            MB_BAND_METERS("_3l", " 3 Left"),
            MB_BAND_METERS("_4l", " 4 Left"),
            MB_BAND_METERS("_5l", " 5 Left"),
            MB_BAND_METERS("_6l", " 6 Left"),
            MB_BAND_METERS("_7l", " 7 Left"),

            MB_BAND_METERS("_0r", " 0 Right"),
            MB_BAND_METERS("_1r", " 1 Right"),
            MB_BAND_METERS("_2r", " 2 Right"),
            MB_BAND_METERS("_3r", " 3 Right"),
            MB_BAND_METERS("_4r", " 4 Right"),
            MB_BAND_METERS("_5r", " 5 Right"),
            MB_BAND_METERS("_6r", " 6 Right"),
            MB_BAND_METERS("_7r", " 7 Right"),

            PORTS_END
        };

        static const port_t sc_mb_dyna_processor_lr_ports[] =
        {
            PORTS_STEREO_PLUGIN,
            PORTS_STEREO_SIDECHAIN,
            MB_DYNA_SHM_LINK_STEREO,
            MB_COMMON(mb_dyna_sc_lr_bands),
            MB_CHANNEL("_l", " Left", " L"),
            MB_CHANNEL("_r", " Right", " R"),
            MB_FFT_METERS("_l", " Left", " L"),
            MB_CHANNEL_METERS("_l", " Left"),
            MB_FFT_METERS("_r", " Right", " R"),
            MB_CHANNEL_METERS("_r", " Right"),

            MB_SPLIT("_1l", " 1 Left", " 1 L", 0.0f, 40.0f),
            MB_SPLIT("_2l", " 2 Left", " 2 L", 1.0f, 100.0f),
            MB_SPLIT("_3l", " 3 Left", " 3 L", 0.0f, 252.0f),
            MB_SPLIT("_4l", " 4 Left", " 4 L", 1.0f, 632.0f),
            MB_SPLIT("_5l", " 5 Left", " 5 L", 0.0f, 1587.0f),
            MB_SPLIT("_6l", " 6 Left", " 6 L", 1.0f, 3984.0f),
            MB_SPLIT("_7l", " 7 Left", " 7 L", 0.0f, 10000.0f),

            MB_SPLIT("_1r", " 1 Right", " 1 R", 0.0f, 40.0f),
            MB_SPLIT("_2r", " 2 Right", " 2 R", 1.0f, 100.0f),
            MB_SPLIT("_3r", " 3 Right", " 3 R", 0.0f, 252.0f),
            MB_SPLIT("_4r", " 4 Right", " 4 R", 1.0f, 632.0f),
            MB_SPLIT("_5r", " 5 Right", " 5 R", 0.0f, 1587.0f),
            MB_SPLIT("_6r", " 6 Right", " 6 R", 1.0f, 3984.0f),
            MB_SPLIT("_7r", " 7 Right", " 7 R", 0.0f, 10000.0f),

            MB_SC_SPLIT_BAND("_0l", " 0 Left", " 0 L", 0, 8, 10.0f, 40.0f),
            MB_SC_SPLIT_BAND("_1l", " 1 Left", " 1 L", 1, 8, 40.0f, 100.0f),
            MB_SC_SPLIT_BAND("_2l", " 2 Left", " 2 L", 2, 8, 100.0f, 252.0f),
            MB_SC_SPLIT_BAND("_3l", " 3 Left", " 3 L", 3, 8, 252.0f, 632.0f),
            MB_SC_SPLIT_BAND("_4l", " 4 Left", " 4 L", 4, 8, 632.0f, 1587.0f),
            MB_SC_SPLIT_BAND("_5l", " 5 Left", " 5 L", 5, 8, 1587.0f, 3984.0f),
            MB_SC_SPLIT_BAND("_6l", " 6 Left", " 6 L", 6, 8, 3984.0f, 10000.0f),
            MB_SC_SPLIT_BAND("_7l", " 7 Left", " 7 L", 7, 8, 10000.0f, 20000.0f),

            MB_SC_SPLIT_BAND("_0r", " 0 Right", " 0 R", 0, 8, 10.0f, 40.0f),
            MB_SC_SPLIT_BAND("_1r", " 1 Right", " 1 R", 1, 8, 40.0f, 100.0f),
            MB_SC_SPLIT_BAND("_2r", " 2 Right", " 2 R", 2, 8, 100.0f, 252.0f),
            MB_SC_SPLIT_BAND("_3r", " 3 Right", " 3 R", 3, 8, 252.0f, 632.0f),
            MB_SC_SPLIT_BAND("_4r", " 4 Right", " 4 R", 4, 8, 632.0f, 1587.0f),
            MB_SC_SPLIT_BAND("_5r", " 5 Right", " 5 R", 5, 8, 1587.0f, 3984.0f),
            MB_SC_SPLIT_BAND("_6r", " 6 Right", " 6 R", 6, 8, 3984.0f, 10000.0f),
            MB_SC_SPLIT_BAND("_7r", " 7 Right", " 7 R", 7, 8, 10000.0f, 20000.0f),

            MB_BAND_METERS("_0l", " 0 Left"),
            MB_BAND_METERS("_1l", " 1 Left"),
            MB_BAND_METERS("_2l", " 2 Left"),
            MB_BAND_METERS("_3l", " 3 Left"),
            MB_BAND_METERS("_4l", " 4 Left"),
            MB_BAND_METERS("_5l", " 5 Left"),
            MB_BAND_METERS("_6l", " 6 Left"),
            MB_BAND_METERS("_7l", " 7 Left"),

            MB_BAND_METERS("_0r", " 0 Right"),
            MB_BAND_METERS("_1r", " 1 Right"),
            MB_BAND_METERS("_2r", " 2 Right"),
            MB_BAND_METERS("_3r", " 3 Right"),
            MB_BAND_METERS("_4r", " 4 Right"),
            MB_BAND_METERS("_5r", " 5 Right"),
            MB_BAND_METERS("_6r", " 6 Right"),
            MB_BAND_METERS("_7r", " 7 Right"),

            PORTS_END
        };

        static const port_t sc_mb_dyna_processor_ms_ports[] =
        {
            PORTS_STEREO_PLUGIN,
            PORTS_STEREO_SIDECHAIN,
            MB_DYNA_SHM_LINK_STEREO,
            MB_COMMON(mb_dyna_sc_ms_bands),
            MB_CHANNEL("_m", " Mid", " M"),
            MB_CHANNEL("_s", " Side", " S"),
            MB_FFT_METERS("_m", " Mid", " M"),
            MB_CHANNEL_METERS("_l", " Left"),
            MB_FFT_METERS("_s", " Side", " S"),
            MB_CHANNEL_METERS("_r", " Right"),

            MB_SPLIT("_1m", " 1 Mid", " 1 M", 0.0f, 40.0f),
            MB_SPLIT("_2m", " 2 Mid", " 2 M", 1.0f, 100.0f),
            MB_SPLIT("_3m", " 3 Mid", " 3 M", 0.0f, 252.0f),
            MB_SPLIT("_4m", " 4 Mid", " 4 M", 1.0f, 632.0f),
            MB_SPLIT("_5m", " 5 Mid", " 5 M", 0.0f, 1587.0f),
            MB_SPLIT("_6m", " 6 Mid", " 6 M", 1.0f, 3984.0f),
            MB_SPLIT("_7m", " 7 Mid", " 7 M", 0.0f, 10000.0f),

            MB_SPLIT("_1s", " 1 Side", " 1 S", 0.0f, 40.0f),
            MB_SPLIT("_2s", " 2 Side", " 2 S", 1.0f, 100.0f),
            MB_SPLIT("_3s", " 3 Side", " 3 S", 0.0f, 252.0f),
            MB_SPLIT("_4s", " 4 Side", " 4 S", 1.0f, 632.0f),
            MB_SPLIT("_5s", " 5 Side", " 5 S", 0.0f, 1587.0f),
            MB_SPLIT("_6s", " 6 Side", " 6 S", 1.0f, 3984.0f),
            MB_SPLIT("_7s", " 7 Side", " 7 S", 0.0f, 10000.0f),

            MB_SC_SPLIT_BAND("_0m", " 0 Mid", " 0 M", 0, 8, 10.0f, 40.0f),
            MB_SC_SPLIT_BAND("_1m", " 1 Mid", " 1 M", 1, 8, 40.0f, 100.0f),
            MB_SC_SPLIT_BAND("_2m", " 2 Mid", " 2 M", 2, 8, 100.0f, 252.0f),
            MB_SC_SPLIT_BAND("_3m", " 3 Mid", " 3 M", 3, 8, 252.0f, 632.0f),
            MB_SC_SPLIT_BAND("_4m", " 4 Mid", " 4 M", 4, 8, 632.0f, 1587.0f),
            MB_SC_SPLIT_BAND("_5m", " 5 Mid", " 5 M", 5, 8, 1587.0f, 3984.0f),
            MB_SC_SPLIT_BAND("_6m", " 6 Mid", " 6 M", 6, 8, 3984.0f, 10000.0f),
            MB_SC_SPLIT_BAND("_7m", " 7 Mid", " 7 M", 7, 8, 10000.0f, 20000.0f),

            MB_SC_SPLIT_BAND("_0s", " 0 Side", " 0 S", 0, 8, 10.0f, 40.0f),
            MB_SC_SPLIT_BAND("_1s", " 1 Side", " 1 S", 1, 8, 40.0f, 100.0f),
            MB_SC_SPLIT_BAND("_2s", " 2 Side", " 2 S", 2, 8, 100.0f, 252.0f),
            MB_SC_SPLIT_BAND("_3s", " 3 Side", " 3 S", 3, 8, 252.0f, 632.0f),
            MB_SC_SPLIT_BAND("_4s", " 4 Side", " 4 S", 4, 8, 632.0f, 1587.0f),
            MB_SC_SPLIT_BAND("_5s", " 5 Side", " 5 S", 5, 8, 1587.0f, 3984.0f),
            MB_SC_SPLIT_BAND("_6s", " 6 Side", " 6 S", 6, 8, 3984.0f, 10000.0f),
            MB_SC_SPLIT_BAND("_7s", " 7 Side", " 7 S", 7, 8, 10000.0f, 20000.0f),

            MB_BAND_METERS("_0m", " 0 Mid"),
            MB_BAND_METERS("_1m", " 1 Mid"),
            MB_BAND_METERS("_2m", " 2 Mid"),
            MB_BAND_METERS("_3m", " 3 Mid"),
            MB_BAND_METERS("_4m", " 4 Mid"),
            MB_BAND_METERS("_5m", " 5 Mid"),
            MB_BAND_METERS("_6m", " 6 Mid"),
            MB_BAND_METERS("_7m", " 7 Mid"),

            MB_BAND_METERS("_0s", " 0 Side"),
            MB_BAND_METERS("_1s", " 1 Side"),
            MB_BAND_METERS("_2s", " 2 Side"),
            MB_BAND_METERS("_3s", " 3 Side"),
            MB_BAND_METERS("_4s", " 4 Side"),
            MB_BAND_METERS("_5s", " 5 Side"),
            MB_BAND_METERS("_6s", " 6 Side"),
            MB_BAND_METERS("_7s", " 7 Side"),

            PORTS_END
        };

        const meta::bundle_t mb_dyna_processor_bundle =
        {
            "mb_dyna_processor",
            "Multiband Dynamics Processor",
            B_MB_DYNAMICS,
            "6ZuW28u1qns",
            "This plugin performs multiband dynamics processing of input signal. Flexible sidechain\n"
            "control configuration provided. As opposite to most available multiband\n"
            "processors, this dynamics processor provides numerous special functions: 'modern'\n"
            "operating mode, 'Sidechain boost', 'Lookahead' option and up to 8 frequency\n"
            "bands for processing."
        };

        // Multiband Dynamics Processor
        const meta::plugin_t  mb_dyna_processor_mono =
        {
            "Multi-band Dynamikprozessor Mono x8",
            "Multiband Dynamics Processor Mono x8",
            "MB Dynamics Processor Mono",
            "MBDP8M",
            &developers::v_sadovnikov,
            "mb_dyna_processor_mono",
            {
                LSP_LV2_URI("mb_dyna_processor_mono"),
                LSP_LV2UI_URI("mb_dyna_processor_mono"),
                "mdp0",
                LSP_VST3_UID("mbdp8m  mdp0"),
                LSP_VST3UI_UID("mbdp8m  mdp0"),
                LSP_LADSPA_MB_DYNA_PROCESSOR_BASE + 0,
                LSP_LADSPA_URI("mb_dyna_processor_mono"),
                LSP_CLAP_URI("mb_dyna_processor_mono"),
                LSP_GST_UID("mb_dyna_processor_mono"),
            },
            LSP_PLUGINS_MB_DYNA_PROCESSOR_VERSION,
            plugin_classes,
            clap_features_mono,
            E_INLINE_DISPLAY,
            mb_dyna_processor_mono_ports,
            "dynamics/processor/multiband/mono.xml",
            NULL,
            mono_plugin_port_groups,
            &mb_dyna_processor_bundle
        };

        const meta::plugin_t  mb_dyna_processor_stereo =
        {
            "Multi-band Dynamikprozessor Stereo x8",
            "Multiband Dynamics Processor Stereo x8",
            "MB Dynamics Processor Stereo",
            "MBDP8S",
            &developers::v_sadovnikov,
            "mb_dyna_processor_stereo",
            {
                LSP_LV2_URI("mb_dyna_processor_stereo"),
                LSP_LV2UI_URI("mb_dyna_processor_stereo"),
                "mdp1",
                LSP_VST3_UID("mbdp8s  mdp1"),
                LSP_VST3UI_UID("mbdp8s  mdp1"),
                LSP_LADSPA_MB_DYNA_PROCESSOR_BASE + 1,
                LSP_LADSPA_URI("mb_dyna_processor_stereo"),
                LSP_CLAP_URI("mb_dyna_processor_stereo"),
                LSP_GST_UID("mb_dyna_processor_stereo"),
            },
            LSP_PLUGINS_MB_DYNA_PROCESSOR_VERSION,
            plugin_classes,
            clap_features_stereo,
            E_INLINE_DISPLAY,
            mb_dyna_processor_stereo_ports,
            "dynamics/processor/multiband/stereo.xml",
            NULL,
            stereo_plugin_port_groups,
            &mb_dyna_processor_bundle
        };

        const meta::plugin_t  mb_dyna_processor_lr =
        {
            "Multi-band Dynamikprozessor LeftRight x8",
            "Multiband Dynamics Processor LeftRight x8",
            "MB Dynamics Processor L/R",
            "MBDP8LR",
            &developers::v_sadovnikov,
            "mb_dyna_processor_lr",
            {
                LSP_LV2_URI("mb_dyna_processor_lr"),
                LSP_LV2UI_URI("mb_dyna_processor_lr"),
                "mdp2",
                LSP_VST3_UID("mbdp8lr mdp2"),
                LSP_VST3UI_UID("mbdp8lr mdp2"),
                LSP_LADSPA_MB_DYNA_PROCESSOR_BASE + 2,
                LSP_LADSPA_URI("mb_dyna_processor_lr"),
                LSP_CLAP_URI("mb_dyna_processor_lr"),
                LSP_GST_UID("mb_dyna_processor_lr"),
            },
            LSP_PLUGINS_MB_DYNA_PROCESSOR_VERSION,
            plugin_classes,
            clap_features_stereo,
            E_INLINE_DISPLAY,
            mb_dyna_processor_lr_ports,
            "dynamics/processor/multiband/lr.xml",
            NULL,
            stereo_plugin_port_groups,
            &mb_dyna_processor_bundle
        };

        const meta::plugin_t  mb_dyna_processor_ms =
        {
            "Multi-band Dynamikprozessor MidSide x8",
            "Multiband Dynamics Processor MidSide x8",
            "MB Dynamics Processor M/S",
            "MBDP8MS",
            &developers::v_sadovnikov,
            "mb_dyna_processor_ms",
            {
                LSP_LV2_URI("mb_dyna_processor_ms"),
                LSP_LV2UI_URI("mb_dyna_processor_ms"),
                "mdp3",
                LSP_VST3_UID("mbdp8ms mdp3"),
                LSP_VST3UI_UID("mbdp8ms mdp3"),
                LSP_LADSPA_MB_DYNA_PROCESSOR_BASE + 3,
                LSP_LADSPA_URI("mb_dyna_processor_ms"),
                LSP_CLAP_URI("mb_dyna_processor_ms"),
                LSP_GST_UID("mb_dyna_processor_ms"),
            },
            LSP_PLUGINS_MB_DYNA_PROCESSOR_VERSION,
            plugin_classes,
            clap_features_stereo,
            E_INLINE_DISPLAY,
            mb_dyna_processor_ms_ports,
            "dynamics/processor/multiband/ms.xml",
            NULL,
            stereo_plugin_port_groups,
            &mb_dyna_processor_bundle
        };


        const meta::plugin_t  sc_mb_dyna_processor_mono =
        {
            "Sidechain Multi-band Dynamikprozessor Mono x8",
            "Sidechain Multiband Dynamics Processor Mono x8",
            "SC MB Dynamics Processor Mono",
            "SCMBDP8M",
            &developers::v_sadovnikov,
            "sc_mb_dyna_processor_mono",
            {
                LSP_LV2_URI("sc_mb_dyna_processor_mono"),
                LSP_LV2UI_URI("sc_mb_dyna_processor_mono"),
                "mdp4",
                LSP_VST3_UID("scmbdp8mmdp4"),
                LSP_VST3UI_UID("scmbdp8mmdp4"),
                LSP_LADSPA_MB_DYNA_PROCESSOR_BASE + 4,
                LSP_LADSPA_URI("sc_mb_dyna_processor_mono"),
                LSP_CLAP_URI("sc_mb_dyna_processor_mono"),
                LSP_GST_UID("sc_mb_dyna_processor_mono"),
            },
            LSP_PLUGINS_MB_DYNA_PROCESSOR_VERSION,
            plugin_classes,
            clap_features_mono,
            E_INLINE_DISPLAY,
            sc_mb_dyna_processor_mono_ports,
            "dynamics/processor/multiband/mono.xml",
            NULL,
            mono_plugin_sidechain_port_groups,
            &mb_dyna_processor_bundle
        };

        const meta::plugin_t  sc_mb_dyna_processor_stereo =
        {
            "Sidechain Multi-band Dynamikprozessor Stereo x8",
            "Sidechain Multiband Dynamics Processor Stereo x8",
            "SC MB Dynamics Processor Stereo",
            "SCMBDP8S",
            &developers::v_sadovnikov,
            "sc_mb_dyna_processor_stereo",
            {
                LSP_LV2_URI("sc_mb_dyna_processor_stereo"),
                LSP_LV2UI_URI("sc_mb_dyna_processor_stereo"),
                "mdp5",
                LSP_VST3_UID("scmbdp8smdp5"),
                LSP_VST3UI_UID("scmbdp8smdp5"),
                LSP_LADSPA_MB_DYNA_PROCESSOR_BASE + 5,
                LSP_LADSPA_URI("sc_mb_dyna_processor_stereo"),
                LSP_CLAP_URI("sc_mb_dyna_processor_stereo"),
                LSP_GST_UID("sc_mb_dyna_processor_stereo"),
            },
            LSP_PLUGINS_MB_DYNA_PROCESSOR_VERSION,
            plugin_classes,
            clap_features_stereo,
            E_INLINE_DISPLAY,
            sc_mb_dyna_processor_stereo_ports,
            "dynamics/processor/multiband/stereo.xml",
            NULL,
            stereo_plugin_sidechain_port_groups,
            &mb_dyna_processor_bundle
        };

        const meta::plugin_t  sc_mb_dyna_processor_lr =
        {
            "Sidechain Multi-band Dynamikprozessor LeftRight x8",
            "Sidechain Multiband Dynamics Processor LeftRight x8",
            "SC MB Dynamics Processor L/R",
            "SCMBDP8LR",
            &developers::v_sadovnikov,
            "sc_mb_dyna_processor_lr",
            {
                LSP_LV2_URI("sc_mb_dyna_processor_lr"),
                LSP_LV2UI_URI("sc_mb_dyna_processor_lr"),
                "mdp6",
                LSP_VST3_UID("scmbdp8lmdp6"),
                LSP_VST3UI_UID("scmbdp8lmdp6"),
                LSP_LADSPA_MB_DYNA_PROCESSOR_BASE + 6,
                LSP_LADSPA_URI("sc_mb_dyna_processor_lr"),
                LSP_CLAP_URI("sc_mb_dyna_processor_lr"),
                LSP_GST_UID("sc_mb_dyna_processor_lr"),
            },
            LSP_PLUGINS_MB_DYNA_PROCESSOR_VERSION,
            plugin_classes,
            clap_features_stereo,
            E_INLINE_DISPLAY,
            sc_mb_dyna_processor_lr_ports,
            "dynamics/processor/multiband/lr.xml",
            NULL,
            stereo_plugin_sidechain_port_groups,
            &mb_dyna_processor_bundle
        };

        const meta::plugin_t  sc_mb_dyna_processor_ms =
        {
            "Sidechain Multi-band Dynamikprozessor MidSide x8",
            "Sidechain Multiband Dynamics Processor MidSide x8",
            "SC MB Dynamics Processor M/S",
            "SCMBDP8MS",
            &developers::v_sadovnikov,
            "sc_mb_dyna_processor_ms",
            {
                LSP_LV2_URI("sc_mb_dyna_processor_ms"),
                LSP_LV2UI_URI("sc_mb_dyna_processor_ms"),
                "mdp7",
                LSP_VST3_UID("scmbdp8mmdp7"),
                LSP_VST3UI_UID("scmbdp8mmdp7"),
                LSP_LADSPA_MB_DYNA_PROCESSOR_BASE + 7,
                LSP_LADSPA_URI("sc_mb_dyna_processor_ms"),
                LSP_CLAP_URI("sc_mb_dyna_processor_ms"),
                LSP_GST_UID("sc_mb_dyna_processor_ms"),
            },
            LSP_PLUGINS_MB_DYNA_PROCESSOR_VERSION,
            plugin_classes,
            clap_features_stereo,
            E_INLINE_DISPLAY,
            sc_mb_dyna_processor_ms_ports,
            "dynamics/processor/multiband/ms.xml",
            NULL,
            stereo_plugin_sidechain_port_groups,
            &mb_dyna_processor_bundle
        };

    } /* namespace meta */
} /* namespace lsp */



