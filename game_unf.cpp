
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <random>
#include <stdexcept>

using namespace std;

// ================= БАЗОВЫЙ КЛАСС =================

class Hero {
protected:
    string name;
    int hp;
    int maxHp;
    int attack;
    bool defending;

public:
    Hero(string n, int health, int damage)
        : name(n), hp(health), maxHp(health),
          attack(damage), defending(false) {}

    virtual ~Hero() = default;

    string getName() const {
        return name;
    }

    int getHp() const {
        return hp;
    }

    bool isAlive() const {
        return hp > 0;
    }

    virtual int getBaseDamage() const {
        return attack;
    }

    void defend() {
        defending = true;
        cout << name << " принимает защитную стойку!\n";
    }

    void takeDamage(int damage) {
        if (defending) {
            damage /= 2;
            defending = false;

            cout << name << " блокирует часть урона!\n";
        }

        hp -= damage;

        if (hp < 0)
            hp = 0;

        cout << name << " получает " << damage
             << " урона. HP: " << hp << "/" << maxHp << "\n";
    }

    void reset() {
        hp = maxHp;
        defending = false;
    }

    virtual void showInfo() const {
        cout << name
             << " | HP: " << hp
             << " | Атака: " << attack << "\n";
    }
};

// ================= КЛАСС ЛУЧНИКА =================

class Archer : public Hero {
public:
    Archer() : Hero("Лучник", 80, 25) {}

    int getBaseDamage() const override {
        return attack;
    }

    void showInfo() const override {
        cout << "Лучник | HP: " << hp
             << " | Атака: " << attack
             << " | Быстрый и меткий боец\n";
    }
};

// ================= КЛАСС ВОИНА =================

class Warrior : public Hero {
public:
    Warrior() : Hero("Воин", 120, 20) {}

    int getBaseDamage() const override {
        return attack;
    }

    void showInfo() const override {
        cout << "Воин | HP: " << hp
             << " | Атака: " << attack
             << " | Обладает большим запасом здоровья\n";
    }
};

// ================= КЛАСС МАГА =================

class Mage : public Hero {
public:
    Mage() : Hero("Маг", 90, 30) {}

    int getBaseDamage() const override {
        return attack;
    }

    void showInfo() const override {
        cout << "Маг | HP: " << hp
             << " | Атака: " << attack
             << " | Обладает мощной атакой\n";
    }
};

// ================= КЛАСС МЕНЕДЖЕРА =================

class GameManager {
private:
    vector<unique_ptr<Hero>> heroes;
    mt19937 rng;

    // Безопасный ввод числа
    int readNumber(int min, int max) {
        while (true) {
            try {
                string input;
                getline(cin, input);

                size_t pos;
                int number = stoi(input, &pos);

                if (pos != input.size())
                    throw invalid_argument("Некорректный ввод");

                if (number < min || number > max)
                    throw out_of_range("Число вне диапазона");

                return number;
            }
            catch (const exception&) {
                cout << "Ошибка! Введите число от "
                     << min << " до " << max << ": ";
            }
        }
    }

    // Случайный урон с разбросом +-5
    int calculateDamage(Hero& hero) {
        uniform_int_distribution<int> distribution(-5, 5);

        int damage = hero.getBaseDamage()
                   + distribution(rng);

        return max(1, damage);
    }

    // Выбор героя игроком
    int selectHero() {
        cout << "\n===== ВЫБОР ГЕРОЯ =====\n";

        for (int i = 0; i < heroes.size(); i++) {
            cout << i + 1 << ". ";
            heroes[i]->showInfo();
        }

        cout << "0. Выход\n";
        cout << "Ваш выбор: ";

        return readNumber(0, 3);
    }

    // Выбор противника
    int selectEnemy(int playerIndex) {
        cout << "\n===== ВЫБОР ПРОТИВНИКА =====\n";

        for (int i = 0; i < heroes.size(); i++) {
            if (i == playerIndex)
                continue;

            cout << i + 1 << ". ";
            heroes[i]->showInfo();
        }

        cout << "Ваш выбор: ";

        while (true) {
            int choice = readNumber(1, 3);

            if (choice - 1 != playerIndex)
                return choice - 1;

            cout << "Нельзя выбрать своего героя!\n";
            cout << "Выберите другого противника: ";
        }
    }

