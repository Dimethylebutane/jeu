const std = @import("std");
const os = @import("std").os; // get CurrentWorkDirectory

const relPath = "ContentBuilder/src";
pub fn compileContentBuilder(flags: []const []const u8, b: *std.build.Builder, ContentBuilder: *std.build.Step.Compile, LibUtils: *std.build.Step.Compile) void {
    var ContentBuilderSources = .{relPath ++ "/gltfLoader.cpp"};

    ContentBuilder.linkSystemLibrary("vulkan");

    ContentBuilder.addIncludePath(.{ .path = "./libUtils/include" });
    ContentBuilder.linkLibrary(LibUtils);

    ContentBuilder.addCSourceFiles(&ContentBuilderSources, flags);
    ContentBuilder.linkLibCpp();
    b.installArtifact(ContentBuilder);
}
