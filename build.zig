const std = @import("std");
const os = @import("std").os; // get CurrentWorkDirectory

const engineStepBuilder = @import("Engine/step.zig");
const libUtilsStepBuilder = @import("libUtils/step.zig");
const contentBuilderStepBuilder = @import("ContentBuilder/step.zig");

const libPath = "/usr/lib";
const inclPath = "/usr/include";

pub fn build(b: *std.build.Builder) void {

    //  CWD=$(pwd)
    const PREF = "\"";
    const SUFF = "/zig-out/Resources\"";

    var RESPATH: [100]u8 = undefined;

    @memcpy(RESPATH[0..PREF.len], PREF);
    const cwd = os.getcwd(RESPATH[PREF.len..]) catch @panic("Cannot determine current path directory");
    @memcpy(RESPATH[cwd.len + PREF.len .. PREF.len + cwd.len + SUFF.len], SUFF);

    var compile_shader_step = b.addSystemCommand(&[_][]const u8{ "/home/greg/prgrm/c++/jeu/Engine/Resources/shaders/compile.sh", "/home/greg/prgrm/c++/jeu/zig-out/Resources/shaders" });

    const flags = [_][]const u8{
        "-Wall",
        "-Wextra",
        "-Werror=return-type",
        "-msse3",
        "-std=c11",
        "-std=c++20",
    };

    const optimize = b.standardOptimizeOption(.{});
    const target = b.standardTargetOptions(.{});

    const libUtilsCompileStep = b.addStaticLibrary(.{
        .name = "Util",
        .root_source_file = null,
        .target = target,
        .optimize = optimize,
    });
    // std.Build.addInstallHeaderFile();

    const jeuCompileStep = b.addExecutable(.{
        .name = "Jeu",
        .root_source_file = null,
        .target = target,
        .optimize = optimize,
    });
    jeuCompileStep.defineCMacro("RESOURCES_PATH", RESPATH[0 .. PREF.len + cwd.len + SUFF.len]);

    const contentBuilderCompileStep = b.addExecutable(.{
        .name = "ContentBuilder",
        .root_source_file = null,
        .target = target,
        .optimize = optimize,
    });

    libUtilsStepBuilder.compileLibUtils(&flags, b, libUtilsCompileStep);
    engineStepBuilder.compileEngine(&flags, b, jeuCompileStep, libUtilsCompileStep);
    contentBuilderStepBuilder.compileContentBuilder(&flags, b, contentBuilderCompileStep, libUtilsCompileStep);

    const run_cmd = b.addRunArtifact(jeuCompileStep);

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    var run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);
    run_step.dependOn(&compile_shader_step.step);

    var compile_jeu_step = b.step("compile", "Compiles the game");
    compile_jeu_step.dependOn(&jeuCompileStep.step);
    compile_jeu_step.dependOn(&b.addInstallArtifact(jeuCompileStep, .{}).step);
    compile_jeu_step.dependOn(&b.addInstallArtifact(libUtilsCompileStep, .{}).step);

    var compile_libUtil_step = b.step("compile_utils", "Compiles libUtils");
    compile_libUtil_step.dependOn(&libUtilsCompileStep.step);
    compile_libUtil_step.dependOn(&b.addInstallArtifact(libUtilsCompileStep, .{}).step);

    var compile_contentBuilder_step = b.step("compile_content_builder", "Compiles content builder");
    compile_contentBuilder_step.dependOn(&contentBuilderCompileStep.step);
    compile_contentBuilder_step.dependOn(&b.addInstallArtifact(contentBuilderCompileStep, .{}).step);
    compile_contentBuilder_step.dependOn(&b.addInstallArtifact(libUtilsCompileStep, .{}).step);

    var compile_all_step = b.step("compile_all", "Compiles all");
    compile_all_step.dependOn(compile_jeu_step);
    compile_all_step.dependOn(compile_libUtil_step);
    compile_all_step.dependOn(compile_contentBuilder_step);
}