    // Ход игрока
    void playerTurn(Hero& player, Hero& enemy) {
        cout << "\nВаш ход!\n";
        cout << "1. Атака\n";
        cout << "2. Защита\n";
        cout << "Выберите действие: ";

        int choice = readNumber(1, 2);

        if (choice == 1) {
            int damage = calculateDamage(player);

            cout << player.getName()
                 << " атакует противника!\n";

            enemy.takeDamage(damage);
        }
        else {
            player.defend();
        }
    }

    // Ход компьютера
    void enemyTurn(Hero& enemy, Hero& player) {
        uniform_int_distribution<int> action(1, 2);

        int choice = action(rng);

        cout << "\nХод противника!\n";

        if (choice == 1) {
            int damage = calculateDamage(enemy);

            cout << enemy.getName()
                 << " атакует!\n";

            player.takeDamage(damage);
        }
        else {
            enemy.defend();
        }
    }

    // Проведение боя
    void startBattle(Hero& player, Hero& enemy) {
        player.reset();
        enemy.reset();

        cout << "\n============================\n";
        cout << "       НАЧАЛО БОЯ!\n";
        cout << "============================\n";

        cout << player.getName()
             << " VS " << enemy.getName() << "\n";

        // Случайное определение первого хода
        uniform_int_distribution<int> turnOrder(0, 1);

        bool playerFirst = turnOrder(rng) == 0;

        if (playerFirst)
            cout << "Первым ходит игрок!\n";
        else
            cout << "Первым ходит противник!\n";

        while (player.isAlive() && enemy.isAlive()) {

            if (playerFirst) {
                playerTurn(player, enemy);

                if (enemy.isAlive())
                    enemyTurn(enemy, player);
            }
            else {
                enemyTurn(enemy, player);

                if (player.isAlive())
                    playerTurn(player, enemy);
            }

            playerFirst = !playerFirst;

            cout << "\n----------------------------\n";
            cout << player.getName()
                 << " HP: " << player.getHp() << "\n";

            cout << enemy.getName()
                 << " HP: " << enemy.getHp() << "\n";
            cout << "----------------------------\n";
        }

        // Результат боя
        if (player.isAlive()) {
            cout << "\nПОБЕДА! Вы выиграли сражение!\n";
        }
        else {
            cout << "\nПОРАЖЕНИЕ! Ваш герой пал.\n";
            cout << "Игра окончена.\n";
        }
    }

public:
    GameManager() : rng(random_device{}()) {
        heroes.push_back(make_unique<Archer>());
        heroes.push_back(make_unique<Warrior>());
        heroes.push_back(make_unique<Mage>());
    }

    // Главное меню
    void run() {
        cout << "============================\n";
        cout << "       АРЕНА ГЕРОЕВ\n";
        cout << "============================\n";

        while (true) {
            cout << "\n===== ГЛАВНОЕ МЕНЮ =====\n";
            cout << "1. Выбрать героя\n";
            cout << "0. Выход из игры\n";
            cout << "Ваш выбор: ";

            int choice = readNumber(0, 1);

            if (choice == 0) {
                cout << "Выход из игры. До свидания!\n";
                break;
            }

            int playerIndex = selectHero();

            if (playerIndex == 0) {
                cout << "Возврат в главное меню.\n";
                continue;
            }

            playerIndex--;

            cout << "\nВы выбрали: "
                 << heroes[playerIndex]->getName() << "\n";

            int enemyIndex = selectEnemy(playerIndex);

            startBattle(
                *heroes[playerIndex],
                *heroes[enemyIndex]
            );

            if (!heroes[playerIndex]->isAlive()) {
                break;
            }

            cout << "\nБой завершен! Возвращение в меню.\n";
        }
    }
};

// ================= ГЛАВНАЯ ФУНКЦИЯ =================

int main() {
    try {
        setlocale(LC_ALL, "");

        GameManager game;
        game.run();
    }
    catch (const exception& e) {
        cerr << "Критическая ошибка: "
             << e.what() << "\n";
        return 1;
    }

    return 0;
}
