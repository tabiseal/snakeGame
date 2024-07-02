#include<easyx.h>
#include<deque> // Add deque to use double-ended queue
#include <windows.h> // Add windows.h to use Sleep function

#define WIDTH 640
#define HEIGHT 480

#define SIZE 20 // Square with side length of 20

#define DELAY 200 // Move at intervals of 200ms

enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

// Snake body
struct Snake
{
    int x;
    int y;
    Direction dir;
};

// Food
struct Food
{
    int x;
    int y;
};

// Draw the snake
void drawSnake(std::deque<Snake>& snake)
{
    // Represented in green (set fill color to green)
    setfillcolor(GREEN);
    // Traverse the snake to draw
    for (const auto& body : snake)
    {
        // Draw a rectangle, the four parameters are the x and y coordinates of the top left and bottom right corners
        // Use a 20*20 rectangle to represent the snake
        fillrectangle(body.x * SIZE, body.y * SIZE, (body.x + 1) * SIZE, (body.y + 1) * SIZE);
    }

    // Control the speed of the snake's movement
    Sleep(DELAY);
}

// Determine if the game is over
void isOver(std::deque<Snake>& snake)
{
    // Detect collision
    // Traverse the snake using an iterator, starting from the element after the head
    for (auto iter = snake.begin() + 1; iter != snake.end(); ++iter)
    {
        if ((snake.front().x == iter->x && snake.front().y == iter->y) // The snake's head collided with itself
            || (snake.front().x < 0) || (snake.front().x >= WIDTH / SIZE) // Out of bounds, the game ends
            || (snake.front().y < 0) || (snake.front().y >= HEIGHT / SIZE)) // Out of bounds, the game ends
        {
            // Set the current text color to red
            settextcolor(RED);
            // Set the current text style. Specify height 100, width 0 for adaptive. _T supports Unicode encoding, Consolas for the font
            settextstyle(100, 0, _T("Consolas"));
            TCHAR endStr[16];
            _stprintf_s(endStr, _T("GAME OVER!!!"));
            // Output the string at the specified location.
            outtextxy(WIDTH / SIZE, HEIGHT / SIZE, endStr);
            // Stay for three seconds
            Sleep(3000);

            closegraph(); // Close the window
            // End the program directly
            exit(0);
        }
    }
}

// Snake moves, 'grow' indicates whether food has been eaten, this parameter is used to distinguish between normal movement and eating food
void moveSnake(std::deque<Snake>& snake, bool grow = false)
{
    // Get a new snake head
    Snake newHead = snake.front();

    // Determine the direction of the snake's movement
    switch (newHead.dir)
    {
    case UP:
        newHead.y--;
        break;
    case DOWN:
        newHead.y++;
        break;
    case LEFT:
        newHead.x--;
        break;
    case RIGHT:
        newHead.x++;
        break;
    }

    // Use the new position as the new head
    snake.push_front(newHead);
    // Each move must determine if the game is over
    isOver(snake);
    // grow=true indicates food was eaten; grow=false indicates normal movement
    if (!grow)
    {
        // Did not eat food
        // Clear the rectangle of the last element of the snake, as it is no longer part of the snake
        clearrectangle(snake.back().x * SIZE, snake.back().y * SIZE, (snake.back().x + 1) * SIZE, (snake.back().y + 1) * SIZE);
        // Remove the last element of the deque
        snake.pop_back();
    }
    // Draw the snake
    drawSnake(snake);
}

