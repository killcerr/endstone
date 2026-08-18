import pytest

from endstone_test.recorder import EventRecorder

# =============================================================================
# Section 1: Connection
# =============================================================================


def test_player_login(recorder: EventRecorder) -> None:
    """Verify PlayerLoginEvent reports the player's skin."""
    snapshot = recorder.require("PlayerLoginEvent")[0]
    assert snapshot["player"]
    assert snapshot["skin_id"]
    assert len(snapshot["skin_shape"]) == 3
    assert snapshot["skin_shape"][2] == 4


def test_player_join(recorder: EventRecorder) -> None:
    """Verify PlayerJoinEvent exposes the joining player's identity."""
    snapshot = recorder.require("PlayerJoinEvent")[0]
    assert snapshot["player"]
    assert snapshot["unique_id"]
    assert snapshot["runtime_id"] > 0
    assert snapshot["ping"] >= 0
    assert isinstance(snapshot["is_op"], bool)


def test_player_quit(recorder: EventRecorder) -> None:
    """Verify PlayerQuitEvent names the departing player."""
    assert recorder.require("PlayerQuitEvent")[0]["player"]


def test_player_kick(recorder: EventRecorder) -> None:
    """Verify PlayerKickEvent carries a reason."""
    snapshot = recorder.require("PlayerKickEvent")[0]
    assert snapshot["player"]
    assert isinstance(snapshot["reason"], str)


# =============================================================================
# Section 2: Movement
# =============================================================================


def test_player_move(recorder: EventRecorder) -> None:
    """Verify PlayerMoveEvent fires only when position or rotation changed."""
    for snapshot in recorder.require("PlayerMoveEvent"):
        assert (
            snapshot["from_xyz"] != snapshot["to_xyz"]
            or snapshot["from_rot"] != snapshot["to_rot"]
        )


def test_player_jump(recorder: EventRecorder) -> None:
    """Verify PlayerJumpEvent always moves the player upwards."""
    for snapshot in recorder.require("PlayerJumpEvent"):
        assert snapshot["to_xyz"][1] > snapshot["from_xyz"][1]


def test_player_teleport(recorder: EventRecorder) -> None:
    """Verify PlayerTeleportEvent records both endpoints."""
    snapshot = recorder.require("PlayerTeleportEvent")[0]
    assert len(snapshot["from_xyz"]) == 3
    assert len(snapshot["to_xyz"]) == 3


def test_player_portal(recorder: EventRecorder) -> None:
    """Verify PlayerPortalEvent moves the player somewhere else."""
    snapshot = recorder.require("PlayerPortalEvent")[0]
    assert snapshot["from_xyz"] != snapshot["to_xyz"]


def test_player_dimension_change(recorder: EventRecorder) -> None:
    """Verify PlayerDimensionChangeEvent reports two different dimensions."""
    snapshot = recorder.require("PlayerDimensionChangeEvent")[0]
    assert snapshot["from_dimension"] != snapshot["to_dimension"]
    assert ":" in snapshot["from_dimension"]


def test_player_toggle_sneak(recorder: EventRecorder) -> None:
    """Verify PlayerToggleSneakEvent reports the new sneaking state."""
    for snapshot in recorder.require("PlayerToggleSneakEvent"):
        assert isinstance(snapshot["is_sneaking"], bool)
    states = {s["is_sneaking"] for s in recorder.snapshots("PlayerToggleSneakEvent")}
    if recorder.count("PlayerToggleSneakEvent") > 1:
        assert states == {True, False}


def test_player_toggle_sprint(recorder: EventRecorder) -> None:
    """Verify PlayerToggleSprintEvent reports the new sprinting state."""
    for snapshot in recorder.require("PlayerToggleSprintEvent"):
        assert isinstance(snapshot["is_sprinting"], bool)


# =============================================================================
# Section 3: Interaction
# =============================================================================


def test_player_interact(recorder: EventRecorder) -> None:
    """Verify PlayerInteractEvent reports a known action."""
    actions = {s["action"] for s in recorder.require("PlayerInteractEvent")}
    assert actions
    for action in actions:
        assert "CLICK" in action.upper()


def test_player_interact_records_block_only_when_present(
    recorder: EventRecorder,
) -> None:
    """Verify PlayerInteractEvent.has_block agrees with the recorded block."""
    for snapshot in recorder.require("PlayerInteractEvent"):
        assert (snapshot["block_type"] is not None) == snapshot["has_block"]


