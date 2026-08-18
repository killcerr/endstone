import pytest
from endstone.inventory import (
    BookMeta,
    BookMetaGeneration,
    CrossbowMeta,
    ItemMeta,
    ItemStack,
    WritableBookMeta,
)

# =============================================================================
# Section 1: Meta types by item
# =============================================================================


@pytest.mark.parametrize(
    "item_type,meta_type",
    [
        ("minecraft:writable_book", WritableBookMeta),
        ("minecraft:written_book", BookMeta),
        ("minecraft:crossbow", CrossbowMeta),
        ("minecraft:diamond_sword", ItemMeta),
    ],
)
def test_meta_type_for_item(item_type: str, meta_type: type) -> None:
    """Verify each item hands back the meta type that matches it."""
    assert isinstance(ItemStack(item_type).item_meta, meta_type)


def test_book_meta_is_a_writable_book_meta() -> None:
    """Verify BookMeta extends WritableBookMeta, as the page API is shared."""
    assert issubclass(BookMeta, WritableBookMeta)
    assert isinstance(ItemStack("minecraft:written_book").item_meta, WritableBookMeta)


# =============================================================================
# Section 2: WritableBookMeta
# =============================================================================


def test_pages_round_trip() -> None:
    """Verify pages can be written and read back through the item."""
    item = ItemStack("minecraft:writable_book")
    meta = item.item_meta
    assert meta.has_pages is False

    meta.pages = ["one", "two"]
    item.set_item_meta(meta)

    assert item.item_meta.pages == ["one", "two"]
    assert item.item_meta.page_count == 2
    assert item.item_meta.has_pages is True


def test_get_and_set_page_are_one_indexed() -> None:
    """Verify get_page and set_page count from one, unlike the pages list."""
    item = ItemStack("minecraft:writable_book")
    meta = item.item_meta
    meta.pages = ["first", "second"]

    assert meta.get_page(1) == "first"
    assert meta.get_page(2) == "second"

    meta.set_page(1, "rewritten")
    assert meta.get_page(1) == "rewritten"
    assert meta.pages[0] == "rewritten"


def test_add_page_takes_varargs() -> None:
    """Verify add_page appends any number of pages at once."""
    meta = ItemStack("minecraft:writable_book").item_meta
    meta.add_page("one")
    meta.add_page("two", "three")
    assert meta.pages == ["one", "two", "three"]
    assert meta.page_count == 3


def test_set_page_truncates_a_long_page() -> None:
    """Verify a page longer than the limit is truncated rather than rejected."""
    meta = ItemStack("minecraft:writable_book").item_meta
    meta.pages = [""]
    meta.set_page(1, "x" * 2000)
    assert len(meta.get_page(1)) <= 1024


def test_page_count_is_capped() -> None:
    """Verify a book cannot hold more than fifty pages."""
    meta = ItemStack("minecraft:writable_book").item_meta
    meta.add_page(*[f"page {i}" for i in range(60)])
    assert meta.page_count <= 50


# =============================================================================
# Section 3: BookMeta
# =============================================================================


def test_title_and_author() -> None:
    """Verify a written book keeps its title and author."""
    item = ItemStack("minecraft:written_book")
    meta = item.item_meta
    assert meta.has_title is False
    assert meta.has_author is False

    meta.title = "The Book"
    meta.author = "Vincent"
    item.set_item_meta(meta)

    assert item.item_meta.title == "The Book"
    assert item.item_meta.author == "Vincent"
    assert item.item_meta.has_title is True
    assert item.item_meta.has_author is True


@pytest.mark.parametrize("generation", list(BookMetaGeneration))
def test_generation_round_trip(generation: BookMetaGeneration) -> None:
    """Verify every book generation round-trips through the item."""
    item = ItemStack("minecraft:written_book")
    meta = item.item_meta
    meta.generation = generation
    item.set_item_meta(meta)

    assert item.item_meta.generation == generation
    assert item.item_meta.has_generation is True


def test_generation_members() -> None:
    """Verify the book generations match Bukkit's."""
    assert [member.name for member in BookMetaGeneration] == [
        "ORIGINAL",
        "COPY_OF_ORIGINAL",
        "COPY_OF_COPY",
    ]


def test_clearing_the_title() -> None:
    """Verify assigning None clears the title again."""
    meta = ItemStack("minecraft:written_book").item_meta
    meta.title = "The Book"
    meta.title = None
    assert meta.has_title is False


# =============================================================================
# Section 4: CrossbowMeta
# =============================================================================


def test_charged_projectile_round_trip() -> None:
    """Verify a crossbow keeps the single projectile charged into it."""
    item = ItemStack("minecraft:crossbow")
    meta = item.item_meta
    assert meta.has_charged_projectile is False
    assert meta.charged_projectile is None

    meta.charged_projectile = ItemStack("minecraft:arrow")
    item.set_item_meta(meta)

    assert item.item_meta.has_charged_projectile is True
    assert item.item_meta.charged_projectile.type == "minecraft:arrow"


def test_clearing_the_charged_projectile() -> None:
    """Verify assigning None discharges the crossbow."""
    meta = ItemStack("minecraft:crossbow").item_meta
    meta.charged_projectile = ItemStack("minecraft:arrow")
    meta.charged_projectile = None
    assert meta.has_charged_projectile is False


def test_crossbow_has_no_plural_accessor() -> None:
    """Verify Bedrock's single-projectile crossbow has no Bukkit-style list."""
    assert not hasattr(ItemStack("minecraft:crossbow").item_meta, "charged_projectiles")


# =============================================================================
# Section 5: Shared ItemMeta surface
# =============================================================================


def test_repair_cost() -> None:
    """Verify the repair cost is read and written."""
    meta = ItemStack("minecraft:diamond_sword").item_meta
    assert meta.has_repair_cost is False
    meta.repair_cost = 7
    assert meta.repair_cost == 7
    assert meta.has_repair_cost is True


def test_unbreakable() -> None:
    """Verify the unbreakable flag is read and written."""
    meta = ItemStack("minecraft:diamond_sword").item_meta
    assert meta.is_unbreakable is False
    meta.is_unbreakable = True
    assert meta.is_unbreakable is True


def test_clone_is_independent() -> None:
    """Verify a cloned meta does not share state with its source."""
    meta = ItemStack("minecraft:diamond_sword").item_meta
    meta.display_name = "Original"

    clone = meta.clone()
    assert clone.display_name == "Original"

    clone.display_name = "Copy"
    assert meta.display_name == "Original"


def test_remove_enchants() -> None:
    """Verify remove_enchants strips every enchantment at once."""
    meta = ItemStack("minecraft:diamond_sword").item_meta
    meta.add_enchant("sharpness", 3, True)
    meta.add_enchant("knockback", 1, True)
    assert meta.has_enchants is True

    meta.remove_enchants()
    assert meta.has_enchants is False
    assert meta.enchants == {}
