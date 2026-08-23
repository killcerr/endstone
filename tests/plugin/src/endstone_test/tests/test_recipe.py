from endstone import Server
from endstone.inventory import (
    ItemStack,
    MultiRecipe,
    Recipe,
    ShapedRecipe,
    ShapelessRecipe,
    SmithingRecipe,
)

RECIPE_TYPES = (ShapedRecipe, ShapelessRecipe, SmithingRecipe, MultiRecipe)


def test_level_recipes_are_snapshots(server: Server) -> None:
    """Verify Level.recipes exposes typed, immutable recipe snapshots."""
    recipes = server.level.recipes
    assert isinstance(recipes, list)
    assert recipes
    assert all(isinstance(recipe, Recipe) for recipe in recipes)
    assert all(recipe.recipe_id for recipe in recipes)
    assert all(isinstance(recipe.tag, str) for recipe in recipes)
    assert all(isinstance(recipe.ingredients, list) for recipe in recipes)


def test_level_recipes_use_matching_specialized_types(server: Server) -> None:
    """Verify recipe snapshots are narrowed to shaped, shapeless, smithing and multi types."""
    recipes = server.level.recipes
    for recipe_type in RECIPE_TYPES:
        assert any(isinstance(recipe, recipe_type) for recipe in recipes), (
            recipe_type.__name__
        )
    assert all(isinstance(recipe, RECIPE_TYPES) for recipe in recipes)
    assert not any(type(recipe) is Recipe for recipe in recipes)


def test_smithing_recipe_has_three_roles(server: Server) -> None:
    """Verify smithing snapshots expose template, base and addition ingredients."""
    smithing = next(
        recipe for recipe in server.level.recipes if isinstance(recipe, SmithingRecipe)
    )
    assert smithing.template_ingredient is not None
    assert smithing.base_ingredient is not None
    assert smithing.addition_ingredient is not None


def test_recipe_result_is_an_item_stack(server: Server) -> None:
    """Verify every recipe snapshot exposes an ItemStack result, including dynamic recipes."""
    assert all(isinstance(recipe.result, ItemStack) for recipe in server.level.recipes)


def test_log_recipe_samples(server: Server, plugin) -> None:
    """Log two recipe snapshots for each recipe type."""
    samples: dict[type[Recipe], list[Recipe]] = {
        recipe_type: [] for recipe_type in RECIPE_TYPES
    }
    for recipe in server.level.recipes:
        for recipe_type in RECIPE_TYPES:
            if isinstance(recipe, recipe_type) and len(samples[recipe_type]) < 2:
                samples[recipe_type].append(recipe)
                break

    for recipe_type, recipes in samples.items():
        plugin.logger.info(f"Recipe samples ({recipe_type.__name__}):")
        for recipe in recipes:
            ingredients = [
                f"{ingredient.kind.name}:{ingredient.identifier} x{ingredient.count}"
                for ingredient in recipe.ingredients
            ]
            plugin.logger.info(
                f"  id={recipe.recipe_id} tag={recipe.tag} result={recipe.result.type.id} "
                f"x{recipe.result.amount} ingredients={ingredients}"
            )
