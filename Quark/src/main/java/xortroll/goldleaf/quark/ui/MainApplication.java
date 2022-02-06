
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

package xortroll.goldleaf.quark.ui;

import java.io.File;
import java.util.Enumeration;
import java.util.List;
import java.util.Optional;
import java.util.Properties;
import java.util.function.BiConsumer;

import javafx.scene.control.ButtonBar.ButtonData;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Dialog;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.concurrent.Task;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.scene.layout.Pane;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import xortroll.goldleaf.quark.Config;
import xortroll.goldleaf.quark.Version;
import xortroll.goldleaf.quark.usb.USBInterface;
import xortroll.goldleaf.quark.usb.cf.Command;
import xortroll.goldleaf.quark.usb.cf.CommandFramework;
import xortroll.goldleaf.quark.usb.cmd.CommandBlock;

public class MainApplication extends Application {
    public static final Version QuarkVersion = new Version(0, 5, 0);
    public static final Version MinimumGoldleafVersion = new Version(0, 10, 0);

    private static Config special_path_cfg;
    private static Object special_path_cfg_lock;

    private static Stage stage;

    private static Object file_selected_lock = new Object();
    private static boolean file_selected = false;
    private static String selected_file = null;

    private MainController controller;
    private Scene scene;

    private USBInterface usb_intf = null;

    private Object dialog_wait_lock = new Object();
    private boolean is_dialog_finished = false;

    private void die() {
        if(this.usb_intf != null) {
            this.usb_intf.finalize();
        }

        Platform.exit();
        System.exit(0);
    }

    private void showOkDialogFromTask(String subtitle, String message, boolean terminate) {
        is_dialog_finished = false;

        Platform.runLater(() -> {
            Dialog<ButtonType> dialog = new Dialog<ButtonType>();
            dialog.setTitle("Quark - " + subtitle);
            dialog.setContentText(message);

            ButtonType type = new ButtonType("Ok", ButtonData.OK_DONE);
            dialog.getDialogPane().getButtonTypes().add(type);

            dialog.showAndWait();

            if(terminate) {
                die();
            }
            synchronized(dialog_wait_lock) {
                is_dialog_finished = true;
            }
        });

        while(true) {
            synchronized(dialog_wait_lock) {
                if(is_dialog_finished) {
                    break;
                }
            }
        }
    }

    private USBInterface showUsbFailReconnectDialogFromTask(String message, boolean initial_start_silent) {
        boolean skip_dialog = initial_start_silent;
        String show_message = message;
        while(true) {
            if(!skip_dialog) {
                is_dialog_finished = false;

                String dialog_msg = show_message;
                Platform.runLater(() -> {
                    Dialog<ButtonType> dialog = new Dialog<ButtonType>();
                    dialog.setTitle("Quark - USB connection");
                    dialog.setContentText(dialog_msg);

                    dialog.getDialogPane().getButtonTypes().add(new ButtonType("Reconnect", ButtonData.YES));
                    dialog.getDialogPane().getButtonTypes().add(new ButtonType("Exit", ButtonData.NO));

                    dialog.showAndWait().ifPresent(ret_type -> {
                        if(ret_type.getButtonData() == ButtonData.NO) {
                            die();
                        }
                    });

                    synchronized(dialog_wait_lock) {
                        is_dialog_finished = true;
                    }
                });

                while(true) {
                    synchronized(dialog_wait_lock) {
                        if(is_dialog_finished) {
                            break;
                        }
                    }
                }
            }

            skip_dialog = false;

            // Try reconnecting, otherwise continue asking the user
            Optional<USBInterface> maybe_intf = USBInterface.createInterface(0);
            if(maybe_intf.isPresent()) {
                USBInterface usb_intf = maybe_intf.get();
                Version product_version = usb_intf.getProductVersion();
                boolean is_dev_version = usb_intf.isDevVersion();

                if(product_version == null) {
                    show_message = "The connection found doesn't seem to be Goldleaf";
                }
                else if(product_version.olderThan(MinimumGoldleafVersion)) {
                    show_message = "The Goldleaf Quark connected to is outdated.\nPlease update to v" + MinimumGoldleafVersion.toString() + " or higher.";
                }
                else {
                    if(is_dev_version) {
                        showOkDialogFromTask("Development version", "The connected Goldleaf (v" + product_version.toString() + ") is a development build.\nThis build might be unstable. Use it at your own risk!", false);
                    }
    
                    return usb_intf;
                }

                usb_intf.finalize();
            }
            else {
                show_message = "Unable to connect via USB.";
            }
        }
    }

