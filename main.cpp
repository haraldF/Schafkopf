#include "Schafkopf.h"

using namespace SchafKopf;

struct CLI
{
    CLI()
    {
        newGame();
    }

    void newGame() {
        game.reset();

        game.gameType = Game::Solo;
        game.gameColor = Color::Herz;
    }

    void playCard(int c)
    {
        game.putCard(c);
        if (game.activePile.numCards == 0) {
            const std::optional<Card>* lastStich = game.lastStichPlayer().lastStich();
            std::cout << "Stich:" << std::endl;
            for (int i = 0; i < numPlayers; ++i)
                std::cout << "    " << *lastStich[i] << std::endl;
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

    void printCards()
    {
        for (int i = 0; i < numPlayers; ++i) {
            std::cout << "Player " << i + 1 << " (" << game.players[i].points << ")" << std::endl;
            for (int j = 0; j < Player::maxCards; ++j)
                if (game.players[i].m_cards[j])
                    std::cout << "    " << j + 1 << ": " << *game.players[i].m_cards[j] << std::endl;
        }
    }

    void printPrompt()
    {
        std::cout << "Schaf> " << std::flush;
    }

    void printActivePile()
    {
        std::cout << "Active Pile" << std::endl;
        for (int i = 0; i < game.activePile.numCards; ++i)
            std::cout << "    " << *game.activePile.m_cards[i] << std::endl;
    }

    void start()
    {
        printCards();
        printPrompt();

        std::string line;
        while (std::getline(std::cin, line)) {
            if (line == "q")
                break;

            if (line == "p") {
                printCards();
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
            } else {
                std::cout << "Unknown command: " << line << std::endl;
            }

            printPrompt();
        }
        std::cout << std::endl;
    }

    Game game;
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
