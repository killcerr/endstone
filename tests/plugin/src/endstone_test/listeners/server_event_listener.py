import textwrap

from endstone import ColorFormat
from endstone.event import (
    BroadcastMessageEvent,
    MapInitializeEvent,
    PacketReceiveEvent,
    PacketSendEvent,
    PluginDisableEvent,
    PluginEnableEvent,
    ScriptMessageEvent,
    ServerCommandEvent,
    ServerListPingEvent,
    ServerLoadEvent,
    event_handler,
)

from .event_listener import EventListener


def _hexdump(payload: bytes, limit: int = 30) -> str:
    return textwrap.shorten(" ".join(f"{byte:02x}" for byte in payload[:limit]), 90)


class ServerEventListener(EventListener):
    @event_handler
    def on_server_load(self, event: ServerLoadEvent):
        self.record(
            event,
            f"ServerLoadEvent is called ({event.type})",
            always_log=True,
            load_type=event.type.name,
        )

    @event_handler
    def on_server_command(self, event: ServerCommandEvent):
        self.record(
            event,
            f"{event.sender.name} executed command: {event.command}",
            sender=event.sender.name,
            command=event.command,
        )

    @event_handler
    def on_server_list_ping(self, event: ServerListPingEvent):
        event.motd = ColorFormat.BOLD + event.motd
        event.level_name = ColorFormat.GREEN + "Endstone Test" + ColorFormat.RESET
        self.record(
            event,
            "ServerListPingEvent is called",
            motd=event.motd,
            level_name=event.level_name,
            num_players=event.num_players,
            max_players=event.max_players,
            network_protocol_version=event.network_protocol_version,
            game_mode=str(event.game_mode),
        )

    @event_handler
    def on_broadcast_message(self, event: BroadcastMessageEvent):
        self.record(
            event,
            f"Broadcast: {event.message}",
            recipient_count=len(event.recipients),
            is_asynchronous=event.is_asynchronous,
        )

    @event_handler
    def on_plugin_enable(self, event: PluginEnableEvent):
        self.record(
            event,
            f"{event.plugin.name} is enabled",
            plugin=event.plugin.name,
        )

    @event_handler
    def on_plugin_disable(self, event: PluginDisableEvent):
        self.record(
            event,
            f"{event.plugin.name} is disabled",
            plugin=event.plugin.name,
        )

    @event_handler
    def on_script_message(self, event: ScriptMessageEvent):
        self.record(
            event,
            f"Script message received from {event.sender.name}: "
            f"message_id={event.message_id}, message={event.message}",
            sender=event.sender.name,
            message_id=event.message_id,
            message=event.message,
        )

    @event_handler
    def on_packet_receive(self, event: PacketReceiveEvent):
        self.record(
            event,
            f"IN ({event.packet_id}) >> "
            f"{event.player.name if event.player else None} {event.address} "
            f"{_hexdump(event.payload, 31)}",
            packet_id=event.packet_id,
            payload_size=len(event.payload),
            has_player=event.player is not None,
            sub_client_id=event.sub_client_id,
        )

    @event_handler
    def on_packet_send(self, event: PacketSendEvent):
        self.record(
            event,
            f"OUT({event.packet_id}) >> "
            f"{event.player.name if event.player else None} {event.address} "
            f"{_hexdump(event.payload)}",
            packet_id=event.packet_id,
            payload_size=len(event.payload),
            has_player=event.player is not None,
            sub_client_id=event.sub_client_id,
        )

    @event_handler
    def on_map_initialize(self, event: MapInitializeEvent):
        self.record(
            event,
            f"MapInitializeEvent is called. MapView(id={event.map.id}, "
            f"scale={event.map.scale}, center_x={event.map.center_x}, "
            f"center_z={event.map.center_z}).",
            always_log=True,
            map_id=event.map.id,
            scale=str(event.map.scale),
            center_x=event.map.center_x,
            center_z=event.map.center_z,
        )
