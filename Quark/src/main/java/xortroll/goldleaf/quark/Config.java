
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

package xortroll.goldleaf.quark;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.nio.file.Paths;
import java.util.Properties;

public class Config {
    public static String ConfigPathName = "quark-config.cfg";
    public static String ConfigPath = ConfigPathName;
    private Properties inner_cfg;
    private File cfg_file;

    static {
        try {
            ConfigPath = Paths.get(new File(Config.class.getProtectionDomain().getCodeSource().getLocation().toURI()).getParentFile().getPath(), ConfigPathName).toString();
        }
        catch(Exception e) {
            ConfigPath = ConfigPathName;
        }
    }

    public Config() throws Exception {
        this.inner_cfg = new Properties();
        reloadConfigFile();
    }

    public void reloadConfigFile() throws Exception {
        this.cfg_file = Paths.get(ConfigPath).toFile();
        if(this.cfg_file.isFile()) {
            this.inner_cfg.load(new FileInputStream(this.cfg_file));
        }
        else {
            this.cfg_file.createNewFile();
        }
    }

    public void save() throws Exception {
        this.inner_cfg.store(new FileOutputStream(this.cfg_file), null);
    }

    public Properties getContent() {
        return this.inner_cfg;
    }
}