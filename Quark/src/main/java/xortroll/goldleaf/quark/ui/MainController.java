
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright © 2018-2025 XorTroll

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

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.OutputStream;
import java.io.PrintStream;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;
import javafx.scene.control.Button;
import javafx.scene.control.ListView;
import javafx.scene.control.TextArea;
import javafx.scene.control.ToggleButton;
import javafx.scene.control.TextInputDialog;
import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.collections.ObservableList;
import javafx.concurrent.Task;
import javafx.fxml.FXML;
import javafx.scene.control.Label;
import javafx.scene.layout.Pane;
import javafx.stage.DirectoryChooser;

public class MainController {

    public class UILogger extends OutputStream {
        // Output is buffered and pushed to the UI in batches: queueing one UI task per
        // written byte (and never trimming the text area) pins the CPU during transfers,
        // since every command Goldleaf sends gets logged
        private static final int FlushIntervalMs = 250;
        private static final int MaxChars = 100000;
        private static final int KeepChars = 50000;

        private TextArea area;
        private ByteArrayOutputStream pending = new ByteArrayOutputStream();
        private Object pending_lock = new Object();
        private boolean ui_update_queued = false;

        public UILogger(TextArea area) {
            this.area = area;

            ScheduledExecutorService flusher = Executors.newSingleThreadScheduledExecutor(runnable -> {
                Thread thread = new Thread(runnable, "Quark-UILogger-Flush");
                thread.setDaemon(true);
                return thread;
            });
            flusher.scheduleWithFixedDelay(this::requestFlush, FlushIntervalMs, FlushIntervalMs, TimeUnit.MILLISECONDS);
        }

        public void writeString(String data) {
            byte[] bytes = data.getBytes(StandardCharsets.UTF_8);
            this.write(bytes, 0, bytes.length);
        }

        @Override
        public void write(int b) {
            synchronized(this.pending_lock) {
                this.pending.write(b);
            }
        }

        @Override
        public void write(byte[] b, int off, int len) {
            synchronized(this.pending_lock) {
                this.pending.write(b, off, len);
            }
        }

        private void requestFlush() {
            synchronized(this.pending_lock) {
                if(this.pending.size() == 0 || this.ui_update_queued) {
                    return;
                }
                this.ui_update_queued = true;
            }

            Platform.runLater(this::flushToUi);
        }

        private void flushToUi() {
            String data;
            synchronized(this.pending_lock) {
                data = new String(this.pending.toByteArray(), StandardCharsets.UTF_8);
                this.pending.reset();
                this.ui_update_queued = false;
            }
            if(data.isEmpty()) {
                return;
            }

            // Keep the text area bounded
            int overflow = this.area.getLength() + data.length() - MaxChars;
            if(overflow > 0) {
                int cut = Math.min(this.area.getLength(), Math.max(overflow, this.area.getLength() - KeepChars));
                this.area.deleteText(0, cut);
            }
            this.area.appendText(data);
        }
    }

    @FXML private Label USBStatusLabel;
    @FXML private Pane MainPane;
    @FXML private Button NewPathButton;
    @FXML private Button PathRemoveButton;
    @FXML private TextArea LogArea;
    @FXML private ListView<String> PathList;
    @FXML private ToggleButton ShowLogsToggle;
    private List<String> special_path_names;

    private void refreshConfig() throws Exception {
        PathList.getItems().clear();
        this.special_path_names.clear();

        MainApplication.forEachSpecialPath((name, path) -> {
            this.special_path_names.add((String)name);
            PathList.getItems().add(name + " (" + path + ")");
        });
    }

    public void prepare(Task<Void> usb_task) {
        this.special_path_names = new ArrayList<>();
        try {
            this.refreshConfig();
        }
        catch(Exception e) {
            // TODO
        }
        
        NewPathButton.setOnAction(event -> {
            DirectoryChooser chooser = new DirectoryChooser();
            File dir = chooser.showDialog(MainApplication.getStage());
            if(dir != null) {
                TextInputDialog dialog = new TextInputDialog(dir.getName());
                dialog.setTitle("Quark - Add special path");
                dialog.setHeaderText("Set the special path's name\n(leave the box empty to cancel)");

                Optional<String> result = dialog.showAndWait();
                if(result.isPresent()) {
                    String special_path_name = result.get();
                    String special_path = dir.toString();

                    try {
                        MainApplication.addSpecialPath(special_path_name, special_path);
                        this.refreshConfig();
                    }
                    catch(Exception e) {
                        // TODO
                    }
                }
            }
        });

        PathRemoveButton.setOnAction(event -> {
            ObservableList<Integer> selected_path_idxs = PathList.getSelectionModel().getSelectedIndices();
            List<String> names_to_remove = new ArrayList<>();
            selected_path_idxs.forEach(idx -> {
                names_to_remove.add(this.special_path_names.get(idx));
            });

            try {
                MainApplication.removeSpecialPaths(names_to_remove);
                this.refreshConfig();
            }
            catch(Exception e) {
                // TODO
            }
        });

        ShowLogsToggle.setSelected(true);
        LogArea.visibleProperty().bind(ShowLogsToggle.selectedProperty());
        System.setOut(new PrintStream(new UILogger(LogArea)));
        USBStatusLabel.textProperty().bind(usb_task.messageProperty());
    }
}
