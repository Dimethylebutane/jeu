const std = @import("std");
const os = @import("std").os; // get CurrentWorkDirectory

const relPath = "libUtils/src";
pub fn compileLibUtils(flags: []const []const u8, b: *std.build.Builder, LibUtils: *std.build.Step.Compile) void {
    //-----------------SOURCES
    var libUtilSources = .{relPath ++ "/Logger.cpp"};

    _ = b;

    //-----------------Lib Utils
    LibUtils.addCSourceFiles(&libUtilSources, flags);
    LibUtils.linkLibC();
    LibUtils.linkLibCpp();
    // b.installArtifact(LibUtils);
}
