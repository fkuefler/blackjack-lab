#pragma once

#include <BlackjackGame.h>
#include <Card.h>

#include <string>

namespace BlackjackUtils {
// Convert a string representation of a card rank to its enum value
Card::Rank stringToRank(const std::string& str);
// Convert a card rank enum value to its string representation
std::string rankToString(Card::Rank rank);
// Convert a string representation of a card suit to its enum value
Card::Suit stringToSuit(const std::string& str);
// Convert a card suit enum value to its string representation
std::string suitToString(Card::Suit suit);
// Convert a string representation of a card rank to its integer value
int stringToValue(const std::string& str);
// Convert a PlayerAction enum value to its string representation
std::string playerActionToString(BlackjackGame::PlayerAction action);
// Convert a SurrenderType enum value to its string representation
std::string surrenderTypeToString(BlackjackGame::SurrenderType type);
}  // namespace BlackjackUtils