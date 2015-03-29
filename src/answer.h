#pragma once

#include <string>

namespace ailab {

class answer_t {
  size_t question_id, answer_id;
  bool correct;
  std::string text;

 public:
  answer_t(size_t aid, size_t qid, bool correct, std::string const &text = "") noexcept :
      question_id(qid),
      answer_id(aid),
      correct(correct),
      text(text) {
  }

  size_t get_question_id() const noexcept {
    return question_id;
  }
      
  size_t get_answer_id() const noexcept {
    return answer_id;
  }

  bool is_correct() const noexcept {
    return correct;
  }

  std::string const &get_text() const noexcept {
    return text;
  }
};

}
