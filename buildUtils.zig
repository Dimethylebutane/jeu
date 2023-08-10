const std = @import("std");
const os = @import("std").os; // get CurrentWorkDirectory

pub fn getAllCppFileInDir(path: []const u8, b: *std.build.Builder) std.ArrayList([]const u8) {
    var sources: std.ArrayList([]const u8) = std.ArrayList([]const u8).init(b.allocator);

    var dir = std.fs.cwd().openIterableDir(path, .{}) catch @panic("KJB?B?HB");

    var walker = dir.walk(b.allocator) catch {
        @panic("Error de merde");
    };
    defer walker.deinit();

    const allowed_exts = [_][]const u8{ ".c", ".cpp", ".cxx", ".c++", ".cc" };
    while (walker.next() catch @panic("Error de merde 2")) |entry| {
        const ext = std.fs.path.extension(entry.basename);
        const include_file = for (allowed_exts) |e| {
            if (std.mem.eql(u8, ext, e))
                break true;
        } else false;
        if (include_file) {
            const path_to_return = b.fmt("{s}/{s}", .{ path, b.dupe(entry.path) });
            // std.log.info("c/cpp file: {s}", .{path_to_return});
            sources.append(path_to_return) catch @panic("Error de merde 3");
        }
    }

    std.log.info("All cpp files in {s} directory have been added", .{path});

    return sources;
}
