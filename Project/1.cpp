#include <iostream>
#include <vector>
#include <random>
#include <conio.h>
#include <windows.h>
#include <string>

using namespace std;

const int FieldWidth = 8;
const int FieldHeight = 6;
const char HorizontalBorder = '-';
const char VerticalBorder = '|';
const char FillingField = '.';
const char AppleSymbol = '@';
const char SnakeHeadSymbol = '0';
const char SnakeBodySymbol = 'o';

enum class Direction { UP, DOWN, LEFT, RIGHT };
enum class GameState { PLAYING, GAME_OVER, VICTORY };

struct Position
{
    int x;
    int y;
};

// ==================== ФУНКЦИИ ПОЛЯ ====================

vector<vector<char>> CreateField()
{
    if (FieldWidth < 5 || FieldHeight < 5) {
        cerr << "Error: field size too small!" << endl;
        exit(1);
    }

    vector<vector<char>> field(FieldHeight + 2, vector<char>(FieldWidth + 2, ' '));

    for (int x = 0; x < FieldWidth + 2; x++)
    {
        field[0][x] = HorizontalBorder;
        field[FieldHeight + 1][x] = HorizontalBorder;
    }

    for (int y = 1; y <= FieldHeight; y++)
    {
        field[y][0] = VerticalBorder;
        field[y][FieldWidth + 1] = VerticalBorder;

        for (int x = 1; x <= FieldWidth; x++)
        {
            field[y][x] = FillingField;
        }
    }

    return field;
}

void DrawField(const vector<vector<char>>& field, int snakeLength, GameState gameState = GameState::PLAYING)
{
    system("cls");

    // Рисуем игровое поле
    for (const auto& row : field)
    {
        for (char cell : row)
        {
            cout << cell;
        }
        cout << endl;
    }

    // Надписи снизу
    switch (gameState)
    {
    case GameState::PLAYING:
        cout << "Length: " << snakeLength << endl;
        cout << "WASD - move | R - restart | ESC - exit" << endl;
        break;

    case GameState::GAME_OVER:
        cout << "=== GAME OVER ===" << endl;
        cout << "Final length: " << snakeLength << endl;
        cout << "R - restart | ESC - exit" << endl;
        break;

    case GameState::VICTORY:
        cout << "=== VICTORY! ===" << endl;
        cout << "Perfect score: " << snakeLength << endl;
        cout << "R - restart | ESC - exit" << endl;
        break;
    }
}

// Функция для проверки победы
bool CheckVictory(const vector<Position>& snake)
{
    // Максимально возможная длина змейки = площадь поля
    int maxPossibleLength = FieldWidth * FieldHeight;
    return snake.size() >= maxPossibleLength;
}

// Функция для отображения Game Over внутри поля
void ShowGameOverOnField(vector<vector<char>>& field, int snakeLength, bool victory = false)
{
    // Очищаем поле от змейки и яблока, оставляем только точки
    for (int y = 1; y <= FieldHeight; y++)
    {
        for (int x = 1; x <= FieldWidth; x++)
        {
            field[y][x] = FillingField;
        }
    }

    string mainMsg = victory ? "VICTORY!" : "GAME OVER";
    int msgX = (FieldWidth - mainMsg.length()) / 2 + 1;
    int msgY = FieldHeight / 2;

    for (size_t i = 0; i < mainMsg.length(); i++)
    {
        if (msgX + i <= FieldWidth)
        {
            field[msgY][msgX + i] = mainMsg[i];
        }
    }

    // Сообщение с длиной змейки
    string lengthMsg = "Length: " + to_string(snakeLength);
    int lengthX = (FieldWidth - lengthMsg.length()) / 2 + 1;
    int lengthY = msgY + 2;

    for (size_t i = 0; i < lengthMsg.length(); i++)
    {
        if (lengthX + i <= FieldWidth)
        {
            field[lengthY][lengthX + i] = lengthMsg[i];
        }
    }

    // Дополнительное сообщение для победы
    if (victory)
    {
        string perfectMsg = "PERFECT SCORE!";
        int perfectX = (FieldWidth - perfectMsg.length()) / 2 + 1;
        int perfectY = msgY + 1;

        for (size_t i = 0; i < perfectMsg.length(); i++)
        {
            if (perfectX + i <= FieldWidth)
            {
                field[perfectY][perfectX + i] = perfectMsg[i];
            }
        }
    }
}

// ==================== ФУНКЦИИ ЯБЛОКА ====================

