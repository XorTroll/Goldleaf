package xorTroll.goldleaf.quark;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.nio.file.Paths;
import java.util.Properties;

public class Config
{
    public static final String ConfigPath = "quark-config.cfg";
    public Properties data = new Properties();
    private File cfg;

    public Config()
    {
        try
        {
            cfg = Paths.get(new File(Config.class.getProtectionDomain().getCodeSource().getLocation().toURI()).getParentFile().getPath(), ConfigPath).toFile();
            if(cfg.isFile())
            {
                data.load(new FileInputStream(cfg));
            }
            else cfg.createNewFile();
        }
        catch(Exception e) {}
    }

    public void save()
    {
        try
        {
            data.store(new FileOutputStream(cfg), null);
        }
        catch(Exception e) {}
    }
}