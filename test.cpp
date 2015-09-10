#include "Schafkopf.h"

#include <gtest/gtest.h>

using namespace SchafKopf;

TEST(TestSchafKopf, stiche)
{
    Game game;
    game.gameType = Game::Solo;
    game.gameColor = Color::Herz;

    game.activePile.put(Card{Ass, Schelln});
    game.activePile.put(Card{Zehner, Schelln});
    game.activePile.put(Card{Unter, Schelln});
    game.activePile.put(Card{Siebner, Schelln});
    game.doStich();
    ASSERT_EQ(2, game.m_lastStichPlayer);

    game.activePile.put(Card{Zehner, Schelln});
    game.activePile.put(Card{Ass, Schelln});
    game.activePile.put(Card{Achter, Schelln});
    game.activePile.put(Card{Siebner, Schelln});
    game.doStich();
    ASSERT_EQ(1, game.m_lastStichPlayer);

    game.activePile.put(Card{Ober, Schelln});
    game.activePile.put(Card{Unter, Schelln});
    game.activePile.put(Card{Ober, Eichel});
    game.activePile.put(Card{Siebner, Schelln});
    game.doStich();
    ASSERT_EQ(2, game.m_lastStichPlayer);

    game.activePile.put(Card{Achter, Schelln});
    game.activePile.put(Card{Ass, Gras});
    game.activePile.put(Card{Ass, Eichel});
    game.activePile.put(Card{Siebner, Schelln});
    game.doStich();
    ASSERT_EQ(0, game.m_lastStichPlayer);
}
