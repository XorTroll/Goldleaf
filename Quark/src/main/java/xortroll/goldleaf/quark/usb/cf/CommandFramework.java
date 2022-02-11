
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2022 XorTroll

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

package xortroll.goldleaf.quark.usb.cf;

import java.io.File;
import java.io.RandomAccessFile;
import java.util.List;

import xortroll.goldleaf.quark.Logging;
import xortroll.goldleaf.quark.fs.FileSystem;
import xortroll.goldleaf.quark.ui.MainApplication;
import xortroll.goldleaf.quark.usb.cmd.CommandBlock;

public class CommandFramework {
    public static final int ResultExceptionCaught = 0xBAF1;
    public static final int ResultInvalidIndex = 0xBAF2;
    public static final int ResultInvalidFileMode = 0xBAF3;
    public static final int ResultSelectionCancelled = 0xBAF4;

    public static final int PathTypeInvalid = 0;
    public static final int PathTypeFile = 1;
    public static final int PathTypeDirectory = 2;

    public static final int FileModeRead = 1;
    public static final int FileModeWrite = 2;
    public static final int FileModeAppend = 3;
    
    private static List<String> drives;

    private static RandomAccessFile started_read_file;
    private static RandomAccessFile started_write_file;

    private static boolean isValidFileMode(int mode) {
        return (mode == FileModeRead) || (mode == FileModeWrite) || (mode == FileModeAppend);
    }

    private static void ensureFileStarted(String path, int mode) throws Exception {
        if(mode == FileModeRead) {
            if(started_read_file != null) {
                started_read_file.close();
            }
            started_read_file = new RandomAccessFile(path, "r");
        }
        else {
            if(started_write_file != null) {
                started_write_file.close();
            }
            started_write_file = new RandomAccessFile(path, "rw");
            if(mode == FileModeAppend) {
                started_write_file.seek(started_write_file.length());
            }
        }
    }

    private static void ensureFileEnded(int mode) throws Exception {
        if(mode == FileModeRead) {
            if(started_read_file != null) {
                started_read_file.close();
                started_read_file = null;
            }
        }
        else {
            if(started_write_file != null) {
                started_write_file.close();
                started_write_file = null;
            }
        }
    }

    public static Command GetDriveCount = new Command(1, new CommandHandler() {
        public void handle(CommandBlock block) {
            drives = FileSystem.listDrives();

            int drive_count = drives.size();
            Logging.log("[cf] GetDriveCount() -> count: " + drive_count);

            block.responseStart();
            block.write32(drive_count);
            block.responseEnd();
        }
    });

    public static Command GetDriveInfo = new Command(2, new CommandHandler() {
        public void handle(CommandBlock block) {
            if(drives != null) {
                drives = FileSystem.listDrives();
            }

            int drive_idx = block.read32();
            if(drive_idx < drives.size()) {
                String drive_path = drives.get(drive_idx);
                String drive_name = FileSystem.getDriveLabel(drive_path);
                long drive_total_size = 0; // TODO
                long drive_free_size = 0; // TODO
                Logging.log("[cf] GetDriveInfo(idx: " + drive_idx + ") -> path: '" + drive_path + "', name: '" + drive_name + "', total_size: " + drive_total_size + ", free_size: " + drive_free_size);

                block.responseStart();
                block.writeString(drive_name);
                block.writeString(drive_path);
                block.write64(drive_total_size);
                block.write64(drive_free_size);
                block.responseEnd();
            }
            else {
                block.respondFailure(ResultInvalidIndex);
            }
        }
    });

    public static Command StatPath = new Command(3, new CommandHandler() {
        public void handle(CommandBlock block) {
            String path = FileSystem.denormalizePath(block.readString());

            try {
                File f_path = new File(path);

                int path_type = PathTypeInvalid;
                long file_size = 0;
                if(f_path.isFile()) {
                    path_type = PathTypeFile;
                    file_size = f_path.length();
                }
                else if(f_path.isDirectory()) {
                    path_type = PathTypeDirectory;
                }
                Logging.log("[cf] StatPath(path: '" + path + "') -> path_type: " + path_type + ", file_size: " + file_size);
                
                block.responseStart();
                block.write32(path_type);
                block.write64(file_size);
                block.responseEnd();
            }
            catch(Exception e) {
                block.respondFailure(ResultExceptionCaught);
            }
        }
    });

