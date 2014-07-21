#pragma once

#include <string>

namespace ailab {

class topic_t {
  size_t topic_id, parent_id;
  std::string text;

 public:
  topic_t(size_t topic_id = 0, size_t parent_id = 0, std::string const &text = "") noexcept :
      topic_id(topic_id),
      parent_id(parent_id),
      text(text) {
  }

  size_t get_topic_id() const noexcept {
    return topic_id;
  }

  size_t get_parent_id() const noexcept {
    return parent_id;
  }

  std::string const &get_text() const noexcept {
    return text;
  }
};

}
