/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "game_level.h"

#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>

void GameLevel::Load(unsigned int levelWidth, unsigned int levelHeight)
{
    // clear old data
    this->Bricks.clear();
    // load from file
    unsigned int tileCode;
    GameLevel level;
    std::string line;
    // std::ifstream fstream(file);
    std::vector<std::vector<unsigned int>> tileData;
    std::vector<unsigned int> rowbeg;
    int ncol = 20;
    int nrow = 20;
    srand(time(0));
    int upper = 4;
    int lower = 0;
    int upper1 = 1;
    int lower0 = 0;
    for(int i=0; i<ncol; i++)
        rowbeg.push_back(1);
    tileData.push_back(rowbeg);
    
    for(int i=0; i<nrow-3;i++){
        std::vector<unsigned int> row;
        row.push_back(1);
        for(int j=0; j<ncol-2;j++){
            int random_no = (rand() % (upper - lower + 1)) + lower;
            int random_10 = (rand() % (upper1 - lower0 + 1)) + lower0;
            if(random_10)
                row.push_back(random_no);
            else
                row.push_back(0);
        }
        if(i==0)
            row.push_back(5);
        else
            row.push_back(1);
        tileData.push_back(row);
    }
    
    std::vector<unsigned int> rowlast;
    rowlast.push_back(0);
    for(int i=0; i<ncol-2; i++)
        rowlast.push_back(0);
    rowlast.push_back(1);
    tileData.push_back(rowlast);

    std::vector<unsigned int> rowlast1;
    for(int i=0; i<ncol; i++)
        rowlast1.push_back(1);
    tileData.push_back(rowlast1);


    // for (int i = 0; i < tileData.size(); i++)
    // {
    //     for (int j = 0; j < tileData.size(); j++)
    //     {
    //         printf("%d ", tileData[i][j]);
    //     }   
    //     printf("\n");
    // }
    // if (fstream)
    // {
    //     while (std::getline(fstream, line)) // read each line from level file
    //     {
    //         std::istringstream sstream(line);
    //         std::vector<unsigned int> row;
    //         while (sstream >> tileCode) // read each word separated by spaces
    //             row.push_back(tileCode);
    //         tileData.push_back(row);
    //     }
        
        if (tileData.size() > 0)
            this->init(tileData, levelWidth, levelHeight);
    // }
}

void GameLevel::Draw(SpriteRenderer &renderer)
{
    for (GameObject &tile : this->Bricks)
        if (!tile.Destroyed)
            tile.Draw(renderer);
}

bool GameLevel::IsCompleted()
{
    for (GameObject &tile : this->Bricks)
        if (!tile.IsSolid && !tile.Destroyed)
            return false;
    return true;
}

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, 
                     unsigned int lvlWidth, unsigned int lvlHeight)
{
    // calculate dimensions
    unsigned int height = tileData.size();
    unsigned int width  = tileData[0].size();
    float unit_width    = lvlWidth / static_cast<float>(width);
    float unit_height   = lvlHeight / height;
    // std::cout<<lvlHeight<<" "<<lvlWidth<<std::endl;
    // initialize level tiles based on tileData		
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            int type_choice = -1;
            glm::vec2 pos(unit_width * x, unit_height * y);
            glm::vec2 size(unit_width, unit_height);
            // check block type from level data (2D level array)
            if (tileData[y][x] == 1) // solid
            {
                GameObject obj(pos, size, 
                    ResourceManager::GetTexture("block_solid"), 
                    glm::vec3(0.8f, 0.8f, 0.7f)
                );
                obj.IsSolid = true;
                obj.Type = 1;
                this->Bricks.push_back(obj);
            }
            else if (tileData[y][x] > 1)	
            {
                glm::vec3 color = glm::vec3(1.0f); // original: white
                if (tileData[y][x] == 2){
                    color = glm::vec3(0.8f, 0.498039f, 0.196078f);
                    type_choice = 2;
                    GameObject obj(pos, size, 
                    ResourceManager::GetTexture("gold"), 
                    color
                    );
                    obj.IsSolid = false;
                    obj.Type = 2;
                    this->Bricks.push_back(obj);
                }
                    
                else if (tileData[y][x] == 3){
                    color = glm::vec3(0.560784f, 0.560784f, 0.737255f);
                    type_choice = 3;
                    GameObject obj(pos, size, 
                    ResourceManager::GetTexture("diamond"), 
                    color
                    );
                    obj.IsSolid = false;
                    obj.Type = 3;
                    this->Bricks.push_back(obj);
                }
                    
                else if (tileData[y][x] == 4){
                    color = glm::vec3(0.8f, 0.8f, 0.4f);
                    type_choice = 4;
                    GameObject obj(pos, size, 
                    ResourceManager::GetTexture("coal"), 
                    color
                    );
                    obj.IsSolid = false;
                    obj.Type = 4;
                    this->Bricks.push_back(obj);
                }
                    
                else if (tileData[y][x] == 5){
                   color = glm::vec3(1.0f, 0.5f, 0.0f); 
                   type_choice = 5;
                   GameObject obj(pos, size, 
                    ResourceManager::GetTexture("door"), 
                    color
                    );
                    obj.IsSolid = false;
                    obj.Type = 5;
                    this->Bricks.push_back(obj);

                }


            }
        }
    }  
}