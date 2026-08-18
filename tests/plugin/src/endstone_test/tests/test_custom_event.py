import pytest
from endstone import Server
from endstone.event import Cancellable, Event, EventPriority, event_handler
from endstone.plugin import Plugin


class CustomEvent(Event):
    """A simple non-cancellable custom event."""

    pass


class CancellableCustomEvent(Event, Cancellable):
    """A cancellable custom event."""

    pass


class PriorityCustomEvent(Event, Cancellable):
    """A cancellable custom event used to exercise handler priority."""

    pass


# =============================================================================
# Listeners
# =============================================================================

handled: list[bool] = []
received: list[Event] = []
cancelled: list[bool] = []
priority_results: list[str] = []


class BasicListener:
    @event_handler
    def on_custom(self, event: CustomEvent):
        handled.append(True)
        received.append(event)


class CancellingListener:
    @event_handler
    def on_custom(self, event: CancellableCustomEvent):
        cancelled.append(True)
        event.cancel()


class PriorityListener:
    @event_handler(priority=EventPriority.LOW)
    def cancel_at_low(self, event: PriorityCustomEvent):
        event.cancel()
        priority_results.append("low")

    @event_handler(priority=EventPriority.HIGH, ignore_cancelled=True)
    def skipped_at_high(self, event: PriorityCustomEvent):
        priority_results.append("high_ignore")

    @event_handler(priority=EventPriority.HIGH)
    def still_called_at_high(self, event: PriorityCustomEvent):
        priority_results.append("high_default")


# =============================================================================
# Fixtures
# =============================================================================

_registered = False


@pytest.fixture(autouse=True)
def listeners(plugin: Plugin):
    """Register the listeners once for the life of the server and reset the sinks.

    There is no API to unregister an event listener, so registering per test would
    stack a fresh set of handlers on every `/test` run.
    """
    global _registered
    if not _registered:
        plugin.register_events(BasicListener())
        plugin.register_events(CancellingListener())
        plugin.register_events(PriorityListener())
        _registered = True

    for sink in (handled, received, cancelled, priority_results):
        sink.clear()


# =============================================================================
# Section 1: A plain custom event
# =============================================================================


class TestCustomEvent:
    def test_fire_and_handle(self, server: Server):
        server.plugin_manager.call_event(CustomEvent())
        assert len(handled) == 1

    def test_event_name(self):
        event = CustomEvent()
        assert event.event_name.endswith("test_custom_event.CustomEvent")

    def test_handler_receives_event_instance(self, server: Server):
        event = CustomEvent()
        server.plugin_manager.call_event(event)
        assert len(received) == 1
        assert received[0] is event

    def test_firing_twice_calls_the_handler_twice(self, server: Server):
        server.plugin_manager.call_event(CustomEvent())
        server.plugin_manager.call_event(CustomEvent())
        assert len(handled) == 2


# =============================================================================
# Section 2: A cancellable custom event
# =============================================================================


class TestCancellableCustomEvent:
    def test_fire_and_handle(self, server: Server):
        server.plugin_manager.call_event(CancellableCustomEvent())
        assert len(cancelled) == 1

    def test_cancel_in_handler(self, server: Server):
        event = CancellableCustomEvent()
        assert event.is_cancelled is False
        server.plugin_manager.call_event(event)
        assert event.is_cancelled is True


# =============================================================================
# Section 3: Priority and ignore_cancelled
# =============================================================================


class TestHandlerPriority:
    def test_ignore_cancelled_skips_handler(self, server: Server):
        server.plugin_manager.call_event(PriorityCustomEvent())
        assert "low" in priority_results
        assert "high_ignore" not in priority_results

    def test_default_handler_still_runs_when_cancelled(self, server: Server):
        event = PriorityCustomEvent()
        server.plugin_manager.call_event(event)
        assert "low" in priority_results
        assert "high_default" in priority_results
        assert event.is_cancelled is True

    def test_low_runs_before_high(self, server: Server):
        server.plugin_manager.call_event(PriorityCustomEvent())
        assert priority_results.index("low") < priority_results.index("high_default")
