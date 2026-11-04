import unreal

def build_map(map_package_path, map_name):
    unreal.log(f"=== Project Ascendant: Building {map_name} at {map_package_path} ===")
    
    editor_actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    editor_level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    
    # 1. Create or Open Level
    try:
        unreal.EditorLevelLibrary.new_level(map_package_path)
    except Exception as e:
        unreal.log_warning(f"EditorLevelLibrary.new_level fallback: {e}")
        try:
            editor_level_subsystem.new_level(map_package_path)
        except Exception as e2:
            unreal.log_warning(f"LevelEditorSubsystem fallback: {e2}")

    # 2. Clear existing duplicate actors (bỏ qua WorldSettings và hệ thống core)
    for actor in editor_actor_subsystem.get_all_level_actors():
        if not isinstance(actor, unreal.WorldSettings):
            editor_actor_subsystem.destroy_actor(actor)

    # 3. Ambient Lighting & Atmospheric Setup (Dark Fantasy Mood)
    dir_light = editor_actor_subsystem.spawn_actor_from_class(
        unreal.DirectionalLight,
        unreal.Vector(0, 0, 800),
        unreal.Rotator(-50, 45, 0)
    )
    if dir_light:
        dir_light.set_actor_label(f"DirectionalLight_{map_name}")
        light_comp = dir_light.get_component_by_class(unreal.DirectionalLightComponent)
        if light_comp:
            light_comp.set_mobility(unreal.ComponentMobility.MOVABLE)
            light_comp.set_intensity(18.0)
            light_comp.set_light_color(unreal.LinearColor(0.70, 0.82, 1.0, 1.0))

    sky_light = editor_actor_subsystem.spawn_actor_from_class(
        unreal.SkyLight,
        unreal.Vector(0, 0, 1000),
        unreal.Rotator(0, 0, 0)
    )
    if sky_light:
        sky_light.set_actor_label(f"SkyLight_{map_name}")
        sl_comp = sky_light.get_component_by_class(unreal.SkyLightComponent)
        if sl_comp:
            sl_comp.set_mobility(unreal.ComponentMobility.MOVABLE)
            sl_comp.set_intensity(0.6)

    # 4. Global PostProcess Volume for calibrated HD-2D exposure (chống cháy sáng)
    pp_actor = editor_actor_subsystem.spawn_actor_from_class(
        unreal.PostProcessVolume,
        unreal.Vector(0, 0, 0),
        unreal.Rotator(0, 0, 0)
    )
    if pp_actor:
        pp_actor.set_actor_label(f"PostProcessVolume_{map_name}")
        try:
            pp_actor.set_editor_property("b_unbound", True)
        except Exception:
            try:
                pp_actor.set_editor_property("unbound", True)
            except Exception as e:
                unreal.log_warning(f"Failed to set PostProcess unbound: {e}")
        
        try:
            pp_settings = pp_actor.get_editor_property("settings")
            pp_settings.set_editor_property("override_auto_exposure_method", True)
            pp_settings.set_editor_property("auto_exposure_method", unreal.AutoExposureMethod.AEM_MANUAL)
            pp_settings.set_editor_property("override_auto_exposure_bias", True)
            pp_settings.set_editor_property("auto_exposure_bias", 0.0)
        except Exception as e:
            unreal.log_warning(f"PostProcess exposure calibration fallback: {e}")

    # 5. Fixed Framing Camera (FOV 90 theo DECISIONS.md §13)
    cam_actor = editor_actor_subsystem.spawn_actor_from_class(
        unreal.CameraActor,
        unreal.Vector(-600, 0, 150),
        unreal.Rotator(0, 0, 0)
    )
    if cam_actor:
        cam_actor.set_actor_label(f"Camera_{map_name}")
        cam_comp = cam_actor.get_component_by_class(unreal.CameraComponent)
        if cam_comp:
            try:
                cam_comp.set_editor_property("field_of_view", 90.0)
            except Exception:
                pass

    # 6. Save Level
    try:
        unreal.EditorLevelLibrary.save_current_level()
    except Exception as e:
        unreal.log_warning(f"save_current_level fallback: {e}")
        editor_level_subsystem.save_current_level()

    unreal.log(f"=== Successfully saved {map_name} to {map_package_path} ===")

def main():
    build_map("/Game/Maps/LoginMap", "LoginMap")
    build_map("/Game/Maps/CharacterSelectMap", "CharacterSelectMap")

if __name__ == "__main__":
    main()