    public static Command GetFileCount = new Command(4, new CommandHandler() {
        public void handle(CommandBlock block) {
            String path = FileSystem.denormalizePath(block.readString());

            List<String> files = FileSystem.getFilesIn(path);
            int file_count = files.size();
            Logging.log("[cf] GetFileCount(path: '" + path + "') -> count: " + file_count);

            block.responseStart();
            block.write32(file_count);
            block.responseEnd();
        }
    });

    public static Command GetFile = new Command(5, new CommandHandler() {
        public void handle(CommandBlock block) {
            String path = FileSystem.denormalizePath(block.readString());
            int file_idx = block.read32();

            List<String> files = FileSystem.getFilesIn(path);
            if(file_idx < files.size()) {
                String file = files.get(file_idx);
                Logging.log("[cf] GetFile(path: '" + path + "', idx: " + file_idx + ") -> file: '" + file + "'");

                block.responseStart();
                block.writeString(file);
                block.responseEnd();
            }
            else {
                block.respondFailure(ResultInvalidIndex);
            }
        }
    });

    public static Command GetDirectoryCount = new Command(6, new CommandHandler() {
        public void handle(CommandBlock block) {
            String path = FileSystem.denormalizePath(block.readString());

            List<String> dirs = FileSystem.getDirectoriesIn(path);
            int dir_count = dirs.size();
            Logging.log("[cf] GetDirectoryCount(path: '" + path + "') -> count: " + dir_count);

            block.responseStart();
            block.write32(dir_count);
            block.responseEnd();
        }
    });

    public static Command GetDirectory = new Command(7, new CommandHandler() {
        public void handle(CommandBlock block) {
            String path = FileSystem.denormalizePath(block.readString());
            int subdir_idx = block.read32();

            List<String> dirs = FileSystem.getDirectoriesIn(path);
            if(subdir_idx < dirs.size()) {
                String dir = dirs.get(subdir_idx);
                Logging.log("[cf] GetDirectory(path: '" + path + "', idx: " + subdir_idx + ") -> dir: '" + dir + "'");

                block.responseStart();
                block.writeString(dir);
                block.responseEnd();
            }
            else {
                block.respondFailure(ResultInvalidIndex);
            }
        }
    });

    public static Command StartFile = new Command(8, new CommandHandler() {
        public void handle(CommandBlock block) {
            String path = FileSystem.denormalizePath(block.readString());
            int mode = block.read32();

            Logging.log("[cf] StartFile(path: '" + path + "', mode: " + mode + ")");

            try {
                if(isValidFileMode(mode)) {
                    ensureFileStarted(path, mode);
                    block.respondEmpty();
                }
                else {
                    block.respondFailure(ResultInvalidFileMode);
                }
            }
            catch(Exception e) {
                block.respondFailure(ResultExceptionCaught);
            }
        }
    });

    public static Command ReadFile = new Command(9, new CommandHandler() {
        public void handle(CommandBlock block) {
            String path = FileSystem.denormalizePath(block.readString());
            long offset = block.read64();
            long size = block.read64();

            try {
                boolean close_file = false;
                RandomAccessFile read_file = started_read_file;
                if(read_file == null) {
                    read_file = new RandomAccessFile(path, "r");
                    close_file = true;
                }

                byte[] data = new byte[(int)size];
                read_file.seek(offset);
                int read = read_file.read(data, 0, (int)size);
                if(close_file) {
                    read_file.close();
                }

                Logging.log("[cf] ReadFile(path: '" + path + "', offset: " + offset + ", size: " + size + ") -> read_size: " + read);

                block.responseStart();
                block.write64((long)read);
                block.responseEnd();
                block.sendBuffer(data);
            }
            catch(Exception e) {
                block.respondFailure(ResultExceptionCaught);
            }
        }
    });

    public static Command WriteFile = new Command(10, new CommandHandler() {
        public void handle(CommandBlock block) {
            String path = FileSystem.denormalizePath(block.readString());
            long size = block.read64();
            byte[] data = block.getBuffer((int)size);

            try {
                boolean close_file = false;
                RandomAccessFile write_file = started_write_file;
                if(write_file == null) {
                    write_file = new RandomAccessFile(path, "rw");
                    close_file = true;
                }

                write_file.write(data);
                if(close_file) {
                    write_file.close();
                }

                Logging.log("[cf] WriteFile(path: '" + path + "', size: " + size + ") -> written_size: " + size);

                block.responseStart();
                block.write64(size);
                block.responseEnd();
            }
            catch(Exception e) {
                block.respondFailure(ResultExceptionCaught);
            }
        }
    });

