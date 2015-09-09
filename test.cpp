#include "Schafkopf.h"

using namespace SchafKopf;

int stiche()
{
    Game game;
    game.deck.shuffle();

    game.type = Game::Solo;
    game.color = Color::Herz;
}

int main()
{
    stiche();
}
