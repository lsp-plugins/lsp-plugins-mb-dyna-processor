/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef PRIVATE_PLUGINS_MB_DYNA_PROCESSOR_H_
#define PRIVATE_PLUGINS_MB_DYNA_PROCESSOR_H_

#include <lsp-plug.in/dsp-units/ctl/Bypass.h>
#include <lsp-plug.in/dsp-units/ctl/Counter.h>
#include <lsp-plug.in/dsp-units/dynamics/DynamicProcessor.h>
#include <lsp-plug.in/dsp-units/filters/DynamicFilters.h>
#include <lsp-plug.in/dsp-units/filters/Equalizer.h>
#include <lsp-plug.in/dsp-units/util/Analyzer.h>
#include <lsp-plug.in/dsp-units/util/Delay.h>
#include <lsp-plug.in/dsp-units/util/FFTCrossover.h>
#include <lsp-plug.in/dsp-units/util/Sidechain.h>
#include <lsp-plug.in/plug-fw/core/IDBuffer.h>
#include <lsp-plug.in/plug-fw/plug.h>

#include <private/meta/mb_dyna_processor.h>

namespace lsp
{
    namespace plugins
    {
        /**
         * Base class for the latency compensation delay
         */
        class mb_dyna_processor: public plug::Module
        {
            public:
                enum dp_mode_t
                {
                    MBDP_MONO,
                    MBDP_STEREO,
                    MBDP_LR,
                    MBDP_MS
                };

                enum xover_mode_t
                {
                    XOVER_CLASSIC,                              // Classic mode
                    XOVER_MODERN,                               // Modern mode
                    XOVER_LINEAR_PHASE                          // Linear phase mode
                };

            protected:
                enum sync_t
                {
                    S_DP_CURVE      = 1 << 0,
                    S_DP_MODEL      = 1 << 1,
                    S_EQ_CURVE      = 1 << 2,
                    S_BAND_CURVE    = 1 << 3,

                    S_ALL           = S_DP_CURVE | S_DP_MODEL | S_EQ_CURVE | S_BAND_CURVE
                };

                enum sc_type_t
                {
                    SCT_INTERNAL,
                    SCT_EXTERNAL,
                    SCT_LINK,
                };

                typedef struct dyna_band_t
                {
                    dspu::Sidechain         sSC;                // Sidechain module
                    dspu::Equalizer         sEQ[2];             // Sidechain equalizers
                    dspu::DynamicProcessor  sProc;              // Dynamic Processor
                    dspu::Filter            sPassFilter;        // Passing filter for 'classic' mode
                    dspu::Filter            sRejFilter;         // Rejection filter for 'classic' mode
                    dspu::Filter            sAllFilter;         // All-pass filter for phase compensation
                    dspu::Delay             sScDelay;           // Sidechain delay for lookahead purpose

                    float                  *vBuffer;            // Crossover band data
                    float                  *vSc;                // Transfer function for sidechain
                    float                  *vTr;                // Transfer function for band
                    float                  *vVCA;               // Voltage-controlled amplification value for each band
                    float                   fScPreamp;          // Sidechain preamp

                    float                   fFreqStart;
                    float                   fFreqEnd;

                    float                   fFreqHCF;           // Cutoff frequency for low-pass filter
                    float                   fFreqLCF;           // Cutoff frequency for high-pass filter
                    float                   fMakeup;            // Makeup gain
                    float                   fGainLevel;         // Gain adjustment level
                    uint32_t                nLookahead;         // Lookahead amount

                    bool                    bEnabled;           // Enabled flag
                    bool                    bCustHCF;           // Custom frequency for high-cut filter
                    bool                    bCustLCF;           // Custom frequency for low-cut filter
                    bool                    bMute;              // Mute channel
                    bool                    bSolo;              // Solo channel
                    uint32_t                nScType;            // Sidechain type
                    uint32_t                nSync;              // Synchronize output data flags
                    uint32_t                nFilterID;          // Identifier of the filter

                    plug::IPort            *pScType;            // Sidechain type
                    plug::IPort            *pScSource;          // Sidechain source
                    plug::IPort            *pScSpSource;        // Sidechain split source
                    plug::IPort            *pScMode;            // Sidechain mode
                    plug::IPort            *pScLook;            // Sidechain lookahead
                    plug::IPort            *pScReact;           // Sidechain reactivity
                    plug::IPort            *pScPreamp;          // Sidechain preamp
                    plug::IPort            *pScLpfOn;           // Sidechain low-pass on
                    plug::IPort            *pScHpfOn;           // Sidechain hi-pass on
                    plug::IPort            *pScLcfFreq;         // Sidechain low-cut frequency
                    plug::IPort            *pScHcfFreq;         // Sidechain hi-cut frequency
                    plug::IPort            *pScFreqChart;       // Sidechain band frequency chart

                    plug::IPort            *pEnable;            // Enable processor
                    plug::IPort            *pSolo;              // Soloing
                    plug::IPort            *pMute;              // Muting

                    plug::IPort            *pDotOn[meta::mb_dyna_processor::DOTS];              // Dot enable
                    plug::IPort            *pThreshold[meta::mb_dyna_processor::DOTS];          // Threshold levels
                    plug::IPort            *pGain[meta::mb_dyna_processor::DOTS];               // Gain levels
                    plug::IPort            *pKnee[meta::mb_dyna_processor::DOTS];               // Knees levels
                    plug::IPort            *pAttackOn[meta::mb_dyna_processor::DOTS];           // Attack enable
                    plug::IPort            *pAttackLvl[meta::mb_dyna_processor::DOTS];          // Attack levels
                    plug::IPort            *pAttackTime[meta::mb_dyna_processor::RANGES];       // Attack time
                    plug::IPort            *pReleaseOn[meta::mb_dyna_processor::DOTS];          // Release enable
                    plug::IPort            *pReleaseLvl[meta::mb_dyna_processor::DOTS];         // Release levels
                    plug::IPort            *pReleaseTime[meta::mb_dyna_processor::RANGES];      // Release time
                    plug::IPort            *pHold;              // Hold time
                    plug::IPort            *pLowRatio;          // Low Ratio
                    plug::IPort            *pHighRatio;         // High Ratio
                    plug::IPort            *pMakeup;            // Overall Makeup gain

                    plug::IPort            *pFreqEnd;           // Frequency range end
                    plug::IPort            *pModelGraph;        // Processor curve modelling graph
                    plug::IPort            *pCurveGraph;        // Processor curve graph
                    plug::IPort            *pEnvLvl;            // Envelope level meter
                    plug::IPort            *pCurveLvl;          // Reduction curve level meter
                    plug::IPort            *pMeterGain;         // Reduction gain meter
                } dyna_band_t;

