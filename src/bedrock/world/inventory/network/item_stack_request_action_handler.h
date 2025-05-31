#pragma once
#include "bedrock/bedrock.h"
#include "bedrock/world/inventory/network/item_stack_net_result.h"
#include "bedrock/world/inventory/network/item_stack_request_action.h"
class ItemStackRequestActionHandler {
public:
    ENDSTONE_HOOK ItemStackNetResult handleRequestAction(ItemStackRequestAction const &);
};
