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
      questions(config.variants_count) {
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
          buffer[i * config.questions_count + j] = questions[i][j].get_question_id();
      std::sort(buffer, buffer + questions_count);
      size_t count = 1;
      for (size_t i = 1; i < questions_count; ++i)
        count += buffer[i] != buffer[i - 1];
      fitness = double(count) / questions_count;
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
        fitness += count == questions[i].size();
      }
    }
    {
      for (size_t i = 0; i < questions.size(); ++i) {
        size_t buffer[questions[i].size()];
        for (size_t j = 0; j < questions[i].size(); ++j)
          buffer[j] = questions[i][j].get_select_id();
        std::sort(buffer, buffer + questions[i].size());
        size_t count = 1;
        for (size_t j = 1; j < questions[i].size(); ++j)
          count += buffer[j] != buffer[j - 1];
        fitness += double(count) / questions[i].size();
      }
    }
    {
      double buffer[questions_count];
      for (size_t i = 0; i < questions.size(); ++i)
        for (size_t j = 0; j < questions[i].size(); ++j)
          buffer[i * config.questions_count + j] = questions[i][j].get_difficulty();
      double avg = 0;
      for (size_t i = 0; i < questions_count; ++i)
        avg += buffer[i];
      avg /= questions_count;
      double x = 0;
      for (size_t i = 0; i < questions_count; ++i)
        x += (buffer[i] - avg) * (buffer[i] - avg);
      x /= questions_count;
      x = sqrt(x);
      fitness += x;
    }
    {
      for (size_t i = 0; i < questions.size(); ++i) {
        size_t buffer[questions[i].size()];
        for (size_t j = 0; j < questions[i].size(); ++j)
          buffer[j] = questions[i][j].get_select_id();
        std::sort(buffer, buffer + questions[i].size());
        size_t count = 1;
        for (size_t j = 1; j < questions[i].size(); ++j)
          count += buffer[j] != buffer[j - 1];
        std::pair<size_t, size_t> counter[count];
        ssize_t k = -1;
        for (size_t j = 0; j < questions[i].size(); ++j) {
          if (k == -1 || buffer[j] != counter[k].first) {
            ++k;
            counter[k].second = 0;
          }
          ++counter[k].second;
        }
        std::sort(counter, counter + count, [] (std::pair<size_t, size_t> const &a, std::pair<size_t, size_t> const &b) -> bool {
          return a.second > b.second;
        });
        double x = 0;
        for (size_t j = 1; j < count; ++j)
          x += counter[j].second / counter[j - 1].second;
        fitness += x;
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
      for (size_t j = 0; j < result.questions[i].size() && to_swap; ++j, --to_swap)
        result.questions[i][j] = v.questions[i][j];
    return result;
  }

  std::vector<question_t> &operator [] (size_t i) noexcept {
    changed = true;
    return questions[i];
  }

  std::vector<question_t> const &operator [] (size_t i) const noexcept {
    return questions[i];
  }

  void try_fix(question_shaker_t &shaker) noexcept {
    for (std::vector<question_t> &q : *this) {
      std::sort(q.begin(), q.end(), [] (question_t const &a, question_t const &b) -> bool {
        return a.get_question_id() < b.get_question_id();
      });
      for (size_t i = 1; i < q.size(); ++i) {
        if (q[i].get_question_id() == q[i - 1].get_question_id()) {
          size_t r = rand() % config.topics.size();
          q[i - 1] = shaker.get_question(config.topics[r]);
          q[i - 1].set_select_id(config.topics[r]);
        }
      }
    }
  }
};

}
