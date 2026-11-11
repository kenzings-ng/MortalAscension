#!/usr/bin/env python3
# ==============================================================================
# build_pcg_biome_graphs.py: Procedural Content Generation Configuration Builder
# Project: Project Ascendant (2.5D Isometric Hardcore ARPG MMO on UE 5.8)
# Conforms to: ARCH-PCG-ENV-2026-V1 & Package M6 Environment Asset Foundation
# ==============================================================================

import os
import json

OUT_PCG_DIR = "Content/Maps/PCG"
os.makedirs(OUT_PCG_DIR, exist_ok=True)

# 1. Citadel Coordinates from zone-001 specification
CITADEL_LOCATIONS = [
    {"name": "Verdant Bastion", "x": 0.0, "y": 0.0, "z": 100.0, "safe_radius_cm": 5000.0},
    {"name": "Ashen Keep", "x": 35000.0, "y": 25000.0, "z": 450.0, "safe_radius_cm": 5000.0},
    {"name": "Sanctum Fortress", "x": -30000.0, "y": 45000.0, "z": 800.0, "safe_radius_cm": 5000.0}
]

# 2. Road Spline Graph Definition (Connecting Citadels with Cobblestone & Waymarkers)
ROAD_SPLINE_GRAPH = {
    "graph_name": "PCG_CitadelRoadSplineGraph",
    "version": "1.1",
    "engine_target": "UE_5.8",
    "nodes": [
        {
            "id": "GetSplineData_0",
            "type": "PCGGetSplineData",
            "properties": {
                "actor_filter": "AllWorldActors",
                "tag_filter": "CitadelConnectingRoad"
            }
        },
        {
            "id": "SplineSampler_0",
            "type": "PCGSplineSampler",
            "properties": {
                "mode": "Distance",
                "dimension": "OnSpline",
                "step_distance_cm": 100.0,
                "subdivide_curves": True
            }
        },
        {
            "id": "Projection_0",
            "type": "PCGProjection",
            "properties": {
                "projection_axis": "Z",
                "max_slope_deg": 25.0,
                "target": "Landscape"
            }
        },
        {
            "id": "RoadCrossSection_0",
            "type": "PCGTransformPoints",
            "properties": {
                "path_width_cm": 400.0,
                "falloff_shoulder_cm": 100.0
            }
        },
        {
            "id": "CobblestoneSpawner_0",
            "type": "PCGPaper2DSpriteSpawner",
            "properties": {
                "sprite_paths": [
                    "/Game/Art/Environment/Sprites/Tiles/TILE_Stone_Cobble_Path"
                ],
                "hism_enabled": True
            }
        },
        {
            "id": "WaymarkerSpawner_0",
            "type": "PCGPaper2DSpriteSpawner",
            "properties": {
                "interval_cm": 1500.0,
                "sprite_palette": [
                    "/Game/Art/Environment/Sprites/Props/PROP_Signpost_Waymarker_03",
                    "/Game/Art/Environment/Sprites/Props/PROP_Brazier_Torch_04_sheet"
                ]
            }
        }
    ],
    "connections": [
        {"from": "GetSplineData_0", "to": "SplineSampler_0"},
        {"from": "SplineSampler_0", "to": "Projection_0"},
        {"from": "Projection_0", "to": "RoadCrossSection_0"},
        {"from": "RoadCrossSection_0", "to": "CobblestoneSpawner_0"},
        {"from": "RoadCrossSection_0", "to": "WaymarkerSpawner_0"}
    ]
}

