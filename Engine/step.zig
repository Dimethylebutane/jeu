const std = @import("std");
const os = @import("std").os; // get CurrentWorkDirectory
const buildUtils = @import("../buildUtils.zig");

const relPath = "Engine/src";
pub fn compileEngine(flags: []const []const u8, b: *std.build.Builder, Engine: *std.build.Step.Compile, LibUtils: *std.build.Step.Compile) void {
    // var EngineSources = .{ "Engine/jeu.cpp", relPath ++ "SwapChain.cpp", relPath ++ "Model.cpp", relPath ++ "BuffersUtils.cpp", relPath ++ "QueueUtils.cpp", relPath ++ "Camera.cpp", relPath ++ "DescriptorSet.cpp", relPath ++ "SkyBox.cpp", relPath ++ "ShaderModulesUtils.cpp", relPath ++ "Input.cpp", relPath ++ "FileManagement.cpp" };
    // _ = b;

    var sources = buildUtils.getAllCppFileInDir(relPath, b);
    defer sources.deinit();
    sources.append("Engine/jeu.cpp") catch @panic(",dsbqscbqs,cbsq,cbsd,cbs,d");

    Engine.linkSystemLibrary("vulkan");
    Engine.linkSystemLibrary("glfw");

    // Engine.include_dirs.append(.{"libUtils/include"});
    Engine.addIncludePath(.{ .path = "./libUtils/include" });
    Engine.linkLibrary(LibUtils);

    Engine.addCSourceFiles(sources.items, flags);
    Engine.linkLibC();
    Engine.linkLibCpp();
    // b.installArtifact(Engine);
}
