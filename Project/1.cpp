#include <iostream>
#include <vector>
#include <random>
#include <conio.h>
#include <windows.h>
#include <string>
#include <algorithm>

const int FIELD_WIDTH = 10;
const int FIELD_HEIGHT = 6;
const int MINIMUM_WIDTH_FIELD_SIZE = 10;
const int MINIMUM_HEIGHT_FIELD_SIZE = 4;
const char HORIZONTAL_BORDER = '-';
const char VERTICAL_BORDER = '|';
const char FILLING_FIELD = '.';
const char APPLE_SYMBOL = '@';
const char SNAKE_HEAD_SYMBOL = '0';
const char SNAKE_BODY_SYMBOL = 'o';

const char* ERROR_TOO_SMALL_FIELD = "Error: field size too small!";
const char* CONTROLS_STRING = "WASD - move | R - restart | ESC - exit";
const char* START_MESSAGE = "Press any key to start...";
const char* RESTART_EXIT_STRING = "R - restart | ESC - exit";

const char* GAME_OVER_TITLE = "GAME OVER!";
const char* VICTORY_TITLE = "VICTORY!";
const char* PERFECT_SCORE_STRING = "PERFECT SCORE!";
const char* FINAL_LENGTH_STRING = "Final length: ";
const char* PERFECT_SCORE_LABEL = "Perfect score: ";
const char* LENGTH_LABEL = "Length: ";

enum class Direction { UP, DOWN, LEFT, RIGHT };
enum class GameState { PLAYING, GAME_OVER, VICTORY };

struct Position
{
    int x;
    int y;
};

struct GameData
{
    std::vector<std::vector<char>> field;
    std::vector<Position> snake;
    Position apple;
    Direction currentDirection;
    bool appleEaten;
    GameState state;
};

void ClearConsole()
{
    system("cls");
}

void MoveCursorToHome()
{
    std::cout << "\033[H";
}

