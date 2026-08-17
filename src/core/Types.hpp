#pragma once

enum class EntityKind { 
    Player, 
    Wanderer, 
    Hunter, 
    Guard, 
    Token, 
    Hall 
};

enum class Visibility {
    Unseen, 
    Remembered, 
    Visible
};

enum class GameState {
    Title, 
    Play, 
    Dead, 
    Won
};