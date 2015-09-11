#pragma once

#include "Schafkopf.h"

namespace SchafKopf
{

struct PlayerInfo
{
    enum TriState
    {
        No,
        Yes,
        Unknown
    };

    PlayerInfo()
        : trumpFree(Unknown)
    {
        colorFree[Eichel] = colorFree[Gras] = colorFree[Schelln] = colorFree[Herz] = Unknown;
    }

    TriState trumpFree;
    TriState colorFree[numColors];

    static const char* toString(TriState triState)
    {
        switch (triState) {
        case No:
            return "no";
        case Yes:
            return "yes";
        case Unknown:
            return "unknown";
        }
    }
};

struct GameInfo
{
    GameInfo(const Game& game)
        : m_game(game)
    {
        switch (m_game.gameType) {
        case Game::Solo:
        case Game::SauSpiel:
            // trumps is all colors + other three Unter + other three Ober
            trumpCount = numCardTypes + 3 + 3;
            // color cards are all color withoug Unter and Ober
            colorCardCount = numCardTypes - 2;
            break;
        case Game::Wenz:
        case Game::Geier:
            // only Unter or Ober are trumps
            trumpCount = 4;
            // all other cards are normal color cards
            colorCardCount = numCardTypes - 1;
            break;
        case Game::FarbWenz:
        case Game::FarbGeier:
            // all colors + other three Unter/Ober
            trumpCount = numCardTypes + 3;
            // all except Unter/Ober
            colorCardCount = numCardTypes - 1;
            break;
        }

        // nothing played yet :)
        trumpsLeft = trumpCount;
        colorsLeft[Eichel] = colorsLeft[Gras] = colorsLeft[Herz] = colorsLeft[Schelln] = colorCardCount;
    }

    void cardPlayed(const Card& card)
    {
        if (m_game.isTrump(card))
            --trumpsLeft;
        else
            --colorsLeft[card.color];
    }

    int trumpCount;
    int colorCardCount;

    int trumpsLeft;
    int colorsLeft[numColors];

    const Game& m_game;
};

class ObserverAi : public AI
{
public:
    ObserverAi(const Game& game, const Player& player)
        : m_player(player),
          m_game(game),
          m_gameInfo(game)
    {
    }

    void cardPlayed(const ActivePile& pile, int activePlayer) override
    {
        const Card &playedCard = pile.lastPlayedCard();
        const Card &firstPlayedCard = pile.firstPlayedCard();

        m_gameInfo.cardPlayed(playedCard);
        if (m_gameInfo.trumpCount == 0)
            m_playerInfo[0].trumpFree = m_playerInfo[1].trumpFree
                    = m_playerInfo[2].trumpFree = m_playerInfo[3].trumpFree = PlayerInfo::Yes;
        if (m_gameInfo.colorsLeft[playedCard.color] == 0)
            m_playerInfo[0].colorFree[playedCard.color] = m_playerInfo[1].colorFree[playedCard.color]
                    = m_playerInfo[2].colorFree[playedCard.color] = m_playerInfo[3].colorFree[playedCard.color] = PlayerInfo::Yes;

        if (activePlayer == m_player.id)
            return;

        if (pile.numCards <= 1)
            return; // ### TODO - what assumptions can we make?

        if (m_game.isTrump(firstPlayedCard)) {
            if (!m_game.isTrump(playedCard))
                m_playerInfo[activePlayer].trumpFree = PlayerInfo::Yes;
        } else {
            if (m_game.isTrump(playedCard) || playedCard.color != firstPlayedCard.color)
                m_playerInfo[activePlayer].colorFree[firstPlayedCard.color] = PlayerInfo::Yes;
        }
    }

    int doPlayCard(const ActivePile&) override
    {
        assert(false);
    }

    void reset() override
    {
    }

    const Player& m_player;
    const Game& m_game;

    GameInfo m_gameInfo;
    PlayerInfo m_playerInfo[4];
};

}
