const std = @import("std");
const os = @import("std").os; // get CurrentWorkDirectory
const buildUtils = @import("../buildUtils.zig");

const relPath = "ContentBuilder/src";
pub fn compileContentBuilder(flags: []const []const u8, b: *std.build.Builder, ContentBuilder: *std.build.Step.Compile, LibUtils: *std.build.Step.Compile) void {
    // var ContentBuilderSources = .{relPath ++ "/gltfLoader.cpp"};

    var content_builder_sources = buildUtils.getAllCppFileInDir(relPath, b);
    ContentBuilder.linkSystemLibrary("vulkan");

    ContentBuilder.addIncludePath(.{ .path = "./libUtils/include" });
    ContentBuilder.linkLibrary(LibUtils);

    ContentBuilder.addCSourceFiles(content_builder_sources.items, flags);
    ContentBuilder.linkLibCpp();
    // b.installArtifact(ContentBuilder);
}
