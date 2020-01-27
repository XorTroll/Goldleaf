
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

import java.util.Map;
import org.docopt.Docopt;

public class CommandLineParser {
    private static final String DOCSTRING = "Quark.\n"
            + "\n"
            + "Usage:\n"
            + "  quark.jar [-h] [--config-file=<config-file>]\n"
            + "\n"
            + "Options:\n"
            + "  -h  Show this help message.\n"
            + "  --config-file=<config-file>  Configuration file to use. [default: quark-config.cfg]\n";
    
    public static void parseArguments(String[] args) {
        Docopt parser = new Docopt(DOCSTRING);
        Map<String,Object> parsedArgs = parser.parse(args);
        String configFile = (String) parsedArgs.get("--config-file");
    }
}
