#include <iostream>
#include <vector>
#include <random>
#include <conio.h>
#include <windows.h>
#include <string>

using namespace std;

const int FIELD_WIDTH = 9;
const int FIELD_HEIGHT = 6;
const char HORIZONTAL_BORDER = '-';
const char VERTICAL_BORDER = '|';
const char FILLING_FIELD = '.';
const char APPLE_SYMBOL = '@';
const char SNAKE_HEAD_SYMBOL = '0';
const char SNAKE_BODY_SYMBOL = 'o';
const char* CONTROLS = "WASD - move | R - restart | ESC - exit";
const char* 

enum class Direction { UP, DOWN, LEFT, RIGHT };
enum class GameState { PLAYING, GAME_OVER, VICTORY };

struct Position
{
    int x;
    int y;
};

//  ФУНКЦИИ ПОЛЯ 

vector<vector<char>> CreateField()
{
    if (FIELD_WIDTH < 5 || FIELD_HEIGHT < 5) 
    {
        cerr << "Error: field size too small!" << endl;
        exit(1);
    }

    vector<vector<char>> field(FIELD_HEIGHT + 2, vector<char>(FIELD_WIDTH + 2, ' '));

    for (int x = 0; x < FIELD_WIDTH + 2; x++)
    {
        field[0][x] = HORIZONTAL_BORDER;
        field[FIELD_HEIGHT + 1][x] = HORIZONTAL_BORDER;
    }

    for (int y = 1; y <= FIELD_HEIGHT; y++)
    {
        field[y][0] = VERTICAL_BORDER;
        field[y][FIELD_WIDTH + 1] = VERTICAL_BORDER;

        for (int x = 1; x <= FIELD_WIDTH; x++)
        {
            field[y][x] = FILLING_FIELD;
        }
    }

    return field;
}

void DrawField(const vector<vector<char>>& field, int snakeLength, GameState gameState = GameState::PLAYING)
{
    cout << "\033[H";
 
    for (const auto& row : field)
    {
        for (char cell : row)
        {
            cout << cell;
        }
        cout << "\n";
    }

    // Надписи снизу
    switch (gameState)
    {
    case GameState::PLAYING:
        cout << "Length: " << snakeLength << "\n";
        cout << CONTROLS << "\n";
        break;

    case GameState::GAME_OVER:
        cout << "GAME OVER!" << "\n";
        cout << "Final length: " << snakeLength << "\n";
        cout << "R - restart | ESC - exit" << "\n";
        break;

    case GameState::VICTORY:
        cout << "VICTORY!" << "\n";
        cout << "Perfect score: " << snakeLength << "\n";
        cout << "R - restart | ESC - exit" << "\n";
        break;
    }

    
}

// Функция для проверки победы
bool CheckVictory(const vector<Position>& snake)
{
    // Максимально возможная длина змейки = площадь поля
    int maxPossibleLength = FIELD_WIDTH * FIELD_HEIGHT;
    return snake.size() >= maxPossibleLength;
}

// Функция для отображения Game Over внутри поля
void ShowGameOverOnField(vector<vector<char>>& field, int snakeLength, bool victory = false)
{
    system("cls");
    for (int y = 1; y <= FIELD_HEIGHT; y++)
    {
        for (int x = 1; x <= FIELD_WIDTH; x++)
        {
            field[y][x] = FILLING_FIELD;
        }
    }

    string mainMsg = victory ? "VICTORY!" : "GAME OVER";
    int msgX = (FIELD_WIDTH - mainMsg.length()) / 2 + 1;
    int msgY = FIELD_HEIGHT / 2;

    for (size_t i = 0; i < mainMsg.length(); i++)
    {
        if (msgX + i <= FIELD_WIDTH)
        {
            field[msgY][msgX + i] = mainMsg[i];
        }
    }

    // Сообщение с длиной змейки
    string lengthMsg = "Length: " + to_string(snakeLength);
    int lengthX = (FIELD_WIDTH - lengthMsg.length()) / 2 + 1;
    int lengthY = msgY + 2;

    for (size_t i = 0; i < lengthMsg.length(); i++)
    {
        if (lengthX + i <= FIELD_WIDTH)
        {
            field[lengthY][lengthX + i] = lengthMsg[i];
        }
    }

    // Дополнительное сообщение для победы
    if (victory)
    {
        string perfectMsg = "PERFECT SCORE!";
        int perfectX = (FIELD_WIDTH - perfectMsg.length()) / 2 + 1;
        int perfectY = msgY + 1;

        for (size_t i = 0; i < perfectMsg.length(); i++)
        {
            if (perfectX + i <= FIELD_WIDTH)
            {
                field[perfectY][perfectX + i] = perfectMsg[i];
            }
        }
    }
}

// ФУНКЦИИ ЯБЛОКА 

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
        apple.x = RandomValue(1, FIELD_WIDTH);
        apple.y = RandomValue(1, FIELD_HEIGHT);

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
        field[apple.y][apple.x] = APPLE_SYMBOL;
    }
}

// ФУНКЦИИ ЗМЕЙКИ 

vector<Position> CreateSnake()
{
    vector<Position> snake;

    Position head;
    head.x = max(2, min(FIELD_WIDTH - 1, FIELD_WIDTH / 2));
    head.y = max(2, min(FIELD_HEIGHT - 1, FIELD_HEIGHT / 2));
    snake.push_back(head);

    Position body;
    body.x = head.x - 1;
    body.y = head.y;

    if (body.x >= 1 && body.x <= FIELD_WIDTH && body.y >= 1 && body.y <= FIELD_HEIGHT)
    {
        snake.push_back(body);
    }

    return snake;
}

void PlaceSnake(vector<vector<char>>& field, const vector<Position>& snake)
{
    if (snake.empty()) return;

    if (snake[0].x >= 1 && snake[0].x <= FIELD_WIDTH &&
        snake[0].y >= 1 && snake[0].y <= FIELD_HEIGHT)
    {
        field[snake[0].y][snake[0].x] = SNAKE_HEAD_SYMBOL;
    }

    for (size_t i = 1; i < snake.size(); i++)
    {
        if (snake[i].x >= 1 && snake[i].x <= FIELD_WIDTH &&
            snake[i].y >= 1 && snake[i].y <= FIELD_HEIGHT)
        {
            field[snake[i].y][snake[i].x] = SNAKE_BODY_SYMBOL;
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
    if (newHead.x < 1 || newHead.x > FIELD_WIDTH ||
        newHead.y < 1 || newHead.y > FIELD_HEIGHT)
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

// УПРАВЛЕНИЕ 

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
    system("cls");
    DrawField(gameField, static_cast<int>(snake.size()));
    while (true)
    {
        bool restart = false;
        bool exitGame = false;

        // Обработка ввода
        currentDirection = GetInput(currentDirection, restart, exitGame);

        if (exitGame) 
        {
            break;
        }

        if (restart) 
        {
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
            if (_kbhit()) 
            {
                char key = _getch();
                if (key == 'r' || key == 'R') 
                {
                    GameLoop();
                    return;
                }
                else if (key == 27)  //ESC 
                { 
                    break;
                }
            }
        }

        
        if (gameState == GameState::PLAYING) 
        {
            Sleep(300);
        }
    }
}

int main()
{
    GameLoop();
    return 0;
}