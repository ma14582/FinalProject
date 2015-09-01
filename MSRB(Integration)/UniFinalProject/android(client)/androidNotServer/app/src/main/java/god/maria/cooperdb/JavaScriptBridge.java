/*
 ==============================================================================================
 This class interfaces with the javascript, declaring all the methods which has been called
 from the web such as sending a notification for the user once a new event happens, getting
 all the events from the server if there is a connexion or , otherwise, from the database
 in android. It also inserts all the events in android database from the server every time user
 makes a request, updating them if there s connexion
 ==============================================================================================
*/

package god.maria.cooperdb;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.webkit.JavascriptInterface;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.Timer;
import java.util.TimerTask;

public class JavaScriptBridge {

    // Creating objects of the different types
    private DatabaseAndroidActivity databaseAndroidActivity;
    private Context mContext;
    private ServerConnexion serverConnexion;
    private NotificationManager notificationManager;
    private Notification miNotification;
    private DatabaseHelper myDb;
    private static final int MY_NOTIFICATION_ID = 1;

    // Timer to check if there are new events
    int startTimer = 0;
    Timer timer = new Timer();
    static final int UPDATE_INTERVAL = 10000;           // Every 10 seconds
    int lastId;             // The last id updated

    // JavaScriptBridge Constructor
    public JavaScriptBridge(Context c, NotificationManager notificationManager, DatabaseHelper myDb, ServerConnexion serverConnexion, DatabaseAndroidActivity databaseAndroidActivity) {
        this.mContext = c;
        this.notificationManager = notificationManager;
        this.myDb = myDb;
        //this.eventCache = eventCache;
        this.serverConnexion = serverConnexion;
        this.databaseAndroidActivity = databaseAndroidActivity;
    }

    // Getting all the events from the server or the mobile database, according to the server connexion
    @JavascriptInterface
    public String getEventFromServerOrAndroidDb(String service) throws JSONException {
        String result = null;
        if (serverConnexion.resultFromServer(service).equals("no server connexion")) {
            result = databaseAndroidActivity.getEventFromDb(service);
        } else {
            result = serverConnexion.resultFromServer(service);
            databaseAndroidActivity.insertDataInAndroidIfNew(result);
        }
        return result;
    }

    // Send a notification every time there is a new event in the database server
    public void startTime() {
        timer.scheduleAtFixedRate(new TimerTask() {
            @Override
            public void run() {
                String result = null;
                String stringNumber = null;
                int currentId = 0;
                int newEvents = 0;
                // If there is server connexion check for the last events inserted in the database server
                try {
                    if (!serverConnexion.resultFromServer("getLastEvent").equals("no server connexion")) {
                        result = serverConnexion.resultFromServer("getLastEvent");
                        stringNumber = result.replaceAll("[^0-9]", "");         // Get the id in the string
                        currentId = Integer.parseInt(stringNumber);     // Convert the string number into int
                        // Start the timer and set the current id and the last update
                        if (startTimer == 0) {
                            lastId = currentId;
                            startTimer = 1;
                        } else {
                            // Send a notification in case of new event
                            if (currentId != lastId) {
                                newEvents = currentId - lastId;         // Check how many new events there are
                                miNotification = new Notification.Builder(mContext)
                                        .setContentTitle("It is time to check  ...")
                                        .setContentText("You have " + newEvents + " new events")
                                        .setTicker("Notificatión!")
                                        .setWhen(System.currentTimeMillis())
                                        .setDefaults(Notification.DEFAULT_SOUND)
                                        .setAutoCancel(true)
                                        .setSmallIcon(R.drawable.ic_launcher)
                                        .build();

                                notificationManager.notify(MY_NOTIFICATION_ID, miNotification);
                                lastId = currentId;
                            }
                        }
                    }
                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }

        }, 0, UPDATE_INTERVAL);
    }

}