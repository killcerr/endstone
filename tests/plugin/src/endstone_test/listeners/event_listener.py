import inspect
import typing

from endstone import Server
from endstone.event import Event

if typing.TYPE_CHECKING:
    from endstone_test.plugin import EndstoneTest


class EventListener:
    def __init__(self, plugin: "EndstoneTest"):
        self.plugin = plugin
        self.server: Server = plugin.server
        self.recorder = plugin.recorder
        self.track_events()

    def track_events(self) -> None:
        for attr_name in dir(self):
            func = getattr(self, attr_name)
            if not callable(func) or not getattr(func, "_is_event_handler", False):
                continue

            params = list(inspect.signature(func).parameters.values())
            assert len(params) == 1, "Event handlers must have a single parameter"
            assert inspect.isclass(params[0].annotation)
            assert issubclass(params[0].annotation, Event)

            self.recorder.expect(params[0].annotation)

    def record(
        self, event: Event, summary: str = "", *, always_log: bool = False, **fields
    ) -> None:
        self.recorder.record(event, summary, always_log=always_log, **fields)
