#pragma once

#include <string>

namespace ailab {

class question_t {
  size_t question_id, topic_id, difficulty;
  std::string text;

 public:
  question_t(size_t question_id, size_t topic_id, size_t difficulty, std::string const text = "") noexcept :
      question_id(question_id),
      topic_id(topic_id),
      difficulty(difficulty),
      text(text) {
  }

  size_t get_question_id() const noexcept {
    return question_id;
  }

  size_t get_topic_id() const noexcept {
    return topic_id;
  }

  size_t get_difficulty() const noexcept {
    return difficulty;
  }

  std::string const &get_text() const noexcept {
    return text;
  }
};

}