# 3. World Biome Graph Definition (M6 Environment & Foliage Integration)
WORLD_BIOME_GRAPH = {
    "graph_name": "PCG_WorldBiomeGraph",
    "version": "1.1",
    "engine_target": "UE_5.8",
    "exclusion_zones": CITADEL_LOCATIONS,
    "nodes": [
        {
            "id": "SurfaceSampler_0",
            "type": "PCGSurfaceSampler",
            "properties": {
                "sampling_method": "PoissonDisk",
                "points_per_squared_meter": 0.05,
                "looseness": 1.5
            }
        },
        {
            "id": "DistanceFilter_Exclusion",
            "type": "PCGDistanceFilter",
            "properties": {
                "mode": "ExcludeInsideRadius",
                "reference_points": [
                    {"x": c["x"], "y": c["y"], "radius": c["safe_radius_cm"]} for c in CITADEL_LOCATIONS
                ]
            }
        },
        {
            "id": "SlopeFilter_0",
            "type": "PCGSlopeFilter",
            "properties": {
                "min_slope_deg": 0.0,
                "max_slope_deg": 40.0
            }
        },
        {
            "id": "BiomePartition_0",
            "type": "PCGBiomeSwitch",
            "properties": {
                "biomes": {
                    "VerdantBastion": {
                        "foliage_density": 0.8,
                        "rock_density": 0.3,
                        "tree_palette": [
                            "/Game/Art/Environment/Sprites/Trees/TREE_Oak_Canopy_02",
                            "/Game/Art/Environment/Sprites/Trees/TREE_Sapling_01",
                            "/Game/Art/Environment/Sprites/Trees/TREE_Pine_Evergreen_03"
                        ],
                        "foliage_palette": [
                            "/Game/Art/Environment/Sprites/Foliage/FOL_Bush_Medium_02",
                            "/Game/Art/Environment/Sprites/Foliage/FOL_Bush_Small_01",
                            "/Game/Art/Environment/Sprites/Foliage/FOL_Flower_Poppies_01",
                            "/Game/Art/Environment/Sprites/Foliage/FOL_Flower_Bellflowers_02",
                            "/Game/Art/Environment/Sprites/Foliage/FOL_Flower_Daisies_03",
                            "/Game/Art/Environment/Sprites/Foliage/FOL_Grass_Patch_02",
                            "/Game/Art/Environment/Sprites/Foliage/FOL_Grass_Tuft_01"
                        ],
                        "rock_palette": [
                            "/Game/Art/Environment/Sprites/Props/ROCK_Boulder_Mossy_02",
                            "/Game/Art/Environment/Sprites/Props/ROCK_Pebbles_01"
                        ],
                        "props_palette": [
                            "/Game/Art/Environment/Sprites/Props/PROP_Crate_Oak_01",
                            "/Game/Art/Environment/Sprites/Props/PROP_Barrel_Keg_02"
                        ]
                    },
                    "AshenKeep": {
                        "foliage_density": 0.2,
                        "rock_density": 0.8,
                        "tree_palette": [
                            "/Game/Art/Environment/Sprites/Trees/TREE_Sapling_01",
                            "/Game/Art/Environment/Sprites/Foliage/FOL_Bush_Bramble_03"
                        ],
                        "rock_palette": [
                            "/Game/Art/Environment/Sprites/Props/ROCK_Formation_Crag_03",
                            "/Game/Art/Environment/Sprites/Props/ROCK_Pebbles_01"
                        ],
                        "props_palette": [
                            "/Game/Art/Environment/Sprites/Props/PROP_Brazier_Torch_04_sheet"
                        ]
                    },
                    "SanctumFortress": {
                        "foliage_density": 0.5,
                        "rock_density": 0.6,
                        "tree_palette": [
                            "/Game/Art/Environment/Sprites/Trees/TREE_Pine_Evergreen_03"
                        ],
                        "rock_palette": [
                            "/Game/Art/Environment/Sprites/Props/ROCK_Formation_Crag_03",
                            "/Game/Art/Environment/Sprites/Props/ROCK_Boulder_Mossy_02"
                        ]
                    }
                }
            }
        },
        {
            "id": "FoliageSpawner_0",
            "type": "PCGStaticMeshSpawner",
            "properties": {
                "hism_enabled": True,
                "cull_distance_min": 15000.0,
                "cull_distance_max": 30000.0
            }
        }
    ],
    "connections": [
        {"from": "SurfaceSampler_0", "to": "DistanceFilter_Exclusion"},
        {"from": "DistanceFilter_Exclusion", "to": "SlopeFilter_0"},
        {"from": "SlopeFilter_0", "to": "BiomePartition_0"},
        {"from": "BiomePartition_0", "to": "FoliageSpawner_0"}
    ]
}

