# sw-cpp-battle-test

Пошаговая симуляция боевых столкновений юнитов на сетке W×H.

## Сборка

```bash
cmake -S . -B build -DCMAKE_CXX_COMPILER=clang++
cmake --build build
```

Требования: clang 15+, cmake 3.27+, Ubuntu.

## Запуск

```bash
./build/sw_battle_test commands_example.txt
```

## Формат сценария

```
CREATE_MAP 10 10
SPAWN_SWORDSMAN 1 0 0 5 2      # id x y hp strength
SPAWN_HUNTER   2 9 0 10 5 1 4  # id x y hp agility strength range
MARCH 1 9 0                     # unitId targetX targetY
```

Строки начинающиеся с `//` и пустые строки игнорируются.

## Архитектура

```
src/
├── Core/       — фреймворк (не трогать при добавлении юнитов)
│   ├── Position.hpp   — координаты + дистанция Чебышева
│   ├── Unit.hpp/cpp   — абстрактный базовый класс юнита
│   ├── Map.hpp/cpp    — карта, пространственные запросы, движение, RNG
│   └── Game.hpp/cpp   — цикл симуляции
├── Features/   — конкретные юниты (сюда добавлять новые)
│   ├── Swordsman.hpp/cpp
│   └── Hunter.hpp/cpp
└── IO/         — парсер команд и вывод событий (трогать только для новых команд/событий)
```

### Добавление нового юнита

1. Создать `Features/MyUnit.hpp/.cpp`, унаследоваться от `Unit`, реализовать `getTypeName()` и `act()`.
2. Добавить `SPAWN_MYUNIT` в `IO/Commands/` по аналогии с `SpawnSwordsman.hpp`.
3. Зарегистрировать в `main.cpp`.

Core не трогается.

### Цикл симуляции

- Тик 1 — инициализация (MAP\_CREATED, UNIT\_SPAWNED, MARCH\_STARTED)
- Тик 2+ — каждый тик: юниты ходят в порядке создания, в конце тика удаляются мёртвые
- Юнит, получивший смертельный урон, успевает сходить в текущем тике
- Завершение: меньше двух юнитов осталось, или никто не совершил действия за тик

### Расстояние

Используется метрика Чебышева: `max(|dx|, |dy|)`. Естественна для 8-направленного движения — диагональный шаг стоит столько же, сколько горизонтальный.

## Known Issues

Подробнее см. [KNOWN_ISSUES.md](KNOWN_ISSUES.md).