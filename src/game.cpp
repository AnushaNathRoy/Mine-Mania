#include "game.h"
#include "sprite_renderer.h"
#include "resource_manager.h"
#include "game_level.h"
#include "game_object.h"
#include "ball_object.h"
#include "text_renderer.h"
#include <iostream>
#include <sstream>
#include <string>
#include <ft2build.h>
#include <freetype/freetype.h>
#include <unistd.h>
#include <time.h>
time_t start = time(0);

SpriteRenderer *Renderer;
GameObject *Player;
GameObject *Player1;
TextRenderer *Text;
TextRenderer *gameEnded;
TextRenderer *newLevel;

enum Direction
{
    UP,
    RIGHT,
    DOWN,
    LEFT
};

typedef std::tuple<bool, Direction, glm::vec2> Collision;

// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
const float BALL_RADIUS = 12.5f;

BallObject *Ball;
BallObject *Enemy1;
BallObject *Enemy2;
BallObject *Enemy3;
BallObject *Enemy4;

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{
}

Game::~Game()
{
    delete Renderer;
    delete Ball;
}

void Game::Init()
{
    ResourceManager::LoadShader("../src/shaders/sprite.vs", "../src/shaders/sprite.frag", nullptr, "sprite");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
                                      static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    ResourceManager::LoadTexture("../src/textures/awesomeface.png", true, "face");
    // load textures
    ResourceManager::LoadTexture("../src/textures/background.jpg", false, "background");
    ResourceManager::LoadTexture("../src/textures/block.png", false, "block");
    ResourceManager::LoadTexture("../src/textures/block_solid.png", false, "block_solid");
    ResourceManager::LoadTexture("../src/textures/block.png", true, "paddle");
    ResourceManager::LoadTexture("../src/textures/gold.png", true, "gold");
    ResourceManager::LoadTexture("../src/textures/diamond.png", true, "diamond");
    ResourceManager::LoadTexture("../src/textures/coal.png", true, "coal");
    ResourceManager::LoadTexture("../src/textures/door.png", true, "door");
    ResourceManager::LoadTexture("../src/textures/tnt.png", true, "tnt");

    // load levels
    GameLevel one;
    // one.Load("hello", 2, 5);
    one.Load((unsigned)this->Width, (unsigned)this->Height);

    this->Levels.push_back(one);
    this->Level = 0;
    this->Score = 0;
    
    // glm::vec2 playerPos1 = glm::vec2(1000.0f, 1000.0f);
    // Player1 = new GameObject(playerPos1, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

    glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS,
                                              -BALL_RADIUS * 2.0f);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
                          ResourceManager::GetTexture("face"));

    glm::vec2 e1Pos = glm::vec2(this->Height / 2, this->Width / 2);
    Enemy1 = new BallObject(e1Pos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
                            ResourceManager::GetTexture("tnt"));
    Enemy1->Stuck = false;

    glm::vec2 e2Pos = glm::vec2(0.0f, 0.0f);
    Enemy2 = new BallObject(e2Pos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
                            ResourceManager::GetTexture("tnt"));
    Enemy2->Stuck = false;

    glm::vec2 e3Pos = glm::vec2(this->Height, this->Width);
    Enemy3 = new BallObject(e3Pos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
                            ResourceManager::GetTexture("tnt"));
    Enemy3->Stuck = false;

    Ball->Stuck = true;

    Text = new TextRenderer(this->Width, this->Height);
    Text->Load("../src/ocraext.TTF", 10);

    gameEnded = new TextRenderer(this->Width, this->Height);
    gameEnded->Load("../src/ocraext.TTF", 24);

    newLevel = new TextRenderer(this->Width, this->Height);
    newLevel->Load("../src/ocraext.TTF", 24);
}

