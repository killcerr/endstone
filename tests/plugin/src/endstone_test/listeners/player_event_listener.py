from endstone import ColorFormat
from endstone.event import (
    PlayerBedEnterEvent,
    PlayerBedLeaveEvent,
    PlayerChatEvent,
    PlayerCommandEvent,
    PlayerDeathEvent,
    PlayerDimensionChangeEvent,
    PlayerDropItemEvent,
    PlayerEmoteEvent,
    PlayerGameModeChangeEvent,
    PlayerInteractActorEvent,
    PlayerInteractEvent,
    PlayerItemConsumeEvent,
    PlayerItemHeldEvent,
    PlayerJoinEvent,
    PlayerJumpEvent,
    PlayerKickEvent,
    PlayerLoginEvent,
    PlayerMoveEvent,
    PlayerPickupItemEvent,
    PlayerPortalEvent,
    PlayerQuitEvent,
    PlayerRespawnEvent,
    PlayerSkinChangeEvent,
    PlayerTeleportEvent,
    PlayerToggleSneakEvent,
    PlayerToggleSprintEvent,
    event_handler,
)
from endstone.lang import Translatable

from .event_listener import EventListener


def _xyz(location) -> tuple[float, float, float]:
    return (location.x, location.y, location.z)


def _rot(location) -> tuple[float, float]:
    return (location.pitch, location.yaw)


def _as_text(server, message) -> str:
    if isinstance(message, Translatable):
        return server.language.translate(message)
    return "" if message is None else str(message)


