#pragma once

#include <BlackjackGame.h>
#include <Card.h>

#include <string>

namespace BlackjackUtils {
Card::Rank stringToRank(const std::string& str);
std::string rankToString(Card::Rank rank);
Card::Suit stringToSuit(const std::string& str);
std::string suitToString(Card::Suit suit);
std::string playerActionToString(BlackjackGame::PlayerAction action);
std::string surrenderTypeToString(BlackjackGame::SurrenderType type);
}  // namespace BlackjackUtils