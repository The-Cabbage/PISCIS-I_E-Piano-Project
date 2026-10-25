// Copyright 2008-2026 David Robillard <d@drobilla.net>
// SPDX-License-Identifier: GPL-2.0-or-later or MIT

#include "audioeffectx.h"

#include <dlfcn.h>

#include <cctype>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <utility>

namespace {

struct Record {
	explicit Record(std::string n)
		: base_name{std::move(n)}
	{}

	using UIs = std::list<std::string>;

	std::string base_name;
	UIs         uis;
};

using Manifest = std::map<std::string, Record>;

bool
is_short_form(const std::string& str,
              const size_t       i,
              const char         first,
              const char         second)
{
	return (str[i] == first && str[i + 1] == second);
}

std::string
symbolify(const char* name, char space_char = '_')
{
	std::string str(name);

	// Like This -> Like_This
	for (size_t i = 0; i < str.length(); ++i)
		if (str[i] == ' ')
			str[i] = space_char;

	str[0] = static_cast<char>(std::tolower(str[0]));

	// LikeThis -> like_this
	for (size_t i = 1; i < str.length(); ++i) {
		const char p = str[i - 1]; // previous
		const char c = str[i];     // current
		if (c >= 'A' && c <= 'Z' && p >= 'a' && p <= 'z' &&
		    ((i == str.length() - 1) ||
		     (str[i + 1] <= 'a' && str[i + 1] >= 'Z')) &&
		    (!is_short_form(str, i - 1, 'd', 'B') &&
		     !is_short_form(str, i - 1, 'F', 'X') &&
		     !is_short_form(str, i - 1, 'D', 'C')))
			str = str.substr(0, i) + space_char + str.substr(i);
	}

	// To lowercase, and skip invalids
	for (size_t i = 1; i < str.length();) {
		if (std::isalpha(str[i]) || std::isdigit(str[i])) {
			str[i] = static_cast<char>(std::tolower(str[i]));
			++i;
		} else if (str[i - 1] != space_char) {
			str[i] = space_char;
			++i;
		} else {
			str = str.substr(0, i) + str.substr(i + 1);
		}
	}

	return str;
}

void
write_plugin(AudioEffectX*      effect,
             const std::string& lib_file_name,
             Manifest&          manifest)
{
	const std::string base_name =
		lib_file_name.substr(0, lib_file_name.find_last_of('.'));
	const std::string data_file_name = base_name + ".ttl";

	// VST seems to just blissfully assume buffers are large enough?
	char name_buf[1024] = {0};

	std::fstream os(data_file_name.c_str(), std::ios::out);
	effect->getProductString(name_buf);

	os << "@prefix lv2: <http://lv2plug.in/ns/lv2core#> .\n";
	os << "@prefix doap: <http://usefulinc.com/ns/doap#> .\n\n";
	os << "<" << effect->getURI() << ">\n";
	os << "\tlv2:symbol \"" << effect->getUniqueID() << "\" ;\n";
	os << "\tdoap:name \"" << name_buf << "\" ;\n";
	os << "\tdoap:license <http://usefulinc.com/doap/licenses/gpl> ;\n";
	os << "\tlv2:pluginProperty lv2:hardRTCapable";

	uint32_t num_params     = effect->getNumParameters();
	uint32_t num_audio_ins  = effect->getNumInputs();
	uint32_t num_audio_outs = effect->getNumOutputs();
	uint32_t num_ports      = num_params + num_audio_ins + num_audio_outs;

	if (num_ports > 0)
		os << " ;\n\tlv2:port [\n";
	else
		os << " .\n";

	uint32_t idx = 0;

	for (uint32_t i = idx; i < num_params; ++i, ++idx) {
		effect->getParameterName(i, name_buf);
		os << "\t\ta lv2:InputPort, lv2:ControlPort ;\n";
		os << "\t\tlv2:index" << " " << idx << " ;\n";
		os << "\t\tlv2:name \"" << name_buf << "\" ;\n";
		os << "\t\tlv2:symbol \"" << symbolify(name_buf) << "\" ;\n";
		os << "\t\tlv2:default " << effect->getParameter(i) << " ;\n";
		os << "\t\tlv2:minimum 0.0 ;\n";
		os << "\t\tlv2:maximum 1.0 ;\n";
		os << ((idx == num_ports - 1) ? "\t] .\n" : "\t] , [\n");
	}

	for (uint32_t i = 0; i < num_audio_ins; ++i, ++idx) {
		os << "\t\ta lv2:InputPort, lv2:AudioPort ;\n";
		os << "\t\tlv2:index" << " " << idx << " ;\n";
		os << "\t\tlv2:symbol \"in" << i + 1 << "\" ;\n";
		os << "\t\tlv2:name \"Input " << i + 1 << "\" ;\n";
		os << ((idx == num_ports - 1) ? "\t] ." : "\t] , [\n");
	}

	for (uint32_t i = 0; i < num_audio_outs; ++i, ++idx) {
		os << "\t\ta lv2:OutputPort, lv2:AudioPort ;\n";
		os << "\t\tlv2:index " << idx << " ;\n";
		os << "\t\tlv2:symbol \"out" << i + 1 << "\" ;\n";
		os << "\t\tlv2:name \"Output " << i + 1 << "\" ;\n";
		os << ((idx == num_ports - 1) ? "\t] .\n" : "\t] , [\n");
	}

	os.close();

	Manifest::iterator i = manifest.find(effect->getURI());
	if (i != manifest.end()) {
		i->second.base_name = base_name;
	} else {
		manifest.insert(std::make_pair(effect->getURI(), Record(base_name)));
	}

	if (effect->getNumPrograms() > 1) {
		std::string preset_file = base_name + "-presets.ttl";

		std::fstream pos(preset_file.c_str(), std::ios::out);
		pos << "@prefix lv2: <http://lv2plug.in/ns/lv2core#> .\n";
		pos << "@prefix pset: <http://lv2plug.in/ns/ext/presets#> .\n";
		pos << "@prefix rdfs: <http://www.w3.org/2000/01/rdf-schema#> .\n\n";
		for (int32_t p = 0; p < effect->getNumPrograms(); ++p) {
			effect->setProgram(p);
			effect->getProgramName(name_buf);

			std::string preset_uri = std::string(effect->getURI()) + "#pset-" +
			                         symbolify(name_buf, '-');

			// Write manifest entry
			std::cout << "<" << preset_uri << ">"
					  << "\n\ta pset:Preset ;\n\tlv2:appliesTo <"
					  << effect->getURI() << "> ;\n\t"
					  << "rdfs:seeAlso <" << preset_file << "> .\n\n";

			// Write preset file
			pos << "<" << preset_uri << ">"
				<< "\n\ta pset:Preset ;\n\tlv2:appliesTo <" << effect->getURI()
				<< "> ;\n\t"
				<< "rdfs:label \"" << name_buf << "\"";
			for (uint32_t q = 0; q < num_params; ++q) {
				effect->getParameterName(q, name_buf);
				pos << " ;\n\tlv2:port [\n";
				pos << "\t\tlv2:symbol \"" << symbolify(name_buf) << "\" ;\n";
				pos << "\t\tpset:value " << effect->getParameter(q) << " ;\n";
				pos << "\t]";
			}
			pos << " .\n\n";
		}
		pos.close();
	}
}

void
write_manifest(std::ostream& os, const Manifest& manifest)
{
	os << "@prefix lv2: <http://lv2plug.in/ns/lv2core#> .\n";
	os << "@prefix rdfs: <http://www.w3.org/2000/01/rdf-schema#> .\n";
	os << "@prefix uiext: <http://lv2plug.in/ns/extensions/ui#> .\n";
	for (Manifest::const_iterator i = manifest.begin(); i != manifest.end();
	     ++i) {
		const Record& r = i->second;
		os << "<" << i->first << ">\n\ta lv2:Plugin ;\n";
		os << "\trdfs:seeAlso <" << r.base_name << ".ttl> ;\n";
		os << "\tlv2:binary <" << r.base_name << ".so> ";
		for (Record::UIs::const_iterator j = r.uis.begin(); j != r.uis.end();
		     ++j)
			os << ";\n\tuiext:ui <" << *j << "> ";
		os << ".\n\n";
	}
}

} // namespace

