#include "bedrock/world/inventory/network/item_stack_request_action_handler.h"

#include <iostream>

#include "bedrock/world/inventory/network/crafting/item_stack_request_action_craft_base.h"
#include "endstone/runtime/hook.h"

ItemStackNetResult ItemStackRequestActionHandler::handleRequestAction(ItemStackRequestAction const &action)
{
    std::cout << static_cast<int>(action.type) << "\n";
    if (action.type == ItemStackRequestActionType::CraftRecipe) {
        const auto *craft = reinterpret_cast<const ItemStackRequestActionCraftBase *>(&action);
        std::cout << craft->id.toString() << "\n";
    }
    return ENDSTONE_HOOK_CALL_ORIGINAL(&ItemStackRequestActionHandler::handleRequestAction, this, action);
}
