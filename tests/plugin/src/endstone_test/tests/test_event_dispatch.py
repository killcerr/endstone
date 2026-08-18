import pytest
from endstone import Server
from endstone.event import (
    Cancellable,
    Event,
    EventPriority,
    EventResult,
    ServerEvent,
    ServerLoadEvent,
    event_handler,
)

# =============================================================================
# Section 1: Dispatch matches on the exact event name
# =============================================================================


class BaseCustomEvent(Event):
    pass


class DerivedCustomEvent(BaseCustomEvent):
    pass


class BaseListener:
    def __init__(self):
        self.seen: list[str] = []

    @event_handler
    def on_base(self, event: BaseCustomEvent):
        self.seen.append(event.event_name)


def test_a_base_listener_does_not_receive_a_subclass(server: Server, plugin) -> None:
    """Verify dispatch matches the exact event name, so subclasses do not propagate."""
    listener = BaseListener()
    plugin.register_events(listener)

    server.plugin_manager.call_event(BaseCustomEvent())
    assert len(listener.seen) == 1

    server.plugin_manager.call_event(DerivedCustomEvent())
    assert len(listener.seen) == 1


def test_event_name_is_the_qualified_name() -> None:
    """Verify a Python event reports its qualified name, unlike a built-in one."""
    assert BaseCustomEvent().event_name.endswith("BaseCustomEvent")


# =============================================================================
# Section 2: Priority and ignore_cancelled
# =============================================================================


class CancellableCustomEvent(Event, Cancellable):
    pass


class PriorityListener:
    def __init__(self):
        self.order: list[str] = []

    @event_handler(priority=EventPriority.LOWEST)
    def first(self, event: CancellableCustomEvent):
        self.order.append("lowest")

    @event_handler(priority=EventPriority.LOW)
    def cancel_it(self, event: CancellableCustomEvent):
        self.order.append("low")
        event.cancel()

    @event_handler(priority=EventPriority.HIGH, ignore_cancelled=True)
    def skipped(self, event: CancellableCustomEvent):
        self.order.append("high_ignore_cancelled")

    @event_handler(priority=EventPriority.HIGHEST)
    def still_called(self, event: CancellableCustomEvent):
        self.order.append("highest")


def test_priority_order_and_ignore_cancelled(server: Server, plugin) -> None:
    """Verify handlers run in priority order and ignore_cancelled skips after a cancel."""
    listener = PriorityListener()
    plugin.register_events(listener)

    event = CancellableCustomEvent()
    server.plugin_manager.call_event(event)

    assert event.is_cancelled is True
    assert listener.order == ["lowest", "low", "highest"]


def test_event_priority_is_an_int_enum() -> None:
    """Verify the priorities order from LOWEST to MONITOR."""
    assert [member.name for member in EventPriority] == [
        "LOWEST",
        "LOW",
        "NORMAL",
        "HIGH",
        "HIGHEST",
        "MONITOR",
    ]
    assert EventPriority.LOWEST < EventPriority.NORMAL < EventPriority.MONITOR


# =============================================================================
# Section 3: Known binding gaps
# =============================================================================


def test_server_load_event_is_a_server_event() -> None:
    """Verify ServerLoadEvent carries its ServerEvent base into Python."""
    assert issubclass(ServerLoadEvent, ServerEvent)


def test_server_load_types() -> None:
    """Verify both load types the server fires are exposed, with their shorthands."""
    assert [member.name for member in ServerLoadEvent.LoadType] == ["STARTUP", "RELOAD"]
    assert ServerLoadEvent.STARTUP is ServerLoadEvent.LoadType.STARTUP
    assert ServerLoadEvent.RELOAD is ServerLoadEvent.LoadType.RELOAD


def test_event_result_has_no_consumer() -> None:
    """Verify EventResult is still an orphan enum with no event exposing it."""
    assert [member.name for member in EventResult] == ["DENY", "DEFAULT", "ALLOW"]


def test_built_in_events_cannot_be_constructed() -> None:
    """Verify built-in events bind no constructor, so only listeners can observe them."""
    with pytest.raises(TypeError):
        ServerLoadEvent()


# =============================================================================
# Section 4: Registration
# =============================================================================


def test_there_is_no_way_to_unregister(server: Server, plugin) -> None:
    """Verify listeners are registered for the plugin's lifetime, with no removal API."""
    assert not hasattr(plugin, "unregister_events")
    assert not hasattr(server.plugin_manager, "unregister_event")
    assert not hasattr(server.plugin_manager, "unregister_events")
