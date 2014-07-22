Example:
    /bin/console-tool --variants_count=8 --questions_count=12 --mutation_chance=0.01 --population_size=200 --life_time=200 --mutation_duplicate_chance=0.02 --topics=6,50,47,423,43 --stat_enabled --show_config
    
Command line args:
    "life_time=" - количество итераций
    "population_size=" - размер популяции
    "mutation_chance=" - шанс мутации в процентах
    "log_enabled" - включение лога (текущее время - лучшее значение фитнес-функции)
    "stat_enabled" - включение статистики (количество подходов, мутаци и т.д.)
    "variants_count=" - количество вариантов
    "questions_count=" - количество вопросов в варианте
    "show_config" - показать сгенерированный конфиг
    "mutation_duplicate_chance=" - шанс сохранить оригинальую особь вместе с мутирующей
    "show_topics_disabled" - не выводить дерево тем
    "topics=" - темы, из которых необходимо сгенерировать тестовые задания
    "show_result_disabled" - отключить вывод ответа
    "sort_result_disabled" - отключить сортировку ответа
    "try_generate=" - количество попыток генерации
