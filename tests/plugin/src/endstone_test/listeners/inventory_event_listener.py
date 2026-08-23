from endstone.event import InventoryCloseEvent, InventoryOpenEvent, event_handler

from .event_listener import EventListener


class InventoryEventListener(EventListener):
    @event_handler
    def on_inventory_open(self, event: InventoryOpenEvent):
        self.record(
            event,
            f"{event.player.name} opened an inventory of {event.inventory.size} slots",
            player=event.player.name,
            size=event.inventory.size,
        )

    @event_handler
    def on_inventory_close(self, event: InventoryCloseEvent):
        self.record(
            event,
            f"{event.player.name} closed an inventory of {event.inventory.size} slots",
            player=event.player.name,
            size=event.inventory.size,
        )
