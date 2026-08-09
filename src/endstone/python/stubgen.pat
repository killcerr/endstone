# Stub pieces the bindings cannot express, mostly generics. Because they live
# here rather than in the committed .pyi files, generation never consumes its
# own output.
#
# Queries are re.search, so anchor every one. A body is emitted verbatim, so the
# blank line that separates a block from the next query is part of it.

^endstone\.__prefix__$:
    \import typing
    \from ._version import __version__
    \export __version__
    _T = typing.TypeVar("_T")

^endstone\.Identifier$:
    class Identifier(typing.Generic[_T]):
        """
        Represents a namespaced identifier consisting of a namespace and a key.
        """
        def __init__(self, id: str) -> None: ...
        @property
        def namespace(self) -> str:
            """
            The namespace component of this identifier.
            """
        @property
        def key(self) -> str:
            """
            The key component of this identifier.
            """
        def __hash__(self) -> int: ...
        def __eq__(self, other: object) -> bool: ...
        def __ne__(self, other: object) -> bool: ...

^endstone\.Registry$:
    class Registry(typing.Generic[_T]):
        """
        Presents a registry
        """
        def get(self, id: Identifier[_T] | str) -> _T | None:
            """
            Get the object by its key.
            """
        def get_or_throw(self, id: Identifier[_T] | str) -> _T:
            """
            Get the object by its key or throw if missing.
            """
        def __getitem__(self, id: Identifier[_T] | str) -> _T: ...
        def __iter__(self) -> typing.Iterator[_T]: ...
        def __contains__(self, id: Identifier[_T] | str) -> bool: ...
        def __len__(self) -> int: ...

^endstone\.Server\.get_registry$:
    def get_registry(self, type: type[_T]) -> Registry[_T]:
        \doc

# GameRule is generic in the value type, which nothing carries at runtime -- a
# constant is just its identifier there. Replacing the class means its constants
# live here too, so this list must track include/endstone/game_rule.h.
^endstone\.GameRule$:
    class GameRule(typing.Generic[_T]):
        """
        Represents a game rule.
        """
        @property
        def id(self) -> Identifier[GameRule]:
            """
            The identifier of this game rule.
            """
        @property
        def translation_key(self) -> str:
            """
            The translation key, suitable for use in a translation component.
            """
        @staticmethod
        def get(name: Identifier[GameRule] | str) -> GameRule | None:
            """
            Attempts to get the `GameRule` with the given name.

            Args:
                name: The identifier of the game rule (e.g. `minecraft:dofiretick`).

            Returns:
                The `GameRule`, or `None` if no game rule with that name exists.
            """
        def __hash__(self) -> int: ...
        COMMAND_BLOCK_OUTPUT: typing.ClassVar[Identifier[GameRule[bool]]]
        COMMAND_BLOCKS_ENABLED: typing.ClassVar[Identifier[GameRule[bool]]]
        DO_DAY_LIGHT_CYCLE: typing.ClassVar[Identifier[GameRule[bool]]]
        DO_ENTITY_DROPS: typing.ClassVar[Identifier[GameRule[bool]]]
        DO_FIRE_TICK: typing.ClassVar[Identifier[GameRule[bool]]]
        DO_IMMEDIATE_RESPAWN: typing.ClassVar[Identifier[GameRule[bool]]]
        DO_INSOMNIA: typing.ClassVar[Identifier[GameRule[bool]]]
        DO_LIMITED_CRAFTING: typing.ClassVar[Identifier[GameRule[bool]]]
        DO_MOB_LOOT: typing.ClassVar[Identifier[GameRule[bool]]]
        DO_MOB_SPAWNING: typing.ClassVar[Identifier[GameRule[bool]]]
        DO_TILE_DROPS: typing.ClassVar[Identifier[GameRule[bool]]]
        DO_WEATHER_CYCLE: typing.ClassVar[Identifier[GameRule[bool]]]
        DROWNING_DAMAGE: typing.ClassVar[Identifier[GameRule[bool]]]
        FALL_DAMAGE: typing.ClassVar[Identifier[GameRule[bool]]]
        FIRE_DAMAGE: typing.ClassVar[Identifier[GameRule[bool]]]
        FREEZE_DAMAGE: typing.ClassVar[Identifier[GameRule[bool]]]
        FUNCTION_COMMAND_LIMIT: typing.ClassVar[Identifier[GameRule[int]]]
        KEEP_INVENTORY: typing.ClassVar[Identifier[GameRule[bool]]]
        LOCATOR_BAR: typing.ClassVar[Identifier[GameRule[bool]]]
        MAX_COMMAND_CHAIN_LENGTH: typing.ClassVar[Identifier[GameRule[int]]]
        MOB_GRIEFING: typing.ClassVar[Identifier[GameRule[bool]]]
        NATURAL_REGENERATION: typing.ClassVar[Identifier[GameRule[bool]]]
        PLAYERS_SLEEPING_PERCENTAGE: typing.ClassVar[Identifier[GameRule[int]]]
        PLAYER_WAYPOINTS: typing.ClassVar[Identifier[GameRule[int]]]
        PROJECTILES_CAN_BREAK_BLOCKS: typing.ClassVar[Identifier[GameRule[bool]]]
        PVP: typing.ClassVar[Identifier[GameRule[bool]]]
        RANDOM_TICK_SPEED: typing.ClassVar[Identifier[GameRule[int]]]
        RECIPES_UNLOCK: typing.ClassVar[Identifier[GameRule[bool]]]
        RESPAWN_BLOCKS_EXPLODE: typing.ClassVar[Identifier[GameRule[bool]]]
        SEND_COMMAND_FEEDBACK: typing.ClassVar[Identifier[GameRule[bool]]]
        SHOW_BORDER_EFFECT: typing.ClassVar[Identifier[GameRule[bool]]]
        SHOW_COORDINATES: typing.ClassVar[Identifier[GameRule[bool]]]
        SHOW_DAYS_PLAYED: typing.ClassVar[Identifier[GameRule[bool]]]
        SHOW_DEATH_MESSAGES: typing.ClassVar[Identifier[GameRule[bool]]]
        SHOW_RECIPE_MESSAGES: typing.ClassVar[Identifier[GameRule[bool]]]
        SHOW_TAGS: typing.ClassVar[Identifier[GameRule[bool]]]
        SPAWN_RADIUS: typing.ClassVar[Identifier[GameRule[int]]]
        TNT_EXPLODES: typing.ClassVar[Identifier[GameRule[bool]]]
        TNT_EXPLOSION_DROP_DECAY: typing.ClassVar[Identifier[GameRule[bool]]]

