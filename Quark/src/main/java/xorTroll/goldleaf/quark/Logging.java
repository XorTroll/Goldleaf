package xorTroll.goldleaf.quark;

import java.text.SimpleDateFormat;
import java.util.Calendar;

public class Logging {
    // Wrapper to do a println() printing current time.
    public static void log(String s) {
        Calendar cal = Calendar.getInstance();
        SimpleDateFormat sdf = new SimpleDateFormat("[HH:mm:ss] ");
        System.out.println(sdf.format(cal.getTime()) + s);
    }
}