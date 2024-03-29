#pragma once

#include "Includes.h"

#import "GameObject.h"
#import "Transform.h"
#import "MeshRenderer.h"
#import "PillRotation.h"
#import "SnakeHead.h"

/** This code manages the game. */

class GameManager
{
public:
    static void SceneSetup();
    static void UpdateGameObjects();
    static void FixedUpdateGameObjects();
    static void DrawGameObjects();
    static GameObject* AddGameObject();
    static void SetDirection(int dir, float& elapsedFrames);
    static int score;
    static bool isGameOver;
    static bool playSound;
private:
    static std::vector<GameObject*> objects;
    static SnakeHead* snake;
    static GridComponent* pill;
    static float GetRandomCoord();
    static void CollisionChecker();
    static void KillSnake();
};

std::vector<GameObject*> GameManager::objects;
SnakeHead* GameManager::snake;
GridComponent* GameManager::pill;
bool GameManager::playSound = false;

int GameManager::score = 0;
bool GameManager::isGameOver = false;

/** Sets up scene. */

void GameManager::SceneSetup()
{
    score = 0;
    isGameOver = false;
    
    auto snake = AddGameObject();
    snake->AddComponent(new SnakeHead());
    snake->GetComponent<GridComponent>()->x = 10.0f;
    snake->GetComponent<GridComponent>()->y = 10.0f;
    snake->GetComponent<GridComponent>()->UpdatePos();
    GameManager::snake = snake->GetComponent<SnakeHead>();
    
    auto pill = AddGameObject();
    pill->AddComponent(new GridComponent());
    auto pgc = pill->GetComponent<GridComponent>();
    pgc->x = GetRandomCoord();
    pgc->y = GetRandomCoord();
    pgc->UpdatePos();
    pill->AddComponent(new PillRotation());
    Mesh *m1 = new Mesh("perc60.obj");
    MeshRenderer *mr2 = new MeshRenderer();
    mr2->SetColor({1.0f, 1.0f, 1.0f});
    mr2->SetMesh(m1);
    mr2->SetRenderer(new Renderer(@"drywall.jpg"));
    pill->AddComponent(mr2);
    pill->GetComponent<Transform>()->scale = { 0.7f, 0.7f, 0.7f };
    GameManager::pill = pgc;
    
    auto plane = AddGameObject();
    plane->AddComponent(new Transform());
    auto t = plane->GetComponent<Transform>();
    t->rotation.x = M_PI / 2;
    t->position = { 0.0f, 0.0f, -62.0f };
    t->scale = { 21.0f, 21.0f, 1.0f };
    Mesh *m2 = new Mesh("plane.obj");
    MeshRenderer *mr3 = new MeshRenderer();
    mr3->SetColor({ 1.0f, 0.5f, 0.32f });
    mr3->SetMesh(m2);
    mr3->SetRenderer(new Renderer(@"rock.jpg"));
    plane->AddComponent(mr3);
    
    auto plane2 = AddGameObject();
    plane2->AddComponent(new Transform());
    auto t2 = plane2->GetComponent<Transform>();
    t2->rotation.x = M_PI / 2;
    t2->position = { 0.0f, 0.0f, -63.0f };
    t2->scale = { 50.0f, 50.0f, 1.0f };
    Mesh *m22 = new Mesh("plane.obj");
    MeshRenderer *mr32 = new MeshRenderer();
    mr32->SetColor({ 1.0f, 1.0f, 1.0f });
    mr32->SetMesh(m22);
    mr32->SetRenderer(new Renderer(@"fire.jpg"));
    plane2->AddComponent(mr32);
    
}

/** Updates the objects in the scene. */

void GameManager::UpdateGameObjects()
{
    for (auto &go : objects)
    {
        go->UpdateComponents();
    }
}

/** Updates objects and detects collisions. */

void GameManager::FixedUpdateGameObjects()
{
    for (auto &go : objects)
    {
        go->FixedUpdateComponents();
    }
    GameManager::CollisionChecker();
}

/** Draws up the game objects for the scene. */

void GameManager::DrawGameObjects()
{
    for (auto &go : objects)
    {
        go->GetComponent<MeshRenderer>()->Draw();
    }
}

/** Adds a basic game object to the game and returns it.
    @return go
 */

GameObject* GameManager::AddGameObject()
{
    GameObject *go = new GameObject();
    objects.push_back(go);
    return go;
}

/** Sets the direction.
    @param dir
    @param elapsedFrames
 
 */

void GameManager::SetDirection(int dir, float& elapsedFrames) {
    if((snake->dir + dir) % 2 == 1) {
        snake->dir = dir;
        elapsedFrames = 30.0f;
    }
}

/** Snake dies when colliding with boundaries or a part of its body
*/
void GameManager::KillSnake() {
    isGameOver = true;
    for (int i; i < objects.size(); ++i) {
        free(objects[i]);
    }
    objects.clear();
}

/** Checks for collisions.
 */

void GameManager::CollisionChecker() {
    if(isGameOver)
        return;
    if(snake->gc->x == pill->x && snake->gc->y == pill->y) {
        ++score;
        pill->x = GetRandomCoord();
        pill->y = GetRandomCoord();
        pill->UpdatePos();
        playSound = true;
        
        auto seg = AddGameObject();
        seg->AddComponent(new GridComponent());
        GridComponent *segc = seg->GetComponent<GridComponent>();
        segc->x = snake->gc->x;
        segc->y = snake->gc->y;
        segc->UpdatePos();
        segc->t->scale = {0.0f, 0.0f, 0.0f};
        segc->t->rotation.x = M_PI / 2.0f;
        
        Mesh *m = new Mesh("Cube.obj");
        MeshRenderer *mr = new MeshRenderer();
        mr->SetColor({0.23f, 1.0f, 0.11f});
        mr->SetMesh(m);
        mr->SetRenderer(new Renderer(@"snakeskin.jpg"));
        seg->AddComponent(mr);
        
        snake->body.push_back(seg);
        snake->justExpanded = true;
    } else if (snake->gc->x > 20 || snake->gc->y > 20 || snake->gc->x < 0 || snake->gc->y < 0) {
        KillSnake();
    } else {
        std::deque<GameObject*> copyBody = snake->body;
        while (!copyBody.empty()) {
            if (copyBody.front()->GetComponent<GridComponent>()->x == snake->gc->x && copyBody.front()->GetComponent<GridComponent>()->y == snake->gc->y) {
                //printf("hehe snake dead");
                KillSnake();
                return;
            } else {
                //printf("hehe snake pop");
                copyBody.pop_front();
            }
        }
    }
}

/** Gets a random coordinate from the playing field.
 @returns (float) (std::rand() % 21)
 */

float GameManager::GetRandomCoord() {
    return (float) (std::rand() % 21);
}
