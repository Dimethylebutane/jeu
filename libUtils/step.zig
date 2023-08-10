const std = @import("std");
const os = @import("std").os; // get CurrentWorkDirectory
const buildUtils = @import("../buildUtils.zig");

const relPath = "libUtils/src";
pub fn compileLibUtils(flags: []const []const u8, b: *std.build.Builder, LibUtils: *std.build.Step.Compile) void {
    //-----------------SOURCES
    // var libUtilSources = .{relPath ++ "/Logger.cpp"};
    var libUtil_sources = buildUtils.getAllCppFileInDir(relPath, b);

    //-----------------Lib Utils
    LibUtils.addCSourceFiles(libUtil_sources.items, flags);
    LibUtils.linkLibC();
    LibUtils.linkLibCpp();
    // b.installArtifact(LibUtils);
}
