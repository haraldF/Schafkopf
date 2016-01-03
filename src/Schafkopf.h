#pragma once

#include <boost/math/special_functions/binomial.hpp>

#include <experimental/optional>

#include <algorithm>
#include <iostream>
#include <chrono>
#include <random>
#include <set>

namespace std
{
    using experimental::optional;
}

// Based on the 2007 Schafkopf rules, see
// http://www.schafkopfschule.de/index.php/regeln.html?file=files/inhalte/dokumente/Spielen/Regeln/Schafkopfregeln_Aktuell_29.3.2007.pdf

namespace SchafKopf
{

class Environment
{
public:
    std::default_random_engine& engine() { return m_engine; }

    static Environment &instance()
    {
        static Environment env;
        return env;
    }

private:
    std::default_random_engine m_engine;

    Environment()
        : m_engine(std::chrono::system_clock::now().time_since_epoch().count())
    {
    }

    Environment(const Environment&) = delete;
    Environment& operator=(const Environment&) = delete;
};

enum Color
{
    Schelln,
    Herz,
    Gras,
    Eichel
};

constexpr int numColors = 4;

static const char *colorNames[numColors] = {
    "Schelln",
    "Herz",
    "Gras",
    "Eichel"
};

enum CardType
{
    Siebner,
    Achter,
    Neuner,
    Unter,
    Ober,
    Koenig,
    Zehner,
    Ass
};

constexpr int numCardTypes = 8;

static const char *cardTypeNames[numCardTypes] = {
    "Siebner",
    "Achter",
    "Neuner",
    "Unter",
    "Ober",
    "Koenig",
    "Zehner",
    "Ass"
};

struct CardPoint
{
    CardType card;
    int value;
};

static const CardPoint cardPoints[] = {
    { CardType::Siebner, 0 },
    { CardType::Achter, 0 },
    { CardType::Neuner, 0 },
    { CardType::Unter, 2 },
    { CardType::Ober, 3 },
    { CardType::Koenig, 4 },
    { CardType::Zehner, 10 },
    { CardType::Ass, 11 }
};

static constexpr int numPlayers = 4;

struct Card
{
    CardType cardType;
    Color color;

    bool operator<(const Card& other) const
    {
        if (other.color == color)
            return cardType < other.cardType;
        return color < other.color;
    }

    int points() const
    {
        assert(cardType >= Siebner && cardType <= Ass);
        return cardPoints[cardType].value;
    }

    bool operator==(const Card& other) const { return cardType == other.cardType && color == other.color; }
    bool operator!=(const Card& other) const { return !(*this == other); }

    int hashValue() const
    {
        return (color * numCardTypes) + cardType;
    }

    static Card fromHashValue(int value)
    {
        Card result;
        result.color = static_cast<Color>(value % 4);
        result.cardType = static_cast<CardType>(value - (result.color * numCardTypes));
        return result;
    }
};

struct Deck
{
    Deck()
    {
        Card *card = cards;
        for (int i = 0; i < numCardTypes; ++i)
            for (int j = 0; j < numColors; ++j)
                *card++ = Card{ CardType(i), Color(j) };
    }

    void shuffle()
    {
        std::shuffle(cards, cards + numCards, Environment::instance().engine());
    }

    const Card* begin() const { return cards; }
    const Card* end() const { return cards + numCards; }

    static constexpr int numCards = numColors * numCardTypes;
    Card cards[numCards];
};

struct Stich
{
    bool contains(const Card &c) const
    {
        return std::find_if(begin(), end(), [&c](const std::pair<int, Card> &it) { return it.second == c; }) != end();
    }

    const std::pair<int, Card> *end() const { return cards + 4; }
    const std::pair<int, Card> *begin() const { return cards; }

    std::pair<int, Card> *end() { return cards + 4; }
    std::pair<int, Card> *begin() { return cards; }

    void reset() { std::for_each(begin(), end(), [](std::pair<int, Card> &c) { c.first = -1; c.second = Card{}; }); }

    std::pair<int, Card> cards[4];
};

struct PlayerId
{
    PlayerId(int playerId = 0)
        : id(playerId)
    {}

    int operator++() { return ++id; }
    int operator++(int) { return id++; }

    int operator--() { return --id; }
    int operator--(int) { return id--; }

    operator int() const { return int(id); }

    unsigned char id : 2;
};

struct Player
{
    Player()
        : id(-1),
          numStiche(0),
          points(0)
    {}

    void reset()
    {
        numStiche = 0;
        points = 0;

        for (auto& card : m_cards)
            card = std::optional<Card>();
        for (int i = 0; i < maxCards; ++i)
            m_stiche[i].reset();
    }

    void deal(const Card cards[8])
    {
        for (int i = 0; i < maxCards; ++i)
            m_cards[i] = cards[i];
    }

    std::optional<Card> takeCard(int c)
    {
        std::optional<Card> result;
        std::swap(m_cards[c], result);
        return result;
    }

