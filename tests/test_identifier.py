import pytest

from endstone import Identifier


def test_bare_key_defaults_to_minecraft_namespace() -> None:
    """Verify a key with no namespace is placed in the minecraft namespace."""
    identifier = Identifier("stone")
    assert identifier.namespace == "minecraft"
    assert identifier.key == "stone"
    assert str(identifier) == "minecraft:stone"


def test_explicit_namespace() -> None:
    """Verify an explicit namespace is preserved."""
    identifier = Identifier("endstone:custom")
    assert identifier.namespace == "endstone"
    assert identifier.key == "custom"


def test_two_argument_constructor() -> None:
    """Verify the two-argument constructor, which the type stub omits."""
    identifier = Identifier("foo", "bar")
    assert identifier.namespace == "foo"
    assert identifier.key == "bar"
    assert str(identifier) == "foo:bar"


def test_namespace_splits_on_the_last_colon() -> None:
    """Verify a multi-colon id keeps everything before the last colon as namespace."""
    identifier = Identifier("minecraft:potion_type:water")
    assert identifier.namespace == "minecraft:potion_type"
    assert identifier.key == "water"


def test_str_and_repr() -> None:
    """Verify str() and repr() both render the plain namespace:key form."""
    identifier = Identifier("a:b")
    assert str(identifier) == "a:b"
    assert repr(identifier) == "a:b"


def test_equality_with_identifier() -> None:
    """Verify two identifiers with the same namespace and key compare equal."""
    assert Identifier("a:b") == Identifier("a", "b")
    assert Identifier("a:b") != Identifier("a:c")


def test_equality_with_string() -> None:
    """Verify an identifier compares equal to its string form."""
    assert Identifier("a:b") == "a:b"
    assert Identifier("stone") == "minecraft:stone"
    assert Identifier("a:b") != "a:c"


def test_equality_with_other_types() -> None:
    """Verify an identifier is not equal to an unrelated object."""
    assert Identifier("a:b") != 42
    assert Identifier("a:b") is not None


def test_hash_matches_the_string_form() -> None:
    """Verify an identifier hashes like its string form, so both key one dict."""
    identifier = Identifier("a:b")
    assert hash(identifier) == hash("a:b")
    assert {identifier: 1}[identifier] == 1


@pytest.mark.parametrize(
    "value,message",
    [
        ("", "Identifier string must not be empty."),
        ("a:", "Identifier namespace and key must not be empty."),
        (":b", "Identifier namespace and key must not be empty."),
    ],
)
def test_invalid_identifiers(value: str, message: str) -> None:
    """Verify an empty identifier, namespace or key is rejected."""
    with pytest.raises(ValueError) as err_info:
        Identifier(value)
    assert str(err_info.value) == message


def test_subscript_is_erased_at_runtime() -> None:
    """Verify Identifier[T] exists for type checkers but erases to Identifier."""
    assert Identifier[int] is Identifier
