#include "db_engine.h"
#include "getopt.h"
#include "config.h"
#include "generator.h"

#include <iostream>
#include <unordered_map>
#include <unordered_set>

template<typename graph_t, typename used_t, typename mapping_t, typename questions_count_t>
static void dfs_topic_tree(size_t u, graph_t const &graph, used_t &used, mapping_t const &mapping,
    questions_count_t const &questions_count, size_t offset = 0) {
  used.insert(u);
  if (u != 0) {
    for (size_t i = 0; i + 1 < offset; ++i)
      std::cout << '\t';
    std::cout << mapping.at(u).get_topic_id() << " : " << mapping.at(u).get_text() << " (" << questions_count.at(u) << ")" << std::endl;
  }
  if (graph.find(u) == graph.end())
    return;
  for (size_t v : graph.at(u))
    if (used.find(v) == used.end())
      dfs_topic_tree(v, graph, used, mapping, questions_count, offset + 1);
}

template<typename graph_t, typename used_t, typename questions_count_t>
static size_t dfs_questions_count(size_t u, graph_t const &graph, used_t &used, questions_count_t &questions_count) {
  used.insert(u);
  if (graph.find(u) == graph.end())
    return questions_count[u];
  for (size_t v : graph.at(u))
    if (used.find(v) == used.end())
      questions_count[u] += dfs_questions_count(v, graph, used, questions_count);
  return questions_count[u];
}

static void write_topic_tree(std::vector<ailab::topic_t> const &topics, std::vector<ailab::question_t> const &questions) {
  std::unordered_map<size_t, ailab::topic_t> mapping;
  std::unordered_map<size_t, size_t> questions_count;
  std::unordered_map<size_t, std::vector<size_t>> graph;
  for (auto const &t : topics) {
    mapping[t.get_topic_id()] = t;
    questions_count[t.get_topic_id()] = 0;
  }
  for (auto const &t : topics)
    graph[t.get_parent_id()].push_back(t.get_topic_id());
  for (auto const &q : questions)
    ++questions_count[q.get_topic_id()];
  std::unordered_set<size_t> used;
  dfs_questions_count(0, graph, used, questions_count);
  used.clear();
  dfs_topic_tree(0, graph, used, mapping, questions_count);
}

int main(int argc, char *argv[]) {
  std::unordered_map<std::string, std::string> opts = ailab::getopt(argc, argv, {
    "life_time=",
    "population_size=",
    "mutation_chance=",
    "log_enabled",
    "stat_enabled",
    "variants_count=",
    "questions_count=",
    "show_config",
    "mutation_duplicate_chance=",
    "show_topics_disabled",
    "topics=",
    "show_result_disabled",
    "sort_result_disabled",
    "try_generate="
  });

  ailab::config_t config(opts);

  ailab::db_engine_t db;

  std::vector<ailab::topic_t> topics = db.select_topics();
  std::vector<ailab::question_t> questions = db.select_questions();
  
  std::string s;

  if (opts.find("topics") == opts.end()) {
    if (opts.find("show_topics_disabled") == opts.end())
      write_topic_tree(topics, questions);
    std::cout << "Введите идентификаторы тем через запятую: ";
    std::getline(std::cin, s);
  } else {
    s = opts.at("topics");
  }

  for (size_t i = 0; i < s.length(); ) {
    size_t j = i + 1;
    while (j < s.length() && s[j] != ',')
      ++j;
    config.topics.push_back(std::stoul(s.substr(i, j - i)));
    i = j + 1;
  }

  if (opts.find("show_config") != opts.end())
    std::cerr << config;

  ailab::generator_t generator(config, topics, questions);
  ailab::variants_t variants = generator.generate();

  if (opts.find("show_result_disabled") == opts.end())
    for (size_t i = 0; i < variants.size(); ++i) {
      std::cout << i + 1 << std::endl;
      if (opts.find("sort_result_disabled") == opts.end())
        std::sort(variants[i].begin(), variants[i].end(), [] (ailab::question_t const &a, ailab::question_t const &b) -> bool {
          return a.get_question_id() < b.get_question_id();
        });
      
      static const std::string question_id = "QuestionID", topic_id = "TopicID", select_id = "SelectID", difficulty = "Difficulty", text = "Text";
      static const std::string stick = " | ";
      static const std::string header = question_id + stick + topic_id + stick + select_id + stick + difficulty + stick + text;
      static const size_t length = header.length() + 1;
      
      for (size_t i = 0; i < length; ++i)
        std::cout << '-';
      std::cout << std::endl;
      std::cout << header << std::endl;
      for (size_t i = 0; i < length; ++i)
        std::cout << '-';
      std::cout << std::endl;

      for (ailab::question_t const &q : variants[i]) {
        std::string buf = std::to_string(q.get_question_id());
        while (buf.length() < question_id.length())
          buf += ' ';
        buf += stick;
        buf += std::to_string(q.get_topic_id());
        while (buf.length() < question_id.length() + stick.length() + topic_id.length())
          buf += ' ';
        buf += stick;
        buf += std::to_string(q.get_select_id());
        while (buf.length() < question_id.length() + stick.length() + topic_id.length() + stick.length() + select_id.length())
          buf += ' ';
        buf += stick;
        buf += std::to_string(q.get_difficulty());
        while (buf.length() < question_id.length() + stick.length() + topic_id.length() + stick.length() + select_id.length() + stick.length() + difficulty.length())
          buf += ' ';
        buf += stick;
        buf += q.get_text();
        std::cout << buf << std::endl;
      }

      for (size_t i = 0; i < length; ++i)
        std::cout << '-';
      std::cout << std::endl;
    }

  return 0;
}