    bool hasCard(const Card &card) const
    {
        // linear search is fastest as we only have 8 cards max
        for (const auto &c : m_cards) {
            if (c && *c == card)
                return true;
        }
        return false;
    }

    void addStich(Card cards[4], PlayerId activePlayer)
    {
        assert(numStiche >= 0 && numStiche < 8);
        assert(activePlayer >= 0 && activePlayer < numPlayers);

        for (int i = 0; i < numPlayers; ++i) {
            points += cards[i].points();
            m_stiche[numStiche].cards[i] = std::make_pair(activePlayer, std::move(cards[i]));
            ++activePlayer;
        }
        ++numStiche;
    }

    bool cardInStiche(const Card &card)
    {
        for (int i = 0; i < numStiche; ++i) {
            if (m_stiche[i].contains(card))
                return true;
        }

        return false;
    }

    const Stich &lastStich() const
    {
        assert(numStiche > 0);
        return m_stiche[numStiche - 1];
    }

    static constexpr int maxCards = 8;

    int id;
    int numStiche;
    int points;
    std::optional<Card> m_cards[maxCards];
    Stich m_stiche[maxCards];
};

struct DiscardPile
{
    bool contains(const Card& card) const
    {
        for (int i = 0; i < numPlayers; ++i) {
            if (players[i].cardInStiche(card))
                return true;
        }

        return false;
    }

    Player* players;
};

struct ActivePile
{
    ActivePile()
        : firstPlayer(0),
          numCards(0)
    {}

    void put(Card card, int playerId)
    {
        assert(numCards >= 0 && numCards <= 3);
        if (numCards == 0)
            firstPlayer = playerId;
        m_cards[numCards++] = std::move(card);
    }

    void take(Card cards[numPlayers])
    {
        for (int i = 0; i < numPlayers; ++i) {
            cards[i] = *m_cards[i];
            m_cards[i] = std::optional<Card>();
        }
        numCards = 0;
    }

    const Card& lastPlayedCard() const
    {
        assert(numCards > 0);
        return *m_cards[numCards - 1];
    }

    const Card& firstPlayedCard() const
    {
        assert(numCards > 0);
        return *m_cards[0];
    }

    bool isEmpty() const
    {
        return numCards == 0;
    }

    bool contains(const Card& card) const
    {
        for (int i = 0; i < numCards; ++i) {
            if (*m_cards[i] == card)
                return true;
        }
        return false;
    }

    int firstPlayer;
    int numCards;
    std::optional<Card> m_cards[numPlayers];
};

class AI
{
public:
    AI() {}
    virtual ~AI() {}

    AI(const AI&) = delete;
    AI& operator=(const AI&) = delete;

    virtual void cardPlayed(const ActivePile& pile, int activePlayer) = 0;
    virtual int doPlayCard(const ActivePile& pile) = 0;
    virtual void reset() = 0;
};

static const char *GameTypeNames[] =
{
    "Sauspiel",
    "Farbgeier",
    "Geier",
    "Farbwenz",
    "Wenz",
    "Solo"
};

struct Game
{
    enum Type
    {
        SauSpiel,
        FarbGeier,
        Geier,
        FarbWenz,
        Wenz,
        Solo
    };

    Player players[numPlayers];
    AI *ais[numPlayers];

    PlayerId m_activePlayer;
    int m_lastStichPlayer;
    int numStiche;

    Deck deck;
    DiscardPile discardPile;
    ActivePile activePile;

    Type gameType;
    Color gameColor;

    Game();

    void reset();

    inline const Player& activePlayer() const
    {
        return players[m_activePlayer];
    }

    inline Player& activePlayer()
    {
        return players[m_activePlayer];
    }

    inline Player& lastStichPlayer()
    {
        return players[m_lastStichPlayer];
    }

    inline const Card& firstPileCard() const
    {
        assert(activePile.m_cards[0]);
        return *activePile.m_cards[0];
    }

    // true if the card can be played
    bool canPutCard(const Card& card, const ActivePile& pile, const Player& player) const;
    bool canPutCard(int c) const;

    // figure out who won the round
    void doStich();

    // active player puts card
    void putCard(int c);

    bool sticht(const Card& card, const Card& other) const;

    int trumpScore(const Card& card) const;
    bool isTrump(const Card& card) const;

    bool hasTrump(const Player& player) const
    {
        for (const auto& card : player.m_cards) {
            if (card && isTrump(*card))
                return true;
        }
        return false;
    }

    bool hasColor(const Player& player, Color color) const
    {
        for (const auto& card : player.m_cards) {
            if (card && !isTrump(*card) && card->color == color)
                return true;
        }
        return false;
    }

    double stichProbability(const Player& player, const Card& card) const;
    double passProbabilty(const Player& player, const Card& card) const;
};

}

std::ostream& operator<<(std::ostream& os, const SchafKopf::Card& dt);
