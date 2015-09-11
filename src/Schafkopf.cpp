#include "Schafkopf.h"

namespace SchafKopf
{

Game::Game()
    : discardPile{players}
{
    players[0].id = 0;
    players[1].id = 1;
    players[2].id = 2;
    players[3].id = 3;
    ais[0] = ais[1] = ais[2] = ais[3] = nullptr;
    reset();
}

void Game::reset()
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

bool Game::canPutCard(int c) const
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

void Game::doStich()
{
    assert(activePile.numCards == numPlayers);

    Card pile[numPlayers];
    activePile.take(pile);

    int highestCard = 0;
    for (int i = 1; i < numPlayers; ++i) {
        if (sticht(pile[highestCard], pile[i]))
            highestCard = i;
    }

    int topPlayer = (m_activePlayer + highestCard) % 4;

    players[topPlayer].addStich(pile, m_activePlayer);

    // remember the player who did the last stich
    m_lastStichPlayer = topPlayer;
    // the player to come out is the player who scored last
    m_activePlayer = topPlayer;

    ++numStiche;
}

void Game::putCard(int c)
{
    assert(activePlayer().m_cards[c]);
    assert(canPutCard(c));

    Card card = *activePlayer().takeCard(c);
    activePile.put(std::move(card), m_activePlayer);

    for (auto &ai : ais) {
        if (ai)
            ai->cardPlayed(activePile, m_activePlayer);
    }

    ++m_activePlayer;

    // current round over - figure out the winner
    if (activePile.numCards == numPlayers)
        doStich();
}

bool Game::sticht(const Card& card, const Card& other) const
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

bool Game::isTrump(const Card& card) const
{
    switch (gameType) {
    case Solo:
    case SauSpiel:
        return card.cardType == CardType::Ober
                || card.cardType == CardType::Unter
                || card.color == gameColor;
    case Wenz:
        return card.cardType == CardType::Unter;
    case FarbWenz:
        return card.cardType == CardType::Unter
                || card.color == gameColor;
    case Geier:
        return card.cardType == CardType::Ober;
    case FarbGeier:
        return card.cardType == CardType::Ober
                || card.color == gameColor;
    }
}

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

std::ostream&operator<<(std::ostream& os, const SchafKopf::Card& dt)
{
    os << SchafKopf::colorNames[dt.color] << ' ' << SchafKopf::cardTypeNames[dt.cardType];
    return os;
}
