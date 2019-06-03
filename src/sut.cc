/*
 * Copyright (C) 2019 Robin Gareus <robin@gareus.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#define SUT_URI "http://gareus.org/oss/lv2/sut"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>

enum {
	P_AIN = 0,
	P_AOUT,
	P_LAST,
};

typedef struct {
	/* ports */
	float* ports[P_LAST];

	/* config */
	float rate;
} SuT;

/* *****************************************************************************
 * LV2 Plugin
 */

static LV2_Handle
instantiate (const LV2_Descriptor*     descriptor,
             double                    rate,
             const char*               bundle_path,
             const LV2_Feature* const* features)
{
	SuT* self  = (SuT*)calloc (1, sizeof (SuT));
	self->rate = rate;
	return (LV2_Handle)self;
}

static void
connect_port (LV2_Handle instance,
              uint32_t   port,
              void*      data)
{
	SuT* self = (SuT*)instance;
	if (port < P_LAST) {
		self->ports[port] = (float*)data;
	}
}

static void
run_mono (LV2_Handle instance, uint32_t n_samples)
{
	SuT*         self = (SuT*)instance;
	const float* in   = self->ports[P_AIN];
	float*       out  = self->ports[P_AOUT];

	/* DO DSP HERE */

	if (out != in) {
		memcpy (out, in, n_samples * sizeof (float));
	}
}

static void
cleanup (LV2_Handle instance)
{
	free (instance);
}

static const void*
extension_data (const char* uri)
{
	return NULL;
}

static const LV2_Descriptor descriptor_mono = {
	SUT_URI "#mono",
	instantiate,
	connect_port,
	NULL,
	run_mono,
	NULL,
	cleanup,
	extension_data
};

#undef LV2_SYMBOL_EXPORT
#ifdef _WIN32
# define LV2_SYMBOL_EXPORT __declspec(dllexport)
#else
# define LV2_SYMBOL_EXPORT __attribute__ ((visibility ("default")))
#endif
LV2_SYMBOL_EXPORT
const LV2_Descriptor*
lv2_descriptor (uint32_t index)
{
	switch (index) {
		case 0:
			return &descriptor_mono;
		default:
			return NULL;
	}
}
