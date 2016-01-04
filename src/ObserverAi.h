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
    {
        reset();
    }

    void reset()
    {
        trumpFree = Unknown;
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
    GameInfo(const Game& game, const Player& player)
        : m_game(game)
    {
        reset(player);
    }

    void reset(const Player& player)
    {
        switch (m_game.gameType) {
        case Game::Solo:
        case Game::SauSpiel:
            // trumps is all colors + other three Unter + other three Ober
            trumpCount = numCardTypes + 3 + 3;
            // color cards are all color without Unter and Ober
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

        // remove our cards from game info
        for (const auto &card : player.m_cards) {
            assert(card);
            cardPlayed(*card);
        }
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
          m_gameInfo(game, player)
    {
        updatePlayerInfo();
    }

    void setOthersTrumpFree()
    {
        for (int i = 0; i < numPlayers; ++i) {
            if (i == m_player.id)
                continue;
            m_playerInfo[i].trumpFree = PlayerInfo::Yes;
        }
    }

    void setOthersColorFree(Color color)
    {
        for (int i = 0; i < numPlayers; ++i) {
            if (i == m_player.id)
                continue;
            m_playerInfo[i].colorFree[color] = PlayerInfo::Yes;
        }
    }

    void cardPlayed(const ActivePile& pile, int activePlayer) override
    {
        const Card &playedCard = pile.lastPlayedCard();
        const Card &firstPlayedCard = pile.firstPlayedCard();
        const bool isTrump = m_game.isTrump(playedCard);

        if (activePlayer == m_player.id) {
            updatePlayerInfo();
            return; // we now already know everything about ourselves
        }

        m_gameInfo.cardPlayed(playedCard);
        if (isTrump && m_gameInfo.trumpCount == 0)
            setOthersTrumpFree();
        if (!isTrump && m_gameInfo.colorsLeft[playedCard.color] == 0)
            setOthersColorFree(playedCard.color);

        if (pile.numCards <= 1)
            return; // ### TODO - what assumptions can we make?

        // if a player didn't play according to the first card, he must be free of that type of card
        if (m_game.isTrump(firstPlayedCard)) {
            if (!isTrump)
                m_playerInfo[activePlayer].trumpFree = PlayerInfo::Yes;
        } else {
            if (m_game.isTrump(playedCard) || playedCard.color != firstPlayedCard.color)
                m_playerInfo[activePlayer].colorFree[firstPlayedCard.color] = PlayerInfo::Yes;
        }
    }

    int suggestCard(const ActivePile& pile)
    {
        struct RemainingCards
        {
            Card card;
            bool available = false;
        };

        RemainingCards remaining[numCardTypes * numColors];
        for (int i = 0; i < numCardTypes * numColors; ++i)
        {
            Card card = remaining[i].card = Card::fromHashValue(i);
            if (!m_game.discardPile.contains(card) && !m_player.hasCard(card) && !pile.contains(card))
                remaining[i].available = true;
        }

        for (int i = 0; i < Player::maxCards; ++i) {
            if (!m_player.m_cards[i])
                continue;
            if (!m_game.canPutCard(i))
                continue;

            ActivePile tmpPile = m_game.activePile;
            tmpPile.put(*m_player.m_cards[i], m_player.id);

            for (int i = tmpPile.numCards; i < 4; ++i) {

            }
        }

        return -1; // ### TODO
    }

    int doPlayCard(const ActivePile&) override
    {
        assert(false);
    }

    void updatePlayerInfo()
    {
        // record what we already know from our own colors
        m_playerInfo[m_player.id].trumpFree = PlayerInfo::Yes;
        m_playerInfo[m_player.id].colorFree[Eichel]
                = m_playerInfo[m_player.id].colorFree[Gras]
                = m_playerInfo[m_player.id].colorFree[Schelln]
                = m_playerInfo[m_player.id].colorFree[Herz] = PlayerInfo::Yes;
        for (const auto& card : m_player.m_cards) {
            if (!card)
                continue;

            const Card &c = *card;
            if (m_game.isTrump(c)) {
                m_playerInfo[m_player.id].trumpFree = PlayerInfo::No;
            } else {
                m_playerInfo[m_player.id].colorFree[c.color] = PlayerInfo::No;
            }
        }
    }

    void reset() override
    {
        m_gameInfo.reset(m_player);
        for (PlayerInfo& playerInfo : m_playerInfo)
            playerInfo.reset();
        updatePlayerInfo();
    }

    const Player& m_player;
    const Game& m_game;

    GameInfo m_gameInfo;
    PlayerInfo m_playerInfo[4];
};

}

inline std::ostream& operator<<(std::ostream& os, const SchafKopf::PlayerInfo& info)
{
    os << "Trump " << SchafKopf::PlayerInfo::toString(info.trumpFree);
    for (int i = 0; i < SchafKopf::numColors; ++i)
        os << " " << SchafKopf::colorNames[i] << "frei " << SchafKopf::PlayerInfo::toString(info.colorFree[i]);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const SchafKopf::GameInfo& info)
{
    os << "Game " << "trumps " << info.trumpsLeft << "/" << info.trumpCount;
    for (int i = 0; i < SchafKopf::numColors; ++i)
        os << ", color " << SchafKopf::colorNames[i] << " " << info.colorsLeft[i] << "/" << info.colorCardCount;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const SchafKopf::ObserverAi& ai)
{
    os << "ObserverAi for Player " << ai.m_player.id + 1 << std::endl;
    os << "    " << ai.m_gameInfo << std::endl;
    for (int i = 0; i < SchafKopf::numPlayers; ++i)
        os << "    Player " << i + 1 << ": " << ai.m_playerInfo[i] << std::endl;
    return os;
}
