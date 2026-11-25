
#include <iostream>
#include <string>
#include <memory>
#include <random>
#include <limits>
#include <algorithm>

using namespace std;


class Hero {
protected:
    string name;
    int hp;
    int maxHp;
    int damage;

    int damageBonus = 0;
    int damageReduction = 0;
    int shield = 0;
    bool defending = false;

public:
    Hero(string n, int h, int d)
        : name(n), hp(h), maxHp(h), damage(d) {}

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

    virtual void useAbility(Hero& enemy) = 0;

    virtual string getAbilityName() const = 0;

    virtual string getAbilityDescription() const = 0;

    void attack(Hero& enemy, mt19937& rng) {
        uniform_int_distribution<int> randomDamage(-2, 2);

        int finalDamage = damage + randomDamage(rng);

        finalDamage += damageBonus;
        finalDamage -= damageReduction;

        finalDamage = max(1, finalDamage);

        damageBonus = 0;
        damageReduction = 0;

        cout << name << " атакует противника!\n";

        enemy.takeDamage(finalDamage);
    }

    void takeDamage(int incomingDamage) {
        int finalDamage = incomingDamage;

        // Щит
        if (shield > 0) {
            int absorbed = min(shield, finalDamage);

            finalDamage -= absorbed;
            shield = 0;

            cout << name << " поглощает "
                 << absorbed << " урона щитом!\n";
        }

        if (defending) {
            finalDamage /= 2;
            defending = false;

            cout << name << " блокирует часть урона!\n";
        }

        hp -= finalDamage;

        if (hp < 0)
            hp = 0;

        cout << name << " получает "
             << finalDamage << " урона.\n";

        cout << "Здоровье " << name << ": "
             << hp << "/" << maxHp << "\n";
    }

    void defend() {
        defending = true;

        cout << name
             << " принимает защитную стойку!\n";
    }

    void showInfo() const {
        cout << name
             << " | HP: " << hp << "/" << maxHp
             << " | Урон: " << damage << "\n";
    }

protected:
    friend class Warrior;

    void increaseDamage(int value) {
        damageBonus = value;
    }

    void weakenEnemy(int value) {
        damageReduction = value;
    }

    void activateShield(int value) {
        shield = value;
    }
};

class Archer : public Hero {
public:
    Archer() : Hero("Лучник", 90, 15) {}

    void useAbility(Hero& enemy) override {
        increaseDamage(7);

        cout << name
             << " использует способность: "
             << "меткий выстрел!\n";

        cout << "Следующая атака нанесёт "
             << "дополнительно 7 урона.\n";
    }

    string getAbilityName() const override {
        return "Меткий выстрел";
    }

    string getAbilityDescription() const override {
        return "Увеличивает следующий урон на 7";
    }
};


class Warrior : public Hero {
public:
    Warrior() : Hero("Воин", 120, 12) {}

    void useAbility(Hero& enemy) override {
        enemy.weakenEnemy(5);

        cout << name
             << " использует способность: "
             << "ослабление противника!\n";

        cout << "Следующая атака противника "
             << "будет слабее на 5 урона.\n";
    }

    string getAbilityName() const override {
        return "Ослабление";
    }

    string getAbilityDescription() const override {
        return "Уменьшает следующую атаку врага на 5";
    }
};


class Guardian : public Hero {
public:
    Guardian() : Hero("Страж", 140, 10) {}

    void useAbility(Hero& enemy) override {
        activateShield(12);

        cout << name
             << " использует способность: "
             << "магический щит!\n";

        cout << "Следующий полученный урон "
             << "будет уменьшен на 12.\n";
    }

    string getAbilityName() const override {
        return "Магический щит";
    }

    string getAbilityDescription() const override {
        return "Поглощает до 12 урона";
    }
};


class GameManager {
private:
    mt19937 rng;

    unique_ptr<Hero> createHero(int choice) {
        switch (choice) {
        case 1:
            return make_unique<Archer>();

        case 2:
            return make_unique<Warrior>();

        case 3:
            return make_unique<Guardian>();

        default:
            return nullptr;
        }
    }

    int readChoice(int minValue, int maxValue) {
        int choice;

        while (true) {
            cout << "Ваш выбор: ";

            if (cin >> choice &&
                choice >= minValue &&
                choice <= maxValue) {
                return choice;
            }

            cout << "Ошибка! Введите число от "
                 << minValue << " до "
                 << maxValue << ".\n";

            cin.clear();
            cin.ignore(
                numeric_limits<streamsize>::max(), '\n'
            );
        }
    }

