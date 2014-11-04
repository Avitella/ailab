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
./bin/console-tool --variants-count=8 --questions-count=12 --mutation-chance=0.01 --population-size=200 --life-time=200 --mutation-duplicate_chance=0.02 --topics=6,50,47,423,43 --stat-enabled --show-config
    
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