                typedef struct split_t
                {
                    bool                    bEnabled;           // Split band is enabled
                    float                   fFreq;              // Split band frequency

                    plug::IPort            *pEnabled;           // Enable port
                    plug::IPort            *pFreq;              // Split frequency
                } split_t;

                typedef struct channel_t
                {
                    dspu::Bypass            sBypass;            // Bypass
                    dspu::Filter            sEnvBoost[3];       // Envelope boost filter
                    dspu::Delay             sDelay;             // Delay for lookahead compensation purpose
                    dspu::Delay             sDryDelay;          // Delay for dry signal
                    dspu::Delay             sXOverDelay;        // Delay for crossover
                    dspu::Equalizer         sDryEq;             // Dry equalizer
                    dspu::FFTCrossover      sFFTXOver;          // FFT crossover for linear phase

                    dyna_band_t             vBands[meta::mb_dyna_processor::BANDS_MAX];     // Processor bands
                    split_t                 vSplit[meta::mb_dyna_processor::BANDS_MAX-1];   // Split bands
                    dyna_band_t            *vPlan[meta::mb_dyna_processor::BANDS_MAX];      // Execution plan (band indexes)
                    uint32_t                nPlanSize;          // Plan size

                    float                  *vIn;                // Input data buffer
                    float                  *vOut;               // Output data buffer
                    float                  *vScIn;              // Sidechain data buffer (if present)
                    float                  *vShmIn;             // Shared memory link buffer (if present)

                    float                  *vInAnalyze;         // Input signal analysis
                    float                  *vInBuffer;          // Input buffer
                    float                  *vBuffer;            // Common data processing buffer
                    float                  *vScBuffer;          // Sidechain buffer
                    float                  *vExtScBuffer;       // External sidechain buffer
                    float                  *vShmBuffer;         // Shared memory link buffer
                    float                  *vTr;                // Transfer function
                    float                  *vTrMem;             // Transfer buffer (memory)

                    uint32_t                nAnInChannel;       // Analyzer channel used for input signal analysis
                    uint32_t                nAnOutChannel;      // Analyzer channel used for output signal analysis
                    bool                    bInFft;             // Input signal FFT enabled
                    bool                    bOutFft;            // Output signal FFT enabled

                    plug::IPort            *pIn;                // Input
                    plug::IPort            *pOut;               // Output
                    plug::IPort            *pScIn;              // Sidechain
                    plug::IPort            *pShmIn;             // Shared memory link input
                    plug::IPort            *pFftIn;             // Pre-processing FFT analysis data
                    plug::IPort            *pFftInSw;           // Pre-processing FFT analysis control port
                    plug::IPort            *pFftOut;            // Post-processing FFT analysis data
                    plug::IPort            *pFftOutSw;          // Post-processing FFT analysis controlport
                    plug::IPort            *pAmpGraph;          // Processor's amplitude graph
                    plug::IPort            *pInLvl;             // Input level meter
                    plug::IPort            *pOutLvl;            // Output level meter
                } channel_t;