    void showHeroMenu() {
        cout << "\n========== ВЫБОР ГЕРОЯ ==========\n";

        cout << "1. Лучник\n";
        cout << "   HP: 90 | Урон: 15\n";
        cout << "   Способность: Меткий выстрел\n\n";

        cout << "2. Воин\n";
        cout << "   HP: 120 | Урон: 12\n";
        cout << "   Способность: Ослабление\n\n";

        cout << "3. Страж\n";
        cout << "   HP: 140 | Урон: 10\n";
        cout << "   Способность: Магический щит\n\n";

        cout << "0. Выход\n";
        cout << "=================================\n";
    }

    void showEnemyMenu(int playerChoice) {
        cout << "\n========== ВЫБОР ПРОТИВНИКА ==========\n";

        for (int i = 1; i <= 3; i++) {
            if (i != playerChoice) {
                cout << i << ". ";

                if (i == 1) cout << "Лучник\n";
                if (i == 2) cout << "Воин\n";
                if (i == 3) cout << "Страж\n";
            }
        }

        cout << "=====================================\n";
    }

    void playerTurn(Hero& player, Hero& enemy) {
        cout << "\n========== ВАШ ХОД ==========\n";

        player.showInfo();
        enemy.showInfo();

        cout << "\n1. Атака\n";
        cout << "2. Защита\n";
        cout << "3. Способность ("
             << player.getAbilityName() << ")\n";

        int choice = readChoice(1, 3);

        switch (choice) {
        case 1:
            player.attack(enemy, rng);
            break;

        case 2:
            player.defend();
            break;

        case 3:
            player.useAbility(enemy);
            break;
        }
    }

    // Ход компьютера
    void enemyTurn(Hero& enemy, Hero& player) {
        cout << "\n========== ХОД ПРОТИВНИКА ==========\n";

        uniform_int_distribution<int> action(1, 3);

        int choice = action(rng);

        switch (choice) {
        case 1:
            enemy.attack(player, rng);
            break;

        case 2:
            enemy.defend();
            break;

        case 3:
            enemy.useAbility(player);
            break;
        }
    }

    void startBattle(Hero& player, Hero& enemy) {
        cout << "\n=================================\n";
        cout << "         НАЧАЛО БОЯ!\n";
        cout << player.getName()
             << " VS " << enemy.getName() << "\n";
        cout << "=================================\n";

        uniform_int_distribution<int> firstTurn(0, 1);

        bool playerFirst = firstTurn(rng) == 0;

        cout << "\n";

        if (playerFirst) {
            cout << "Вы ходите первыми!\n";
        }
        else {
            cout << "Противник ходит первым!\n";
        }

        bool playerTurnNow = playerFirst;

        while (player.isAlive() && enemy.isAlive()) {

            if (playerTurnNow) {
                playerTurn(player, enemy);
            }
            else {
                enemyTurn(enemy, player);
            }

            if (!player.isAlive() || !enemy.isAlive())
                break;

            playerTurnNow = !playerTurnNow;
        }

        cout << "\n========== РЕЗУЛЬТАТ БОЯ ==========\n";

        if (player.isAlive()) {
            cout << "ПОБЕДА! Вы выиграли сражение!\n";
        }
        else {
            cout << "ПОРАЖЕНИЕ! Вы проиграли сражение.\n";
            cout << "Игра окончена.\n";
        }

        cout << "===================================\n";
    }

public:
    GameManager() : rng(random_device{}()) {}

    void run() {
        cout << "=================================\n";
        cout << "        АРЕНА ГЕРОЕВ\n";
        cout << "=================================\n";

        while (true) {
            showHeroMenu();

            int playerChoice = readChoice(0, 3);

            if (playerChoice == 0) {
                cout << "Выход из игры. До свидания!\n";
                break;
            }

            unique_ptr<Hero> player =
                createHero(playerChoice);

            showEnemyMenu(playerChoice);

            int enemyChoice = readChoice(1, 3);

            while (enemyChoice == playerChoice) {
                cout << "Нельзя выбрать самого себя!\n";
                enemyChoice = readChoice(1, 3);
            }

            unique_ptr<Hero> enemy =
                createHero(enemyChoice);

            startBattle(*player, *enemy);

            cout << "\n1. Новая игра\n";
            cout << "0. Выход\n";

            int again = readChoice(0, 1);

            if (again == 0) {
                cout << "Спасибо за игру!\n";
                break;
            }
        }
    }
};


int main() {
    try {
        GameManager game;
        game.run();
    }
    catch (const exception& e) {
        cerr << "Произошла ошибка: "
             << e.what() << endl;

        return 1;
    }
    catch (...) {
        cerr << "Произошла неизвестная ошибка!\n";

        return 1;
    }

    return 0;
}
