#pragma once

#include <vector>

#include "config.h"
#include "question.h"
#include "question_shaker.h"
#include "topic.h"
#include "variants.h"

namespace ailab {

class generator_t {
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
    for (size_t i = 0; i < population_size; ++i)
      for (size_t j = 0; j < population[i].size(); ++j)
        for (size_t k = 0; k < population[i][j].size(); ++k) {
          bool b = (1 + rand() % 100) <= 100 * config.mutation_chance;
          if (b) {
            size_t r = rand() % config.topics.size();
            population[i][j][k] = shaker.get_question(config.topics[r]);
          }
        }
  }

  void selection(std::vector<variants_t> &population) const noexcept {
    std::sort(population.begin(), population.end(), [] (variants_t const &a, variants_t const &b) -> bool {
      return a.calculate_fitness_function() > b.calculate_fitness_function();
    });
    population.resize(config.population_size);
  }

  variants_t best(std::vector<variants_t> &population) const noexcept {
    std::sort(population.begin(), population.end(), [] (variants_t const &a, variants_t const &b) -> bool {
      return a.calculate_fitness_function() > b.calculate_fitness_function();
    });
    return population.front();
  }

 public:
  generator_t(config_t const &config, std::vector<topic_t> const &topics, std::vector<question_t> const &questions) noexcept :
      config(config),
      topics(topics),
      questions(questions) {
  }

  variants_t generate() {
    question_shaker_t shaker(topics, questions);
    std::vector<variants_t> population = generate_population(shaker);

    variants_t result = best(population);

    for (size_t current_time = 1; current_time <= config.life_time; ++current_time) {
      crossover(population);
      mutation(population, shaker);
      selection(population);
      
      if (config.log_enabled)
        std::cerr << current_time << '\t' << population.front().calculate_fitness_function() << std::endl;

      variants_t buf = best(population);
      if (buf.calculate_fitness_function() > result.calculate_fitness_function())
        result = buf;
    }

    return result;
  }
};

}
