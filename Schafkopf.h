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
    Eichel,
    Gras,
    Herz,
    Schelln
};

constexpr int numColors = 4;

static const char *colorNames[numColors] = {
    "Eichel",
    "Gras",
    "Herz",
    "Schelln"
};

enum CardType
{
    Ass,
    Zehner,
    Koenig,
    Ober,
    Unter,
    Neuner,
    Achter,
    Siebner
};

constexpr int numCardTypes = 8;

static const char *cardTypeNames[numCardTypes] = {
    "Ass",
    "Zehner",
    "Koenig",
    "Ober",
    "Unter",
    "Neuner",
    "Achter",
    "Siebner"
};

struct CardPoint
{
    CardType card;
    int value;
};

static const CardPoint cardPoints[] = {
    { CardType::Ass, 11 },
    { CardType::Zehner, 10 },
    { CardType::Koenig, 4 },
    { CardType::Ober, 3 },
    { CardType::Unter, 2 },
    { CardType::Neuner, 0 },
    { CardType::Achter, 0 },
    { CardType::Siebner, 0 }
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
        assert(cardType >= Ass && cardType <= Siebner);
        return cardPoints[cardType].value;
    }

    bool operator==(const Card& other) const { return cardType == other.cardType && color == other.color; }
    bool operator!=(const Card& other) const { return !(*this == other); }
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

struct Player
{
    Player()
        : numStiche(0),
          points(0)
    {}

    void reset()
    {
        numStiche = 0;
        points = 0;

        for (auto& card : m_cards)
            card = std::optional<Card>();
        for (int i = 0; i < maxCards; ++i) {
            for (int c = 0; c < numPlayers; ++c) {
                m_stiche[i][c] = std::optional<Card>();
            }
        }
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

    void addStich(Card cards[4])
    {
        assert(numStiche >= 0 && numStiche < 8);
        for (int i = 0; i < numPlayers; ++i) {
            points += cards[i].points();
            m_stiche[numStiche][i] = std::move(cards[i]);
        }
        ++numStiche;
    }

    bool cardInStiche(const Card &card)
    {
        for (int i = 0; i < maxCards; ++i) {
            if (!m_stiche[i][0])
                return false;
            for (int c = 0; c < numPlayers; ++c) {
                if (*m_stiche[i][c] == card)
                    return true;
            }
        }

        return false;
    }

    const std::optional<Card> *lastStich() const
    {
        assert(numStiche > 0);
        return m_stiche[numStiche - 1];
    }

    static constexpr int maxCards = 8;

    int numStiche;
    int points;
    std::optional<Card> m_cards[maxCards];
    std::optional<Card> m_stiche[maxCards][numPlayers];
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
        : numCards(0)
    {}

    void put(Card card)
    {
        assert(numCards >= 0 && numCards <= 3);
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

    int numCards;
    std::optional<Card> m_cards[numPlayers];
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

    // use a bitfield, so it'll wrap when calling operator++
    unsigned char m_activePlayer : 2;
    int m_lastStichPlayer;
    int numStiche;

    Deck deck;
    DiscardPile discardPile;
    ActivePile activePile;

    Type gameType;
    Color gameColor;

    Game()
        : discardPile{players}
    {
        reset();
    }

    void reset()
    {
        m_activePlayer = 0;
        m_lastStichPlayer = 0;
        numStiche = 0;

        Card cards[numPlayers];
        activePile.take(cards);

        deck.shuffle();
        for (int i = 0; i < numPlayers; ++i)
            players[i].deal(deck.begin() + (i * 8));
    }

    inline Player &activePlayer()
    {
        return players[m_activePlayer];
    }

    inline Player &lastStichPlayer()
    {
        return players[m_lastStichPlayer];
    }

    inline Card &firstPileCard()
    {
        assert(activePile.m_cards[0]);
        return *activePile.m_cards[0];
    }

    bool canPutCard(int c)
    {
        assert(activePlayer().m_cards[c]);

        const Card &card = *activePlayer().m_cards[c];

        if (activePile.numCards == 0) {

            if (gameType == SauSpiel) {
                // only one rule for first card - if it's a Sauspiel, one cannot
                // play the color of the Sau if one has the Sau, unless one can run away,
                // see chapter 2.5 of rules

                // ### TODO
            }
            return true;
        }

        const Card &firstCard = firstPileCard();
        if (isTrump(firstCard)) {
            if (hasTrump(activePlayer()))
                return isTrump(card);
            if (gameType == SauSpiel) {
                // ### TODO - cannot play Sau
            }
            return true; // trump is played but we don't have trump - play anything
        }

        if (hasColor(activePlayer(), firstCard.color)) {
            if (gameType == SauSpiel) {
                // ### TODO - must play Sau
            }
            return card.color == firstCard.color;
        }

        return true;
    }

    // figure out who won the round
    void doStich()
    {
        assert(activePile.numCards == numPlayers);

        Card pile[numPlayers];
        activePile.take(pile);

        int topPlayer = m_activePlayer;
        for (int i = 1; i < numPlayers; ++i) {
            if (sticht(pile[topPlayer], pile[i]))
                topPlayer = i;
        }

        players[topPlayer].addStich(pile);
        m_lastStichPlayer = topPlayer;

        ++numStiche;
    }

    // active player puts card
    void putCard(int c)
    {
        assert(activePlayer().m_cards[c]);
        assert(canPutCard(c));

        Card card = *activePlayer().takeCard(c);
        activePile.put(std::move(card));

        m_activePlayer++;

        // current round over - figure out the winner
        if (activePile.numCards == numPlayers)
            doStich();
    }

    bool sticht(const Card& card, const Card& other) const
    {
        // ### other game types
        if (card.cardType == CardType::Ober) {
            if (other.cardType == CardType::Ober)
                return other.color < card.color;
            else
                return false;
        }

        if (card.cardType == CardType::Unter) {
            if (other.cardType == CardType::Ober)
                return true;
            else if (other.cardType == CardType::Unter)
                return other.color < card.color;
            else
                return false;
        }

        if (card.color == gameColor) {
            if (other.cardType == CardType::Ober || other.cardType == CardType::Unter)
                return true;
            if (other.color == gameColor)
                return other.cardType < card.cardType;
            else
                return false;
        }

        if (other.cardType == CardType::Ober || other.cardType == CardType::Unter || other.color == gameColor)
            return true;
        if (card.color == other.color)
            return other.cardType < card.cardType;
        return false;
    }

    bool isTrump(const Card& card) const
    {
        return card.cardType == CardType::Ober
                || card.cardType == CardType::Unter
                || card.color == gameColor;
    }

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

double Game::stichProbability(const Player& player, const Card& card) const
{
    std::set<Card> higherCards;

    return 0.0;
}

double Game::passProbabilty(const Player& player, const Card& card) const
{
    /*
    if (isTrump(card))
        return 0.0;

    int higherCards = card.cardType;
    int trumps = 8 + 8;

    // ### check own cards, reduce higherCards + trumps

    int lowerCards = numCardTypes - (card.cardType + 1);
*/


    return 0.0;
}

}

std::ostream& operator<<(std::ostream& os, const SchafKopf::Card& dt)
{
    os << SchafKopf::colorNames[dt.color] << ' ' << SchafKopf::cardTypeNames[dt.cardType];
    return os;
}
