
include "dependencies.lua"

workspace "PFF"
	platforms "x64"
	startproject "PFF_editor"

	configurations
	{
		"Debug",
		"RelWithDebInfo",
		"Release",
	}

	flags
	{
		"MultiProcessorCompile"
	}

	-- TODO: PFF_USING_EDITOR should not be defined in the backaged builds of game
	defines
	{
		"PFF_USING_EDITOR",
	}

	outputs  = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	if os.target() == "linux" then
		print("---------- target platform is linux => manually compile GLFW ----------")
		os.execute("cmake -S ./PFF/vendor/glfw -B ./PFF/vendor/glfw/build")		-- manuel compilation
		os.execute("cmake --build ./PFF/vendor/glfw/build")						-- manuel compilation
		print("---------- Done compiling GLFW ----------")
	end

group "dependencies"
	include "PFF/vendor/fastgltf"
	include "PFF/vendor/imgui"
	if os.target() == "windows" then
		include "PFF/vendor/glfw"
	end
group ""

function copy_content_of_dir(dir_name)

    -- Resolve the target and source directories
    local target_dir = path.join(_WORKING_DIR, "bin", outputs, dir_name)
    local source_dir = path.join(_WORKING_DIR, dir_name)

    -- Ensure the target directory exists
	print("Copying directory: " .. source_dir .. " to " .. target_dir)
    os.execute("mkdir " .. target_dir)

    -- -- Determine the operating system
    -- local is_windows = os.getenv("OS") == "Windows_NT"

    -- if is_windows then
    --     -- Use xcopy for Windows
    --     os.execute('xcopy "' .. source_dir .. '\\*" "' .. target_dir .. '" /E /I /Y')
    -- else
    --     -- Use cp for Linux
    --     os.execute('cp -r "' .. source_dir .. '"/* "' .. target_dir .. '"')
    -- end
end

-- function copy_content_of_dir(dir_name)
--     -- Resolve the target and source directories
--     local target_dir = path.join(_WORKING_DIR, "bin", outputs, dir_name)
--     local source_dir = path.join(_WORKING_DIR, dir_name)

--     -- Ensure the target directory exists
--     print("Copying directory: " .. source_dir .. " to " .. target_dir)
    
--     -- Create the target directory and any necessary parent directories
--     os.execute("mkdir -p " .. target_dir)  -- Use -p to create parent directories if they don't exist

--     -- Determine the operating system
--     local is_windows = os.getenv("OS") == "Windows_NT"

--     if is_windows then
--         -- Use xcopy for Windows
--         os.execute('xcopy "' .. source_dir .. '\\*" "' .. target_dir .. '" /E /I /Y')
--     else
--         -- Use cp for Linux
--         os.execute('cp -r "' .. source_dir .. '"/* "' .. target_dir .. '"')
--     end
-- end


group "Engine"
	include "PFF"
group ""

group "Tools"
	include "PFF_editor"
	include "PFF_helper"
group ""
