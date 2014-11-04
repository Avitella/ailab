#pragma once

#include <iostream>
#include <unordered_map>

namespace ailab {

struct config_t {
  size_t life_time;
  size_t try_generate;
  size_t population_size;
  double mutation_chance;
  double mutation_duplicate_chance;
  bool log_enabled;
  bool stat_enabled;

  size_t variants_count;
  size_t questions_count;

  std::vector<size_t> topics;

  config_t(size_t v_count = 8, size_t q_count = 8) noexcept :
      life_time(200),
      try_generate(10),
      population_size(100),
      mutation_chance(0.01),
      mutation_duplicate_chance(0.03),
      log_enabled(false),
      stat_enabled(false),
      variants_count(v_count),
      questions_count(q_count),
      topics() {
  }

  config_t(std::unordered_map<std::string, std::string> const &map) noexcept :
      config_t() {
    if (map.find("life-time") != map.end())
      life_time = std::stoul(map.at("life-time"));
    if (map.find("population-size") != map.end())
      population_size = std::stoul(map.at("population-size"));
    if (map.find("mutation-chance") != map.end())
      mutation_chance = std::stod(map.at("mutation-chance"));
    if (map.find("log-enabled") != map.end())
      log_enabled = true;
    if (map.find("variants-count") != map.end())
      variants_count = std::stoul(map.at("variants-count"));
    if (map.find("questions-count") != map.end())
      questions_count = std::stoul(map.at("questions-count"));
    if (map.find("stat-enabled") != map.end())
      stat_enabled = true;
    if (map.find("mutation-duplicate-chance") != map.end())
      mutation_duplicate_chance = std::stod(map.at("mutation-duplicate-chance"));
    if (map.find("try-generate") != map.end())
      try_generate = std::stod(map.at("try-generate"));
  }
};

std::ostream& operator << (std::ostream& out, config_t const &config) {
  out << "config.life_time = " << config.life_time << std::endl;
  out << "config.population_size = " << config.population_size << std::endl;
  out << "config.mutation_chance = " << config.mutation_chance << std::endl;
  out << "config.mutation_duplicate_chance = " << config.mutation_duplicate_chance << std::endl;
  out << "config.log_enabled = " << config.log_enabled << std::endl;
  out << "config.stat_enabled = " << config.stat_enabled << std::endl;
  out << "config.variants_count = " << config.variants_count << std::endl;
  out << "config.questions_count = " << config.questions_count << std::endl;
  out << "config.try_generate = " << config.try_generate << std::endl;
  out << "config.topics = ";
  for (size_t i = 0; i < config.topics.size(); ++i) {
    out << config.topics[i];
    if (i + 1 != config.topics.size())
      out << ", ";
  }
  out << std::endl;

  return out;
}

}