# Identifier class constants. The bindings expose them as plain values, so the
# generic parameter is lost; the owners are listed because a query cannot tell
# these apart from enum members, which are ALL_CAPS too. The annotation types the
# constant, the id spells out what is in it. No trailing blank line, so the
# constants stay packed together.
^endstone\.\w+\.(?P<owner>ActorType|Attribute|Dimension|EffectType|Enchantment|PotionType)\.(?P<name>[A-Z][A-Z0-9_]*)$:
    \from endstone import Identifier
    \name: Identifier[\owner] = \value
^endstone\.level\.__prefix__$:
    \import typing
    \from endstone import GameRule, Identifier
    _T = typing.TypeVar("_T")

# The bindings take a bare Identifier<GameRule>, so the value type has to come
# back from the rule. The fallback is str alone: widening it to Identifier[GameRule]
# would swallow the typed overload and lose the value check on assignment.
^endstone\.level\.Level\.get_game_rule$:
    @typing.overload
    def get_game_rule(self, rule: Identifier[GameRule[_T]]) -> _T: ...
    @typing.overload
    def get_game_rule(self, rule: str) -> bool | int | float:
        \doc

^endstone\.level\.Level\.set_game_rule$:
    @typing.overload
    def set_game_rule(self, rule: Identifier[GameRule[_T]], value: _T) -> None: ...
    @typing.overload
    def set_game_rule(self, rule: str, value: bool | int | float) -> None:
        \doc

^endstone\.event\.__prefix__$:
    \import typing
    _F = typing.TypeVar("_F", bound=(typing.Callable[..., None]))

^endstone\.event\.event_handler$:
    @typing.overload
    def event_handler(func: _F, /) -> _F: ...
    @typing.overload
    def event_handler(*, priority: EventPriority = ..., ignore_cancelled: bool = ...) -> typing.Callable[[_F], _F]: ...
