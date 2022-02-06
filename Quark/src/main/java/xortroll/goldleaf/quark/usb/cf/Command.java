
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

import xortroll.goldleaf.quark.usb.cmd.CommandBlock;

public class Command {
    private int id;
    private CommandHandler handler;

    public Command(int id, CommandHandler handler) {
        this.id = id;
        this.handler = handler;
    }

    public int getId() {
        return this.id;
    }

    public void handle(CommandBlock block) {
        this.handler.handle(block);
    }
}