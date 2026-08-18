from endstone import Server
from endstone.level import Dimension

FAR_CHUNK = (30_000, 30_000)
NEVER_LOADED_CHUNK = (29_000, 29_000)


def test_a_far_chunk_is_not_loaded(server: Server) -> None:
    """Verify a chunk far outside the spawn area is not resident."""
    overworld = server.level.get_dimension(Dimension.OVERWORLD)
    assert overworld.is_chunk_loaded(*NEVER_LOADED_CHUNK) is False


def test_load_chunk_registers_a_ticket(server: Server) -> None:
    """Verify load_chunk accepts the request; it is honoured on a later tick."""
    overworld = server.level.get_dimension(Dimension.OVERWORLD)
    try:
        assert overworld.load_chunk(*FAR_CHUNK) is True
        assert overworld.is_chunk_loaded(*FAR_CHUNK) is False
    finally:
        overworld.unload_chunk(*FAR_CHUNK)


def test_unload_chunk_releases_the_ticket(server: Server) -> None:
    """Verify unload_chunk releases a ticket that load_chunk took."""
    overworld = server.level.get_dimension(Dimension.OVERWORLD)
    overworld.load_chunk(*FAR_CHUNK)
    assert overworld.unload_chunk(*FAR_CHUNK) is True


def test_loaded_chunks_is_a_list(server: Server) -> None:
    """Verify the dimension reports its resident chunks as a list."""
    overworld = server.level.get_dimension(Dimension.OVERWORLD)
    assert isinstance(overworld.loaded_chunks, list)
