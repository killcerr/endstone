from endstone import Player
from endstone.block import Biome


def test_block_reports_its_biome(player: Player) -> None:
    """Verify the block under the player knows which biome it is in."""
    block = player.dimension.get_block_at(player.location)
    assert isinstance(block.biome, Biome)
    assert ":" in str(block.biome.id)


def test_biome_is_in_the_registry(player: Player) -> None:
    """Verify a biome read off a block is the same object the registry holds."""
    block = player.dimension.get_block_at(player.location)
    registry = player.server.get_registry(Biome)
    assert registry.get(str(block.biome.id)) is block.biome


def test_neighbouring_blocks_share_a_biome(player: Player) -> None:
    """Verify a block and the one above it report the same biome."""
    location = player.location
    dimension = player.dimension
    here = dimension.get_block_at(
        x=location.block_x, y=location.block_y, z=location.block_z
    )
    above = dimension.get_block_at(
        x=location.block_x, y=location.block_y + 1, z=location.block_z
    )
    assert here.biome == above.biome