// Change the direction of snake movement
void changeDirection(std::deque<Snake>& snake)
{
    ExMessage msg = { 0 };
    // Capture keyboard messages
    peekmessage(&msg, EX_KEY);
    // When a key is pressed
    if (msg.message == WM_KEYDOWN)
    {

        // Determine the key pressed
        switch (msg.vkcode)
        {
        case 'w':
        case VK_UP:
        case 'W':
            // Cannot move in the opposite direction
            if (snake.front().dir != DOWN)
            {
                snake.front().dir = UP;
            }
            break;

        case 's':
        case VK_DOWN:
        case 'S':
            // Cannot move in the opposite direction
            if (snake.front().dir != UP)
            {
                snake.front().dir = DOWN;
            }
            break;

        case 'a':
        case VK_LEFT:
        case 'A':
            // Cannot move in the opposite direction
            if (snake.front().dir != RIGHT)
            {
                snake.front().dir = LEFT;
            }
            break;

        case 'd':
        case VK_RIGHT:
        case 'D':
            // Cannot move in the opposite direction
            if (snake.front().dir != LEFT)
            {
                snake.front().dir = RIGHT;
            }
            break;
        }
    }
}

// Generate food, it cannot be generated on the snake
void generateFood(Food& food, std::deque<Snake>& snake)
{
    // Keep generating until food is not on the snake
    while (true)
    {
        // Generate randomly
        // Consider the entire window as an array, each element of the array occupies a space of 20*20 pixels
        food.x = rand() % (WIDTH / SIZE);
        food.y = rand() % (HEIGHT / SIZE);

        // Use a variable to store whether the generated position overlaps with the snake's position
        bool overlap = false;
        // Traverse the snake
        for (const auto& body : snake)
        {

            // The position of the food coincides with the snake
            if (food.x == body.x && food.y == body.y)
            {
                // Already overlapped with the snake, no need to continue traversing, break out of the for loop, regenerate in the while loop
                overlap = true;
                break;
            }
        }

        // If it does not overlap, the food is successfully generated, and the while loop can end
        if (!overlap)
        {
            break;
        }
    }
}

// Check if it overlaps with the position of the food
bool checkCollision(std::deque<Snake>& snake, Food& food)
{
    // The snake's head overlaps with the food
    if (snake.front().x == food.x && snake.front().y == food.y)
    {
        return true;
    }
    // Check if it overlaps with the position of the snake's body (not necessary, as the snake's body will not pass where the head hasn't passed)
    for (auto iter = snake.begin() + 1; iter != snake.end(); ++iter)
    {
        if (iter->x == food.x && iter->y == food.y)
        {
            return true;
        }
    }
    return false;
}


int main()
{
    initgraph(WIDTH, HEIGHT); // Draw window, two parameters are the width and length of the window

    // Define a snake
    std::deque<Snake> snake;
    // Initialize the snake
    // Insert an element at the head of the deque as the snake head
    snake.push_front({ WIDTH / SIZE / 2,HEIGHT / SIZE / 2,RIGHT });

    Food food;
    // Generate food
    generateFood(food, snake);
    // Draw food, red indicates food
    setfillcolor(RED);
    fillrectangle(food.x * SIZE, food.y * SIZE, (food.x + 1) * SIZE, (food.y + 1) * SIZE);

    drawSnake(snake);

    // The initial score is 0
    int score = 0;
    while (true)
    {
        // Snake moves
        moveSnake(snake);
        // Change the direction of snake movement
        changeDirection(snake);

        // Check if food was eaten
        if (checkCollision(snake, food))
        {
            // The snake ate the food
            score++;
            // Regenerate food
            generateFood(food, snake);
            // true, increase the length of the snake
            moveSnake(snake, true);
        }

        // Draw food, red indicates food
        setfillcolor(RED);
        fillrectangle(food.x * SIZE, food.y * SIZE, (food.x + 1) * SIZE, (food.y + 1) * SIZE);

        // Set the current text color to yellow
        settextcolor(YELLOW);
        // Set the current text style. Specify height 20, width 0 for adaptive. _T supports Unicode encoding, Consolas for the font
        settextstyle(20, 0, _T("Consolas"));
        TCHAR strScore[16];
        _stprintf_s(strScore, _T("Score:%d"), score);
        // Output the string at the specified location.
        outtextxy(10, 10, strScore);
    }

    system("pause"); // Prevent the window from opening and closing immediately, let it pause for a while
    closegraph(); // Close the window

    return 0;
}
