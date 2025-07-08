package fr.elydev.deltaplayer;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;
import android.view.KeyEvent;
import android.util.Log;
import android.net.Uri;
import java.io.File;

// No longer need most androidx.media.session or androidx.media imports here, as the Service handles them
// Just basic NotificationCompat for general notification building if you still need it here for other types of notifications.
import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;
// Keep this one if you create generic notifications elsewhere
// import androidx.media.app.NotificationCompat.MediaStyle;

// No need for MediaSessionCompat, MediaMetadataCompat, PlaybackStateCompat directly here anymore
// import androidx.media.session.MediaSessionCompat;
// import androidx.media.MediaMetadataCompat;
// import androidx.media.session.PlaybackStateCompat;


public class MediaNotification {

    // You might still use this if you need to manually stop the service from QML
    // private static native void onAction(String action);

    // This method will now *start* the MediaPlayerService
    public static void startMediaService(Context context, String albumArt, String title, String artist) {
        Log.d("MediaNotification", "Starting MediaPlayerService.");

        Intent serviceIntent = new Intent(context, MediaPlayerService.class);
        // Pass metadata to the service via extras
        serviceIntent.putExtra("title", title);
        serviceIntent.putExtra("artist", artist);
        Log.d("MediaNotification", albumArt);
        Uri albumArtUri = Uri.fromFile(new File(albumArt));
        serviceIntent.putExtra("albumArtUri", albumArtUri);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            context.startForegroundService(serviceIntent);
        } else {
            context.startService(serviceIntent);
        }
    }

    // You can add a method to stop the service if needed
    public static void stopMediaService(Context context) {
        Log.d("MediaNotification", "Stopping MediaPlayerService.");
        Intent serviceIntent = new Intent(context, MediaPlayerService.class);
        context.stopService(serviceIntent);
    }

    // You can keep this if your QML code still calls it, but it should now call startMediaService/stopMediaService
    public static void createNotificationOnUiThread(final Context context,
                                                       final String albumArt,
                                                       final String title,
                                                       final String string) {
        new Handler(Looper.getMainLooper()).post(new Runnable() {
            @Override
            public void run() {
                // Now, this just starts the service
                startMediaService(context, albumArt, title, string);
            }
        });
    }
}
