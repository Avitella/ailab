#pragma once

#include <vector>

#include "config.h"
#include "question.h"
#include "question_shaker.h"
#include "topic.h"
#include "variants.h"

namespace ailab {

class generator_t {
  struct stat_t {
    size_t mutations_count;
    size_t mutations_attempts;
    size_t mutation_duplicate_count;
    size_t mutation_duplicate_attempts;
    size_t tried_generate;
    double first_fitness_function;
    double best_fitness_function;

    stat_t() :
        mutations_count(0),
        mutations_attempts(0),
        mutation_duplicate_count(0),
        mutation_duplicate_attempts(0),
        tried_generate(0),
        first_fitness_function(0),
        best_fitness_function(0) {
    }
  };

  mutable stat_t stat;

  config_t config;
  std::vector<topic_t> topics;
  std::vector<question_t> questions;

  std::vector<variants_t> generate_population(question_shaker_t const &shaker) const {
    std::vector<variants_t> result;

    for (size_t k = 0; k < config.population_size; ++k) {
      result.push_back(variants_t(config));
      for (size_t i = 0; i < config.variants_count; ++i) {
        for (size_t j = 0; j < config.questions_count; ++j) {
          size_t r = rand() % config.topics.size();
          result[k][i].push_back(shaker.get_question(config.topics[r]));
        }
      }
    }

    return result;
  }

  void crossover(std::vector<variants_t> &population) const noexcept {
    std::sort(population.begin(), population.end(), [] (variants_t const &a, variants_t const &b) -> bool {
      return a.calculate_fitness_function() > b.calculate_fitness_function();
    });
    size_t population_size = population.size();
    for (size_t i = 0; i + 1 < population_size; i += 2)
      population.push_back(population[i].crossover(population[i + 1]));
    for (size_t i = 0; i * 2 < population_size; ++i) {
      size_t r = rand() % (population_size - i) + i;
      population.push_back(population[i].crossover(population[r]));
    }
  }

  void mutation(std::vector<variants_t> &population, question_shaker_t const &shaker) const {
    size_t population_size = population.size();
    for (size_t i = 0; i < population_size; ++i) {
      bool b = (1 + rand() % 100) <= 100 * config.mutation_duplicate_chance;
      ++stat.mutation_duplicate_attempts;
      if (b) {
        population.push_back(population[i]);
        ++stat.mutation_duplicate_count;
      }
      for (size_t j = 0; j < population[i].size(); ++j)
        for (size_t k = 0; k < population[i][j].size(); ++k) {
          bool b = (1 + rand() % 100) <= 100 * config.mutation_chance;
          ++stat.mutations_attempts;
          if (b) {
            size_t r = rand() % config.topics.size();
            population[i][j][k] = shaker.get_question(config.topics[r]);
            ++stat.mutations_count;
          }
        }
    }
  }

  void selection(std::vector<variants_t> &population) const noexcept {
    std::sort(population.begin(), population.end(), [] (variants_t const &a, variants_t const &b) -> bool {
      return a.calculate_fitness_function() > b.calculate_fitness_function();
    });
    std::random_shuffle(population.begin() + config.population_size / 2, population.end());
    population.resize(config.population_size + config.population_size / 2);
  }

  variants_t best(std::vector<variants_t> &population) const noexcept {
    std::sort(population.begin(), population.end(), [] (variants_t const &a, variants_t const &b) -> bool {
      return a.calculate_fitness_function() > b.calculate_fitness_function();
    });
    return population.front();
  }

  void strong_mutation(std::vector<variants_t> &population, question_shaker_t &shaker) const noexcept {
    for (variants_t &v : population)
      v.strong_mutation(shaker);
  }

  bool good_result(variants_t const &v) const noexcept {
    for (size_t i = 0; i < v.size(); ++i) {
      std::unordered_set<size_t> counter;
      for (size_t j = 0; j < v[i].size(); ++j)
        counter.insert(v[i][j].get_question_id());
      if (counter.size() < v[i].size())
        return false;
    }
    return true;
  }

 public:
  generator_t(config_t const &cnf, std::vector<topic_t> const &topics, std::vector<question_t> const &questions) noexcept :
      config(cnf),
      topics(topics),
      questions(questions) {
  }

  variants_t generate() {
    ++stat.tried_generate;

    question_shaker_t shaker(topics, questions);
    std::vector<variants_t> population = generate_population(shaker);

    variants_t result = best(population);
    stat.best_fitness_function = stat.first_fitness_function = result.calculate_fitness_function();

    for (size_t current_time = 1; current_time <= config.life_time; ++current_time) {
      crossover(population);
      selection(population);
      mutation(population, shaker);
      strong_mutation(population, shaker);
      
      if (config.log_enabled)
        std::cerr << current_time << '\t' << result.calculate_fitness_function() << std::endl;

      variants_t buf = best(population);
      if (buf.calculate_fitness_function() > result.calculate_fitness_function()) {
        result = buf;
        stat.best_fitness_function = result.calculate_fitness_function();
      }
    }

    if (good_result(result) || stat.tried_generate == config.try_generate) {
      if (config.stat_enabled) {
        std::cerr << std::endl;
        std::cerr << "stat.tried_generate = " << stat.tried_generate << std::endl;
        std::cerr << "stat.mutations_attempts = " << stat.mutations_attempts << std::endl;
        std::cerr << "stat.mutations_count = " << stat.mutations_count << std::endl;
        std::cerr << "stat.mutation_duplicate_attempts = " << stat.mutation_duplicate_attempts << std::endl;
        std::cerr << "stat.mutation_duplicate_count = " << stat.mutation_duplicate_count << std::endl;
        std::cerr << "stat.first_fitness_function = " << stat.first_fitness_function << std::endl;
        std::cerr << "stat.best_fitness_function = " << stat.best_fitness_function << std::endl;
        std::cerr << std::endl;
      }
      return result;
    } else {
      return generate();
    }
  }
};

}
