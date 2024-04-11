
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2023 XorTroll

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

import java.nio.file.Paths;
import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.DefaultParser;
import org.apache.commons.cli.Options;
import xortroll.goldleaf.quark.ui.MainApplication;

public class Main {
    public static final String ConfigFileOption = "cfgfile";

    public static void main(String[] args) {
        Options options = new Options();
        options.addOption(ConfigFileOption, true, "Config file");
        
        CommandLineParser parser = new DefaultParser();
        try {
            CommandLine cmd = parser.parse(options, args);
            if(cmd.hasOption(ConfigFileOption)) {
                Config.ConfigPath = Paths.get(cmd.getOptionValue(ConfigFileOption));
            }
        }
        catch(Exception e) {
            // TODO
        }

        MainApplication.run(args);
    }
}