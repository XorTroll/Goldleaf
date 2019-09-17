package xorTroll.goldleaf.quark.fs;

import java.io.File;
import java.nio.file.FileStore;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Vector;

import com.sun.javafx.PlatformUtil;

public class FileSystem
{
    public static final String HomeDrive = "Home";
    public static final java.nio.file.FileSystem DefaultFs = FileSystems.getDefault();

    public static boolean isWindows()
    {
        return PlatformUtil.isWindows();
    }

    public static Vector<String> listDrives()
    {
        Vector<String> drives = new Vector<String>();
        if(isWindows())
        {
            for(Path root: DefaultFs.getRootDirectories())
            {
                try
                {
                    Files.getFileStore(root); // Can we avoid "not working" drives like DVD ROMs like this...?
                    drives.add(root.toString().substring(0, 1));
                }
                catch(Exception e)
                {
                }
            }
        }
        else
        {
            drives.add(HomeDrive);
        }
        return drives;
    }

    public static String getDriveLabel(String drive)
    {
        if(isWindows())
        {
            Path root = Path.of(drive + ":\\");
            try
            {
                FileStore store = Files.getFileStore(root);
                return store.name();
            }
            catch(Exception e)
            {
                return "Drive " + drive;
            }
        }
        return "Home root";
    }

    public static Vector<String> getFilesIn(String path)
    {
        Vector<String> files = new Vector<String>();
        File[] all = new File(path).listFiles();
        for(File f: all)
        {
            if(f.isFile()) files.add(f.getName());
        }
        return files;
    }

    public static Vector<String> getDirectoriesIn(String path)
    {
        Vector<String> files = new Vector<String>();
        File[] all = new File(path).listFiles();
        for(File f: all)
        {
            if(f.isDirectory()) files.add(f.getName());
        }
        return files;
    }

    public static String normalizePath(String path)
    {
        String normalized = path.replace('\\', '/').replace("//", "/");
        if(!isWindows())
        {
            if(normalized.startsWith("/"))
            {
                return HomeDrive + ":" + normalized;
            }
        }
        return normalized;
    }

    public static String denormalizePath(String path)
    {
        String denormalized = path;
        if(isWindows()) return denormalized.replace('/', '\\');
        else
        {
            if(denormalized.startsWith(HomeDrive + ":"))
            {
                return denormalized.replace(HomeDrive + ":", "");
            }
        }
        return denormalized;
    }

    public static void deletePath(File file)
    {
        if(file.isDirectory())
        {
            File[] entries = file.listFiles();
            if(entries != null)
            {
                for(File entry: entries)
                {
                    deletePath(entry);
                }
            }
        }
        file.delete();
    }
}