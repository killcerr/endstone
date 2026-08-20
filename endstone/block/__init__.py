import lazy_loader as lazy

__getattr__, __dir__, __all__ = lazy.attach(
    "endstone._python",
    submod_attrs={
        "block": [
            "Biome",
            "Block",
            "BlockData",
            "BlockFace",
            "BlockList",
            "BlockState",
            "BlockType",
            "Campfire",
            "Container",
            "CreatureSpawner",
            "Furnace",
            "ItemFrame",
            "Lectern",
            "Sign",
            "SignSide",
        ],
    },
)
