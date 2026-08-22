from endstone import Server
from endstone.inventory import (
    ItemStack,
    MultiRecipe,
    Recipe,
    RecipeType,
    ShapedRecipe,
    ShapelessRecipe,
    SmithingRecipe,
)


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
    """Verify recipe snapshots preserve shaped, shapeless, smithing and multi types."""
    types = {recipe.type for recipe in server.level.recipes}
    assert types >= {RecipeType.SHAPED, RecipeType.SHAPELESS, RecipeType.SMITHING, RecipeType.MULTI}
    assert all(
        (recipe.type == RecipeType.SHAPED) == isinstance(recipe, ShapedRecipe)
        for recipe in server.level.recipes
    )
    assert all(
        (recipe.type == RecipeType.SHAPELESS) == isinstance(recipe, ShapelessRecipe)
        for recipe in server.level.recipes
    )
    assert all(
        (recipe.type == RecipeType.SMITHING) == isinstance(recipe, SmithingRecipe)
        for recipe in server.level.recipes
    )
    assert all(
        (recipe.type == RecipeType.MULTI) == isinstance(recipe, MultiRecipe)
        for recipe in server.level.recipes
    )


def test_smithing_recipe_has_three_roles(server: Server) -> None:
    """Verify smithing snapshots expose template, base and addition ingredients."""
    smithing = next(recipe for recipe in server.level.recipes if recipe.type == RecipeType.SMITHING)
    assert isinstance(smithing, SmithingRecipe)
    assert smithing.template_ingredient is not None
    assert smithing.base_ingredient is not None
    assert smithing.addition_ingredient is not None


def test_recipe_result_is_an_item_stack(server: Server) -> None:
    """Verify every recipe snapshot exposes an ItemStack result, including dynamic recipes."""
    assert all(isinstance(recipe.result, ItemStack) for recipe in server.level.recipes)


def test_log_recipe_samples(server: Server, plugin) -> None:
    """Log two recipe snapshots for each recipe type."""
    samples: dict[RecipeType, list[Recipe]] = {recipe_type: [] for recipe_type in RecipeType}
    for recipe in server.level.recipes:
        bucket = samples[recipe.type]
        if len(bucket) < 2:
            bucket.append(recipe)

    for recipe_type, recipes in samples.items():
        plugin.logger.info(f"Recipe samples ({recipe_type.name}):")
        for recipe in recipes:
            ingredients = [
                f"{ingredient.kind.name}:{ingredient.identifier} x{ingredient.count}"
                for ingredient in recipe.ingredients
            ]
            plugin.logger.info(
                f"  id={recipe.recipe_id} tag={recipe.tag} result={recipe.result.type.id} "
                f"x{recipe.result.amount} ingredients={ingredients}"
            )

