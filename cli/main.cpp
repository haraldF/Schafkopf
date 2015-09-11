#include <Schafkopf.h>
#include <RandomAi.h>
#include <ObserverAi.h>

using namespace SchafKopf;

static constexpr int version = 1;

struct CLI
{
    CLI()
        : ai0{game, game.players[0]},
          ai1{game, game.players[1]},
          ai2{game, game.players[2]},
          ai3{game, game.players[3]}
    {
        newGame();
    }

    void newGame() {
        game.reset();

        game.gameType = Game::Solo;
        game.gameColor = Color::Herz;

        game.ais[0] = &ai0;
        game.ais[1] = &ai1;
        game.ais[2] = &ai2;
        game.ais[3] = &ai3;
    }

    void playCard(int c)
    {
        game.putCard(c);
        if (game.activePile.numCards == 0) {
            const Stich& lastStich = game.lastStichPlayer().lastStich();
            std::cout << "Stich:" << std::endl;
            for (int i = 0; i < numPlayers; ++i)
                std::cout << "    " << lastStich.cards[i].second << std::endl;
            std::cout << "Stich went to player " << int(game.m_lastStichPlayer) + 1 << std::endl;
        } else {
            printActivePile();
        }

        if (game.numStiche == 8) {
            std::cout << "Game over" << std::endl;
            for (int i = 0; i < numPlayers; ++i)
                std::cout << "    Player " << i + 1 << ": " << game.players[i].points << " points" << std::endl;
            game.reset();
        }
    }

    void printCards() const
    {
        for (int i = 0; i < numPlayers; ++i) {
            std::cout << "Player " << i + 1 << " (" << game.players[i].points << ")";
            if (game.m_activePlayer == i)
                std::cout << " *** Active ***";
            std::cout << std::endl;
            for (int j = 0; j < Player::maxCards; ++j)
                if (game.players[i].m_cards[j])
                    std::cout << "    " << j + 1 << ": " << *game.players[i].m_cards[j] << std::endl;
        }
    }

    static void printPrompt()
    {
        std::cout << "Schaf> " << std::flush;
    }

    void printActivePile() const
    {
        std::cout << "Active Pile" << std::endl;
        PlayerId playerId = game.activePile.firstPlayer;
        for (int i = 0; i < game.activePile.numCards; ++i) {
            std::cout << "    " << "Player " << playerId++ + 1 << ": " << *game.activePile.m_cards[i] << std::endl;
        }
    }

    AI& aiForPlayer(int player)
    {
        switch (player) {
        case 0:
            return ai0;
        case 1:
            return ai1;
        case 2:
            return ai2;
        case 3:
            return ai3;
        }
        assert(false);
        return ai0;
    }

    void printObservations() const
    {
        std::cout << "Observer AI:" << std::endl;
        for (int i = 1; i < 4; ++i) {
            std::cout << "    Player " << i + 1 << ":";
            if (ai0.m_playerInfo[i].trumpFree != PlayerInfo::Unknown)
                std::cout << " trump free: " << PlayerInfo::toString(ai0.m_playerInfo[i].trumpFree);
            for (int c = 0; c < numColors; ++c) {
                if (ai0.m_playerInfo[i].colorFree[c] != PlayerInfo::Unknown)
                    std::cout << " " << colorNames[c] << " free: " << PlayerInfo::toString(ai0.m_playerInfo[i].colorFree[c]);
            }
            std::cout << std::endl;
        }
    }

    void start()
    {
        std::cout << "Schafkopf CLI v" << std::to_string(version) << ". Enter '?' for help." << std::endl;
        printPrompt();

        std::string line;
        while (std::getline(std::cin, line)) {
            if (line == "q")
                break;

            if (line == "p") {
                printCards();
            } else if (line == "pp") {
                printActivePile();
            } else if (line == "1" || line == "2" || line == "3" || line == "4"
                      || line == "5" || line == "6" || line == "7" || line == "8") {
                char card = line.at(0) - '1';
                if (!game.activePlayer().m_cards[int(card)]) {
                    std::cout << "No such card: " << card << std::endl;
                } else if (!game.canPutCard(card)) {
                    std::cout << "Cannot play card " << card << std::endl;
                } else {
                    playCard(card);
                }
            } else if (line == "a") {
                if (game.m_activePlayer == 0) {
                    std::cout << "No AI for Player 1" << std::endl;
                } else {
                    int card = aiForPlayer(game.m_activePlayer).doPlayCard(game.activePile);
                    std::cout << "Player " << int(game.m_activePlayer) + 1 << " plays " << *game.players[game.m_activePlayer].m_cards[card] << std::endl;
                    playCard(card);
                }
            } else if (line == "r") {
                std::cout << "Reset game" << std::endl;
                newGame();
                printCards();
            } else if (line == "o") {
                printObservations();
            } else if (line == "?") {
                std::cout << "Commands:" << std::endl
                          << "    '1' - '8' : Play the card with that number" << std::endl
                          << "    'p'       : Print current cards" << std::endl
                          << "    'pp'      : Print current active pile" << std::endl
                          << "    'a'       : Let AI play the next card" << std::endl
                          << "    'r'       : Reset the game" << std::endl
                          << "    'o'       : Print observations made so far" << std::endl
                          << "    'q'       : Quit" << std::endl;
            } else {
                std::cout << "Unknown command: " << line << std::endl;
            }

            printPrompt();
        }
        std::cout << std::endl;
    }

    Game game;
    ObserverAi ai0;
    RandomAi ai1;
    RandomAi ai2;
    RandomAi ai3;
};

int main()
{
    Environment::instance();

    /*
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

    std::cout << (int)boost::math::binomial_coefficient<double>(32, 8) << std::endl;
    */

    CLI cli;
    cli.start();

    return 0;
}
