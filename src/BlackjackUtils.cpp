#include <BlackjackUtils.h>

#include <stdexcept>

Card::Rank stringToRank(const std::string& str) {
  if (str == "2") return Card::Rank::Two;
  if (str == "3") return Card::Rank::Three;
  if (str == "4") return Card::Rank::Four;
  if (str == "5") return Card::Rank::Five;
  if (str == "6") return Card::Rank::Six;
  if (str == "7") return Card::Rank::Seven;
  if (str == "8") return Card::Rank::Eight;
  if (str == "9") return Card::Rank::Nine;
  if (str == "10" || str == "T" || str == "t") return Card::Rank::Ten;
  if (str == "J" || str == "j") return Card::Rank::Jack;
  if (str == "Q" || str == "q") return Card::Rank::Queen;
  if (str == "K" || str == "k") return Card::Rank::King;
  if (str == "A" || str == "a") return Card::Rank::Ace;
  throw std::invalid_argument("Invalid rank string");
}

std::string rankToString(Card::Rank rank) {
  if (rank == Card::Rank::Two) return "2";
  if (rank == Card::Rank::Three) return "3";
  if (rank == Card::Rank::Four) return "4";
  if (rank == Card::Rank::Five) return "5";
  if (rank == Card::Rank::Six) return "6";
  if (rank == Card::Rank::Seven) return "7";
  if (rank == Card::Rank::Eight) return "8";
  if (rank == Card::Rank::Nine) return "9";
  if (rank == Card::Rank::Ten) return "T";
  if (rank == Card::Rank::Jack) return "J";
  if (rank == Card::Rank::Queen) return "Q";
  if (rank == Card::Rank::King) return "K";
  if (rank == Card::Rank::Ace) return "A";
  throw std::invalid_argument("Invalid rank");
}

Card::Suit stringToSuit(const std::string& str) {
  if (str == "H" || str == "h" || str == "Hearts") return Card::Suit::Hearts;
  if (str == "D" || str == "d" || str == "Diamonds")
    return Card::Suit::Diamonds;
  if (str == "C" || str == "c" || str == "Clubs") return Card::Suit::Clubs;
  if (str == "S" || str == "s" || str == "Spades") return Card::Suit::Spades;
  throw std::invalid_argument("Invalid suit string");
}

std::string suitToString(Card::Suit suit) {
  if (suit == Card::Suit::Hearts) return "Hearts";
  if (suit == Card::Suit::Diamonds) return "Diamonds";
  if (suit == Card::Suit::Clubs) return "Clubs";
  if (suit == Card::Suit::Spades) return "Spades";
  throw std::invalid_argument("Invalid suit");
}

std::string playerActionToString(BlackjackGame::PlayerAction action) {
  if (action == BlackjackGame::PlayerAction::Hit) return "Hit";
  if (action == BlackjackGame::PlayerAction::Stand) return "Stand";
  if (action == BlackjackGame::PlayerAction::Split) return "Split";
  if (action == BlackjackGame::PlayerAction::Double) return "Double";
  if (action == BlackjackGame::PlayerAction::Surrender) return "Surrender";
  if (action == BlackjackGame::PlayerAction::None) return "None";
  throw std::invalid_argument("Invalid player action");
}

std::string surrenderTypeToString(BlackjackGame::SurrenderType type) {
  if (type == BlackjackGame::SurrenderType::Early) return "Early";
  if (type == BlackjackGame::SurrenderType::Late) return "Late";
  if (type == BlackjackGame::SurrenderType::None) return "None";
  throw std::invalid_argument("Invalid surrender type");
}