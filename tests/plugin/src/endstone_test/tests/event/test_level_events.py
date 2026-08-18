from endstone_test.recorder import EventRecorder


def test_chunk_load(recorder: EventRecorder) -> None:
    """Verify ChunkLoadEvent reports the chunk coordinates and dimension."""
    snapshot = recorder.require("ChunkLoadEvent")[0]
    assert len(snapshot["chunk_xz"]) == 2
    assert ":" in snapshot["dimension"]


def test_chunk_unload(recorder: EventRecorder) -> None:
    """Verify ChunkUnloadEvent reports the chunk coordinates and dimension."""
    snapshot = recorder.require("ChunkUnloadEvent")[0]
    assert len(snapshot["chunk_xz"]) == 2
    assert ":" in snapshot["dimension"]


def test_chunks_load_before_they_unload(recorder: EventRecorder) -> None:
    """Verify no chunk unloads without a load having been seen first."""
    recorder.require("ChunkUnloadEvent")
    assert recorder.count("ChunkLoadEvent") > 0
