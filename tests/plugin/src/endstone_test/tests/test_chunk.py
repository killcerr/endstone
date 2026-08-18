from endstone import Server
from endstone.level import Chunk, Dimension

FAR_CHUNK = (30_000, 30_000)


def test_spawn_chunk_is_loaded(server: Server) -> None:
    """Verify the chunk at the world origin is resident."""
    overworld = server.level.get_dimension(Dimension.OVERWORLD)
    assert overworld.is_chunk_loaded(0, 0) is True


def test_a_far_chunk_is_not_loaded(server: Server) -> None:
    """Verify a chunk far outside the spawn area is not resident."""
    overworld = server.level.get_dimension(Dimension.OVERWORLD)
    assert overworld.is_chunk_loaded(*FAR_CHUNK) is False


def test_load_chunk_registers_a_ticket(server: Server) -> None:
    """Verify load_chunk accepts the request; it is honoured on a later tick."""
    overworld = server.level.get_dimension(Dimension.OVERWORLD)
    try:
        assert overworld.load_chunk(*FAR_CHUNK) is True
    finally:
        overworld.unload_chunk(*FAR_CHUNK)


def test_unload_chunk_releases_the_ticket(server: Server) -> None:
    """Verify unload_chunk releases a ticket that load_chunk took."""
    overworld = server.level.get_dimension(Dimension.OVERWORLD)
    overworld.load_chunk(*FAR_CHUNK)
    assert overworld.unload_chunk(*FAR_CHUNK) is True


def test_loaded_chunks(server: Server) -> None:
    """Verify the dimension reports its resident chunks."""
    overworld = server.level.get_dimension(Dimension.OVERWORLD)
    chunks = overworld.loaded_chunks
    assert len(chunks) > 0
    for chunk in chunks:
        assert isinstance(chunk, Chunk)


def test_chunk_properties(server: Server) -> None:
    """Verify a chunk knows its coordinates, level and dimension."""
    overworld = server.level.get_dimension(Dimension.OVERWORLD)
    chunk = overworld.loaded_chunks[0]
    assert isinstance(chunk.x, int)
    assert isinstance(chunk.z, int)
    assert chunk.level is server.level
    assert chunk.dimension.id == overworld.id
    assert str(chunk)
    assert repr(chunk)


def test_loaded_chunks_agree_with_is_chunk_loaded(server: Server) -> None:
    """Verify every chunk in loaded_chunks reports itself as loaded."""
    overworld = server.level.get_dimension(Dimension.OVERWORLD)
    for chunk in overworld.loaded_chunks[:16]:
        assert overworld.is_chunk_loaded(chunk.x, chunk.z) is True
