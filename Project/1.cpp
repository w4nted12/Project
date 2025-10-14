#include <iostream>
#include <vector>
#include <random>

using namespace std;

const int FieldWidth = 40;
const int FieldHeight = 25;
const char HorizontalBorder = '-';
const char VerticalBorder = '|';
const char FillingField = '.';
const char AppleSymbol = '6';
const char SnakeHeadSymbol = '0';
const char SnakeBodySymbol = 'O';


// Функция для создания игрового поля в виде вектора векторов
vector<vector<char>> CreateField()
{
    // Создаем поле с учетом границ (высота + 2, ширина + 2)
    vector<vector<char>> field(FieldHeight + 2, vector<char>(FieldWidth + 2, ' '));

    // Заполняем верхнюю и нижнюю границы
    for (int x = 0; x < FieldWidth + 2; x++)
    {
        field[0][x] = HorizontalBorder;                    // Верхняя граница
        field[FieldHeight + 1][x] = HorizontalBorder;      // Нижняя граница
    }

    // Заполняем боковые границы и внутреннее пространство
    for (int y = 1; y <= FieldHeight; y++)
    {
        field[y][0] = VerticalBorder;                      // Левая граница
        field[y][FieldWidth + 1] = VerticalBorder;         // Правая граница

        // Заполняем внутреннее пространство точками
        for (int x = 1; x <= FieldWidth; x++)
        {
            field[y][x] = FillingField;
        }
    }

    return field;
}

// Функция для отрисовки поля из вектора
void DrawField(const vector<vector<char>>& field)
{
    system("cls");

    for (const auto& row : field)
    {
        for (char cell : row)
        {
            cout << cell;
        }
        cout << endl;
    }
}

struct Position
{
    int x;
    int y;
};

int RandomValue(int MinimumValue, int MaximumValue)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(MinimumValue, MaximumValue);
    return dist(gen);
}

Position CreateApple(const vector<Position>& SnakeBody)
{
    Position apple;
    bool ValidPosition = false;
    while (!ValidPosition)
    {
        apple.x = RandomValue(1, FieldWidth);
        apple.y = RandomValue(1, FieldHeight);
        ValidPosition = true;
        for (const auto& segment : SnakeBody)
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
    field[apple.y][apple.x] = AppleSymbol;
}

vector<Position> CreateSnake()
{
    vector<Position> snake;
    Position head;
    head.x = FieldWidth / 2;
    head.y = FieldHeight / 2;
    snake.push_back(head);

    Position body;
    body.x = head.x - 1;
    body.y = head.y;
    snake.push_back(body);

    return snake;
}

void PlaceSnake(vector<vector<char>>& field, const vector<Position>& snake)
{
    if (snake.empty()) return;
    field[snake[0].y][snake[0].x] = SnakeHeadSymbol;

    for (size_t i = 1; i < snake.size(); i++)
    {
        field[snake[i].y][snake[i].x] = SnakeBodySymbol;
    }
}

int main()
{
    auto GameField = CreateField();
    vector<Position> snake = CreateSnake();
    PlaceSnake(GameField, snake);

    PlaceApple(GameField, CreateApple(snake));
    DrawField(GameField);
    return 0;
}