void Game::Update(float dt)
{
    Ball->Move(dt, this->Width);
    Enemy1->Move(dt, this->Width);
    if (this->Level > 0)
    {
        Enemy2->Move(dt, this->Width);
    }
    if (this->Level > 1)
    {
        Enemy3->Move(dt, this->Width);
    }

    // check for collisions
    this->DoCollisions();
    // check loss condition
    // if (Ball->Position.y >= this->Height) // did ball reach bottom edge?
    // {
    //     this->ResetLevel();
    //     this->ResetPlayer();
    // }
}

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_ACTIVE)
    {
        float velocity = PLAYER_VELOCITY * dt;
        // move playerboard
        if (this->Keys[GLFW_KEY_A])
        {
            if (Player->Position.x >= 0.0f)
            {
                Player->Position.x -= velocity;
            }
        }
        if (this->Keys[GLFW_KEY_D])
        {
            if (Player->Position.x <= this->Width - Player->Size.x)
                Player->Position.x += velocity;
        }
        if (this->Keys[GLFW_KEY_W])
        {
            if (Player->Position.y >= 0.0f)
                Player->Position.y -= velocity;
        }

        if (this->Keys[GLFW_KEY_S])
        {
            if (Player->Position.y <= this->Height - Player->Size.y)
                Player->Position.y += velocity;
        }
        if (this->Keys[GLFW_KEY_LEFT])
        {
            if (Ball->Position.x >= 0.0f)
                Ball->Position.x -= velocity;
        }
        if (this->Keys[GLFW_KEY_RIGHT])
        {
            if (Ball->Position.x <= this->Width - Ball->Size.x)
                Ball->Position.x += velocity;
        }
        if (this->Keys[GLFW_KEY_UP])
        {
            if (Ball->Position.y >= 0.0f)
                Ball->Position.y -= velocity;
        }

        if (this->Keys[GLFW_KEY_DOWN])
        {
            if (Ball->Position.y <= this->Height - Ball->Size.y)
                Ball->Position.y += velocity;
        }
    }
}

