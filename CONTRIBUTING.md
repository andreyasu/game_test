# Как добавлять фичи

Этот проект спроектирован так, чтобы много разработчиков добавляли юнитов и механики
параллельно, не наступая друг другу на ноги и **не редактируя `Core/`**. Ниже —
рецепты.

## Сборка и тесты

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Воспроизводимый прогон для отладки: `./build/sw_battle_test --seed 42 <scenario>`.

## Добавить новый юнит

Юнит = `UnitTraits` (что он есть) + список действий (что он делает). Обычно это пара
файлов в `Features/` и одна команда в `IO/`. Пример — Лучник-новичок, который лечит и
отступает:

1. **`Features/Medic.hpp` / `Medic.cpp`** — отнаследоваться от `Unit`, в конструкторе
   задать traits и собрать действия из готовых `Actions/`:

   ```cpp
   Medic::Medic(uint32_t id, Position pos, uint32_t hp, uint32_t spirit)
       : Unit(id, "Medic", pos, UnitTraits{.hp = static_cast<int32_t>(hp)})
   {
       addAction<features::Heal>(spirit, /*radius=*/2);
       addAction<features::MoveToTarget>();
   }
   ```

2. **`IO/Commands/SpawnMedic.hpp`** — описать команду по образцу `SpawnHealer.hpp`
   (имя `SPAWN_MEDIC`, поля, `visit`).

3. **`main.cpp`** — одна строка регистрации:

   ```cpp
   .add<io::SpawnMedic>([&](auto c) {
       sim.addUnit(std::make_shared<Medic>(c.unitId, Position{c.x, c.y}, c.hp, c.spirit));
   })
   ```

4. **Тест** в `tests/` — задать сценарий и проверить поведение (см. `SimulationTests.cpp`).

`Core/` при этом не меняется. Файлы подхватываются CMake автоматически
(`GLOB … CONFIGURE_DEPENDS`).

## Добавить новое действие (механику)

Если поведение принципиально новое (рывок, призыв, аура), это новый класс `IAction` в
`Features/Actions/`:

```cpp
class Dash final : public IAction
{
public:
    explicit Dash(uint32_t cells) : _cells(cells) {}
    bool perform(Unit& self, TurnContext& ctx) override
    {
        // ... использовать ctx.map / ctx.rng / эмитить событие через ctx.log ...
        return /* true, если действие отработало */;
    }
private:
    uint32_t _cells;
};
```

Контракт: `perform` возвращает `true`, если юнит что-то сделал (ход потрачен), иначе
`false` — тогда движок попробует следующее действие. Боевые правила (кого можно бить,
модификаторы) живут здесь, не в `Core`.

## Добавить новую характеристику

Поле в `UnitTraits` со значением по умолчанию, плюс учёт в нужном действии. Например,
броня:

```cpp
struct UnitTraits { /* ... */ uint32_t armor = 0; };
// в действии-атаке: amount = (amount > target.armor()) ? amount - target.armor() : 0;
```

Дефолт сохраняет поведение всех существующих юнитов.

## Добавить новое событие

Заголовок в `IO/Events/` по образцу (поле `Name`, поля данных, шаблон `visit`). Лог
печатает его автоматически через визитор — отдельный код вывода не нужен. Эмитится из
действия или из `Simulation` через `ctx.log.log(tick, io::MyEvent{...})`.

## Конвенции

- **`Core/` неприкосновенен** для фич. Нужна правка `Core` — значит, не хватает
  обобщённого крючка (поле в `UnitTraits` / метод в `Map`); добавляйте крючок, а не
  ветвление по типу.
- Стиль — `.clang-format` в корне; CI проверяет (`clang-format --dry-run --Werror`).
- Предупреждения компилятора — ошибки в CI (`-DSW_WERROR=ON`); собирайтесь с `-Wall
  -Wextra` локально.
- Только стандартная библиотека — никаких сторонних зависимостей (включая тесты).
- Новая фича — новый тест.
