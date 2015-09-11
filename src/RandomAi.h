#pragma once

#include "Schafkopf.h"

namespace SchafKopf
{
class RandomAi : public AI
{
public:
    RandomAi(const Game& game, const Player& player)
        : m_game(game),
          m_player(player)
    {
        assert(player.id >= 0 && player.id < 4);
    }

    void cardPlayed(const ActivePile&, int) override
    {
        // nothing - we're stupid
    }

    // since cards are randomly shuffled, just put the first card that can be played
    virtual int doPlayCard(const ActivePile& pile) override
    {
        for (int i = 0; i < Player::maxCards; ++i) {
            if (!m_player.m_cards[i])
                continue;
            if (!m_game.canPutCard(i))
                continue;
            return i;
        }

        assert(false); // called on a player w/o cards
        return 0;
    }

    virtual void reset() override
    {
        // we don't have a state, nothing to do
    }

private:
    const Game& m_game;
    const Player& m_player;
};

}
