from endstone.event import (
    ActorDamageEvent,
    ActorDeathEvent,
    ActorExplodeEvent,
    ActorKnockbackEvent,
    ActorRemoveEvent,
    ActorSpawnEvent,
    ActorTeleportEvent,
    event_handler,
)

from .event_listener import EventListener


class ActorEventListener(EventListener):
    @event_handler
    def on_actor_damage(self, event: ActorDamageEvent):
        self.record(
            event,
            f"{event.actor.name} ({event.actor.type}) hurt "
            f"(source: {event.damage_source}, damage: {event.damage}).",
            actor_type=str(event.actor.type),
            damage=event.damage,
            damage_type=event.damage_source.type,
            is_cancellable=hasattr(event, "cancel"),
        )

    @event_handler
    def on_actor_death(self, event: ActorDeathEvent):
        self.record(
            event,
            f"{event.actor.name} ({event.actor.type}) died "
            f"(source: {event.damage_source}).",
            actor_type=str(event.actor.type),
            damage_type=event.damage_source.type,
        )

    @event_handler
    def on_actor_explode(self, event: ActorExplodeEvent):
        self.record(
            event,
            f"{event.actor.name} ({event.actor.type}) exploded.",
            actor_type=str(event.actor.type),
            block_count=len(event.block_list),
            x=event.location.x,
            y=event.location.y,
            z=event.location.z,
        )

    @event_handler
    def on_actor_knockback(self, event: ActorKnockbackEvent):
        self.record(
            event,
            f"{event.actor.name} ({event.actor.type}) is knocked by {event.knockback}",
            actor_type=str(event.actor.type),
            knockback=(event.knockback.x, event.knockback.y, event.knockback.z),
            has_source=event.source is not None,
        )

    @event_handler
    def on_actor_remove(self, event: ActorRemoveEvent):
        self.record(
            event,
            f"{event.actor.name} ({event.actor.type}) is removed from the world.",
            actor_type=str(event.actor.type),
        )

    @event_handler
    def on_actor_spawn(self, event: ActorSpawnEvent):
        self.record(
            event,
            f"{event.actor.name} ({event.actor.type}) just spawned "
            f"- {event.actor.__class__.__name__}.",
            actor_type=str(event.actor.type),
            python_type=event.actor.__class__.__name__,
        )

    @event_handler
    def on_actor_teleport(self, event: ActorTeleportEvent):
        self.record(
            event,
            f"{event.actor.name} ({event.actor.type}) teleported from "
            f"{event.from_location} to {event.to_location}",
            actor_type=str(event.actor.type),
            from_xyz=(
                event.from_location.x,
                event.from_location.y,
                event.from_location.z,
            ),
            to_xyz=(event.to_location.x, event.to_location.y, event.to_location.z),
        )