# 4. Canonical 4-Bit Autotile Bitmask Mapping (32x32 Tileset)
# Directional weights: North=1, East=2, South=4, West=8
AUTOTILE_BITMASK_32X32 = {
    "tileset_name": "M6_Terrain_Autotile_32x32",
    "tile_size": 32,
    "bitmask_type": "4_bit_cardinal_with_corners",
    "base_dirt": "TILE_Dirt_Base.png",
    "base_grass": "TILE_Grass_Base.png",
    "cobblestone_path": "TILE_Stone_Cobble_Path.png",
    "cliff_wall": "TILE_Cliff_Ledge_Wall.png",
    "masks": {
        # 4 Cardinal Edges (Values represent neighbor grass flags: N=1, E=2, S=4, W=8)
        "edge_north": {"tile": "TILE_Grass_Edge_N.png", "cardinal_pattern": "E+S+W (No Grass North)", "mask": 14},
        "edge_south": {"tile": "TILE_Grass_Edge_S.png", "cardinal_pattern": "N+E+W (No Grass South)", "mask": 11},
        "edge_west":  {"tile": "TILE_Grass_Edge_W.png", "cardinal_pattern": "N+E+S (No Grass West)", "mask": 7},
        "edge_east":  {"tile": "TILE_Grass_Edge_E.png", "cardinal_pattern": "N+S+W (No Grass East)", "mask": 13},
        # Outer Corners
        "corner_north_west": {"tile": "TILE_Grass_Corner_NW.png", "cardinal_pattern": "S+E (Grass South & East)", "mask": 6},
        "corner_north_east": {"tile": "TILE_Grass_Corner_NE.png", "cardinal_pattern": "S+W (Grass South & West)", "mask": 12},
        "corner_south_west": {"tile": "TILE_Grass_Corner_SW.png", "cardinal_pattern": "N+E (Grass North & East)", "mask": 3},
        "corner_south_east": {"tile": "TILE_Grass_Corner_SE.png", "cardinal_pattern": "N+W (Grass North & West)", "mask": 9},
        # Inner Corners
        "inner_north_west": {"tile": "TILE_Grass_Inner_NW.png", "diagonal_missing": "NW", "mask": 254},
        "inner_north_east": {"tile": "TILE_Grass_Inner_NE.png", "diagonal_missing": "NE", "mask": 251},
        "inner_south_west": {"tile": "TILE_Grass_Inner_SW.png", "diagonal_missing": "SW", "mask": 239},
        "inner_south_east": {"tile": "TILE_Grass_Inner_SE.png", "diagonal_missing": "SE", "mask": 191}
    }
}


def build_pcg_assets():
    road_path = os.path.join(OUT_PCG_DIR, "PCG_CitadelRoadSplineGraph.json")
    with open(road_path, "w") as f:
        json.dump(ROAD_SPLINE_GRAPH, f, indent=2)
    print(f"✅ Generated PCG Road Spline Graph: {road_path}")

    biome_path = os.path.join(OUT_PCG_DIR, "PCG_WorldBiomeGraph.json")
    with open(biome_path, "w") as f:
        json.dump(WORLD_BIOME_GRAPH, f, indent=2)
    print(f"✅ Generated PCG World Biome Graph:  {biome_path}")

    exclusion_path = os.path.join(OUT_PCG_DIR, "Citadel_SafeZone_Exclusion_Volumes.json")
    with open(exclusion_path, "w") as f:
        json.dump(CITADEL_LOCATIONS, f, indent=2)
    print(f"✅ Generated Exclusion Volumes:      {exclusion_path}")

    autotile_path = os.path.join(OUT_PCG_DIR, "Autotile_Terrain_32x32_Bitmask.json")
    with open(autotile_path, "w") as f:
        json.dump(AUTOTILE_BITMASK_32X32, f, indent=2)
    print(f"✅ Generated Autotile Bitmask:       {autotile_path}")


if __name__ == "__main__":
    build_pcg_assets()
