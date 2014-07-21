#pragma once

#include <iostream>
#include <unordered_map>

namespace ailab {

struct config_t {
  size_t life_time;
  size_t population_size;
  double mutation_chance;
  bool log_enabled;

  size_t variants_count;
  size_t questions_count;

  std::vector<size_t> topics;

  config_t(size_t v_count = 8, size_t q_count = 8) noexcept :
      life_time(500),
      population_size(200),
      mutation_chance(0.05),
      log_enabled(false),
      variants_count(v_count),
      questions_count(q_count),
      topics() {
  }

  config_t(std::unordered_map<std::string, std::string> const &map) noexcept :
      config_t() {
    if (map.find("life_time") != map.end())
      life_time = std::stoul(map.at("life_time"));
    if (map.find("population_size") != map.end())
      population_size = std::stoul(map.at("population_size"));
    if (map.find("mutation_chance") != map.end())
      mutation_chance = std::stod(map.at("mutation_chance"));
    if (map.find("log_enabled") != map.end())
      log_enabled = std::stoul(map.at("log_enabled"));
    if (map.find("variants_count") != map.end())
      variants_count = std::stoul(map.at("variants_count"));
    if (map.find("questions_count") != map.end())
      questions_count = std::stoul(map.at("questions_count"));
  }
};

std::ostream& operator << (std::ostream& out, config_t const &config) {
  out << "config.life_time = " << config.life_time << std::endl;
  out << "config.population_size = " << config.population_size << std::endl;
  out << "config.mutation_chance = " << config.mutation_chance << std::endl;
  out << "config.log_enabled = " << config.log_enabled << std::endl;
  out << "config.variants_count = " << config.variants_count << std::endl;
  out << "config.questions_count = " << config.questions_count << std::endl;

  return out;
}

}
