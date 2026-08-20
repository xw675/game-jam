#pragma once

enum class EntityKind { 
    Player, 
    Hunter, 
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