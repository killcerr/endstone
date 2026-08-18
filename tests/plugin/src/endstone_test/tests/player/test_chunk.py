from endstone import Player
from endstone.level import Chunk


def test_the_players_chunk_is_loaded(player: Player) -> None:
    """Verify the chunk the player is standing in is resident."""
    x, z = player.location.block_x >> 4, player.location.block_z >> 4
    assert player.dimension.is_chunk_loaded(x, z) is True


def test_loaded_chunks(player: Player) -> None:
    """Verify the dimension reports its resident chunks."""
    chunks = player.dimension.loaded_chunks
    assert len(chunks) > 0
    for chunk in chunks:
        assert isinstance(chunk, Chunk)


def test_chunk_properties(player: Player) -> None:
    """Verify a chunk knows its coordinates, level and dimension."""
    x, z = player.location.block_x >> 4, player.location.block_z >> 4
    chunk = next(c for c in player.dimension.loaded_chunks if (c.x, c.z) == (x, z))
    assert chunk.level is player.server.level
    assert chunk.dimension.id == player.dimension.id
    assert str(chunk)
    assert repr(chunk)


def test_loaded_chunks_agree_with_is_chunk_loaded(player: Player) -> None:
    """Verify every chunk in loaded_chunks reports itself as loaded."""
    for chunk in player.dimension.loaded_chunks[:16]:
        assert player.dimension.is_chunk_loaded(chunk.x, chunk.z) is True
