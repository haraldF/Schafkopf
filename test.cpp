#include "Schafkopf.h"

#include <gtest/gtest.h>

using namespace SchafKopf;


static void testStiche(Game &game, Card card1, Card card2, Card card3, Card card4, int winner)
{
    game.activePile.put(card1, game.m_activePlayer);
    game.activePile.put(card2, game.m_activePlayer);
    game.activePile.put(card3, game.m_activePlayer);
    game.activePile.put(card4, game.m_activePlayer);
    game.doStich();
    ASSERT_EQ(winner, game.m_lastStichPlayer);
}

static void testStiche(Card card1, Card card2, Card card3, Card card4, int winner)
{
    Game game;
    game.gameType = Game::Solo;
    game.gameColor = Color::Herz;

    ASSERT_NO_FATAL_FAILURE(testStiche(game, card1, card2, card3, card4, winner));
}

TEST(TestSchafKopf, stiche)
{
    ASSERT_NO_FATAL_FAILURE(
        testStiche(Card{Ass, Schelln},
                   Card{Zehner, Schelln},
                   Card{Unter, Schelln},
                   Card{Siebner, Schelln},
                   2));

    ASSERT_NO_FATAL_FAILURE(
        testStiche(Card{Zehner, Schelln},
                   Card{Ass, Schelln},
                   Card{Achter, Schelln},
                   Card{Siebner, Schelln},
                   1));

    ASSERT_NO_FATAL_FAILURE(
        testStiche(Card{Ober, Schelln},
                   Card{Unter, Schelln},
                   Card{Ober, Eichel},
                   Card{Siebner, Schelln},
                   2));

    ASSERT_NO_FATAL_FAILURE(
        testStiche(Card{Achter, Schelln},
                   Card{Ass, Gras},
                   Card{Ass, Eichel},
                   Card{Siebner, Schelln},
                   0));

    ASSERT_NO_FATAL_FAILURE(
        testStiche(Card{Achter, Schelln},
                   Card{Ass, Gras},
                   Card{Ass, Eichel},
                   Card{Siebner, Schelln},
                   0));
}

TEST(TestSchafKopf, sticheInGame)
{
    Game game;
    game.gameType = Game::Solo;
    game.gameColor = Color::Herz;

    ASSERT_NO_FATAL_FAILURE(
        testStiche(game,
                   Card{Ass, Schelln},
                   Card{Zehner, Schelln},
                   Card{Unter, Schelln},
                   Card{Siebner, Schelln},
                   2));

    ASSERT_NO_FATAL_FAILURE(
        testStiche(game,
                   Card{Zehner, Gras},
                   Card{Ass, Eichel},
                   Card{Achter, Schelln},
                   Card{Neuner, Schelln},
                   2));

    ASSERT_NO_FATAL_FAILURE(
        testStiche(game,
                   Card{Achter, Gras},
                   Card{Zehner, Eichel},
                   Card{Neuner, Eichel},
                   Card{Siebner, Herz},
                   1));
}
