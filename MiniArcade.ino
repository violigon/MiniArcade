#include <U8glib.h>


U8GLIB_ST7920_128X64_1X display(6, 5, 4, 7);

void PrepareDisplay()
{
    display.setFont(u8g_font_6x10);
    display.setFontRefHeightExtendedText();
    display.setDefaultForegroundColor();
    display.setFontPosTop();
}

void Draw(int collumn, int row, int value)
{
    PrepareDisplay();

    char converted[16];
    itoa(value, converted, 10);

    const char constant[16] = {&converted};

    display.setFont(u8g_font_unifont);
    display.drawStr(collumn, row, converted);
}

void Draw(int collumn, int row, String value)
{
    PrepareDisplay();

    char converted[50];

    value.toCharArray(converted, 50);

    display.setFont(u8g_font_unifont);
    display.drawStr(collumn, row, converted);
}

void Draw(int collumn, int row, char value[], bool typing = false)
{
    PrepareDisplay();

    display.setFont(u8g_font_unifont);
    display.drawStr(collumn, row, value);
}

void Draw(int collumn, int row, char value)
{
    PrepareDisplay();

    display.setFont(u8g_font_unifont);
    display.drawStr(collumn, row, value);
}

void Draw(int collumn, int row, bool value)
{
    PrepareDisplay();

    display.setFont(u8g_font_unifont);
    display.drawStr(collumn, row, value ? "true" : "false");
}

void ClearDisplay()
{
}

struct Button
{
  private:
    int input;
    bool pressed = false;

  public:
    Button(int _input)
    {
        input = _input;
        pinMode(input, INPUT);
    };

    bool IsPressed()
    {
        const int buttonState = digitalRead(input);
        bool buttonPressed = buttonState == HIGH;
        bool wasPressing = pressed;

        pressed = buttonPressed;

        return buttonPressed && !wasPressing;
    };
};

Button acceptButton = Button(11);
Button denyButton = Button(10);
Button upArrowButton = Button(9);
Button downArrowButton = Button(8);

struct Operation
{
  private:
    int result;
    int firstNumber;
    int secondNumber;
    char type;

    int answer;
    bool correct;

  public:
    Operation(int first, int second, int opNumber)
    {
        firstNumber = first;
        secondNumber = second;

        switch (opNumber)
        {
        case 1:
            result = first + second;
            type = '+';
            break;
        case 2:
            result = first - second;
            type = '-';
            break;
        case 3:
            result = first * second;
            type = '*';
            break;
        }
    }

    int First()
    {
        return firstNumber;
    }

    int Second()
    {
        return secondNumber;
    }

    int ResultLength()
    {
        return String(result).length();
    }

    char GetType()
    {
        return type;
    }

    bool Validate(int response)
    {
        answer = response;
        correct = response == result;

        return correct;
    }

    bool IsCorrect()
    {
        return correct;
    }
};
const int operationsLength = 10;
struct Operation operations[operationsLength] = {
    Operation(0, 0, 1),
    Operation(0, 0, 1),
    Operation(0, 0, 1),
    Operation(0, 0, 1),
    Operation(0, 0, 1),
    Operation(0, 0, 1),
    Operation(0, 0, 1),
    Operation(0, 0, 1),
    Operation(0, 0, 1),
    Operation(0, 0, 1)};

int dificulty = 2;

bool gameRunning = false;

void StartGame()
{
    ClearDisplay();

    Draw(3, 0, "Arcadinho");
    Draw(0, 1, "Modulo");
    Draw(7, 1, "Fiap School");

    delay(2000);

    for (int i = 0; i < operationsLength; i++)
    {
        int type = random(1, 4);
        int first = random(1, pow(10, dificulty));
        int second = random(0, type == 2 ? first : pow(10, dificulty));

        struct Operation operation = Operation(first, second, type);
        operations[i] = operation;
    }

    gameRunning = true;
}

void setup()
{

    StartGame();
}

bool operationDrawn = false;

void DrawOperation(struct Operation operation)
{
    ClearDisplay();

    int firstCollumn = 15 - String(operation.First()).length();
    int secondCollumn = 15 - String(operation.Second()).length();

    Draw(0, 0, operation.First());
    Draw(secondCollumn, 0, operation.Second());
    Draw(8, 0, operation.GetType());

    operationDrawn = true;
}

void ShowOperationResultScreen(bool correct)
{
    ClearDisplay();

    if (correct)
    {
        Draw(2, 1, "Muito bem! :)");
    }
    else
    {
        Draw(3, 1, "Que pena :(");
    }
}

void ShowResultScreen()
{
    ClearDisplay();

    Draw(3, 0, "Resultado");

    for (int i = 0; i < operationsLength; i++)
    {
        bool correct = operations[i].IsCorrect();
        Draw(3 + i, 1, correct ? 'O' : 'X');
    }

    delay(5000);

    EndGame();
}

int resultIndex = 0;
int operationIndex = 0;

void NextOperation()
{
    operationIndex++;
    resultIndex = 0;
    operationDrawn = false;
}

String showedResult = "000000";

void CaptureInput()
{
    Operation *operation = &operations[operationIndex];

    if (upArrowButton.IsPressed())
    {
        int number = int(showedResult.charAt(resultIndex) - '0');

        if (number + 1 > 9)
        {
            number = 0;
        }
        else
        {
            number++;
        }

        showedResult[resultIndex] = String(number)[0];
    }

    if (downArrowButton.IsPressed())
    {
        int number = int(showedResult.charAt(resultIndex) - '0');

        if (number - 1 < 0)
        {
            number = 9;
        }
        else
        {
            number--;
        }

        showedResult[resultIndex] = String(number)[0];
    }

    if (denyButton.IsPressed())
    {
        if (resultIndex > 0)
            resultIndex--;
    }

    if (acceptButton.IsPressed())
    {
        if (++resultIndex >= showedResult.length())
        {
            bool correct = operation->Validate(showedResult.toInt());
            ShowOperationResultScreen(correct);
            delay(2000);

            if (operationIndex >= operationsLength - 1)
            {
                ShowResultScreen();
                delay(5000);
            }
            else
            {
                NextOperation();
            }
        }
    }
}

void loop()
{
    if (gameRunning)
    {
        Operation operation = operations[operationIndex];
        if (!operationDrawn)
        {
            DrawOperation(operation);

            String result = "";
            for (int i = 0; i < operation.ResultLength(); i++)
            {
                result += '0';
            }

            showedResult = String(result);
        }

        int thirdCollumn = 15 - operation.ResultLength();
        Draw(thirdCollumn, 1, showedResult);

        CaptureInput();
    }
}

void EndGame()
{
    ClearDisplay();

    operationDrawn = false;
    resultIndex = 0;
    operationIndex = 0;

    StartGame();
}