class PlayerEventListener(EventListener):
    @event_handler
    def on_player_login(self, event: PlayerLoginEvent):
        skin = event.player.skin
        self.record(
            event,
            ColorFormat.YELLOW + f"{event.player.name} logged in.",
            player=event.player.name,
            skin_id=skin.id,
            cape_id=skin.cape_id,
            skin_shape=tuple(skin.image.shape),
            has_cape=skin.cape_image is not None,
        )

    @event_handler
    def on_player_join(self, event: PlayerJoinEvent):
        player = event.player
        self.record(
            event,
            _as_text(self.server, event.join_message),
            player=player.name,
            unique_id=str(player.unique_id),
            xuid=player.xuid,
            runtime_id=player.runtime_id,
            game_mode=str(player.game_mode),
            locale=player.locale,
            device_os=str(player.device_os),
            ping=player.ping,
            is_op=player.is_op,
        )

        for line in (
            "===========================",
            f"Name: {player.name}",
            f"UUID: {player.unique_id}",
            f"XUID: {player.xuid}",
            f"Entity Id: {player.runtime_id}",
            f"Address: {player.address}",
            f"Game mode: {player.game_mode}",
            f"Location: {player.location}",
            f"Velocity: {player.velocity}",
            f"Op status: {player.is_op}",
            f"Ping: {player.ping}ms",
            f"Locale: {player.locale}",
            f"Device: {player.device_os} {player.device_id}",
            f"Game version: {player.game_version}",
            "===========================",
        ):
            self.plugin.logger.info(line)

        if self.recorder.boss_bar is not None:
            self.recorder.boss_bar.add_player(player)

    @event_handler
    def on_player_quit(self, event: PlayerQuitEvent):
        self.record(
            event,
            _as_text(self.server, event.quit_message),
            player=event.player.name,
        )

    @event_handler
    def on_player_chat(self, event: PlayerChatEvent):
        self.record(
            event,
            f"{event.player.name} says: {event.message}",
            player=event.player.name,
            message=event.message,
            recipient_count=len(event.recipients),
        )

    @event_handler
    def on_player_command(self, event: PlayerCommandEvent):
        self.record(
            event,
            f"{event.player.name} runs command: {event.command}",
            player=event.player.name,
            command=event.command,
        )

    @event_handler
    def on_player_emote(self, event: PlayerEmoteEvent):
        self.record(
            event,
            f"{event.player.name} sends an emote: {event.emote_id}",
            player=event.player.name,
            emote_id=event.emote_id,
            is_muted=event.is_muted,
        )

    @event_handler
    def on_player_interact(self, event: PlayerInteractEvent):
        self.record(
            event,
            f"{event.player.name} interact ({event.action}) with {event.block} "
            f"(face={event.block_face}) using {event.item} item",
            always_log=True,
            player=event.player.name,
            action=str(event.action),
            has_block=event.has_block,
            has_item=event.has_item,
            block_type=str(event.block.type) if event.has_block else None,
            block_face=str(event.block_face),
        )

    @event_handler
    def on_player_interact_actor(self, event: PlayerInteractActorEvent):
        self.record(
            event,
            f"{event.player.name} interacts with actor {event.actor.name}",
            player=event.player.name,
            actor_type=str(event.actor.type),
        )

    @event_handler
    def on_player_kick(self, event: PlayerKickEvent):
        self.record(
            event,
            f"{event.player.name} has been kicked due to {event.reason}",
            player=event.player.name,
            reason=event.reason,
        )
        event.reason = ColorFormat.BOLD + event.reason

    @event_handler
    def on_player_game_mode_change(self, event: PlayerGameModeChangeEvent):
        self.record(
            event,
            f"{event.player.name} changed game mode to {event.new_game_mode}",
            player=event.player.name,
            new_game_mode=str(event.new_game_mode),
        )

    @event_handler
    def on_player_jump(self, event: PlayerJumpEvent):
        self.record(
            event,
            f"{event.player.name} {ColorFormat.YELLOW}jumps{ColorFormat.RESET} from "
            f"{event.from_location} to {event.to_location}",
            player=event.player.name,
            from_xyz=_xyz(event.from_location),
            to_xyz=_xyz(event.to_location),
            from_rot=_rot(event.from_location),
            to_rot=_rot(event.to_location),
        )

    @event_handler
    def on_player_move(self, event: PlayerMoveEvent):
        self.record(
            event,
            f"{event.player.name} {ColorFormat.GREEN}moves{ColorFormat.RESET} from "
            f"{event.from_location} to {event.to_location}",
            player=event.player.name,
            from_xyz=_xyz(event.from_location),
            to_xyz=_xyz(event.to_location),
            from_rot=_rot(event.from_location),
            to_rot=_rot(event.to_location),
        )

    @event_handler
    def on_player_teleport(self, event: PlayerTeleportEvent):
        self.record(
            event,
            f"{event.player.name} teleported from {event.from_location} to "
            f"{event.to_location}",
            player=event.player.name,
            from_xyz=_xyz(event.from_location),
            to_xyz=_xyz(event.to_location),
            from_rot=_rot(event.from_location),
            to_rot=_rot(event.to_location),
        )

    @event_handler
    def on_player_portal(self, event: PlayerPortalEvent):
        self.record(
            event,
            f"{event.player.name} teleported from {event.from_location} to "
            f"{event.to_location} {ColorFormat.GREEN}via portal",
            always_log=True,
            player=event.player.name,
            from_xyz=_xyz(event.from_location),
            to_xyz=_xyz(event.to_location),
            from_rot=_rot(event.from_location),
            to_rot=_rot(event.to_location),
        )

    @event_handler
    def on_player_dimension_change(self, event: PlayerDimensionChangeEvent):
        self.record(
            event,
            f"{event.player.name} has changed dimension from "
            f"{event.from_dimension.id} to {event.to_dimension.id}.",
            always_log=True,
            player=event.player.name,
            from_dimension=str(event.from_dimension.id),
            to_dimension=str(event.to_dimension.id),
        )

    @event_handler
    def on_player_death(self, event: PlayerDeathEvent):
        self.record(
            event,
            f"{_as_text(self.server, event.death_message)} "
            f"(source: {event.damage_source}).",
            player=event.player.name,
            damage_type=event.damage_source.type,
        )

    @event_handler
    def on_player_respawn(self, event: PlayerRespawnEvent):
        self.record(
            event,
            f"{event.player.name} respawned ({event.respawn_reason}).",
            player=event.player.name,
            respawn_reason=event.respawn_reason.name,
        )

    @event_handler
    def on_player_item_consume(self, event: PlayerItemConsumeEvent):
        self.record(
            event,
            f"{event.player.name} consumes {event.item}.",
            player=event.player.name,
            item_type=str(event.item.type),
            hand=str(event.hand),
        )

    @event_handler
    def on_player_item_held(self, event: PlayerItemHeldEvent):
        self.record(
            event,
            f"{event.player.name} changes slot from {event.previous_slot} to "
            f"{event.new_slot}.",
            player=event.player.name,
            previous_slot=event.previous_slot,
            new_slot=event.new_slot,
        )

    @event_handler
    def on_player_drop_item(self, event: PlayerDropItemEvent):
        self.record(
            event,
            f"{event.player.name} drops {event.item}.",
            player=event.player.name,
            item_type=str(event.item.type),
        )

    @event_handler
    def on_player_pickup_item(self, event: PlayerPickupItemEvent):
        self.record(
            event,
            f"{event.player.name} picks up {event.item} ({event.item.item_stack}).",
            player=event.player.name,
            item_type=str(event.item.item_stack.type),
        )

    @event_handler
    def on_player_bed_enter(self, event: PlayerBedEnterEvent):
        self.record(
            event,
            f"{event.player.name} enters bed ({event.bed}).",
            always_log=True,
            player=event.player.name,
            bed_type=str(event.bed.type),
        )

    @event_handler
    def on_player_bed_leave(self, event: PlayerBedLeaveEvent):
        self.record(
            event,
            f"{event.player.name} leaves bed ({event.bed}).",
            always_log=True,
            player=event.player.name,
            bed_type=str(event.bed.type),
        )

    @event_handler
    def on_player_skin_change(self, event: PlayerSkinChangeEvent):
        self.record(
            event,
            f"{event.player.name} changes skin to {event.new_skin.id}.",
            player=event.player.name,
            new_skin_id=event.new_skin.id,
        )

    @event_handler
    def on_player_toggle_sneak(self, event: PlayerToggleSneakEvent):
        self.record(
            event,
            f"{event.player.name} sneaking -> {event.is_sneaking}",
            player=event.player.name,
            is_sneaking=event.is_sneaking,
            player_is_sneaking=event.player.is_sneaking,
        )

    @event_handler
    def on_player_toggle_sprint(self, event: PlayerToggleSprintEvent):
        self.record(
            event,
            f"{event.player.name} sprinting -> {event.is_sprinting}",
            player=event.player.name,
            is_sprinting=event.is_sprinting,
            player_is_sprinting=event.player.is_sprinting,
        )
