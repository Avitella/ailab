#pragma once

#include <algorithm>
#include <cstdlib>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "question.h"
#include "exception.h"

namespace ailab {

// TODO: make it without O(n^2) size
class question_shaker_t {
  std::unordered_map<size_t, topic_t> topics;
  std::unordered_map<size_t, std::vector<question_t>> questions;

  void dfs(size_t u, std::unordered_map<size_t, std::vector<size_t>> const &g, std::unordered_set<size_t> &used) noexcept {
    used.insert(u);
    if (g.find(u) == g.end())
      return;
    for (size_t i = 0; i < g.at(u).size(); ++i) {
      size_t v = g.at(u)[i];
      if (used.find(v) == used.end()) {
        dfs(v, g, used);
        for (question_t const &q : questions[v])
          questions[u].push_back(q);
      }
    }
  }

 public:
  question_shaker_t(std::vector<topic_t> const &t, std::vector<question_t> const &q) noexcept {
    for (question_t const &question : q)
      questions[question.get_topic_id()].push_back(question);
    for (topic_t const &topic : t)
      topics[topic.get_topic_id()] = topic;
    std::unordered_map<size_t, std::vector<size_t>> g;
    std::unordered_set<size_t> used;
    for (topic_t const &topic : t)
      if (topic.get_parent_id() != 0)
        g[topic.get_parent_id()].push_back(topic.get_topic_id());
    for (topic_t const &topic : t)
      if (used.find(topic.get_topic_id()) == used.end())
        dfs(topic.get_topic_id(), g, used);
    for (auto &p : questions)
      std::random_shuffle(p.second.begin(), p.second.end());
    srand(time(0));
  }

  question_t get_question(size_t topic_id) const {
    if (questions.at(topic_id).empty())
      throw exception_t("Not enough questions for topic: %lu, %s", topic_id, topics.at(topic_id).get_text().c_str());
    size_t r = rand() % questions.at(topic_id).size();
    return questions.at(topic_id)[r];
  }
};

}
