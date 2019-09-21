
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

package xorTroll.goldleaf.quark.ui;

import java.io.File;
import java.io.RandomAccessFile;
import java.util.Enumeration;
import java.util.Optional;
import java.util.Vector;

import com.jfoenix.controls.JFXButton;
import com.jfoenix.controls.JFXAlert;
import com.jfoenix.controls.JFXDialogLayout;

import javafx.application.Application;
import javafx.application.Platform;
import javafx.concurrent.Task;
import javafx.fxml.FXMLLoader;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.scene.layout.Pane;
import javafx.scene.text.Text;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import javafx.stage.Modality;
import xorTroll.goldleaf.quark.Config;
import xorTroll.goldleaf.quark.Version;
import xorTroll.goldleaf.quark.usb.Command;
import xorTroll.goldleaf.quark.usb.USBInterface;
import xorTroll.goldleaf.quark.fs.FileSystem;
import xorTroll.goldleaf.quark.ui.MainController;

public class MainApplication extends Application
{
    public static final Version QuarkVer = new Version(0, 2, 0);

    public MainController controller;
    public Stage stage;
    public Scene scene;
    public RandomAccessFile openedfile = null;
    public USBInterface usbInterface = null;
    public String openedpath = null;

    public Object selectlock = new Object();
    public boolean selected = false;
    public String selectedfile = null;

    public Object daemonlock = new Object();
    public boolean daemonflag = false;

    public Object cfglock = new Object();
    public Config cfg;

    public void die()
    {
        if(usbInterface != null) usbInterface.finalize();
        Platform.exit();
        System.exit(0);
    }

    public void showDialog(String subtitle, String message, String buttontext, boolean terminate)
    {
        daemonflag = false;
        Platform.runLater(() ->
        {
            JFXDialogLayout lyt = new JFXDialogLayout();
            lyt.setHeading(new Text("Quark - " + subtitle));
            lyt.setBody(new Text(message));

            JFXAlert<Void> al = new JFXAlert<>(stage);
            al.initModality(Modality.APPLICATION_MODAL);
            al.setOverlayClose(false);

            JFXButton ok = new JFXButton(buttontext);
            ok.setAlignment(Pos.BOTTOM_CENTER);
            ok.setOnAction(event ->
            {
                al.close();
            });

            lyt.setActions(ok);
            al.setContent(lyt);
            al.showAndWait();

            if(terminate) die();
            synchronized(daemonlock)
            {
                daemonflag = true;
            }
        });
        while(true)
        {
            synchronized(daemonlock)
            {
                if(daemonflag) break;
            }
        }
    }

