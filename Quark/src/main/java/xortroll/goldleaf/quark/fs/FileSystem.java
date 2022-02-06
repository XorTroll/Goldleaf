
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

package xortroll.goldleaf.quark.fs;

import java.io.File;
import java.nio.file.FileStore;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;

public class FileSystem {
    public static final String HomeDrive = "Home";

    private static String os = System.getProperty("os.name").toLowerCase();
   
    public static boolean isWindows() {
        return os.indexOf("windows") >= 0;
    }

    public static List<String> listDrives() {
        List<String> drives = new ArrayList<String>();
        if(isWindows()) {
            java.nio.file.FileSystem fs = FileSystems.getDefault();
            for(Path root: fs.getRootDirectories()) {
                try {
                    Files.getFileStore(root); // Can we avoid "not working" drives like DVD ROMs like this...?
                    drives.add(root.toString().substring(0, 1));
                }
                catch(Exception e) {}
            }
        }
        else {
            // TODO: another way of finding non-Windows drives?
            drives.add(HomeDrive);
        }
        return drives;
    }

    public static String getDriveLabel(String drive) {
        if(isWindows()) {
            Path root = Paths.get(drive + ":\\");
            try {
                FileStore store = Files.getFileStore(root);
                String name = store.name();
                return name;
            }
            catch(Exception e) {
                return "Drive " + drive;
            }
        }
        return "Home root";
    }

    public static List<String> getFilesIn(String path) {
        List<String> files = new ArrayList<String>();
        File[] all = new File(path).listFiles();
        for(File f: all) {
            if(f.isFile()) {
                files.add(f.getName());
            }
        }
        return files;
    }

    public static List<String> getDirectoriesIn(String path) {
        List<String> files = new ArrayList<String>();
        File[] all = new File(path).listFiles();
        for(File f: all) {
            if(f.isDirectory()) {
                files.add(f.getName());
            }
        }
        return files;
    }

    public static String normalizePath(String path) {
        String normalized = path.replace('\\', '/').replace("//", "/");
        if(!isWindows()) {
            if(normalized.startsWith("/")) {
                return HomeDrive + ":" + normalized;
            }
        }
        return normalized;
    }

    public static String denormalizePath(String path) {
        String denormalized = path;
        if(isWindows()) {
            return denormalized.replace('/', '\\');
        }
        else {
            if(denormalized.startsWith(HomeDrive + ":")) {
                return denormalized.replace(HomeDrive + ":", "");
            }
        }
        return denormalized;
    }

    public static void deletePath(File file) {
        if(file.isDirectory()) {
            File[] entries = file.listFiles();
            if(entries != null) {
                for(File entry: entries) {
                    deletePath(entry);
                }
            }
        }
        file.delete();
    }
}