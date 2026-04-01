rule("RyuGame")
    on_load(function(target)
        local with_editor = has_config("ryu-build-with-editor")

        if with_editor then
            -- Static link to editor
            target:set("kind", "object")
            target:add("defines", "RYU_WITH_EDITOR")
        else
            -- Standalone executable
            target:set("kind", "binary")
            if not is_mode("debug", "releasedbg") then
                target:add("rules", "win.sdk.application")
            end
        end

        target:add("extrafiles", "Config/**.toml")
    end)

    on_config(function(target)
        import("core.project.project")

        local with_editor = has_config("ryu-build-with-editor")

        if with_editor then
            local editor = project.target("RyuEditor")
            if editor then
                -- Static link
                editor:add("deps", target:name())

                local game_root = target:scriptdir()
                editor:add("runargs", "--App.ProjectDir=" .. game_root)

                -- NOTE: Adding this for now so that testbench is built along with the editor
				--editor:add("deps", target:name())
            end
        end
    end)
rule_end()

