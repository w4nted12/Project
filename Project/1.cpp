#include <iostream>
#include <vector>

using namespace std;

const int FieldWidth = 40;
const int FieldHeight = 25;
const char HorizontalBorder = '-';
const char VerticalBorder = '|';
const char FillingField = '.';

void DrawField() 
{
    system("cls");

    // ������� �������
    for (int i = 0; i < FieldWidth + 2; i++) 
    {
        cout << HorizontalBorder;
    }
    cout << endl;

    for (int y = 0; y < FieldHeight; y++) 
    {
        cout << VerticalBorder; // ����� �������

        for (int x = 0; x < FieldWidth; x++) 
        {
            cout << FillingField; // ������ ������������
        }

        cout << VerticalBorder << endl; // ������ �������
    }

    // ������ �������
    for (int i = 0; i < FieldWidth + 2; i++) 
    {
        cout << HorizontalBorder;
    }
    cout << endl;
}

int main() 
{
    DrawField();
    return 0;
}