    @Override
    public void start(Stage primaryStage) throws Exception {
        ClassLoader this_loader = getClass().getClassLoader();
        FXMLLoader loader = new FXMLLoader(this_loader.getResource("Main.fxml"));
        Pane base = loader.load();

        special_path_cfg = new Config();
        special_path_cfg_lock = new Object();

        double width = base.getPrefWidth();
        double height = base.getPrefHeight();

        this.scene = new Scene(base, width, height);
        this.scene.getStylesheets().add(this_loader.getResource("Main.css").toExternalForm());

        this.controller = loader.getController();
        
        stage = primaryStage;
        stage.getIcons().add(new Image(this_loader.getResource("Icon.png").toExternalForm()));
        stage.setTitle("Quark v" + QuarkVersion.toString() + " - Goldleaf's USB client");
        stage.setScene(this.scene);
        stage.setMinWidth(width);
        stage.setMinHeight(height);

        Task<Void> usb_task = new Task<Void>() {
            @Override
            protected Void call() throws Exception {
                USBInterface usb_intf = showUsbFailReconnectDialogFromTask("", true);
                updateMessage("Connected to Goldleaf v" + usb_intf.getProductVersion().toString() + (usb_intf.isDevVersion() ? " (dev build)" : "") + " - Processing USB input...");

                while(true) {
                    CommandBlock block = new CommandBlock(usb_intf);
                    if(!block.isValid()) {
                        usb_intf.finalize();

                        usb_intf = showUsbFailReconnectDialogFromTask("Unable to receive data from Goldleaf.", false);
                        updateMessage("Reconnected! Processing USB input from Goldleaf...");
                    }

                    int cmd_id = block.validateCommand();
                    boolean handled = false;
                    for(Command cmd : CommandFramework.AvailableCommands) {
                        if(cmd.getId() == cmd_id) {
                            cmd.handle(block);
                            handled = true;
                            break;
                        }
                    }
                    if(!handled) {
                        usb_intf = showUsbFailReconnectDialogFromTask("An invalid command was received from Goldleaf.", false);
                        updateMessage("Reconnected! Processing USB input from Goldleaf...");
                        break;
                    }
                }

                die();
                return null;
            }
        };

        controller.prepare(usb_task);
        stage.show();

        Thread usb_thread = new Thread(usb_task);
        usb_thread.setDaemon(true);
        usb_thread.start();
    }

    public static Stage getStage() {
        return stage;
    }

    public static int getSpecialPathCount() {
        synchronized(special_path_cfg_lock) {
            return special_path_cfg.getContent().size();
        }
    }

    public static String[] getSpecialPath(int idx) {
        synchronized(special_path_cfg_lock) {
            Properties special_path_cnt = special_path_cfg.getContent();
            if(idx < special_path_cnt.size()) {
                int tmp_idx = 0;
                Enumeration<?> enums = special_path_cnt.propertyNames();
                while(enums.hasMoreElements()) {
                    String key = (String)enums.nextElement();
                    String value = special_path_cnt.getProperty(key);
                    if(tmp_idx == idx) {
                        return new String[] { key, value };
                    }
                    tmp_idx++;
                }
            }
            
            return null;
        }
    }

    public static String selectFile() {
        synchronized(file_selected_lock) {
            file_selected = false;
            selected_file = null;
        }

        Platform.runLater(() -> {
            synchronized(file_selected_lock) {
                File tmp_file = new FileChooser().showOpenDialog(stage);
                if(tmp_file != null) {
                    selected_file = tmp_file.toString();
                }

                file_selected = true;
            }
        });

        while(true) {
            synchronized(file_selected_lock) {
                if(file_selected) {
                    break;
                }
            }
        }

        return selected_file;
    }

    public static void addSpecialPath(String name, String path) throws Exception {
        synchronized(special_path_cfg_lock) {
            Properties special_path_cnt = special_path_cfg.getContent();
            special_path_cnt.setProperty(name, path);
            special_path_cfg.save();
        }
    }

    public static void removeSpecialPaths(List<String> names) throws Exception {
        synchronized(special_path_cfg_lock) {
            Properties special_path_cnt = special_path_cfg.getContent();
            for(String name : names) {
                special_path_cnt.remove(name);
            }
            special_path_cfg.save();
        }
    }

    public static void forEachSpecialPath(BiConsumer<? super Object,? super Object> fn) throws Exception {
        synchronized(special_path_cfg_lock) {
            Properties special_path_cnt = special_path_cfg.getContent();
            special_path_cnt.forEach(fn);
        }
    }
    
    public static void run(String[] args) {
        Application.launch(args);
    }
}