std::vector<std::vector<char>> CreateField()
{
    if (FIELD_WIDTH < MINIMUM_WIDTH_FIELD_SIZE || FIELD_HEIGHT < MINIMUM_HEIGHT_FIELD_SIZE)
    {
        std::cerr << ERROR_TOO_SMALL_FIELD << std::endl;
        exit(1);
    }

    std::vector<std::vector<char>> field(FIELD_HEIGHT + 2, std::vector<char>(FIELD_WIDTH + 2, ' '));

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

void DrawGameInfo(int snakeLength, GameState gameState)
{
    switch (gameState)
    {
    case GameState::PLAYING:
        std::cout << LENGTH_LABEL << snakeLength << "\n";
        std::cout << CONTROLS_STRING << "\n";
        break;

    case GameState::GAME_OVER:
        std::cout << GAME_OVER_TITLE << "\n";
        std::cout << FINAL_LENGTH_STRING << snakeLength << "\n";
        std::cout << RESTART_EXIT_STRING << "\n";
        break;

    case GameState::VICTORY:
        std::cout << VICTORY_TITLE << "\n";
        std::cout << PERFECT_SCORE_LABEL << snakeLength << "\n";
        std::cout << RESTART_EXIT_STRING << "\n";
        break;
    }
}

void DrawField(const std::vector<std::vector<char>>& field, int snakeLength, GameState gameState = GameState::PLAYING)
{
    MoveCursorToHome();

    for (const auto& row : field)
    {
        for (char cell : row)
        {
            std::cout << cell;
        }
        std::cout << "\n";
    }

    DrawGameInfo(snakeLength, gameState);
}

void DrawCenteredTextOnField(std::vector<std::vector<char>>& field, int row, const std::string& text)
{
    int startX = (FIELD_WIDTH - static_cast<int>(text.length())) / 2 + 1;

    for (std::size_t i = 0; i < text.length() && startX + static_cast<int>(i) <= FIELD_WIDTH; i++)
    {
        field[row][startX + i] = text[i];
    }
}

void ClearFieldContent(std::vector<std::vector<char>>& field)
{
    for (int y = 1; y <= FIELD_HEIGHT; y++)
    {
        for (int x = 1; x <= FIELD_WIDTH; x++)
        {
            field[y][x] = FILLING_FIELD;
        }
    }
}

void ShowGameOverOnField(std::vector<std::vector<char>>& field, int snakeLength, bool victory = false)
{
    ClearConsole();
    ClearFieldContent(field);

    std::string mainMessage = victory ? VICTORY_TITLE : GAME_OVER_TITLE;
    std::string lengthMessage = LENGTH_LABEL + std::to_string(snakeLength);

    int centerRow = FIELD_HEIGHT / 2;

    DrawCenteredTextOnField(field, centerRow, mainMessage);

    if (victory)
    {
        DrawCenteredTextOnField(field, centerRow + 1, PERFECT_SCORE_STRING);
        DrawCenteredTextOnField(field, centerRow + 3, lengthMessage);
    }
    else
    {
        DrawCenteredTextOnField(field, centerRow + 2, lengthMessage);
    }
}

int RandomValue(int min, int max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

Position CreateApple(const std::vector<Position>& snakeBody)
{
    Position apple;
    bool validPosition = false;
    int attempts = 0;
    const int MAX_ATTEMPTS = 100;

    while (!validPosition && attempts < MAX_ATTEMPTS)
    {
        attempts++;
        apple.x = RandomValue(1, FIELD_WIDTH);
        apple.y = RandomValue(1, FIELD_HEIGHT);

        validPosition = true;
        for (const auto& segment : snakeBody)
        {
            if (segment.x == apple.x && segment.y == apple.y)
            {
                validPosition = false;
                break;
            }
        }
    }

    return apple;
}

void PlaceApple(std::vector<std::vector<char>>& field, const Position& apple)
{
    if (apple.y >= 0 && apple.y < static_cast<int>(field.size()) &&
        apple.x >= 0 && apple.x < static_cast<int>(field[apple.y].size()))
    {
        field[apple.y][apple.x] = APPLE_SYMBOL;
    }
}

bool CheckVictory(const std::vector<Position>& snake)
{
    int maxPossibleLength = FIELD_WIDTH * FIELD_HEIGHT;
    return static_cast<int>(snake.size()) >= maxPossibleLength;
}

std::vector<Position> CreateSnake()
{
    std::vector<Position> snake;

    int headX = FIELD_WIDTH / 2;
    if (headX < 2) headX = 2;
    if (headX > FIELD_WIDTH - 1) headX = FIELD_WIDTH - 1;

    int headY = FIELD_HEIGHT / 2;
    if (headY < 2) headY = 2;
    if (headY > FIELD_HEIGHT - 1) headY = FIELD_HEIGHT - 1;

    snake.push_back({ headX, headY });  

    if (headX - 1 >= 1)
    {
        snake.push_back({ headX - 1, headY });  
    }
    else if (headY - 1 >= 1)
    {
        snake.push_back({ headX, headY - 1 });  
    }

    return snake;
}

void PlaceSnake(std::vector<std::vector<char>>& field, const std::vector<Position>& snake)
{
    if (snake.empty()) return;

    if (snake[0].x >= 1 && snake[0].x <= FIELD_WIDTH &&
        snake[0].y >= 1 && snake[0].y <= FIELD_HEIGHT)
    {
        field[snake[0].y][snake[0].x] = SNAKE_HEAD_SYMBOL;
    }

    for (std::size_t i = 1; i < snake.size(); i++)
    {
        if (snake[i].x >= 1 && snake[i].x <= FIELD_WIDTH &&
            snake[i].y >= 1 && snake[i].y <= FIELD_HEIGHT)
        {
            field[snake[i].y][snake[i].x] = SNAKE_BODY_SYMBOL;
        }
    }
}

bool MoveSnake(std::vector<Position>& snake, Direction direction, Position* apple = nullptr, bool* appleEaten = nullptr)
{
    if (snake.empty()) return false;

    Position newHead = snake.front(); 
    switch (direction)
    {
    case Direction::UP:    newHead.y -= 1; break;
    case Direction::DOWN:  newHead.y += 1; break;
    case Direction::LEFT:  newHead.x -= 1; break;
    case Direction::RIGHT: newHead.x += 1; break;
    }

    if (newHead.x < 1 || newHead.x > FIELD_WIDTH ||
        newHead.y < 1 || newHead.y > FIELD_HEIGHT)
    {
        return false;
    }

    for (std::size_t i = 0; i < snake.size() - 1; i++) 
    {
        if (snake[i].x == newHead.x && snake[i].y == newHead.y)
        {
            return false;
        }
    }

    bool wasAppleEaten = false;
    if (apple != nullptr)
    {
        wasAppleEaten = (newHead.x == apple->x && newHead.y == apple->y);
    }
    snake.insert(snake.begin(), newHead);

    if (!wasAppleEaten)
    {
        snake.pop_back();
    }
    else if (appleEaten != nullptr)
    {
        *appleEaten = true;
    }

    return true;
}

Direction ProcessInput(Direction currentDirection, bool& restart, bool& exitGame)
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

void ShowStartMessage()
{
    std::cout << START_MESSAGE << std::endl;
}

GameData InitializeGame()
{
    GameData data;
    data.field = CreateField();
    data.snake = CreateSnake();
    data.apple = CreateApple(data.snake);
    data.currentDirection = Direction::RIGHT;
    data.appleEaten = false;
    data.state = GameState::PLAYING;

    PlaceSnake(data.field, data.snake);
    PlaceApple(data.field, data.apple);

    return data;
}

void ShowGameStart(GameData& data)
{
    DrawField(data.field, static_cast<int>(data.snake.size()));
    ShowStartMessage();
    _getch();
    ClearConsole();
    DrawField(data.field, static_cast<int>(data.snake.size()));
}

bool HandleGameOverInput()
{
    if (_kbhit())
    {
        char key = _getch();
        if (key == 'r' || key == 'R')
        {
            return true; // Рестарт
        }
        else if (key == 27) // ESC
        {
            exit(0); // Выход
        }
    }
    return false;
}

bool ProcessGameOver(GameData& data)
{
    if (data.state == GameState::GAME_OVER || data.state == GameState::VICTORY)
    {
        return HandleGameOverInput();
    }
    return false;
}

bool ProcessGamePlaying(GameData& data)
{
    if (!MoveSnake(data.snake, data.currentDirection, &data.apple, &data.appleEaten))
    {
        data.state = GameState::GAME_OVER;
        ShowGameOverOnField(data.field, static_cast<int>(data.snake.size()), false);
        DrawField(data.field, static_cast<int>(data.snake.size()), data.state);
        return true;
    }

    if (CheckVictory(data.snake))
    {
        data.state = GameState::VICTORY;
        ShowGameOverOnField(data.field, static_cast<int>(data.snake.size()), true);
        DrawField(data.field, static_cast<int>(data.snake.size()), data.state);
        return true;
    }

    if (data.appleEaten)
    {
        data.apple = CreateApple(data.snake);
        data.appleEaten = false;
    }

    data.field = CreateField();
    PlaceSnake(data.field, data.snake);
    PlaceApple(data.field, data.apple);
    DrawField(data.field, static_cast<int>(data.snake.size()), data.state);

    return false;
}

void GameLoop()
{
    GameData gameData = InitializeGame();
    ShowGameStart(gameData);

    bool restart = false;
    bool exitGame = false;

    while (true)
    {
        gameData.currentDirection = ProcessInput(gameData.currentDirection, restart, exitGame);

        if (exitGame) break;
        if (restart)
        {
            GameLoop();
            return;
        }

        if (gameData.state == GameState::PLAYING)
        {
            if (ProcessGamePlaying(gameData))
            {
                continue;
            }
            Sleep(300);
        }
        else
        {
            if (ProcessGameOver(gameData))
            {
                GameLoop();
                return;
            }
        }
    }
}

int main()
{
    GameLoop();
    return 0;
}