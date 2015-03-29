#include "db_engine.h"
#include "getopt.h"
#include "config.h"
#include "generator.h"

#include <iostream>
#include <unordered_map>
#include <unordered_set>

template<
  typename graph_t,
  typename used_t,
  typename mapping_t,
  typename questions_count_t,
  typename answers_map_t,
  typename questions_map_t
>
static void dfs_topic_tree(
    size_t u,
    graph_t const &graph,
    used_t &used,
    mapping_t const &mapping,
    questions_count_t const &questions_count,
    questions_map_t const &q_map,
    answers_map_t const &a_map,
    bool write_questions,
    size_t offset = 0)
{
  used.insert(u);
  if (u != 0) {
    for (size_t i = 0; i + 1 < offset; ++i)
      std::cout << '\t';
    std::cout << mapping.at(u).get_topic_id() << " : " << mapping.at(u).get_text() << " (count: " << questions_count.at(u) << ")" << std::endl;
    if (write_questions && q_map.find(u) != q_map.end())
      for (ailab::question_t const &q : q_map.at(u)) {
        for (size_t i = 0; i < offset; ++i)
          std::cout << '\t';
        std::cout << "[q: " << q.get_text() << "]" << std::endl;
        if (a_map.find(q.get_question_id()) != a_map.end())
          for (ailab::answer_t const &a : a_map.at(q.get_question_id())) {
            for (size_t i = 0; i < offset + 1; ++i)
              std::cout << '\t';
            std::cout << "{" << (a.is_correct() ? "+" : "-") << ": " << a.get_text() << "}" << std::endl;
          }
      }
  }
  if (graph.find(u) == graph.end())
    return;
  for (size_t v : graph.at(u))
    if (used.find(v) == used.end())
      dfs_topic_tree(v, graph, used, mapping, questions_count, q_map, a_map, write_questions, offset + 1);
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

static void write_topic_tree(std::vector<ailab::topic_t> const &topics, std::vector<ailab::question_t> const &questions,
    std::vector<ailab::answer_t> const &answers, bool write_questions) {
  std::unordered_map<size_t, ailab::topic_t> mapping;
  std::unordered_map<size_t, size_t> questions_count;
  std::unordered_map<size_t, std::vector<ailab::question_t>> questions_map;
  std::unordered_map<size_t, std::vector<ailab::answer_t>> answers_map;
  std::unordered_map<size_t, std::vector<size_t>> graph;
  for (auto const &q : questions)
    questions_map[q.get_topic_id()].push_back(q);
  for (auto const &a : answers)
    answers_map[a.get_question_id()].push_back(a);
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
  dfs_topic_tree(0, graph, used, mapping, questions_count, questions_map, answers_map, write_questions);
}

void write_usage(std::vector<std::string> const &opts) {
  std::cout << "OPTIONS:" << std::endl;
  for (std::string const &opt : opts) {
    std::cout << "  ";
    if (opt[opt.length() - 1] == '=') {
      std::string str = opt.substr(0, opt.length() - 1);
      if (str.length() > 1) {
        std::cout << "--" << str << " [ARG]" << std::endl;
      } else {
        std::cout << "-" << str << " [ARG]" << std::endl;
      }
    } else {
      std::string str = opt;
      if (str.length() > 1) {
        std::cout << "--" << str << std::endl;
      } else {
        std::cout << "-" << str << std::endl;
      }
    }
  }
}

int main(int argc, char *argv[]) {
  std::vector<std::string> opt_strs = {
    "h",
    "help",
    "life-time=",
    "log-enabled",
    "mutation-chance=",
    "mutation-duplicate-chance=",
    "population-size=",
    "question-id-field=",
    "questions-count=",
    "show-config",
    "show-result-disabled",
    "show-topics-disabled",
    "shuffle-result",
    "sort-result-disabled",
    "sql",
    "stat-enabled",
    "topics=",
    "try-generate=",
    "tsv",
    "variant-id-field=",
    "variants-count=",
    "variants-first-index=",
    "variants-first-index=",
    "variants-table=",
    "show-questions"
  };

  std::unordered_map<std::string, std::string> opts = ailab::getopt(argc, argv, opt_strs);

  if (opts.find("h") != opts.end() || opts.find("help") != opts.end()) {
    write_usage(opt_strs);
    return 0;
  }

  if (opts.find("variants-first-index") == opts.end()) {
    opts["variants-first-index"] = "1";
  }

  if (opts.find("sql") != opts.end() &&
      (opts.find("variants-table") == opts.end() || opts.find("variant-id-field") == opts.end() || opts.find("question-id-field") == opts.end())) {
    throw ailab::exception_t("try to use sql option without sql options");
  }

  ailab::config_t config(opts);

  ailab::db_engine_t db;

  std::vector<ailab::topic_t> topics = db.select_topics();
  std::vector<ailab::question_t> questions = db.select_questions();
  std::vector<ailab::answer_t> answers = db.select_answers();
  
  std::string s;

  if (opts.find("topics") == opts.end()) {
    if (opts.find("show-topics-disabled") == opts.end())
      write_topic_tree(topics, questions, answers, opts.find("show-questions") != opts.end());
    std::cout << "Введите идентификаторы тем через запятую: ";
    std::getline(std::cin, s);
  } else {
    s = opts.at("topics");
  }

  if (s.empty()) {
    std::cout << "Отсутствует список тем." << std::endl;
    return 0;
  }

  for (size_t i = 0; i < s.length(); ) {
    size_t j = i + 1;
    while (j < s.length() && s[j] != ',')
      ++j;
    config.topics.push_back(std::stoul(s.substr(i, j - i)));
    i = j + 1;
  }

  if (opts.find("show-config") != opts.end())
    std::cerr << config;

  ailab::generator_t generator(config, topics, questions);
  ailab::variants_t variants = generator.generate();

  if (opts.find("tsv") != opts.end()) {
    std::cout << "Variant\tQuestionID\tText\tifficulty" << std::endl;
    size_t first_index = std::stol(opts["variants-first-index"]);
    for (size_t i = 0; i < variants.size(); ++i) {
      for (ailab::question_t const &q : variants[i]) {
        std::cout << i + first_index << '\t' << q.get_question_id() << '\t' << q.get_text() << '\t' << q.get_difficulty() << '\n';
      }
    }

    return 0;
  }

  if (opts.find("sql") != opts.end()) {
    size_t first_index = std::stol(opts["variants-first-index"]);
    std::cout << "insert into " << opts["variants-table"] << " (" << opts["variant-id-field"] << ", " << opts["question-id-field"] << ") values " << std::endl;
    for (size_t i = 0; i < variants.size(); ++i) {
      if (opts.find("shuffle-result") != opts.end()) {
        std::random_shuffle(variants[i].begin(), variants[i].end());
      }
      for (size_t j = 0; j < variants[i].size(); ++j) {
        ailab::question_t const &q = variants[i][j];
        std::cout << "(" << first_index + i << ", " << q.get_question_id() << ")";
        if (j + 1 == variants[i].size() && i + 1 == variants.size()) {
          std::cout << ";" << std::endl;
        } else {
          std::cout << "," << std::endl;
        }
      }
    }
    return 0;
  }

  if (opts.find("show-result-disabled") == opts.end()) {
    size_t first_index = std::stol(opts["variants-first-index"]);
     
    for (size_t i = 0; i < variants.size(); ++i) {
      std::cout << i + first_index << std::endl;
      if (opts.find("sort-result-disabled") == opts.end())
        std::sort(variants[i].begin(), variants[i].end(), [] (ailab::question_t const &a, ailab::question_t const &b) -> bool {
          return a.get_question_id() < b.get_question_id();
        });
      if (opts.find("shuffle-result") != opts.end())
        std::random_shuffle(variants[i].begin(), variants[i].end());
      
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

  }

  return 0;
}
