// BlackjackEVCalculator.cpp : This file contains the 'main' function. Program
// execution begins and ends there.
//

#include <iostream>

#include "../include/BlackjackGame.h"

int main() {
  Deck testDeck = Deck(6);
  Card p1 = Card(Rank::Ten, Suit::Spades);
  Card p2 = Card(Rank::Ace, Suit::Hearts);
  Card d1 = Card(Rank::Nine, Suit::Clubs);

  // Remove the dealt cards from the deck to ensure correct remaining card
  // counts
  testDeck.dealCard(p1);
  testDeck.dealCard(p2);
  testDeck.dealCard(d1);

  Hand playerHand = Hand();
  playerHand.addCard(p1);
  playerHand.addCard(p2);

  BlackjackGame game = BlackjackGame();
  GameState state = game.getGameState(playerHand, d1, testDeck, true);
  double standEV = game.calculateEVForStand(state);
  std::string dealerOutcomes = game.getDealerOutcomesAsString(state);

  std::cout << "Player has 21 vs Dealer Nine" << std::endl;
  std::cout << "EV for standing: " << standEV << std::endl;
  std::cout << "Dealer Outcomes: " << std::endl;
  std::cout << dealerOutcomes << std::endl;

  return 0;
}