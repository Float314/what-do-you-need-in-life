/*
*****************************************************************************
*    What do you need in your life? [Base template for a game]
*    Copyright (C) 2025  Float
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*****************************************************************************

*/

#include "C:/raylib/raylib/src/raylib.h"
#include <cmath>
#include <iostream>
#include <vector>

struct EnemyBall
{
    Vector2 position;
    int radius;
    float speed;
    Color color;
    bool initialized;
};

int main(void)
{
    // Initialization, window stuff
    const int screenWidth = 800;
    const int screenHeight = 450;

    TraceLog(LOG_INFO, "Window opened");
    InitWindow(screenWidth, screenHeight, "What do you need in your life?");
    
    // Red ball stuff
    Vector2 ballPosition = { (float)screenWidth / 2, (float)screenHeight / 2 };
    int ballRadius = 15;
    Color ballColor = RED;

    // Enemy ball stuff
    std::vector<EnemyBall> enemyBalls;
    float enemySpawnTimer = 0.0f;
    float enemySpawnInterval = 2.0f;
    int maxEnemyBalls = 7;
    srand(time(0));

    // Red Ball physics
    float gravity = 9.8f;
    float velocityY = 0.0f;
    float bounceFactor = -0.9f;
    float minVelocity = 0.5f;
    float jumpForce = -8.0f;

    bool gameOver = false; // Game over flag

    // Rod stuff
    Color rodColor = RED;
    float rodThickness = 5.0f;
    float rodLength = 100.0f;

    SetTargetFPS(60); // Set the FPS

    while (!WindowShouldClose())
    {
        // Ball physics
        velocityY += gravity / 60.0f;
        ballPosition.y += velocityY;
        Vector2 mousePosition = GetMousePosition();
        float angle = atan2(mousePosition.y - ballPosition.y, mousePosition.x - ballPosition.x);
        Vector2 rodEnd;
        rodEnd.x = ballPosition.x + rodLength * cos(angle);
        rodEnd.y = ballPosition.y + rodLength * sin(angle);
        
        // Enemy ball physics
        enemySpawnTimer += GetFrameTime();
        if (enemySpawnTimer >= enemySpawnInterval && enemyBalls.size() < maxEnemyBalls)
        {
            EnemyBall newEnemy;
            newEnemy.radius = 15;
            newEnemy.speed = 2.0f;
            newEnemy.color = BLUE;
            newEnemy.initialized = false;

            int side = rand() % 4;
            if (side == 0)
            {
                newEnemy.position.x = -newEnemy.radius;
                newEnemy.position.y = rand() % screenHeight;
            }
            else if (side == 1)
            {
                newEnemy.position.x = screenWidth + newEnemy.radius;
                newEnemy.position.y = rand() % screenHeight;
            }
            else if (side == 2)
            {
                newEnemy.position.x = rand() % screenWidth;
                newEnemy.position.y = -newEnemy.radius;
            }
            else
            {
                newEnemy.position.x = rand() % screenWidth;
                newEnemy.position.y = screenHeight + newEnemy.radius;
            }

            enemyBalls.push_back(newEnemy);
            enemySpawnTimer = 0.0f; // Reset the timer AFTER the ball is spawned.
        }
        
        // Enemy ball physics (again)
        for (auto &enemy : enemyBalls)
        {
            float angle = atan2(ballPosition.y - enemy.position.y, ballPosition.x - enemy.position.x);
            enemy.position.x += enemy.speed * cos(angle);
            enemy.position.y += enemy.speed * sin(angle);
            enemy.initialized = true;
        }

        if (ballPosition.y + ballRadius > screenHeight)
        {
            gameOver = true;
        }

        if (ballPosition.y - ballRadius < 0)
        {
            ballPosition.y = ballRadius;
            velocityY *= bounceFactor;
            if (std::abs(velocityY) < minVelocity)
            {
                velocityY = 0;
            }
        }

        // Collision detection (b/w red ball and blue balls)
        for (int i = 0; i < enemyBalls.size(); i++)
        {
            float distance = sqrt(pow(enemyBalls[i].position.x - ballPosition.x, 2) + pow(enemyBalls[i].position.y - ballPosition.y, 2));
            float rodDistance = sqrt(pow(rodEnd.x - ballPosition.x, 2) + pow(rodEnd.y - ballPosition.y, 2));
            float rodEnemyDistance = sqrt(pow(enemyBalls[i].position.x - rodEnd.x, 2) + pow(enemyBalls[i].position.y - rodEnd.y, 2));

            if (enemyBalls[i].initialized && distance + enemyBalls[i].radius >= rodDistance && rodEnemyDistance + enemyBalls[i].radius <= rodLength + 5)
            {
                enemyBalls.erase(enemyBalls.begin() + i);
                i--;
            }
        }

        // Collision Application
        for (const auto &enemy : enemyBalls)
        {
            float distance = sqrt(pow(ballPosition.x - enemy.position.x, 2) + pow(ballPosition.y - enemy.position.y, 2));
            if (distance < ballRadius + enemy.radius)
            {
                gameOver = true;
                break;
            }
        }

        // Input (Space = Jump)
        if (IsKeyPressed(KEY_SPACE))
        {
            velocityY = jumpForce;
        }

        // Canvas Start (Canvas is the window)
        BeginDrawing();

        ClearBackground(RAYWHITE);
        if (!gameOver)
        {
            DrawCircleV(ballPosition, (float)ballRadius, ballColor);
            DrawLineEx(ballPosition, rodEnd, rodThickness, rodColor);
            DrawText("Avoid the Ground and Blue balls!", 10, 10, 20, LIGHTGRAY);
            for (const auto &enemy : enemyBalls)
            {
                DrawCircleV(enemy.position, (float)enemy.radius, enemy.color);
            }
        }
        else
        {
            DrawText("GAME OVER!", screenWidth / 2 - MeasureText("GAME OVER!", 40) / 2, screenHeight / 2 - 20, 40, RED);
        }

        EndDrawing();
        // Canvas End
    }

    CloseWindow(); // Cleanin' up
    TraceLog(LOG_INFO, "Window closed");

    return 0; // Self-explanatory
}