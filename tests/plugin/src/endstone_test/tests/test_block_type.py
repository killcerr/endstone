import pytest
from endstone import Identifier, Server
from endstone.block import BlockData, BlockType

# =============================================================================
# Fixtures
# =============================================================================


@pytest.fixture
def stone(server: Server) -> BlockType:
    return server.get_registry(BlockType).get_or_throw("minecraft:stone")


# =============================================================================
# Section 1: BlockType identity
# =============================================================================


def test_id_is_an_identifier(stone: BlockType) -> None:
    """Verify BlockType.id is an Identifier, not a string."""
    assert isinstance(stone.id, Identifier)
    assert stone.id.namespace == "minecraft"
    assert stone.id.key == "stone"


def test_str_is_the_plain_namespaced_id(stone: BlockType) -> None:
    """Verify str() gives the plain namespace:key form, not the Identifier repr."""
    assert str(stone) == "minecraft:stone"


def test_repr_names_the_type(stone: BlockType) -> None:
    """Verify repr() wraps the id in the class name."""
    assert repr(stone) == "BlockType(minecraft:stone)"


def test_equality_with_string(stone: BlockType) -> None:
    """Verify a block type compares equal to its namespaced string."""
    assert stone == "minecraft:stone"
    assert stone != "minecraft:dirt"


def test_hashes_like_its_string(stone: BlockType) -> None:
    """Verify a block type hashes like its string form."""
    assert hash(stone) == hash("minecraft:stone")


def test_is_not_a_string_subclass(stone: BlockType) -> None:
    """Verify a block type is a real object rather than a str."""
    assert not isinstance(stone, str)
    assert not hasattr(stone, "startswith")


def test_translation_key(stone: BlockType) -> None:
    """Verify a block type exposes a non-empty translation key."""
    assert stone.translation_key
    assert isinstance(stone.translation_key, str)


def test_has_item_type(stone: BlockType) -> None:
    """Verify stone reports that it has a matching item type."""
    assert stone.has_item_type is True


def test_get_returns_none_for_an_unknown_type(server: Server) -> None:
    """Verify BlockType.get returns None rather than raising for a bogus id."""
    assert server.get_registry(BlockType).get("minecraft:not_a_block") is None


# =============================================================================
# Section 2: Block.type and BlockData
# =============================================================================


def test_create_block_data_from_the_type(stone: BlockType) -> None:
    """Verify a block type can build its own default block data."""
    data = stone.create_block_data()
    assert isinstance(data, BlockData)
    assert data.type == "minecraft:stone"


def test_block_data_type_is_a_block_type(server: Server) -> None:
    """Verify BlockData.type returns a BlockType object, not a string."""
    data = server.create_block_data("minecraft:stone")
    assert isinstance(data.type, BlockType)
    assert data.type == "minecraft:stone"
    assert str(data.type) == "minecraft:stone"


def test_block_data_translation_key(server: Server) -> None:
    """Verify BlockData exposes a translation key."""
    data = server.create_block_data("minecraft:stone")
    assert data.translation_key
    assert data.translation_key == data.type.translation_key


def test_create_block_data_accepts_an_identifier(server: Server) -> None:
    """Verify create_block_data takes an Identifier as well as a plain string."""
    from_string = server.create_block_data("minecraft:stone")
    from_identifier = server.create_block_data(Identifier("minecraft", "stone"))
    assert from_string.type == from_identifier.type


def test_air_constants(server: Server) -> None:
    """Verify the AIR constants are Identifiers usable wherever an id is expected."""
    from endstone.inventory import ItemType

    assert isinstance(BlockType.AIR, Identifier)
    assert isinstance(ItemType.AIR, Identifier)
    assert BlockType.AIR == "minecraft:air"
    assert ItemType.AIR == "minecraft:air"
    assert BlockType.AIR.namespace == "minecraft"
    assert BlockType.AIR.key == "air"


def test_create_block_data_accepts_a_block_type_constant(server: Server) -> None:
    """Verify BlockType.AIR is accepted where a block id is expected."""
    assert server.create_block_data(BlockType.AIR).type == "minecraft:air"
