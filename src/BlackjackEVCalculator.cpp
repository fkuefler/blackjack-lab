// BlackjackEVCalculator.cpp : This file contains the 'main' function. Program
// execution begins and ends there.
//

#include <iostream>

#include "../include/BlackjackGame.h"

// Helper function to create a card from a string like "A", "K", "7"
Card cardFromStr(const std::string& s) {
  Rank r;
  if (s == "A")
    r = Rank::Ace;
  else if (s == "2")
    r = Rank::Two;
  else if (s == "3")
    r = Rank::Three;
  else if (s == "4")
    r = Rank::Four;
  else if (s == "5")
    r = Rank::Five;
  else if (s == "6")
    r = Rank::Six;
  else if (s == "7")
    r = Rank::Seven;
  else if (s == "8")
    r = Rank::Eight;
  else if (s == "9")
    r = Rank::Nine;
  else if (s == "T" || s == "J" || s == "Q" || s == "K")
    r = Rank::Ten;  // Group 10-value cards
  else
    throw std::invalid_argument("Invalid card rank input");
  return Card(r, Suit::Hearts);  // Suit doesn't matter for calculations
}

int main() {
  std::string p1_str, p2_str, d1_str;
  std::cout << "Enter player's first card (A, 2-9, T, J, Q, K): ";
  std::cin >> p1_str;
  std::cout << "Enter player's second card: ";
  std::cin >> p2_str;
  std::cout << "Enter dealer's up card: ";
  std::cin >> d1_str;

  Card p1 = cardFromStr(p1_str);
  Card p2 = cardFromStr(p2_str);
  Card d1 = cardFromStr(d1_str);

  Deck testDeck = Deck(6);
  testDeck.dealCard(p1);
  testDeck.dealCard(p2);
  testDeck.dealCard(d1);

  Hand playerHand = Hand();
  playerHand.addCard(p1);
  playerHand.addCard(p2);

  BlackjackGame game = BlackjackGame();
  GameState state = game.getGameState(playerHand, d1, testDeck, true);

  std::cout << "\n--- Results ---" << std::endl;
  std::cout << "Player has " << playerHand.getValue() << " vs Dealer "
            << d1.getValue() << std::endl;

  double standEV = game.calculateEVForStand(state);
  std::cout << "EV for standing: " << standEV << std::endl;
  double doubleEV = game.calculateEVForDouble(state);
  std::cout << "EV for doubling: " << doubleEV << std::endl;
  double splitEV = game.calculateEVForSplit(state);
  std::cout << "EV for splitting: " << splitEV << std::endl;
  double hitEV = game.calculateEVForHit(state);
  std::cout << "EV for hitting: " << hitEV << std::endl;
  double surrenderEV = game.calculateEVForSurrender(state);
  std::cout << "EV for surrender: " << surrenderEV << std::endl;
  double insuranceEV = game.calculateEVForInsurance(state);
  std::cout << "EV for insurance: " << insuranceEV << std::endl;
  double optimalEV = game.calculateEVForOptimalStrategy(state);
  std::cout << "EV for optimal strategy: " << optimalEV << std::endl;

  return 0;
}