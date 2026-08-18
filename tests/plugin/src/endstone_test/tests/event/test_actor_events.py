from endstone_test.recorder import EventRecorder


def test_actor_spawn(recorder: EventRecorder) -> None:
    """Verify ActorSpawnEvent names a namespaced actor type."""
    for snapshot in recorder.require("ActorSpawnEvent"):
        assert ":" in snapshot["actor_type"]
        assert snapshot["python_type"]


def test_actor_remove(recorder: EventRecorder) -> None:
    """Verify ActorRemoveEvent names a namespaced actor type."""
    assert ":" in recorder.require("ActorRemoveEvent")[0]["actor_type"]


def test_actor_damage(recorder: EventRecorder) -> None:
    """Verify ActorDamageEvent reports a positive damage and a source type."""
    for snapshot in recorder.require("ActorDamageEvent"):
        assert snapshot["damage"] >= 0
        assert snapshot["damage_type"]


def test_actor_damage_is_cancellable(recorder: EventRecorder) -> None:
    """Verify ActorDamageEvent is delivered as a cancellable event."""
    assert all(s["is_cancellable"] for s in recorder.require("ActorDamageEvent"))


def test_actor_death(recorder: EventRecorder) -> None:
    """Verify ActorDeathEvent reports the damage source type."""
    snapshot = recorder.require("ActorDeathEvent")[0]
    assert ":" in snapshot["actor_type"]
    assert snapshot["damage_type"]


def test_player_death_also_fires_actor_death(recorder: EventRecorder) -> None:
    """Verify a player death reaches ActorDeathEvent, since Player is a Mob."""
    if recorder.count("PlayerDeathEvent"):
        types = {s["actor_type"] for s in recorder.require("ActorDeathEvent")}
        assert "minecraft:player" in types


def test_actor_knockback(recorder: EventRecorder) -> None:
    """Verify ActorKnockbackEvent carries a three-component knockback vector."""
    for snapshot in recorder.require("ActorKnockbackEvent"):
        assert len(snapshot["knockback"]) == 3


def test_actor_teleport(recorder: EventRecorder) -> None:
    """Verify ActorTeleportEvent moves the actor somewhere else."""
    assert (
        recorder.require("ActorTeleportEvent")[0]["from_xyz"]
        != recorder.snapshots("ActorTeleportEvent")[0]["to_xyz"]
    )


def test_actor_explode(recorder: EventRecorder) -> None:
    """Verify ActorExplodeEvent reports the explosion location and block list."""
    snapshot = recorder.require("ActorExplodeEvent")[0]
    assert snapshot["block_count"] >= 0
    assert isinstance(snapshot["y"], float)
