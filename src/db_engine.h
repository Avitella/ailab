#pragma once

#include <mysql/mysql.h>

#include <stdlib.h>

#include <string>
#include <vector>

#include "question.h"
#include "exception.h"
#include "topic.h"

namespace ailab {

struct db_engine_t {
  struct config_t {
    std::string host, user, password, db;
    unsigned int port;

    config_t() :
        host("localhost"),
        user("root"),
        password(""),
        db("labdb"),
        port(0) {
    }
  };

 private:
  config_t config;
  MYSQL *connection;

 public:
  db_engine_t(config_t const &config = config_t()) :
      config(config),
      connection(nullptr) {
    connection = mysql_init(nullptr);
    
    if (!connection)
      throw exception_t("%s", mysql_error(connection));
    
    if (!mysql_real_connect(connection, config.host.c_str(), config.user.c_str(),
        config.password.c_str(), config.db.c_str(), config.port, nullptr, 0)) {
      mysql_close(connection);
      throw exception_t("%s", mysql_error(connection));
    }
  }

  std::vector<question_t> select_questions() const {
    std::vector<question_t> questions;
    
    if (mysql_query(connection, "SELECT QuestionID, TopicID, Text, Difficulty FROM Question"))
      throw exception_t("%s", mysql_error(connection));

    MYSQL_RES *result = mysql_store_result(connection);
    if (!result)
      throw exception_t("%s", mysql_error(connection));
    
    MYSQL_ROW row = mysql_fetch_row(result);
    
    while (row) {
      size_t question_id = atoll(row[0]);
      size_t topic_id = atoll(row[1]);
      std::string text = row[2];
      size_t difficulty = atoll(row[3]);
    
      questions.push_back(question_t(question_id, topic_id, difficulty, text));

      row = mysql_fetch_row(result);
    }

    return questions;
  }

  std::vector<topic_t> select_topics() const {
    std::vector<topic_t> topics;
    
    if (mysql_query(connection, "SELECT TopicID, ParentID, Name FROM Topic"))
      throw exception_t("%s", mysql_error(connection));

    MYSQL_RES *result = mysql_store_result(connection);
    if (!result)
      throw exception_t("%s", mysql_error(connection));
    
    MYSQL_ROW row = mysql_fetch_row(result);
    
    while (row) {
      size_t topic_id = row[0] ? atoll(row[0]) : 0;
      size_t parent_id = row[1] ? atoll(row[1]) : 0;
      std::string text = row[2];
 
      topics.push_back(topic_t(topic_id, parent_id, text));

      row = mysql_fetch_row(result);
    }

    return topics;
  }

  ~db_engine_t() {
    if (connection)
      mysql_close(connection);
  }
};

}
