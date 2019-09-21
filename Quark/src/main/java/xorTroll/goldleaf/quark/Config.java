
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2019  XorTroll

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