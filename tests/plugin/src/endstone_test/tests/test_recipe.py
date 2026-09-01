from collections.abc import Iterable

import pytest
from endstone import Server
from endstone.inventory import (
    BlastingRecipe,
    BrewingContainerRecipe,
    BrewingMixRecipe,
    CampfireRecipe,
    ComplexRecipe,
    CookingRecipe,
    ExactIngredient,
    FurnaceRecipe,
    ItemStack,
    ItemTagIngredient,
    ItemType,
    ItemTypeIngredient,
    Recipe,
    ShapedRecipe,
    ShapelessRecipe,
    SmithingTransformRecipe,
    SmithingTrimRecipe,
    SmokingRecipe,
    StonecuttingRecipe,
)

from endstone.plugin import Plugin

RECIPE_TYPES = (
    ShapedRecipe,
    ShapelessRecipe,
    StonecuttingRecipe,
    FurnaceRecipe,
    BlastingRecipe,
    SmokingRecipe,
    CampfireRecipe,
    ComplexRecipe,
    SmithingTransformRecipe,
    SmithingTrimRecipe,
    BrewingMixRecipe,
    BrewingContainerRecipe,
)


def _item(name: str) -> ItemType:
    item = ItemType.get(name)
    assert item is not None, name
    return item


def _of_type(recipes: Iterable[Recipe], recipe_type: type[Recipe]) -> list[Recipe]:
    return [recipe for recipe in recipes if isinstance(recipe, recipe_type)]


def _format_ingredient(ingredient) -> str:
    if ingredient is None:
        return "-"
    if isinstance(ingredient, ExactIngredient):
        item = ingredient.item_stack
        return f"Exact({item.type} x{item.amount})"
    if isinstance(ingredient, ItemTypeIngredient):
        return f"ItemType({ingredient.item_type} x{ingredient.count})"
    if isinstance(ingredient, ItemTagIngredient):
        return f"ItemTag({ingredient.item_tag} x{ingredient.count})"
    return f"{type(ingredient).__name__}(count={ingredient.count})"


def _format_recipe(recipe: Recipe) -> str:
    result = recipe.result
    parts = [
        type(recipe).__name__,
        f"id={recipe.recipe_id}",
        f"tag={recipe.tag}",
        f"priority={recipe.priority}",
        f"unlocking={recipe.unlocking_requirement.context.name}",
        f"result={result.type} x{result.amount}",
    ]
    if isinstance(recipe, ShapedRecipe):
        parts.append(f"{recipe.width}x{recipe.height}")
    if isinstance(recipe, (CookingRecipe, StonecuttingRecipe)):
        parts.append(f"input={_format_ingredient(recipe.input)}")
    elif isinstance(recipe, (BrewingMixRecipe, BrewingContainerRecipe)):
        parts.append(f"input={_format_ingredient(recipe.input)}")
        parts.append(f"reagent={_format_ingredient(recipe.reagent)}")
    elif isinstance(recipe, (SmithingTransformRecipe, SmithingTrimRecipe)):
        parts.append(f"template={_format_ingredient(recipe.template)}")
        parts.append(f"base={_format_ingredient(recipe.base)}")
        parts.append(f"addition={_format_ingredient(recipe.addition)}")
    else:
        ingredients = ", ".join(_format_ingredient(ingredient) for ingredient in recipe.ingredients)
        parts.append(f"ingredients=[{ingredients}]")

    return " ".join(parts)


def _log_recipes(plugin: Plugin, recipes: list[Recipe], recipe_type: type[Recipe], limit: int = 2) -> None:
    assert recipes, f"no {recipe_type.__name__} recipes"
    for recipe in recipes[:limit]:
        plugin.logger.info(_format_recipe(recipe))
        assert isinstance(recipe, recipe_type)
        assert recipe.recipe_id
        assert recipe.tag


@pytest.mark.parametrize("recipe_type", RECIPE_TYPES, ids=lambda t: t.__name__)
def test_vanilla_recipes(plugin: Plugin, server: Server, recipe_type: type[Recipe]) -> None:
    """Log a sample of vanilla recipes of each public recipe type."""
    _log_recipes(plugin, _of_type(server.recipes, recipe_type), recipe_type)


