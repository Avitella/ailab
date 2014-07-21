#pragma once

#include <vector>

#include "question.h"

namespace ailab {

class variants_t {
  mutable double fitness;
  mutable bool changed;

  config_t const &config;

  size_t questions_count;
  std::vector<std::vector<question_t>> questions;

 public:
  variants_t(config_t const &config = config_t()) noexcept :
      fitness(0),
      changed(true),
      config(config),
      questions(config.questions_count) {
  }

  variants_t(variants_t const &v) noexcept :
      fitness(v.fitness),
      changed(v.changed),
      config(v.config),
      questions(v.questions) {
  }

  variants_t(variants_t &&v) noexcept :
      config(v.config) {
    std::swap(fitness, v.fitness);
    std::swap(changed, v.changed);
    std::swap(questions, v.questions);
  }

  variants_t &operator = (variants_t &&v) noexcept {
    std::swap(fitness, v.fitness);
    std::swap(changed, v.changed);
    std::swap(questions, v.questions);

    return *this;
  }

  variants_t &operator = (variants_t const &v) noexcept {
    fitness = v.fitness;
    changed = v.changed;
    questions = v.questions;
    
    return *this;
  }
  
  void push_back(std::vector<question_t> const &q) noexcept {
    questions.push_back(q);
  }

  size_t size() const noexcept {
    return questions.size();
  }

  std::vector<std::vector<question_t>>::iterator begin() noexcept {
    return questions.begin();
  }

  std::vector<std::vector<question_t>>::iterator end() noexcept {
    return questions.end();
  }

  double calculate_fitness_function() const noexcept {
    if (!changed) 
      return fitness;
    size_t questions_count = 0;
    for (auto const &v : questions)
      questions_count += v.size();
    {
      size_t buffer[questions_count];
      for (size_t i = 0; i < questions.size(); ++i)
        for (size_t j = 0; j < questions[i].size(); ++j)
          buffer[i * questions.front().size() + j] = questions[i][j].get_question_id();
      std::sort(buffer, buffer + questions_count);
      size_t count = 1;
      for (size_t i = 1; i < questions_count; ++i)
        count += buffer[i] != buffer[i - 1];
      fitness = questions.size() * double(count) * 10 / questions_count;
    }
    {
      for (size_t i = 0; i < questions.size(); ++i) {
        size_t buffer[questions[i].size()];
        for (size_t j = 0; j < questions[i].size(); ++j)
          buffer[j] = questions[i][j].get_question_id();
        std::sort(buffer, buffer + questions[i].size());
        size_t count = 1;
        for (size_t j = 1; j < questions[i].size(); ++j)
          count += buffer[j] != buffer[j - 1];
        fitness *= double(count) * 10 / questions[i].size();
      }
    }
    {
      size_t buffer[questions_count];
      for (size_t i = 0; i < questions.size(); ++i)
        for (size_t j = 0; j < questions[i].size(); ++j)
          buffer[i * questions.front().size() + j] = questions[i][j].get_difficulty();
      double x = 0, y = 0;
      for (size_t i = 0; i < questions_count; ++i) {
        x += buffer[i] * buffer[i];
        y += buffer[i];
      }
      y = y * y;
      double d = (x - y / questions_count) / questions_count; // D 
      fitness *= questions.size() * (1 / (d + 1)) * 10;
    }
    {
      for (size_t i = 0; i < questions.size(); ++i) {
        size_t buffer[questions[i].size()];
        for (size_t j = 0; j < questions[i].size(); ++j)
          buffer[j] = questions[i][j].get_topic_id();
        std::sort(buffer, buffer + questions[i].size());
        size_t count = 1;
        for (size_t j = 1; j < questions[i].size(); ++j)
          count += buffer[j] != buffer[j - 1];
        fitness *= double(count) * 10 / questions[i].size();
      }
    }
    changed = false;
    return fitness;
  }

  variants_t crossover(variants_t const &v) const noexcept {
    variants_t result(*this);
    size_t questions_count = 0;
    for (auto const &v : questions)
      questions_count += v.size();
    size_t to_swap = rand() % questions_count;
    for (size_t i = 0; i < result.questions.size() && to_swap; ++i)
      for (size_t j = 0; j < result.questions.size() && to_swap; ++j, --to_swap)
        result.questions[i][j] = v.questions[i][j];
    return result;
  }

  std::vector<question_t> &operator [] (size_t i) noexcept {
    changed = true;
    return questions[i];
  }
};

}