int RandomValue(int min, int max)
{
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

Position CreateApple(const vector<Position>& snakeBody)
{
    Position apple;
    bool ValidPosition = false;
    int attempts = 0;
    const int maxAttempts = 100;

    while (!ValidPosition && attempts < maxAttempts)
    {
        attempts++;
        apple.x = RandomValue(1, FieldWidth);
        apple.y = RandomValue(1, FieldHeight);

        ValidPosition = true;
        for (const auto& segment : snakeBody)
        {
            if (segment.x == apple.x && segment.y == apple.y)
            {
                ValidPosition = false;
                break;
            }
        }
    }

    return apple;
}

void PlaceApple(vector<vector<char>>& field, const Position& apple)
{
    if (apple.y >= 0 && apple.y < field.size() &&
        apple.x >= 0 && apple.x < field[apple.y].size())
    {
        field[apple.y][apple.x] = AppleSymbol;
    }
}

// ==================== ФУНКЦИИ ЗМЕЙКИ ====================

vector<Position> CreateSnake()
{
    vector<Position> snake;

    Position head;
    head.x = max(2, min(FieldWidth - 1, FieldWidth / 2));
    head.y = max(2, min(FieldHeight - 1, FieldHeight / 2));
    snake.push_back(head);

    Position body;
    body.x = head.x - 1;
    body.y = head.y;

    if (body.x >= 1 && body.x <= FieldWidth && body.y >= 1 && body.y <= FieldHeight)
    {
        snake.push_back(body);
    }

    return snake;
}

void PlaceSnake(vector<vector<char>>& field, const vector<Position>& snake)
{
    if (snake.empty()) return;

    if (snake[0].x >= 1 && snake[0].x <= FieldWidth &&
        snake[0].y >= 1 && snake[0].y <= FieldHeight)
    {
        field[snake[0].y][snake[0].x] = SnakeHeadSymbol;
    }

    for (size_t i = 1; i < snake.size(); i++)
    {
        if (snake[i].x >= 1 && snake[i].x <= FieldWidth &&
            snake[i].y >= 1 && snake[i].y <= FieldHeight)
        {
            field[snake[i].y][snake[i].x] = SnakeBodySymbol;
        }
    }
}

bool MoveSnake(vector<Position>& snake, Direction direction, Position* apple = nullptr, bool* appleEaten = nullptr)
{
    if (snake.empty()) return false;

    Position newHead = snake[0];

    switch (direction)
    {
    case Direction::UP:    newHead.y -= 1; break;
    case Direction::DOWN:  newHead.y += 1; break;
    case Direction::LEFT:  newHead.x -= 1; break;
    case Direction::RIGHT: newHead.x += 1; break;
    }

    // Проверка границ
    if (newHead.x < 1 || newHead.x > FieldWidth ||
        newHead.y < 1 || newHead.y > FieldHeight)
    {
        return false;
    }

    // Проверка столкновения с собой
    for (size_t i = 1; i < snake.size(); i++)
    {
        if (snake[i].x == newHead.x && snake[i].y == newHead.y)
        {
            return false;
        }
    }

    // Проверка поедания яблока
    bool wasAppleEaten = false;
    if (apple != nullptr)
    {
        wasAppleEaten = (newHead.x == apple->x && newHead.y == apple->y);

        if (wasAppleEaten && appleEaten != nullptr)
        {
            *appleEaten = true;
            snake.push_back(snake.back());
        }
    }

    // Движение тела
    for (int i = static_cast<int>(snake.size()) - 1; i > 0; i--)
    {
        snake[i] = snake[i - 1];
    }

    snake[0] = newHead;
    return true;
}

// ==================== УПРАВЛЕНИЕ И ИГРОВАЯ ЛОГИКА ====================

Direction GetInput(Direction currentDirection, bool& restart, bool& exitGame)
{
    if (_kbhit())
    {
        char key = _getch();

        switch (key)
        {
        case 'w': case 'W':
            if (currentDirection != Direction::DOWN)
                return Direction::UP;
            break;

        case 's': case 'S':
            if (currentDirection != Direction::UP)
                return Direction::DOWN;
            break;

        case 'a': case 'A':
            if (currentDirection != Direction::RIGHT)
                return Direction::LEFT;
            break;

        case 'd': case 'D':
            if (currentDirection != Direction::LEFT)
                return Direction::RIGHT;
            break;

        case 'r': case 'R':
            restart = true;
            break;

        case 27: // ESC
            exitGame = true;
            break;
        }
    }

    return currentDirection;
}

void GameLoop()
{
    vector<vector<char>> gameField = CreateField();
    vector<Position> snake = CreateSnake();
    Position apple = CreateApple(snake);

    Direction currentDirection = Direction::RIGHT;
    bool appleEaten = false;
    GameState gameState = GameState::PLAYING;

    // Начальная отрисовка
    PlaceSnake(gameField, snake);
    PlaceApple(gameField, apple);
    DrawField(gameField, static_cast<int>(snake.size()));
    cout << "Press any key to start..." << endl;
    _getch();

    // Игровой цикл
    while (true)
    {
        bool restart = false;
        bool exitGame = false;

        // Обработка ввода
        currentDirection = GetInput(currentDirection, restart, exitGame);

        if (exitGame) {
            break;
        }

        if (restart) {
            GameLoop(); // Перезапуск игры
            return;
        }

        if (gameState == GameState::PLAYING)
        {
            // Движение змейки
            if (!MoveSnake(snake, currentDirection, &apple, &appleEaten))
            {
                gameState = GameState::GAME_OVER;
                ShowGameOverOnField(gameField, static_cast<int>(snake.size()), false);
                DrawField(gameField, static_cast<int>(snake.size()), gameState);
            }
            else
            {
                // Проверка победы
                if (CheckVictory(snake))
                {
                    gameState = GameState::VICTORY;
                    ShowGameOverOnField(gameField, static_cast<int>(snake.size()), true);
                    DrawField(gameField, static_cast<int>(snake.size()), gameState);
                }
                else
                {
                    // Обработка яблока
                    if (appleEaten)
                    {
                        apple = CreateApple(snake);
                        appleEaten = false;
                    }

                    // Обновление и отрисовка поля
                    gameField = CreateField();
                    PlaceSnake(gameField, snake);
                    PlaceApple(gameField, apple);
                    DrawField(gameField, static_cast<int>(snake.size()), gameState);
                }
            }
        }
        else
        {
            // Если игра окончена или победа, поле больше не обновляется
            // Обрабатываем только рестарт и выход
            if (_kbhit()) {
                char key = _getch();
                if (key == 'r' || key == 'R') {
                    GameLoop();
                    return;
                }
                else if (key == 27) { // ESC
                    break;
                }
            }
        }

        // Задержка только если игра не окончена
        if (gameState == GameState::PLAYING) {
            Sleep(200);
        }
    }
}

int main()
{
    GameLoop();
    return 0;
}