def test_player_interact_actor(recorder: EventRecorder) -> None:
    """Verify PlayerInteractActorEvent names the actor type."""
    assert ":" in recorder.require("PlayerInteractActorEvent")[0]["actor_type"]


def test_player_item_held(recorder: EventRecorder) -> None:
    """Verify PlayerItemHeldEvent reports a real slot change."""
    for snapshot in recorder.require("PlayerItemHeldEvent"):
        assert 0 <= snapshot["new_slot"] <= 8
        assert 0 <= snapshot["previous_slot"] <= 8


def test_player_drop_item(recorder: EventRecorder) -> None:
    """Verify PlayerDropItemEvent names the dropped item type."""
    assert ":" in recorder.require("PlayerDropItemEvent")[0]["item_type"]


def test_player_pickup_item(recorder: EventRecorder) -> None:
    """Verify PlayerPickupItemEvent reads through the Item actor's stack."""
    assert ":" in recorder.require("PlayerPickupItemEvent")[0]["item_type"]


def test_player_item_consume(recorder: EventRecorder) -> None:
    """Verify PlayerItemConsumeEvent reports the item and the hand used."""
    snapshot = recorder.require("PlayerItemConsumeEvent")[0]
    assert ":" in snapshot["item_type"]
    assert snapshot["hand"]


def test_player_chat(recorder: EventRecorder) -> None:
    """Verify PlayerChatEvent carries a message and its recipients."""
    snapshot = recorder.require("PlayerChatEvent")[0]
    assert snapshot["message"]
    assert snapshot["recipient_count"] >= 1


def test_player_command(recorder: EventRecorder) -> None:
    """Verify PlayerCommandEvent records the command line."""
    assert recorder.require("PlayerCommandEvent")[0]["command"]


def test_player_emote(recorder: EventRecorder) -> None:
    """Verify PlayerEmoteEvent carries an emote id."""
    assert recorder.require("PlayerEmoteEvent")[0]["emote_id"]


# =============================================================================
# Section 4: Bed, death and respawn
# =============================================================================


def test_player_bed_enter(recorder: EventRecorder) -> None:
    """Verify PlayerBedEnterEvent points at a bed block."""
    assert "bed" in recorder.require("PlayerBedEnterEvent")[0]["bed_type"]


def test_player_bed_leave(recorder: EventRecorder) -> None:
    """Verify PlayerBedLeaveEvent points at a bed block."""
    assert "bed" in recorder.require("PlayerBedLeaveEvent")[0]["bed_type"]


def test_player_death(recorder: EventRecorder) -> None:
    """Verify PlayerDeathEvent reports the damage source type."""
    snapshot = recorder.require("PlayerDeathEvent")[0]
    assert snapshot["player"]
    assert snapshot["damage_type"]


def test_player_respawn(recorder: EventRecorder) -> None:
    """Verify PlayerRespawnEvent reports a known RespawnReason."""
    for snapshot in recorder.require("PlayerRespawnEvent"):
        assert snapshot["respawn_reason"] in ("DEATH", "END_PORTAL")


def test_player_death_precedes_respawn(recorder: EventRecorder) -> None:
    """Verify a DEATH respawn is only recorded after a death."""
    reasons = [s["respawn_reason"] for s in recorder.require("PlayerRespawnEvent")]
    if "DEATH" in reasons:
        assert recorder.count("PlayerDeathEvent") > 0


# =============================================================================
# Section 5: Miscellaneous
# =============================================================================


def test_player_game_mode_change(recorder: EventRecorder) -> None:
    """Verify PlayerGameModeChangeEvent reports the new game mode."""
    assert recorder.require("PlayerGameModeChangeEvent")[0]["new_game_mode"]


def test_player_skin_change(recorder: EventRecorder) -> None:
    """Verify PlayerSkinChangeEvent carries the new skin id."""
    assert recorder.require("PlayerSkinChangeEvent")[0]["new_skin_id"]


@pytest.mark.parametrize(
    "event_name",
    [
        "PlayerToggleSneakEvent",
        "PlayerToggleSprintEvent",
        "PlayerJoinEvent",
        "PlayerQuitEvent",
        "PlayerRespawnEvent",
        "PlayerBedLeaveEvent",
        "PlayerDeathEvent",
        "PlayerDimensionChangeEvent",
    ],
)
def test_not_cancellable(event_name: str) -> None:
    """Verify the events Bukkit makes cancellable but Endstone does not."""
    from endstone import event as event_module

    event_cls = getattr(event_module, event_name)
    assert not hasattr(event_cls, "cancel")
    assert not hasattr(event_cls, "is_cancelled")
