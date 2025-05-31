#include "bedrock/world/inventory/network/item_stack_request_action.h"
#include "bedrock/world/item/crafting/recipe.h"
class ItemStackRequestActionCraftBase : public ItemStackRequestAction {
public:
    RecipeNetId id;
};