int
main(int argc, char** argv)
{
	if (argc == 0) {
		std::cout << "Usage: gendata [PLUGINLIB1] [PLUGINLIB2]...\n"
					 "Each argument is a path to a LVZ plugin library.\n"
					 "For each, a description will be written\n"
					 "to the LV2 data file with the same base name.\n"
					 "A manifest of the output is written to stdout.\n";
		return 1;
	}

	using Constructor = AudioEffectX* (*)();

	Constructor   constructor{nullptr};
	AudioEffectX* effect{nullptr};
	Manifest      manifest{};
	for (int i = 1; i < argc; ++i) {
		void* handle = dlopen(argv[i], RTLD_LAZY);
		if (handle == nullptr) {
			std::cerr << "error: failed to open \"" << argv[i] << "\" ("
					  << dlerror() << ")\n";
			continue;
		}

		std::string lib_path   = argv[i];
		size_t      last_slash = lib_path.find_last_of('/');
		if (last_slash != std::string::npos)
			lib_path = lib_path.substr(last_slash + 1);

		constructor = reinterpret_cast<Constructor>(
			dlsym(handle, "lvz_new_audioeffectx"));
		if (constructor != nullptr) {
			effect = constructor();
			write_plugin(effect, lib_path, manifest);
		}

		if (constructor == nullptr) {
			std::cerr << "error: no lvz_new_audioeffectx in \"" << argv[i]
					  << "\"\n";
		}

		dlclose(handle);
	}

	write_manifest(std::cout, manifest);

	return 0;
}
