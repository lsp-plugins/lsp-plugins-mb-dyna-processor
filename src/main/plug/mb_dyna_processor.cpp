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

#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/common/debug.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/dsp-units/units.h>
#include <lsp-plug.in/plug-fw/meta/func.h>

#include <private/plugins/mb_dyna_processor.h>

/* The size of temporary buffer for audio processing */
#define BUFFER_SIZE         0x1000U

#define TRACE_PORT(p)       lsp_trace("  port id=%s", (p)->metadata()->id);

namespace lsp
{
    namespace plugins
    {
        //-------------------------------------------------------------------------
        // Plugin factory
        typedef struct plugin_settings_t
        {
            const meta::plugin_t   *metadata;
            bool                    sc;
            uint8_t                 mode;
        } plugin_settings_t;

        static const meta::plugin_t *plugins[] =
        {
            &meta::mb_dyna_processor_mono,
            &meta::mb_dyna_processor_stereo,
            &meta::mb_dyna_processor_lr,
            &meta::mb_dyna_processor_ms,
            &meta::sc_mb_dyna_processor_mono,
            &meta::sc_mb_dyna_processor_stereo,
            &meta::sc_mb_dyna_processor_lr,
            &meta::sc_mb_dyna_processor_ms
        };

        static const plugin_settings_t plugin_settings[] =
        {
            { &meta::mb_dyna_processor_mono,        false, mb_dyna_processor::MBDP_MONO         },
            { &meta::mb_dyna_processor_stereo,      false, mb_dyna_processor::MBDP_STEREO       },
            { &meta::mb_dyna_processor_lr,          false, mb_dyna_processor::MBDP_LR           },
            { &meta::mb_dyna_processor_ms,          false, mb_dyna_processor::MBDP_MS           },
            { &meta::sc_mb_dyna_processor_mono,     true,  mb_dyna_processor::MBDP_MONO         },
            { &meta::sc_mb_dyna_processor_stereo,   true,  mb_dyna_processor::MBDP_STEREO       },
            { &meta::sc_mb_dyna_processor_lr,       true,  mb_dyna_processor::MBDP_LR           },
            { &meta::sc_mb_dyna_processor_ms,       true,  mb_dyna_processor::MBDP_MS           },

            { NULL, 0, false }
        };

        static plug::Module *plugin_factory(const meta::plugin_t *meta)
        {
            for (const plugin_settings_t *s = plugin_settings; s->metadata != NULL; ++s)
                if (s->metadata == meta)
                    return new mb_dyna_processor(s->metadata, s->sc, s->mode);
            return NULL;
        }

        static plug::Factory factory(plugin_factory, plugins, 8);

    }
}


