#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace ailab {

/*
 * h
 * help
 * r=
 * j=
 * i=
 */

inline std::unordered_map<std::string, std::string> getopt(int argc, char *argv[],
		std::vector<std::string> const &set) {
	std::unordered_map<std::string, bool> has_arg;

	for (auto &s : set) {
		if (s.back() == '=') {
			has_arg[s.substr(0, s.length() - 1)] = true;
		} else {
			has_arg[s] = false;
		}
	}

	std::unordered_map<std::string, std::string> res;

	int i = 1;
	while (i < argc) {
		std::string s = argv[i];

		if (s.length() > 2 && s[0] == '-' && s[1] == '-' && s.find('=') != std::string::npos) {
			std::string opt = s.substr(2, s.find('=') - 2);
			std::string arg = s.substr(s.find('=') + 1, s.length() - s.find('=') - 1);

			if (has_arg.find(opt) == has_arg.end())
				throw exception_t("unexpected option: --%s in %s", opt.c_str(), s.c_str());

			if (!has_arg[opt])
				throw exception_t("unexpected arg with option: --%s in %s", opt.c_str(), s.c_str());

			if (!res[opt].empty())
				res[opt] += ",";
			res[opt] += arg;
			++i;

		} else if (s.length() > 2 && s[0] == '-' && s[1] == '-') {
			std::string opt = s.substr(2, s.length() - 2);

			if (has_arg.find(opt) == has_arg.end())
				throw exception_t("unexpected option: --%s", opt.c_str());

			if (has_arg[opt]) {
				if (i + 1 == argc)
					throw exception_t("expected arg with option: --%s", opt.c_str());

				if (!res[opt].empty())
					res[opt] += ",";
				res[opt] += std::string(argv[i + 1]);
				i += 2;

			} else {
				if (!res[opt].empty())
					res[opt] += ",";
				res[opt] += std::string();
				++i;
			}

		} else if (s.length() > 1 && s[0] == '-') {
			std::string opt = s.substr(1, 1);

			if (has_arg.find(opt) == has_arg.end())
				throw exception_t("unexpected option: -%s", opt.c_str());

			if (has_arg[opt]) {
				std::string buf = s.substr(2, s.length() - 2);

				if (buf.empty()) {
					if (i + 1 == argc)
						throw exception_t("expected arg with option: -%s", opt.c_str());

					if (!res[opt].empty())
						res[opt] += ",";
					res[opt] += std::string(argv[i + 1]);
					i += 2;

				} else {
					if (!res[opt].empty())
						res[opt] += ",";
					res[opt] += buf;
					++i;
				}

			} else {
				if (!res[opt].empty())
					res[opt] += ",";
				res[opt] += std::string();
				++i;
			}

		} else {
			if (!res[""].empty())
				res[""] += " ";
			res[""] += s;
			++i;
		}
	}

	return res;
}

}
