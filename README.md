## Build
scons

## Depends
* libmysqlclient-dev
* mysql-server
* mysql-client
* g++
* scons

## Install depends on ubuntu
sudo apt-get install libmysqlclient-dev mysql-server mysql-client g++ scons

## Usage example:
./bin/console-tool --variants_count=8 --questions_count=12 --mutation_chance=0.01 --population_size=200 --life_time=200 --mutation_duplicate_chance=0.02 --topics=6,50,47,423,43 --stat_enabled --show_config
    
## Command line args:
OPTIONS:
* -h
* --help
* --life-time [ARG]
* --log-enabled
* --mutation-chance [ARG]
* --mutation-duplicate-chance [ARG]
* --population-size [ARG]
* --question-id-field [ARG]
* --questions-count [ARG]
* --show-config
* --show-result-disabled
* --show-topics-disabled
* --shuffle-result
* --sort-result-disabled
* --sql
* --stat-enabled
* --topics [ARG]
* --try-generate [ARG]
* --tsv
* --variant-id-field [ARG]
* --variants-count [ARG]
* --variants-first-index [ARG]
* --variants-first-index [ARG]
* --variants-table [ARG]