    @Override
    public void start(Stage primaryStage) throws Exception
    {
        FXMLLoader loader = new FXMLLoader(getClass().getClassLoader().getResource("Main.fxml"));
        Pane base = loader.load();
        cfg = new Config();

        scene = new Scene(base, 590, 390);
        scene.getStylesheets().add(getClass().getClassLoader().getResource("Main.css").toExternalForm());
        controller = loader.getController();
        primaryStage.getIcons().add(new Image(getClass().getClassLoader().getResource("Icon.png").toExternalForm()));
        primaryStage.setResizable(false);
        stage = primaryStage;
        stage.setTitle("Quark v" + QuarkVer.toString() + " - Goldleaf USB client");
        stage.setScene(scene);

        Task<Void> usbtask = new Task<Void>()
        {
            @Override
            protected Void call() throws Exception
            {
                Optional<USBInterface> intf = USBInterface.createInterface(0);
                if(!intf.isPresent())
                {
                    showDialog("USB fail", "No USB connection was found. Check the following:\n\n - Is the console connected properly to this system?\n - Are you sure Goldleaf is open? \n - Is a small USB icon shown above?", "Ok", true);
                }
                usbInterface = intf.get();
                updateMessage("Connected! Processing USB input from Goldleaf...");
                Vector<String> drives = null;
                while(true)
                {
                    Command c = new Command(usbInterface);
                    if(!c.isValid())
                    {
                        usbInterface.finalize();
                        usbInterface = null;
                        showDialog("Bad USB response", "USB isn't responding corrently (Goldleaf has been closed?, USB cable stopped working?)\n\n - If you want to reconnect, close this dialog and Quark will attempt to do so.\n - If no connection is found again Quark will close.\n\n - If you want to exit Quark, close this dialog.", "Ok", false);
                        Optional<USBInterface> intf2 = USBInterface.createInterface(0);
                        if(intf2.isPresent())
                        {
                            updateMessage("Reconnected! Processing USB input from Goldleaf...");
                            usbInterface = intf2.get();
                            continue;
                        }
                        else
                        {
                            break;
                        }
                    }
                    int magic = c.read32();
                    if(magic == Command.GLCI)
                    {
                        int cmdid = c.read32();
                        Command.Id id = Command.Id.from32(cmdid);
                        if((openedfile != null) && (id != Command.Id.ReadFile) && (id != Command.Id.WriteFile))
                        {
                            openedfile.close();
                            openedfile = null;
                            updateMessage("Processing USB input from Goldleaf...");
                            updateProgress(-1.0, -1.0);
                        }
                        System.out.println("Command: " + id.toString());
                        switch(id)
                        {
                            case GetDriveCount:
                            {
                                drives = FileSystem.listDrives();
                                c.responseStart();
                                c.write32(drives.size());
                                c.responseEnd();
                                break;
                            }
                            case GetDriveInfo:
                            {
                                if(drives == null) drives = FileSystem.listDrives();
                                int idx = c.read32();
                                if(idx < drives.size())
                                {
                                    String drive = drives.elementAt(idx);
                                    c.responseStart();
                                    c.writeString(FileSystem.getDriveLabel(drive));
                                    c.writeString(drive);
                                    c.write32(0);
                                    c.write32(0);
                                    c.responseEnd();
                                }
                                else c.respondFailure(0xDEAD);
                                break;
                            }
                            case StatPath:
                            {
                                String path = FileSystem.denormalizePath(c.readString());
                                try
                                {
                                    File f = new File(path);
                                    int type = 0;
                                    long filesz = 0;
                                    if(f.isFile())
                                    {
                                        type = 1;
                                        filesz = f.length();
                                    }
                                    if(f.isDirectory()) type = 2;
                                    if(type == 0) c.respondFailure(0xDEAD);
                                    else
                                    {
                                        c.responseStart();
                                        c.write32(type);
                                        c.write64(filesz);
                                        c.responseEnd();
                                    }
                                }
                                catch(Exception e)
                                {
                                    c.respondFailure(0xDEAD);
                                }
                                break;
                            }
                            case GetFileCount:
                            {
                                String path = FileSystem.denormalizePath(c.readString());
                                int count = FileSystem.getFilesIn(path).size();
                                c.responseStart();
                                c.write32(count);
                                c.responseEnd();
                                break;
                            }
                            case GetFile:
                            {
                                String path = FileSystem.denormalizePath(c.readString());
                                int idx = c.read32();
                                Vector<String> files = FileSystem.getFilesIn(path);
                                if(idx < files.size())
                                {
                                    c.responseStart();
                                    c.writeString(files.elementAt(idx));
                                    c.responseEnd();
                                }
                                else c.respondFailure(0xDEAD);
                                break;
                            }
                            case GetDirectoryCount:
                            {
                                String path = FileSystem.denormalizePath(c.readString());
                                int count = FileSystem.getDirectoriesIn(path).size();
                                c.responseStart();
                                c.write32(count);
                                c.responseEnd();
                                break;
                            }
                            case GetDirectory:
                            {
                                String path = FileSystem.denormalizePath(c.readString());
                                int idx = c.read32();
                                Vector<String> dirs = FileSystem.getDirectoriesIn(path);
                                if(idx < dirs.size())
                                {
                                    c.responseStart();
                                    c.writeString(dirs.elementAt(idx));
                                    c.responseEnd();
                                }
                                else c.respondFailure(0xDEAD);
                                break;
                            }
                            case ReadFile:
                            {
                                String path = FileSystem.denormalizePath(c.readString());
                                long offset = c.read64();
                                long size = c.read64();
                                try
                                {
                                    if((openedfile == null) || !openedpath.equalsIgnoreCase(path))
                                    {
                                        openedpath = path;
                                        if(openedfile != null)
                                        {
                                            openedfile.close();
                                            openedfile = null;
                                        }
                                        openedfile = new RandomAccessFile(openedpath, "rw");
                                    }
                                    else
                                    {
                                        updateMessage("Goldleaf is reading a file: " + path);
                                        updateProgress(offset, openedfile.length());
                                    }
                                    byte[] block = new byte[(int)size];
                                    openedfile.seek(offset);
                                    int read = openedfile.read(block, 0, (int)size);
                                    c.responseStart();
                                    c.write64((long)read);
                                    c.responseEnd();
                                    c.sendBuffer(block);
                                }
                                catch(Exception e)
                                {
                                    c.respondFailure(0xDEAD);
                                }
                                break;
                            }
                            case WriteFile:
                            {
                                String path = FileSystem.denormalizePath(c.readString());
                                long size = c.read64();
                                byte[] data = c.getBuffer((int)size);
                                try
                                {
                                    if((openedfile == null) || !openedpath.equalsIgnoreCase(path))
                                    {
                                        openedpath = path;
                                        if(openedfile != null)
                                        {
                                            openedfile.close();
                                            openedfile = null;
                                        }
                                        openedfile = new RandomAccessFile(openedpath, "rw");
                                    }
                                    openedfile.seek(openedfile.length());
                                    openedfile.write(data);
                                    c.respondEmpty();
                                }
                                catch(Exception e)
                                {
                                    c.respondFailure(0xDEAD);
                                }
                                break;
                            }
                            case Create:
                            {
                                int type = c.read32();
                                String path = FileSystem.denormalizePath(c.readString());
                                try
                                {
                                    if(type == 1) new File(path).createNewFile();
                                    else if(type == 2) new File(path).mkdir();
                                    c.respondEmpty();
                                }
                                catch(Exception e)
                                {
                                    c.respondFailure(0xDEAD);
                                }
                                break;
                            }
                            case Delete:
                            {
                                int type = c.read32();
                                String path = FileSystem.denormalizePath(c.readString());
                                try
                                {
                                    if((type == 1) || (type == 2)) FileSystem.deletePath(new File(path));
                                    c.respondEmpty();
                                }
                                catch(Exception e)
                                {
                                    c.respondFailure(0xDEAD);
                                }
                                break;
                            }
                            case Rename:
                            {
                                int type = c.read32();
                                String path = FileSystem.denormalizePath(c.readString());
                                String newpath = FileSystem.denormalizePath(c.readString());
                                if((type != 1) && (type != 2)) c.respondFailure(0xDEAD);
                                else
                                {
                                    try
                                    {
                                        File p = new File(path);
                                        p.renameTo(new File(p.getParent(), newpath));
                                        c.respondEmpty();
                                    }
                                    catch(Exception e)
                                    {
                                        c.respondFailure(0xDEAD);
                                    }
                                }
                                break;
                            }
                            case GetSpecialPathCount:
                            {
                                c.responseStart();
                                synchronized(cfglock)
                                {
                                    c.write32(cfg.data.size());
                                }
                                c.responseEnd();
                                break;
                            }
                            case GetSpecialPath:
                            {
                                int idx = c.read32();
                                synchronized(cfglock)
                                {
                                    if(idx < cfg.data.size())
                                    {
                                        int tmpidx = 0;
                                        Enumeration<?> enums = cfg.data.propertyNames();
                                        while(enums.hasMoreElements())
                                        {
                                            String key = (String)enums.nextElement();
                                            String value = cfg.data.getProperty(key);
                                            if(tmpidx == idx)
                                            {
                                                c.responseStart();
                                                c.writeString(key);
                                                c.writeString(FileSystem.normalizePath(value));
                                                c.responseEnd();
                                                break;
                                            }
                                            tmpidx++;
                                        }
                                    }
                                    else c.respondFailure(0xDEAD);
                                }
                                break;
                            }
                            case SelectFile:
                            {
                                selected = false;
                                Platform.runLater(() ->
                                {
                                    synchronized(selectlock)
                                    {
                                        File tmpfile = new FileChooser().showOpenDialog(primaryStage);
                                        if(tmpfile != null) selectedfile = tmpfile.toString();
                                        selected = true;
                                    }
                                });
                                while(true)
                                {
                                    synchronized(selectlock)
                                    {
                                        if(selected) break;
                                    }
                                }
                                if(selectedfile != null)
                                {
                                    c.responseStart();
                                    c.writeString(FileSystem.normalizePath(selectedfile));
                                    c.responseEnd();
                                }
                                else c.respondFailure(0xDEAD);
                                break;
                            }
                            default:
                            {
                                System.out.println("Unknown Id: " + cmdid);
                                break;
                            }
                        }
                    }
                }
                die();
                return null;
            }
        };

        controller.prepare(this, usbtask);
        stage.show();

        Thread usb = new Thread(usbtask);
        usb.setDaemon(true);
        usb.start();
    }
    
    public static void main(String[] args)
    {
        Application.launch(args);
    }
}
