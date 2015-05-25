#include <algorithm>
#include <iostream>
#include <chrono>
#include <random>
#include <set>

#include <experimental/optional>

namespace std
{
    using experimental::optional;
}

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

struct Value
{
    CardType card;
    int value;
};

static const Value values[] = {
    { CardType::Ass, 11 },
    { CardType::Zehner, 10 },
    { CardType::Koenig, 4 },
    { CardType::Ober, 3 },
    { CardType::Unter, 2 },
    { CardType::Neuner, 0 },
    { CardType::Achter, 0 },
    { CardType::Siebner, 0 }
};

struct Card
{
    CardType cardType;
    Color color;

    bool operator<(const Card& other)
    {
        if (other.color == color)
            return cardType < other.cardType;
        return color < other.color;
    }
};

}

using namespace SchafKopf;

std::ostream& operator<<(std::ostream& os, const Card& dt)
{
    os << colorNames[dt.color] << ' ' << cardTypeNames[dt.cardType];
    return os;
}

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
    void deal(const Card cards[8])
    {
        for (int i = 0; i < maxCards; ++i)
            m_cards[i] = cards[i];
    }

    static constexpr int maxCards = 8;

    std::optional<Card> m_cards[maxCards];
};

struct DiscardPile
{
    std::optional<Card> m_cards[Deck::numCards];
};

struct Game
{
    enum Type
    {
        Sau,
        FarbGeier,
        Geier,
        FarbWenz,
        Wenz,
        Solo
    };

    Deck deck;
    DiscardPile pile;
    Player players[4];

    Type type;
    Color color;

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

        if (card.color == color) {
            if (other.cardType == CardType::Ober || other.cardType == CardType::Unter)
                return true;
            if (other.color == color)
                return other.cardType < card.cardType;
            else
                return false;
        }

        if (other.cardType == CardType::Ober || other.cardType == CardType::Unter || other.color == color)
            return true;
        if (card.color == other.color)
            return other.cardType < card.cardType;
        return false;
    }

    double stichProbability(const Player& player, const Card& card) const;
};

double Game::stichProbability(const Player &player, const Card &card) const
{
    std::set<Card> higherCards;

    return 0.0;
}

int main()
{
    Environment::instance();

    Game game;
    game.deck.shuffle();

    game.type = Game::Solo;
    game.color = Color::Herz;

    for (int i = 0; i < 4; ++i) {
        game.players[i].deal(game.deck.begin() + (i * 8));

        std::cout << "Player " << i + 1 << std::endl;
        for (std::optional<Card> &card : game.players[i].m_cards)
            std::cout << "    " << *card << std::endl;
    }

    //std::cout << game.stichProbability(game.players[0], *game.players[0].m_cards[0]) << std::endl;
    std::cout << game.sticht(*game.players[0].m_cards[0], *game.players[1].m_cards[0]) << std::endl;
    std::cout << game.sticht(*game.players[0].m_cards[0], *game.players[2].m_cards[0]) << std::endl;
    std::cout << game.sticht(*game.players[0].m_cards[0], *game.players[3].m_cards[0]) << std::endl;

    return 0;
}