            protected:
                dspu::Analyzer          sAnalyzer;              // Analyzer
                dspu::DynamicFilters    sFilters;               // Dynamic filters for each band in 'modern' mode
                dspu::Counter           sCounter;               // Sync counter
                uint32_t                nMode;                  // Processor mode
                bool                    bSidechain;             // External side chain
                bool                    bEnvUpdate;             // Envelope filter update
                bool                    bUseExtSc;              // External sidechain is in use
                bool                    bUseShmLink;            // Shared memory link is in use
                xover_mode_t            enXOver;                // Crossover mode
                bool                    bStereoSplit;           // Stereo split mode
                uint32_t                nEnvBoost;              // Envelope boost
                channel_t              *vChannels;              // Processor channels
                float                   fInGain;                // Input gain
                float                   fDryGain;               // Dry gain
                float                   fWetGain;               // Wet gain
                float                   fZoom;                  // Zoom
                uint8_t                *pData;                  // Aligned data pointer
                float                  *vSc[2];                 // Sidechain signal data
                float                  *vAnalyze[4];            // Analysis buffer
                float                  *vBuffer;                // Temporary buffer
                float                  *vEnv;                   // Processor envelope buffer
                float                  *vTr;                    // Transfer buffer
                float                  *vPFc;                   // Pass filter characteristics buffer
                float                  *vRFc;                   // Reject filter characteristics buffer
                float                  *vFreqs;                 // Analyzer FFT frequencies
                float                  *vCurve;                 // Curve
                uint32_t               *vIndexes;               // Analyzer FFT indexes
                core::IDBuffer         *pIDisplay;              // Inline display buffer

                plug::IPort            *pBypass;                // Bypass port
                plug::IPort            *pMode;                  // Global mode
                plug::IPort            *pInGain;                // Input gain port
                plug::IPort            *pOutGain;               // Output gain port
                plug::IPort            *pDryGain;               // Dry gain port
                plug::IPort            *pWetGain;               // Wet gain port
                plug::IPort            *pDryWet;                // Dry/Wet gain balance port
                plug::IPort            *pReactivity;            // Reactivity
                plug::IPort            *pShiftGain;             // Shift gain port
                plug::IPort            *pZoom;                  // Zoom port
                plug::IPort            *pEnvBoost;              // Envelope adjust
                plug::IPort            *pStereoSplit;           // Split left/right independently

            protected:
                static bool compare_bands_for_sort(const dyna_band_t *b1, const dyna_band_t *b2);

                static dspu::sidechain_source_t     decode_sidechain_source(int source, bool split, size_t channel);
                static size_t                       select_fft_rank(size_t sample_rate);
                static void                         process_band(void *object, void *subject, size_t band, const float *data, size_t sample, size_t count);

            protected:
                void                do_destroy();
                void                preprocess_channel_input(size_t count);
                uint32_t            decode_sidechain_type(uint32_t sc) const;
                void                process_input_mono(float *out, const float *in, size_t count);
                void                process_input_stereo(float *l_out, float *r_out, const float *l_in, const float *r_in, size_t count);
                const float        *select_buffer(const dyna_band_t *band, const channel_t *channel);

            public:
                explicit mb_dyna_processor(const meta::plugin_t *metadata, bool sc, size_t mode);
                mb_dyna_processor(const mb_dyna_processor &) = delete;
                mb_dyna_processor(mb_dyna_processor &&) = delete;
                virtual ~mb_dyna_processor() override;

                mb_dyna_processor & operator = (const mb_dyna_processor &) = delete;
                mb_dyna_processor & operator = (mb_dyna_processor &&) = delete;

                virtual void        init(plug::IWrapper *wrapper, plug::IPort **ports) override;
                virtual void        destroy() override;

            public:
                virtual void        update_settings() override;
                virtual void        update_sample_rate(long sr) override;
                virtual void        ui_activated() override;

                virtual void        process(size_t samples) override;
                virtual bool        inline_display(plug::ICanvas *cv, size_t width, size_t height) override;

                virtual void        dump(dspu::IStateDumper *v) const override;
        };

    } /* namespace plugins */
} /* namespace lsp */


#endif /* PRIVATE_PLUGINS_MB_DYNA_PROCESSOR_H_ */

