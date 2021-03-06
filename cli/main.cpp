#include <Schafkopf.h>
#include <RandomAi.h>
#include <ObserverAi.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

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
                std::cout << "    P" << lastStich.cards[i].first + 1 << ": " << lastStich.cards[i].second << std::endl;
            std::cout << "Stich went to player " << int(game.m_lastStichPlayer) + 1 << std::endl;
        } else {
            printActivePile();
        }

        if (game.numStiche == 8) {
            std::cout << "Game over" << std::endl;
            for (int i = 0; i < numPlayers; ++i)
                std::cout << "    Player " << i + 1 << ": " << game.players[i].points << " points" << std::endl;

            // ### TODO - support sauspiel
            if (game.players[0].points > 60)
                std::cout << "Player 1 won!" << std::endl;
            else
                std::cout << "Player 1 lost!" << std::endl;
            game.reset();
        }
    }

    void printCards() const
    {
        std::cout << "Game: ";
        if (game.gameType != Game::Wenz && game.gameType != Game::Geier)
            std::cout << colorNames[game.gameColor] << " ";
        std::cout << GameTypeNames[game.gameType] << std::endl;

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
        for (int i = 0; i < 4; ++i) {
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

    void aiPlay()
    {
        if (game.m_activePlayer == 0) {
            std::cout << "No AI for Player 1" << std::endl;
            return;
        }

        int card = aiForPlayer(game.m_activePlayer).doPlayCard(game.activePile);
        std::cout << "Player " << int(game.m_activePlayer) + 1 << " plays " << *game.players[game.m_activePlayer].m_cards[card] << std::endl;
        playCard(card);
    }

    static Color toColorId(const std::string &color)
    {
        if (color == "schelln")
            return Color::Schelln;
        if (color == "herz")
            return Color::Herz;
        if (color == "gras")
            return Color::Gras;
        return Color::Eichel;
    }

    static Game::Type toGameType(const std::string& line)
    {
        if (line == "geier")
            return Game::Geier;
        if (line == "wenz")
            return Game::Wenz;
        if (line == "farbwenz")
            return Game::FarbWenz;
        if (line == "farbgeier")
            return Game::FarbGeier;
        return Game::Solo;
    }

    static bool getLine(std::string& line)
    {
        return bool(std::getline(std::cin, line));
    }

    bool handleCommand(const std::string& line)
    {
        if (line == "q")
            return false;

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
            aiPlay();
        } else if (line == "A") {
            while (game.m_activePlayer != 0)
                aiPlay();
        } else if (line == "r") {
            std::cout << "Reset game" << std::endl;
            newGame();
            printCards();
        } else if (line == "o") {
            printObservations();
        } else if (line == "schelln" || line == "herz" || line == "gras" || line == "eichel") {
            if (game.numStiche != 0 || !game.activePile.isEmpty()) {
                std::cout << "cannot change color during running game" << std::endl;
            } else {
                game.gameColor = toColorId(line);
                std::cout << "game color changed to " << line << std::endl;
            }
        } else if (line == "geier" || line == "wenz" || line == "farbwenz" || line == "farbgeier" || line == "solo") {
            if (game.numStiche != 0 || !game.activePile.isEmpty()) {
                std::cout << "cannot change type during running game" << std::endl;
            } else {
                game.gameType = toGameType(line);
                std::cout << "game type changed to " << line << std::endl;
            }
        } else if (line == "?") {
            std::cout << "Commands:" << std::endl
                      << "    'schelln',"  << std::endl
                      << "    'herz',"  << std::endl
                      << "    'gras',"  << std::endl
                      << "    'eichel'  : Color of the game" << std::endl
                      << "    'geier'," << std::endl
                      << "    'wenz'," << std::endl
                      << "    'farbwenz'," << std::endl
                      << "    'farbgeier'," << std::endl
                      << "    'solo'    : Type of the game" << std::endl
                      << "    '1' - '8' : Play the card with that number" << std::endl
                      << "    'p'       : Print current cards" << std::endl
                      << "    'pp'      : Print current active pile" << std::endl
                      << "    'a'       : Let AI play the next card" << std::endl
                      << "    'A'       : Let AI play until player's turn" << std::endl
                      << "    'r'       : Reset the game" << std::endl
                      << "    'o'       : Print observations made so far" << std::endl
                      << "    'q'       : Quit" << std::endl;
        } else {
            std::cout << "Unknown command: " << line << std::endl;
        }

        printPrompt();
        return true;
    }

    void start()
    {
        std::cout << "Schafkopf CLI v" << std::to_string(version) << ". Enter '?' for help." << std::endl;
        printPrompt();

#ifndef EMSCRIPTEN
        std::string line;
        while (getLine(line)) {
            if (!handleCommand(line))
                break;
        }
        std::cout << std::endl;
#endif
    }

    Game game;
    ObserverAi ai0;
    RandomAi ai1;
    RandomAi ai2;
    RandomAi ai3;
};

#ifdef EMSCRIPTEN
EM_BOOL key_callback(int eventType, const EmscriptenKeyboardEvent* ev, void* userData)
{
    static std::string lineBuffer;

    CLI* cli = reinterpret_cast<CLI*>(userData);

    if (ev->which == 13) {
        cli->handleCommand(lineBuffer);
        lineBuffer = std::string();
    } else {
        lineBuffer += char(ev->which);
    }

    return 1;
}

void main_loop()
{
    // std::cout << "main loop" << std::endl;
}
#endif

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

#ifdef EMSCRIPTEN
    emscripten_set_keypress_callback(0, &cli, 1, key_callback);
    emscripten_set_main_loop(main_loop, 0, true);
#endif

    std::cout << "Bye." << std::endl;

    return 0;
}
