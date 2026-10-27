#include "ItemState.h"
#include "InvalidTransition.h"

void ItemState::assign(Item&) { throw InvalidTransition("Cannot assign an item in " + name()); }
void ItemState::start(Item&) { throw InvalidTransition("Cannot start an item in " + name()); }
void ItemState::complete(Item&, int) { throw InvalidTransition("Cannot complete an item in " + name()); }
void ItemState::reject(Item&, const std::string&) { throw InvalidTransition("Cannot reject an item in " + name()); }
