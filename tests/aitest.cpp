#include <Schafkopf.h>
#include <ObserverAi.h>
#include <RandomAi.h>

#include <gtest/gtest.h>

using namespace SchafKopf;

constexpr bool verbose = false;

static void dumpGame(const Game& game)
{
    std::cout << "Game: " << colorNames[game.gameColor] << " " << GameTypeNames[game.gameType] << std::endl;
    for (int player = 0; player < 4; ++player) {
        std::cout << "  Player " << player + 1 << ":" << std::endl;
        for (const auto &card : game.players[player].m_cards) {
            std::cout << "    ";
            if (!card)
                std::cout << "<played>" << std::endl;
            else
                std::cout << *card << std::endl;
        }
    }
}

// A simple test AI combining a random player and an observer
class TestAi : public AI
{
public:
    TestAi(const Game& game, const Player& player)
        : observerAi(game, player)
        , randomAi(game, player)
    {
    }

    void reset() override
    {
        observerAi.reset();
    }

    int doPlayCard(const ActivePile &pile) override
    {
        return randomAi.doPlayCard(pile);
    }

    void cardPlayed(const ActivePile& pile, int player) override
    {
        observerAi.cardPlayed(pile, player);
    }

    ObserverAi observerAi;
    RandomAi randomAi;
};

TEST(TestRandomAi, randomGame)
{
    Game game;
    game.gameType = Game::Solo;
    game.gameColor = Color::Herz;

    RandomAi randomAi[4] = {
        {game, game.players[0]},
        {game, game.players[1]},
        {game, game.players[2]},
        {game, game.players[3]}
    };

    for (int i = 0; i < 4; ++i)
        game.ais[i] = &randomAi[i];

    for (int round = 0; round < 8; ++round) {
        for (int player = 0; player < 4; ++player) {
            int card = randomAi[game.m_activePlayer].doPlayCard(game.activePile);
            game.putCard(card);
        }
    }

    // make sure 8 rounds were played
    GTEST_ASSERT_EQ(8, game.numStiche);

    // total acquired points must be 120
    int points = 0;
    for(const Player& player : game.players)
        points += player.points;
    GTEST_ASSERT_EQ(120, points);
}

void testObservations(const Game& game, const TestAi testAi[4])
{
    for (int player = 0; player < 4; ++player) {
        bool hasTrump = false;
        bool hasColor[4] = { false, false, false, false };

        for (const auto &card : game.players[player].m_cards) {
            if (!card)
                continue;
            if (game.isTrump(*card))
                hasTrump = true;
            else
                hasColor[card->color] = true;
        }

        for (int i = 0; i < 4; ++i) {
            const PlayerInfo &info = testAi[i].observerAi.m_playerInfo[player];
            if (info.trumpFree == PlayerInfo::Yes)
                ASSERT_FALSE(hasTrump);
            else if (info.trumpFree == PlayerInfo::No)
                ASSERT_TRUE(hasTrump);

            for (int color = 0; color < 4; ++color) {
                if (info.colorFree[color] == PlayerInfo::Yes)
                    ASSERT_FALSE(hasColor[color]) << "Player " << player + 1 << " ai " << i + 1 <<  " color "
                                                  << colorNames[color] << std::endl << game.activePile << std::endl << testAi[i].observerAi;
                else if (info.colorFree[color] == PlayerInfo::No)
                    ASSERT_TRUE(hasColor[color]) << "Player " << player + 1 << " ai " << i + 1 << " color " << colorNames[color] << std::endl << game.activePile;
            }
        }
    }
}

TEST(TestAi, randomObservedGame)
{
    Game game;
    game.gameType = Game::Solo;
    game.gameColor = Color::Herz;

    TestAi testAi[4] = {
        {game, game.players[0]},
        {game, game.players[1]},
        {game, game.players[2]},
        {game, game.players[3]}
    };

    for (int i = 0; i < 4; ++i)
        game.ais[i] = &testAi[i];

    if (verbose)
        dumpGame(game);

    // make sure that our assumptions are true even before the first card is played
    ASSERT_NO_FATAL_FAILURE(testObservations(game, testAi));

    for (int round = 0; round < 8; ++round) {
        for (int player = 0; player < 4; ++player) {
            int card = testAi[game.m_activePlayer].doPlayCard(game.activePile);
            game.putCard(card);

            ASSERT_NO_FATAL_FAILURE(testObservations(game, testAi));
        }

        if (verbose) {
            std::cout << game.lastStichPlayer().lastStich() << std::endl;
            std::cout << "-> winner: " << game.m_lastStichPlayer + 1 << std::endl;
        }
    }
}
