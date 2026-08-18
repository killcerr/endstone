"""Tests for the Object type hierarchy (as/is/isinstance) via Python."""

from endstone import Player, Server
from endstone.actor import Actor, Mob
from endstone.command import CommandSender, ConsoleCommandSender
from endstone.level import Dimension

# =============================================================================
# Section 1: Player isinstance checks
# =============================================================================


def test_player_isinstance_player(player: Player):
    """Verify player is recognized as Player."""
    assert isinstance(player, Player)


def test_player_isinstance_mob(player: Player):
    """Verify player is recognized as Mob."""
    assert isinstance(player, Mob)


def test_player_isinstance_actor(player: Player):
    """Verify player is recognized as Actor."""
    assert isinstance(player, Actor)


def test_player_isinstance_command_sender(player: Player):
    """Verify player is recognized as CommandSender."""
    assert isinstance(player, CommandSender)


def test_player_is_not_console(player: Player):
    """Verify player is not a ConsoleCommandSender."""
    assert not isinstance(player, ConsoleCommandSender)


# =============================================================================
# Section 2: Polymorphic dispatch through base references
# =============================================================================


def test_player_as_actor_resolves_to_player(player: Player, server: Server):
    """Verify that getting actors and finding the player returns the correct Player type."""
    level = server.level
    assert level is not None
    for dim_id in [Dimension.OVERWORLD, Dimension.NETHER, Dimension.THE_END]:
        dim = level.get_dimension(dim_id)
        if dim is None:
            continue
        for actor in dim.actors:
            if isinstance(actor, Player) and actor.name == player.name:
                # The actor came from a list of Actor*, but should resolve to Player
                assert isinstance(actor, Player)
                assert isinstance(actor, Mob)
                return
    # If player is not found in any dimension actors, skip
    assert False, "Player not found in any dimension's actor list"