void Game::Render()
{
    if (this->State == GAME_ACTIVE)
    {
        // draw background
        Renderer->DrawSprite(ResourceManager::GetTexture("background"),
                             glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
        // draw level
        this->Levels[0].Draw(*Renderer);

        // draw player
        // Player1->Draw(*Renderer);
        Ball->Draw(*Renderer);
        std::stringstream ss, ss1, ss2;
        ss << this->Score;
        ss1 << this->Level;

        Enemy1->Draw(*Renderer);

        if (this->Level > 0)
            Enemy2->Draw(*Renderer);

        if (this->Level > 1)
            Enemy3->Draw(*Renderer);

        double seconds = difftime(time(0), start);
        int hr = (int)(seconds / 3600);
        int min = ((int)(seconds / 60)) % 60;
        int sec = (int)(int(seconds) % 60);
        char timestr[100];
        sprintf(timestr, "%02d:%02d:%02d", hr, min, sec);
        ss2 << timestr;
        Text->RenderText("Score: " + ss.str() + " Level: " + ss1.str() + " Time Elapsed: " + ss2.str(), 1.0f, 1.0f, 0.25f);
    }
    else if (this->State == GAME_MENU)
    {
        Renderer->DrawSprite(ResourceManager::GetTexture("background"),
                             glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
        gameEnded->RenderText("GAME OVER.YOU DIED.", 1.0f, 1.0f, 0.25f);
    }
    else if (this->State == GAME_WIN)
    {
        Renderer->DrawSprite(ResourceManager::GetTexture("background"),
                             glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
        gameEnded->RenderText("GAME OVER.YOU ESCAPED.", 1.0f, 1.0f, 0.25f);
    }
}

void Game::ResetLevel()
{
    // if (this->Level == 0)
    //     this->Levels[0].Load("levels/one.lvl", this->Width, this->Height / 2);
    // else if (this->Level == 1)
    //     this->Levels[1].Load("levels/two.lvl", this->Width, this->Height / 2);
    // else if (this->Level == 2)
    //     this->Levels[2].Load("levels/three.lvl", this->Width, this->Height / 2);
    // else if (this->Level == 3)
    //     this->Levels[3].Load("levels/four.lvl", this->Width, this->Height / 2);
    this->Levels[0].Load(this->Width, this->Height);
}

void Game::ResetPlayer()
{
    // reset player/ball stats
    // Player->Size = PLAYER_SIZE;
    // Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
}

// collision detection
bool CheckCollision(GameObject &one, GameObject &two);
Collision CheckCollision(BallObject &one, GameObject &two);
Direction VectorDirection(glm::vec2 closest);

void Game::DoCollisions()
{
    for (GameObject &box : this->Levels[0].Bricks)
    {
        if (!box.Destroyed)
        {
            Collision collision = CheckCollision(*Ball, box);
            Collision enemy1 = CheckCollision(*Ball, *Enemy1);

            if (this->Level > 0)
            {
                Collision enemy2 = CheckCollision(*Ball, *Enemy2);
                if (std::get<0>(enemy2))
                {
                    //std::cout << "game ended 2";
                    this->State = GAME_MENU;

                   // std::cout << "Score: " << this->Score;
                    gameEnded->RenderText("GAME OVER", this->Width / 2, this->Height / 2, 1.0f);
                    // sleep(2);
                    // exit(-1);
                }
            }

            if (this->Level > 1)
            {
                Collision enemy3 = CheckCollision(*Ball, *Enemy3);
                if (std::get<0>(enemy3))
                {
                    // std::cout << "game ended 3";
                    this->State = GAME_MENU;

                    // std::cout << "Score: " << this->Score;
                    gameEnded->RenderText("GAME OVER", this->Width / 2, this->Height / 2, 1.0f);
                    // sleep(2);
                    // exit(-1);
                }
            }

            if (std::get<0>(enemy1))
            {
                // std::cout << "game ended 1";
                this->State = GAME_MENU;

                // std::cout << "Score: " << this->Score;
                gameEnded->RenderText("GAME OVER", this->Width / 2, this->Height / 2, 1.0f);
                // sleep(2);
                // exit(-1);
            }

            if (std::get<0>(collision)) // if collision is true
            {
                // destroy block if not solid
                if (!box.IsSolid && box.Type != 5)
                    box.Destroyed = true;

                if (box.Type == 2)
                {
                    this->Score += 10;
                }

                if (box.Type == 3)
                {
                    this->Score += 15;
                }

                if (box.Type == 4)
                {
                    this->Score += 5;
                }

                if (box.Type == 5)
                {
                    this->Level += 1;
                    this->ResetLevel();
                    this->ResetPlayer();

                    if (this->Level == 3)
                    {
                        this->State = GAME_WIN;
                    }
                }
                // collision resolution
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if (dir == LEFT || dir == RIGHT) // horizontal collision
                {
                    Ball->Velocity.x = -Ball->Velocity.x; // reverse horizontal velocity
                    // relocate
                    float penetration = Ball->Radius - std::abs(diff_vector.x);
                    if (dir == LEFT)
                        Ball->Position.x += penetration; // move ball to right
                    else
                        Ball->Position.x -= penetration; // move ball to left;
                }
                else // vertical collision
                {
                    Ball->Velocity.y = -Ball->Velocity.y; // reverse vertical velocity
                    // relocate
                    float penetration = Ball->Radius - std::abs(diff_vector.y);
                    if (dir == UP)
                        Ball->Position.y -= penetration; // move ball bback up
                    else
                        Ball->Position.y += penetration; // move ball back down
                }
            }
        }
    }
    // check collisions for player pad (unless stuck)
    Collision result = CheckCollision(*Ball, *Player);
    if (!Ball->Stuck && std::get<0>(result))
    {
        // check where it hit the board, and change velocity based on where it hit the board
        float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
        float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
        float percentage = distance / (Player->Size.x / 2.0f);
        // then move accordingly
        float strength = 2.0f;
        glm::vec2 oldVelocity = Ball->Velocity;
        Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
        // Ball->Velocity.y = -Ball->Velocity.y;
        Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity); // keep speed consistent over both axes (multiply by length of old velocity, so total strength is not changed)
        // fix sticky paddle
        Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);
    }
}

bool CheckCollision(GameObject &one, GameObject &two) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
                      two.Position.x + two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
                      two.Position.y + two.Size.y >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}

Collision CheckCollision(BallObject &one, GameObject &two) // AABB - Circle collision
{
    // get center point circle first
    glm::vec2 center(one.Position + one.Radius);
    // calculate AABB info (center, half-extents)
    glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
    glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x, two.Position.y + aabb_half_extents.y);
    // get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // now that we know the the clamped values, add this to AABB_center and we get the value of box closest to circle
    glm::vec2 closest = aabb_center + clamped;
    // now retrieve vector between center circle and closest point AABB and check if length < radius
    difference = closest - center;

    if (glm::length(difference) < one.Radius) // not <= since in that case a collision also occurs when object one exactly touches object two, which they are at the end of each collision resolution stage.
        return std::make_tuple(true, VectorDirection(difference), difference);
    else
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

// calculates which direction a vector is facing (N,E,S or W)
Direction VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),  // up
        glm::vec2(1.0f, 0.0f),  // right
        glm::vec2(0.0f, -1.0f), // down
        glm::vec2(-1.0f, 0.0f)  // left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}