"""Tests for the Object type hierarchy (as/is/isinstance) via Python."""

from endstone import Player, Server
from endstone.actor import Actor
from endstone.command import CommandSender, ConsoleCommandSender

# =============================================================================
# Section 1: Console command sender isinstance checks
# =============================================================================


def test_console_is_command_sender(server: Server):
    """Verify console command sender is recognized as CommandSender."""
    sender = server.command_sender
    assert isinstance(sender, CommandSender)
    assert isinstance(sender, ConsoleCommandSender)


def test_console_is_not_actor(server: Server):
    """Verify console command sender is not an Actor."""
    sender = server.command_sender
    assert not isinstance(sender, Actor)
    assert not isinstance(sender, Player)
