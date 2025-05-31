#pragma once
class Level;
#include "bedrock/core/utility/binary_stream.h"
#include "bedrock/world/inventory/network/item_stack_request_action_type.h"
#include "bedrock/world/level/block_palette.h"
class ItemStackRequestActionCraftBase;
class ItemStackRequestAction {
public:
    ItemStackRequestActionType type;
    virtual ~ItemStackRequestAction() = 0;
    [[nodiscard]] virtual ItemStackRequestActionCraftBase const *getCraftAction() const = 0;
    [[nodiscard]] virtual int getFilteredStringIndex() const = 0;
    virtual void postLoadItems_DEPRECATEDASKTYLAING(::BlockPalette &, bool) = 0; /*NOLINT*/
    virtual void _write(::BinaryStream &) const = 0;                             /*NOLINT*/
    virtual ::Bedrock::Result<void> _read(::ReadOnlyBinaryStream &) = 0;         /*NOLINT*/
};
