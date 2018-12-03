#include <iostream>
#include <windows.h>
#include <vector>
#include <array>
#include <cstdlib>
#include <ctime>

// Символы живых и мертвых клеток
const char alive = '';
const char dead = ' ';
// Символ который обозначает живую клетку при вводе с клавиатуры
const char keyboard_alive = 'X';

// Считает количество соседей рядом с клеткой pos
int count_neighbors(const std::vector< std::vector<char> > &universe, const std::pair<int, int> &pos);
// Возвращает true если все клетки умерли
bool all_died(const std::vector< std::vector<char> > &universe);
// Возвращает true если предыдущее поколение не отличается от текущего
bool without_changes(const std::vector< std::vector <char> > &current_generation, const std::vector< std::vector<char> > &previous_generation);
// Считает количество живых клеток
int population(const std::vector< std::vector<char> > &universe);
// Заполняет вселенную случайным образом
void random_values(std::vector< std::vector<char> > &universe);
// Заполнение вселенной вручную
void manual_values(std::vector< std::vector<char> > &universe);

int main()
{
    // Ширина и высота вселенной
    int width, height;
    std::cout << "Enter the width of the Universe: ";
    std::cin >> width;
    std::cout << "Enter the height of the Universe: ";
    std::cin >> height;
    // Вселенная на данный момент
    std::vector< std::vector<char> > universe(height, std::vector<char>(width, dead));
    // Предыдущее поколение
    std::vector< std::vector<char> > previous_generation(height, std::vector<char>(width, dead));
    // Генерация начального поколения
    // Выбор режима генерации (ручной, случайный)
    unsigned short mode;
    std::cout << "Enter the initial mode (0 - manual, 1 - random): ";
    std::cin >> mode;
    // Если введено неверное значение, ввести заново
    while(mode > 1)
    {
        std::cout << "Only 0 and 1: ";
        std::cin >> mode;
    }
    enum Mode {MANUAL, RANDOM};
    switch (mode)
    {
        case Mode::MANUAL:
            manual_values(universe);
            break;
        case Mode::RANDOM:
            random_values(universe);
            break;
        default:
            break;
    }
    // Количество шагов ( поколений )
    int steps = 0;
    // Сколько умерло за игру
    int was_died = 0;
    // Сколько было живо за игру
    // Изначально записывается также количество тех кто изначально было жив
    int was_alive = population(universe);
    // Причина проигрыша
    std::string cause_of_loss;
    // Запуск цикла
    system("cls");
    while(true)
    {
        ++steps;
        // Запоминаем предыдущее расположение
        std::vector< std::vector<char> > temp = previous_generation = universe;
        // Изменение вносятся во временную вселенную
        for (int i = 0; i < height; ++i)
        {
            for (int j = 0; j < width; ++j)
            {
                // Может ли здесь родится клетка?
                if (universe[i][j] == dead && (count_neighbors(universe, std::pair<int, int>(i, j)) == 3))
                {
                    ++was_alive;
                    temp[i][j] = alive;
                }
                // Может ли эта клетка умереть?
                else if(universe[i][j] == alive && (count_neighbors(universe, std::pair<int, int>(i, j)) < 2 || count_neighbors(universe, std::pair<int, int>(i, j)) > 3))
                {
                    ++was_died;
                    temp[i][j] = dead;
                }
            }
        }
        // Сохраняем изменения
        universe = temp;
        // Вывод вселенной
        for (const auto &i : universe)
        {
            for (const auto &j : i)
            {
                std::cout << j;
            }
            std::cout << std::endl;
        }
        // Вывод количества живых клеток и количество шагов ( поколений )
        std::cout << "Population = " << population(universe) << '\n';
        std::cout << "Step #" << steps << '\n';
        std::cout << std::endl;
        // Остановить консоль для просмотра
        Sleep(1000);
        system("cls");
        // Определение причины проигрыша при проигрыше
        if(all_died(universe))
        {
            cause_of_loss = "All died";
            break;
        }
        else if(without_changes(universe, previous_generation))
        {
            cause_of_loss = "Without changes";
            break;
        }
    }
    system("cls");
    // Вывод причины проигрыша
    std::cout << cause_of_loss << std::endl;
    // Если причина проигрыша в том что повторилось предыдущее положение
    // То выводим, как оно выглядит
    if(cause_of_loss == "Without changes")
    {
        // Вывод
        for (const auto &i : previous_generation)
        {
            for (const auto &j : i)
            {
                std::cout << j << ' ';
            }
            std::cout << std::endl;
        }
        // Вывод статистики
        std::cout << "Population = " << population(universe) << '\n';
        std::cout << "Was dead = " << was_died << '\n';
    }
    std::cout << "Was alive = " << was_alive << '\n';
    std::cout << "Number of steps = " << steps << '\n';
    return 0;
}

// Считает количество соседей рядом с клеткой pos
int count_neighbors(const std::vector< std::vector<char> > &universe, const std::pair<int, int> &pos)
{
    // Счетчик живых соседей
    int count = 0;
    // Размеры пространства
    int width = universe[0].size();
    int height = universe.size();
    // Проверка всех соседей
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            // Пропустить текущую позицию
            if (i == 0 && j == 0)
            {
                continue;
            }
            // Координаты соседа
            std::pair<int, int> neighbor(pos.first + i, pos.second + j);
            // Если выход за границу, начинаем с другой стороны
            neighbor.first = (neighbor.first == height) ? 0 : neighbor.first;
            neighbor.first = (neighbor.first < 0) ? height - 1 : neighbor.first;
            neighbor.second = (neighbor.second == width) ? 0 : neighbor.second;
            neighbor.second = (neighbor.second < 0) ? width - 1 : neighbor.second;
            // +1 если клетка живая
            count += (universe[neighbor.first][neighbor.second] == alive) ? 1 : 0;
        }
    }
    return count;
}

// Возвращает true если все клетки умерли
bool all_died(const std::vector< std::vector<char> > &universe)
{
    for (const auto &i : universe)
    {
        for (const auto &j : i)
        {
            // Если хоть одна клетка жива, значит умерли не все
            if (j == alive)
            {
                return false;
            }
        }
    }
    // Все клетки мерты
    return true;
}

// Возвращает true если предыдущее поколение не отличается от текущего
bool without_changes(const std::vector< std::vector <char> > &current_generation, const std::vector< std::vector<char> > &previous_generation)
{
    return (current_generation == previous_generation);
}

// Считает количество живых клеток
int population(const std::vector< std::vector<char> > &universe)
{
    int count = 0;
    for (const auto &i : universe)
    {
        for (const auto &j : i)
        {
            // +1 если клетка живая
            count += (j == alive) ? 1 : 0;
        }
    }
    return count;
}

// Заполняет вселенную случайным образом
void random_values(std::vector< std::vector<char> > &universe)
{
    int width = universe[0].size();
    int height = universe.size();
    // Случайные значения
    std::cout << "Enter chance percentage: ";
    double percentage;
    std::cin >> percentage;
    srand(time(nullptr));
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            if((rand() % 100 + 1) <= percentage)
            {
                universe[i][j] = alive;
            }
        }
    }
}

// Заполнение вселенной вручную
void manual_values(std::vector< std::vector<char> > &universe)
{
    int width = universe[0].size();
    int height = universe.size();
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            char input;
            std::cin >> input;
            if(input == keyboard_alive)
            {
                universe[i][j] = alive;
            }
            else
            {
                universe[i][j] = dead;
            }
        }
    }
}