def test_constructed_recipes(plugin: Plugin, server: Server) -> None:
    """Register and log one constructed recipe of each public type."""
    stick = ItemTypeIngredient(_item("minecraft:stick"))
    diamond = ItemTypeIngredient(_item("minecraft:diamond"))
    constructed = [
        ShapedRecipe(
            "endstone_test:shaped_diamond_sword",
            ["##/"],
            [("#", diamond), ("/", stick)],
            ItemStack("minecraft:diamond_sword"),
        ),
        ShapelessRecipe(
            "endstone_test:shapeless_apple_from_dirt",
            [ItemTypeIngredient(_item("minecraft:dirt"))],
            ItemStack("minecraft:apple"),
        ),
        StonecuttingRecipe(
            "endstone_test:stonecutter_dirt_to_diamond",
            ItemTypeIngredient(_item("minecraft:dirt")),
            ItemStack("minecraft:diamond"),
        ),
        FurnaceRecipe(
            "endstone_test:furnace_dirt_to_diamond",
            ItemTypeIngredient(_item("minecraft:dirt")),
            ItemStack("minecraft:diamond"),
        ),
        BlastingRecipe(
            "endstone_test:blasting_dirt_to_diamond",
            ItemTypeIngredient(_item("minecraft:dirt")),
            ItemStack("minecraft:diamond"),
        ),
        SmokingRecipe(
            "endstone_test:smoking_dirt_to_diamond",
            ItemTypeIngredient(_item("minecraft:dirt")),
            ItemStack("minecraft:diamond"),
        ),
        CampfireRecipe(
            "endstone_test:campfire_dirt_to_diamond",
            ItemTypeIngredient(_item("minecraft:dirt")),
            ItemStack("minecraft:diamond"),
        ),
        BrewingMixRecipe(
            "endstone_test:dirt_potion_mix",
            ExactIngredient(ItemStack("minecraft:dirt")),
            ExactIngredient(ItemStack("minecraft:stick")),
            ItemStack("minecraft:diamond"),
        ),
        BrewingContainerRecipe(
            "endstone_test:dirt_potion_container",
            ExactIngredient(ItemStack("minecraft:dirt")),
            ExactIngredient(ItemStack("minecraft:cobblestone")),
            ItemStack("minecraft:diamond"),
        ),

        SmithingTransformRecipe(
            "endstone_test:dirt_to_diamond",
            ItemTypeIngredient(_item("minecraft:stick")),
            ItemTypeIngredient(_item("minecraft:dirt")),
            ItemTypeIngredient(_item("minecraft:cobblestone")),
            ItemStack("minecraft:diamond"),
        ),
        SmithingTrimRecipe(
            "endstone_test:dirt_trim",
            ItemTypeIngredient(_item("minecraft:stick")),
            ItemTypeIngredient(_item("minecraft:dirt")),
            ItemTypeIngredient(_item("minecraft:cobblestone")),
        ),
    ]
    for recipe in constructed:
        assert server.register_recipe(recipe)
        _log_recipes(plugin, [recipe], type(recipe), limit=1)
        assert any(entry.recipe_id == recipe.recipe_id for entry in server.recipes)


def test_register_and_unregister_recipe(plugin: Plugin, server: Server) -> None:
    """Register a shapeless recipe and brewing recipes, then remove them again."""
    recipes = [
        ShapelessRecipe(
            "endstone_test:registered_planks",
            [ItemTypeIngredient(_item("minecraft:birch_log"))],
            ItemStack("minecraft:birch_planks", 4),
        ),
        BrewingMixRecipe(
            "endstone_test:registered_mix",
            ExactIngredient(ItemStack("minecraft:dirt")),
            ExactIngredient(ItemStack("minecraft:stick")),
            ItemStack("minecraft:diamond"),
        ),
        BrewingContainerRecipe(
            "endstone_test:registered_container",
            ExactIngredient(ItemStack("minecraft:dirt")),
            ExactIngredient(ItemStack("minecraft:cobblestone")),
            ItemStack("minecraft:diamond"),
        ),
    ]
    for recipe in recipes:
        recipe_id = recipe.recipe_id
        assert server.register_recipe(recipe)
        assert any(entry.recipe_id == recipe_id for entry in server.recipes)
        server.update_recipes()
        assert server.unregister_recipe(recipe_id)
        assert all(entry.recipe_id != recipe_id for entry in server.recipes)
        server.update_recipes()
