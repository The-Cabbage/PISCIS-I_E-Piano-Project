// Copyright 2008-2026 David Robillard <d@drobilla.net>
// SPDX-License-Identifier: GPL-2.0-or-later or MIT

#ifndef PLUGIN_CLASS
#error "This file requires PLUGIN_CLASS to be defined"
#endif
#ifndef URI_PREFIX
#error "This file requires URI_PREFIX to be defined"
#endif
#ifndef PLUGIN_URI_SUFFIX
#error "This file requires PLUGIN_URI_SUFFIX to be defined"
#endif
#ifndef PLUGIN_HEADER
#error "This file requires PLUGIN_HEADER to be defined"
#endif

#include PLUGIN_HEADER

#ifdef MDA_EXPOSE_AUDIOEFFECTX
#include "audioeffectx.h"
#endif

#include <lv2/atom/atom.h>
#include <lv2/core/lv2.h>
#include <lv2/midi/midi.h>
#include <lv2/urid/urid.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>

extern "C" {

/* Plugin */

namespace {

struct LVZPlugin {
	PLUGIN_CLASS* effect;
	float*        controls;
	float**       control_buffers;
	const float** inputs;
	float**       outputs;
};

void
lvz_cleanup(LV2_Handle instance)
{
	LVZPlugin* plugin = (LVZPlugin*)instance;
	free(plugin->controls);
	free(plugin->control_buffers);
	free(plugin->inputs);
	free(plugin->outputs);
	delete plugin->effect;
	free(plugin);
}

void
lvz_connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	LVZPlugin*     plugin      = (LVZPlugin*)instance;
	const uint32_t num_params  = plugin->effect->getNumParameters();
	const uint32_t num_inputs  = plugin->effect->getNumInputs();
	const uint32_t num_outputs = plugin->effect->getNumOutputs();

	if (port < num_params) {
		plugin->control_buffers[port] = (float*)data;
	} else if (port < num_params + num_inputs) {
		plugin->inputs[port - num_params] = (float*)data;
	} else if (port < num_params + num_inputs + num_outputs) {
		plugin->outputs[port - num_params - num_inputs] = (float*)data;
	} else if (port == num_params + num_inputs + num_outputs) {
		plugin->effect->setEventInput((LV2_Atom_Sequence*)data);
	}
}

int
master_callback(int, int ver, int, int, int, int)
{
	return 0;
}

LV2_Handle
lvz_instantiate(const LV2_Descriptor*     descriptor,
                double                    rate,
                const char*               bundle_path,
                const LV2_Feature* const* features)
{
	PLUGIN_CLASS* effect = new PLUGIN_CLASS(master_callback);
	effect->setURI(URI_PREFIX PLUGIN_URI_SUFFIX);
	effect->setSampleRate(rate);

	uint32_t num_params  = effect->getNumParameters();
	uint32_t num_inputs  = effect->getNumInputs();
	uint32_t num_outputs = effect->getNumOutputs();

	LVZPlugin* plugin = (LVZPlugin*)malloc(sizeof(LVZPlugin));
	plugin->effect    = effect;

	for (int i = 0; features[i]; ++i) {
		if (!strcmp(features[i]->URI, LV2_URID__map)) {
			LV2_URID_Map* map = (LV2_URID_Map*)features[i]->data;
			plugin->effect->setMidiEventType(
				map->map(map->handle, LV2_MIDI__MidiEvent));
			break;
		}
	}

	if (num_params > 0) {
		plugin->controls        = (float*)malloc(sizeof(float) * num_params);
		plugin->control_buffers = (float**)malloc(sizeof(float*) * num_params);
		for (uint32_t i = 0; i < num_params; ++i) {
			plugin->controls[i]        = effect->getParameter(i);
			plugin->control_buffers[i] = nullptr;
		}
	} else {
		plugin->controls        = nullptr;
		plugin->control_buffers = nullptr;
	}

	if (num_inputs > 0) {
		plugin->inputs = (const float**)malloc(sizeof(float*) * num_inputs);
		for (uint32_t i = 0; i < num_inputs; ++i) {
			plugin->inputs[i] = nullptr;
		}
	} else {
		plugin->inputs = nullptr;
	}

	if (num_outputs > 0) {
		plugin->outputs = (float**)malloc(sizeof(float*) * num_outputs);
		for (uint32_t i = 0; i < num_outputs; ++i) {
			plugin->outputs[i] = nullptr;
		}
	} else {
		plugin->outputs = nullptr;
	}

	return (LV2_Handle)plugin;
}

void
lvz_run(LV2_Handle instance, uint32_t sample_count)
{
	LVZPlugin* plugin = (LVZPlugin*)instance;

	for (int32_t i = 0; i < plugin->effect->getNumParameters(); ++i) {
		float val = plugin->control_buffers[i][0];
		if (val != plugin->controls[i]) {
			plugin->effect->setParameter(i, val);
			plugin->controls[i] = val;
		}
	}

	plugin->effect->processReplacing(
		plugin->inputs, plugin->outputs, sample_count);
}

const void*
lvz_extension_data(const char* uri)
{
	return nullptr;
}

void
lvz_deactivate(LV2_Handle instance)
{
	LVZPlugin* plugin = (LVZPlugin*)instance;
	plugin->effect->suspend();
}

} // namespace

/* Library */

LV2_SYMBOL_EXPORT
const LV2_Descriptor*
lv2_descriptor(uint32_t index)
{
	static const LV2_Descriptor descriptor = {URI_PREFIX PLUGIN_URI_SUFFIX,
	                                          lvz_instantiate,
	                                          lvz_connect_port,
	                                          nullptr,
	                                          lvz_run,
	                                          lvz_deactivate,
	                                          lvz_cleanup,
	                                          lvz_extension_data};

	return !index ? &descriptor : nullptr;
}

#ifdef MDA_EXPOSE_AUDIOEFFECTX

/** Entry point for LVZ gendata */
LV2_SYMBOL_EXPORT
AudioEffectX*
lvz_new_audioeffectx()
{
	PLUGIN_CLASS* effect = new PLUGIN_CLASS(master_callback);
	effect->setURI(URI_PREFIX PLUGIN_URI_SUFFIX);
	return effect;
}

#endif

} // extern "C"