    public static Command EndFile = new Command(11, new CommandHandler() {
        public void handle(CommandBlock block) {
            int mode = block.read32();

            Logging.log("[cf] EndFile(mode: " + mode + ")");

            try {
                if(isValidFileMode(mode)) {
                    ensureFileEnded(mode);
                    block.respondEmpty();
                }
                else {
                    block.respondFailure(ResultInvalidFileMode);
                }
            }
            catch(Exception e) {
                block.respondFailure(ResultExceptionCaught);
            }
        }
    });

    public static Command Create = new Command(12, new CommandHandler() {
        public void handle(CommandBlock block) {
            String path = FileSystem.denormalizePath(block.readString());
            int path_type = block.read32();

            Logging.log("[cf] Create(path: '" + path + "', path_type: " + path_type + ")");

            try {
                File f_path = new File(path);
                if(path_type == PathTypeFile) {
                    f_path.createNewFile();
                }
                else if(path_type == PathTypeDirectory) {
                    f_path.mkdir();
                }
                block.respondEmpty();
            }
            catch(Exception e) {
                block.respondFailure(ResultExceptionCaught);
            }
        }
    });

    public static Command Delete = new Command(13, new CommandHandler() {
        public void handle(CommandBlock block) {
            String path = FileSystem.denormalizePath(block.readString());

            Logging.log("[cf] Delete(path: '" + path + "')");

            try {
                FileSystem.deletePath(new File(path));
                block.respondEmpty();
            }
            catch(Exception e) {
                block.respondFailure(ResultExceptionCaught);
            }
        }
    });

    public static Command Rename = new Command(14, new CommandHandler() {
        public void handle(CommandBlock block) {
            String path = FileSystem.denormalizePath(block.readString());
            String new_name = FileSystem.denormalizePath(block.readString());

            Logging.log("[cf] Rename(path: '" + path + "', new_name: '" + new_name + "'')");

            try {
                File f_path = new File(path);
                f_path.renameTo(new File(f_path.getParent(), new_name));
                block.respondEmpty();
            }
            catch(Exception e) {
                block.respondFailure(ResultExceptionCaught);
            }
        }
    });

    public static Command GetSpecialPathCount = new Command(15, new CommandHandler() {
        public void handle(CommandBlock block) {
            int special_path_count = MainApplication.getSpecialPathCount();
            Logging.log("[cf] GetSpecialPathCount() -> count: " + special_path_count);

            block.responseStart();
            block.write32(special_path_count);
            block.responseEnd();
        }
    });

    public static Command GetSpecialPath = new Command(16, new CommandHandler() {
        public void handle(CommandBlock block) {
            int special_path_idx = block.read32();

            String[] special_path_info = MainApplication.getSpecialPath(special_path_idx);
            if(special_path_info != null) {
                String special_path_name = special_path_info[0];
                String special_path = FileSystem.normalizePath(special_path_info[1]);
                Logging.log("[cf] GetSpecialPath(idx: " + special_path_idx + ") -> name: '" + special_path_name + "', path: '" + special_path + "'");

                block.responseStart();
                block.writeString(special_path_name);
                block.writeString(special_path);
                block.responseEnd();
            }
            else {
                block.respondFailure(ResultInvalidIndex);
            }
        }
    });

    public static Command SelectFile = new Command(17, new CommandHandler() {
        public void handle(CommandBlock block) {
            String path = MainApplication.selectFile();
            if(path != null) {
                Logging.log("[cf] SelectFile() -> path: '" + path + "'");

                block.responseStart();
                block.writeString(FileSystem.normalizePath(path));
                block.responseEnd();
            }
            else {
                block.respondFailure(ResultSelectionCancelled);
            }
        }
    });

    public static Command[] AvailableCommands = {
        GetDriveCount,
        GetDriveInfo,
        StatPath,
        GetFileCount,
        GetFile,
        GetDirectoryCount,
        GetDirectory,
        StartFile,
        ReadFile,
        WriteFile,
        EndFile,
        Create,
        Delete,
        Rename,
        GetSpecialPathCount,
        GetSpecialPath,
        SelectFile
    };
}