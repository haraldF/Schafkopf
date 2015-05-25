#include <algorithm>
#include <iostream>
#include <chrono>
#include <random>

#include <ctime>

class Environment
{
public:
    const std::default_random_engine& engine() const { return m_engine; }

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
    CardType card;
    Color color;
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
        std::random_shuffle(cards, cards + numCards);
    }

    static constexpr int numCards = numColors * numCardTypes;
    Card cards[numCards];
};

int main()
{
    std::srand ( unsigned ( std::time(nullptr) ) );
}
